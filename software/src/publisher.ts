import * as fs from 'fs';
import {v4 as uuid} from 'uuid';
import * as jwt from 'jsonwebtoken';
import * as mqtt from 'mqtt';
import * as inquirer from 'inquirer';
import chalk from 'chalk';
import { existsSync, mkdirSync, writeFileSync } from 'fs';

import { getApp } from 'firebase/app';
import { getFunctions, httpsCallable } from 'firebase/functions';
import { getFirestore, doc, setDoc, collection, getDocs, query, where, DocumentReference } from 'firebase/firestore';
import { getAuth } from 'firebase/auth';

import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow';
import * as Spinner from './ui';
import { fetchServerCert } from './utils';
import { ControllerInstructions } from './controller';

// TYPES

/**
 * A single datapoint.
 */
type DataPoint = {
  timestamp: number,
  value: number
};

/**
 * Batch of datapoints to be published, organized by dataset label
 */
export type DataBatch = {
  [key: string]: DataPoint[],
};

/**
 * Message TO the publisher.
 */
export type PubSubMessage = {
  type: 'info' | 'debug' | 'error',
  data: any,
} | {
  type: 'data',
  metadata: {
    owner: string,
    project: string,
    run: string,
  }
  data: DataBatch,
};

/**
 * Message FROM the publisher (command).
 */
export type PubSubCommand = {
  type: 'instructions',
  data: ControllerInstructions,
};

/**
 * Message FROM the publisher (config). WIP.
 */
export type PubSubConfig = string;

/**
 * Types of publishers.
 */
export enum PublishingMode {
  ONLINE = "Online",
  OFFLINE = "Offline",
};

/**
 * Response from the device registration cloud function.
 */
type RegisterResponse = {
  id: string,
  name: string,
  privateKey: string,
}

/**
 * GCP IoT Core configuration parameters.
 */
export type IoTConfig = {
  deviceid?: string,
  projectid: string,
  cloudregion: string,
  registryid: string,
  jwtexpiryminutes: number,
}

/**
 * Base type for any publisher.
 */
export type Publisher = {
  start(onConfig?: (message: PubSubConfig)=>void, onCommand?: (message: PubSubCommand)=>void) : Promise<{projectid: string, projectname?: string, run: string}>,
  stop(): void,
  publish(msg: PubSubMessage): void,
}

// MAIN CLASS

/** 
 * Interface for publishing data to and receiving instructions from Google Cloud/Firebase via IoT Core PubSub.
 */
export default class PubSubPublisher implements Publisher {

  private tokenRefreshInterval?: NodeJS.Timer;
  private mqttclient?: mqtt.MqttClient;
  private deviceid!: string;
  
  constructor(readonly iotConfig: IoTConfig, readonly authConfig: DeviceFlowUIOptions){ }

  publish(msg: PubSubMessage): void {
    if (!this.mqttclient || !this.mqttclient.connected) {
      throw new Error('MQTT client not connected!');
    }

    // Build topic path
    const topic = `/devices/${ this.deviceid }/` + (msg.type === 'data' ? 'events/data' : msg.type === 'info' ? 'events' : 'state');

    // Strip type from published object
    this.mqttclient.publish(topic, JSON.stringify({ ...msg, type: undefined }), { qos: 1 });
  }

  async start(onConfig: (message: PubSubConfig) => void, onCommand: (message: PubSubCommand) => void) {

    // AUTHENTICATION - FIREBASE DEVICE FLOW
    const auth = new DeviceFlowUI(getApp(), this.authConfig);
    const user = await auth.signIn();
    if (user.displayName) {
      Spinner.info(`Welcome, ${ chalk.bold(user.displayName) }!`);
    } else {
      Spinner.info('Welcome!');
    }
    
    // GET DEVICE INFO OR REGISTER
    const {privatekey, deviceid} = await this.getDeviceInfoOrRegister(user.uid);
    this.deviceid = deviceid;

    // CHOOSE PROJECT, RUN, PROGRAM
    // TODO: choose program
    const [project, projectname] = await this.selectProject();
    const run = await this.createRun(project);

    // SETUP MQTT
    Spinner.start('Fetching Google root CA certificates...');
    const servercert = await fetchServerCert();
    Spinner.succeed('Certificates fetched!');
    
    Spinner.start('Connecting to MQTT broker...');
    await this.connect(servercert, this.refreshToken(privatekey));
    Spinner.succeed('Device connected!');
    
    // Token Refresh
    this.tokenRefreshInterval = setInterval(async () => {
      Spinner.start('Refreshing token...');
      await this.connect(servercert, this.refreshToken(privatekey));
      Spinner.succeed('Token refreshed. Reconnected.');
    }, this.iotConfig.jwtexpiryminutes*60*1000);
    
    // Message listeners
    this.mqttclient?.subscribe(`/devices/${ this.deviceid }/config`, { qos: 1 });
    this.mqttclient?.subscribe(`/devices/${ this.deviceid }/commands/#`, { qos: 0 });
    
    this.mqttclient?.on('error', err => { throw err });
    this.mqttclient?.on('message', (topic, message) => {
      if (topic === `/devices/${this.deviceid}/config`) {
        try {
          const config: PubSubConfig = JSON.parse(message.toString());
          onConfig(config);
        } catch (e) {
          if (e instanceof SyntaxError) {
            // TODO: handle invalid config
          }
        }
      } else if (topic.startsWith(`/devices/${this.deviceid}/commands`)) {
        try {
          const command: PubSubCommand = JSON.parse(message.toString());
          onCommand(command);
        } catch (e) {
          if (e instanceof SyntaxError) {
            // TODO: handle invalid command
          }
        }
      }
    });

    return { projectid: project.id, projectname, run: run.id };
  }
  
  /**
  * Select a project owned by the current user
  * @returns A tuple containing both the document reference and the project name (if any)
  */
  private async selectProject(): Promise<[DocumentReference, string]> {
    const myProjects = query(collection(getFirestore(), 'projects'), where('owners', 'array-contains', getAuth().currentUser?.uid));
    const projects = (await getDocs(myProjects)).docs;
    if (projects.length < 1) {
      throw new Error("No projects found! Create one first.");
    }
    const ref = (await inquirer.prompt<{ref: [DocumentReference, string]}>([
      {
        type: 'list',
        name: 'ref',
        message: 'Select a project:',
        choices: projects.map(project => ({
          name: project.get('name')+' - '+project.id, 
          value: [project.ref, project.get('name')] 
        }))
      }
    ])).ref;
    return ref;
  }
  
  /**
  * Publish a new project.
  * @param project Document reference to the project
  * @returns Documetn reference to the new run
  */
  private async createRun(project: DocumentReference): Promise<DocumentReference> {
    const runid = project.id+'-'+uuid();
    const rundoc = doc(getFirestore(), project.path+'/runs/'+runid);
    setDoc(rundoc, {
      owner: getAuth().currentUser?.uid,
      deviceId: this.deviceid,
    });
    return rundoc;
  }

  /**
   * Look for the private key and device info in local files. If not found, register the device and store the new key and info.
   * @param user The UID of the user to whom this device is to be registered.
   * @returns The private key and device ID.
   */
  private async getDeviceInfoOrRegister(user: string): Promise<{privatekey: string, deviceid: string}> {
    if (fs.existsSync('./rsa_private.pem') && fs.existsSync('./deviceInfo.json')) {
      Spinner.succeed('Private key and device info found!');
      const privatekey = fs.readFileSync('./rsa_private.pem').toString();

      let deviceinfo: {id: string, owner: string} = JSON.parse(fs.readFileSync('./deviceInfo.json').toString());

      if (deviceinfo['owner'] != user) {
        throw new Error('This PeaPod is not owned by this user!');
      }
      return { privatekey, deviceid: deviceinfo.id };
    } else {
      Spinner.info('Private key and/or device info not found!');

      Spinner.start('Registering device...');
      const registerDevice = httpsCallable<void, RegisterResponse>(getFunctions(), 'registerDevice');
      let result = (await registerDevice()).data;
      Spinner.succeed('Device '+(result.id) + ' registered!');
      
      fs.writeFileSync('./rsa_private.pem', result.privateKey);
      fs.writeFileSync('./deviceInfo.json', JSON.stringify({name: result.name, id: result.id, owner: getAuth().currentUser?.uid}, null, 2));

      return { privatekey: result.privateKey, deviceid: result.id };
    }
  }
  
  /**
  * Sign a new JWT.
  * @returns JSON Web Token string payload.
  */
  private refreshToken(privatekey: string) : string {
    const now = Date.now() / 1000;
    const token = {
      iat: now,
      exp: now + this.iotConfig.jwtexpiryminutes * 60,
      aud: this.iotConfig.projectid,
    };
    return jwt.sign(token, privatekey, {algorithm: 'RS256'});
  }
  
  /**
  * Connect to the MQTT broker.
  * @param servercert Root CA certificate.
  * @param password JWT
  */
  private async connect(servercert: string, password: string): Promise<void> {
    // Disconnect existing client
    this.disconnect();
    
    let client = mqtt.connect({
      host: 'mqtt.googleapis.com',
      port: 8883,
      clientId: `projects/${ this.iotConfig.projectid }/locations/${ this.iotConfig.cloudregion }/registries/${ this.iotConfig.registryid }/devices/${ this.deviceid }`,
      username: 'unused',
      password,
      protocol: 'mqtts',
      secureProtocol: 'TLSv1_2_method',
      ca: [servercert],
    });
    
    return new Promise<void>(res => {
      client.on('connect', packet => {
        if (!packet) {
          throw new Error('Could not connect to MQTT broker!');
        }
        this.mqttclient = client;
        res();
      });
    });
  }
  
  /**
  * If the MQTT client is connected, disconnect it.
  */
  private async disconnect(): Promise<void>{
    if (this.mqttclient && this.mqttclient.connected) {
      await new Promise<void>(res => {
        this.mqttclient?.end(true, undefined, err => {
          if (err) {
            throw err;
          } else {
            res();
          }
        });
      });
    }
  }
  
  stop() {
    if(this.tokenRefreshInterval) clearInterval(this.tokenRefreshInterval);
    if(this.mqttclient) this.disconnect();
  }
}

/**
 * Publishes data batches to local JSON files.
 */
export class OfflinePublisher implements Publisher {
  async start() {
    let config = { projectid: 'testproject', run: 'testrun-'+uuid() };
    Spinner.info(`Logging data to ${ chalk.bold('projects/'+config.projectid+'/runs/'+config.run+'/') }`);
    return config;
  }
  stop() { };
  publish(msg: PubSubMessage): void {
    switch(msg.type) {
      case 'data':
        for (const label of Object.keys(msg.data)) {
          for (const datum of msg.data[label]) {
            console.log(`[${ chalk.magenta(msg.type.toUpperCase()) }] - [${ (new Date(datum.timestamp)).toLocaleTimeString() }] - ${ label }: ${ datum.value }`);
          }
          const dir = `./projects/${ msg.metadata.project }/runs/${ msg.metadata.run }/${ label }/`;
          if (!existsSync(dir)) {
            mkdirSync(dir, { recursive: true });
          }
          writeFileSync(`${ dir }${ label+'-'+uuid()+'.json' }`, JSON.stringify(msg.data[label], null, 2))
        }
        break;
      default:
        Spinner.log(`[${ chalk.yellow(msg.type.toUpperCase()) }] - ${ JSON.stringify(msg.data) }`);
        break;
    }
  }
}