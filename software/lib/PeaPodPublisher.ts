import * as jwt from 'jsonwebtoken';
import * as fs from 'fs';
import * as mqtt from 'mqtt';
import Spinner from './ui'; //UI utils
import { getApp } from 'firebase/app';
import { getFunctions, httpsCallable } from 'firebase/functions';
import { getFirestore, doc, setDoc, collection, getDocs, query, where, DocumentReference } from 'firebase/firestore';
import { getAuth } from 'firebase/auth';
import { fetchServerCert } from './utils';
import * as inquirer from 'inquirer';
import {v4 as uuid} from 'uuid';
import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import chalk from 'chalk';

export type PeaPodDataBatch = {
  [key: string]: {
    batch: {
      timestamp: number,
      value: number
    }[]
  }
}

// PeaPod Message to Cloud
export type PeaPodMessage = {
  type: 'info' | 'debug' | 'error',
  data: any
} | {
  type: 'data',
  metadata: {
    owner: string,
    project: string,
    run: string
  }
  data: PeaPodDataBatch
}

export type PeaPodCommand = {
  type: 'livestreamoffer',
  data: any
  // TODO: import types from WebRTC
}

/**
* Abstract base class for any PeaPod message destination.
*/
export type IPeaPodPublisher = {
  start(onConfig?: (message: string)=>void, onCommand?: (message: PeaPodCommand)=>void) : Promise<{projectid: string, projectname?: string, run: string}>,
  stop(): void;
  publish(msg : PeaPodMessage) : void
}

type RegisterResponse = {
  id: string,
  name: string,
  privateKey: string
}

export type IoTConfig = {
  deviceid?: string,
  projectid: string,
  cloudregion: string,
  registryid: string,
  jwtexpiryminutes: number
}

export default class PeaPodPubSub implements IPeaPodPublisher {
  private tokenRefreshInterval?: NodeJS.Timer;
  private mqttclient?: mqtt.MqttClient;
  private deviceId: string = '';
  constructor(readonly iotConfig: IoTConfig, readonly authConfig: DeviceFlowUIOptions){}
  publish(msg: PeaPodMessage): void {
    if(!this.mqttclient || !this.mqttclient.connected){
      throw new Error('MQTT client not connected!');
    }
    // Build topic path
    const topic = `/devices/${this.deviceId}/` + (msg.type == 'data' ? 'events/data' : msg.type == 'info' ? 'events' : 'state');
    // Strip type from published object
    this.mqttclient.publish(topic, JSON.stringify({...msg, type: undefined}), {qos: 1});
  }
  async start(onConfig: (message: string)=>void, onCommand: (message: PeaPodCommand)=>void) {
    let privatekey = '';

    // Authenticate the user with Firebase
    const auth = new DeviceFlowUI(getApp(), this.authConfig);
    const user = await auth.signIn();
    if(user.displayName){
      Spinner.info(`Welcome, ${chalk.bold(user.displayName)}!`);
    } else {
      Spinner.info('Welcome!');
    }
    
    if(fs.existsSync('./rsa_private.pem') && fs.existsSync('./deviceInfo.json')){
      Spinner.succeed('Private key and device info found!');
      privatekey = fs.readFileSync('./rsa_private.pem').toString();
      let deviceinfo = JSON.parse(fs.readFileSync('./deviceInfo.json').toString());
      this.deviceId = deviceinfo['id'];
      if(deviceinfo['owner'] != user.uid){
        throw new Error('This PeaPod is not owned by this user!');
      }
    } else {
      Spinner.info('Private key and/or device info not found!');
      Spinner.start('Registering device...');
      const registerDevice = httpsCallable<void, RegisterResponse>(getFunctions(), 'registerDevice');
      let result = (await registerDevice()).data;
      Spinner.succeed('Device '+(result.id) + ' registered!');
      
      fs.writeFileSync('./rsa_private.pem', result.privateKey);
      fs.writeFileSync('./deviceInfo.json', JSON.stringify({name: result.name, id: result.id, owner: getAuth().currentUser?.uid}, null, 2));
      privatekey = result.privateKey;
      this.deviceId = result.id;
    }
    
    Spinner.start('Fetching Google root CA certificates...');
    const servercert = await fetchServerCert();
    Spinner.succeed('Certificates fetched!');

    const [project, projectname] = await this.selectProject();
    const run = await this.createRun(project);
    
    Spinner.start('Connecting to MQTT broker...');
    await this.connect(servercert, this.refreshToken(privatekey));
    Spinner.succeed('Device connected!');
    
    // Token Refresh
    this.tokenRefreshInterval = setInterval(async ()=>{
      Spinner.start('Refreshing token...');
      await this.connect(servercert, this.refreshToken(privatekey));
      Spinner.succeed('Token refreshed. Reconnected.');
    }, this.iotConfig.jwtexpiryminutes*60*1000);
    
    // Message listeners
    this.mqttclient?.subscribe(`/devices/${this.deviceId}/config`, {qos: 1});
    this.mqttclient?.subscribe(`/devices/${this.deviceId}/commands/#`, {qos: 0});
    
    this.mqttclient?.on('error', err => { throw err });
    this.mqttclient?.on('message', (topic, message) => {
      if (topic === `/devices/${this.deviceId}/config`) {
        onConfig(message.toString());
      } else if (topic.startsWith(`/devices/${this.deviceId}/commands`)) {
        onCommand(JSON.parse(message.toString()) as PeaPodCommand);
      }
    });

    return {projectid: project.id, projectname, run: run.id}
  }
  
  /**
  * Select a project owned by the current user
  * @returns {Promise<[DocumentReference, string]>}
  */
  private async selectProject(): Promise<[DocumentReference, string]> {
    const myProjects = query(collection(getFirestore(), 'projects'), where('owners', 'array-contains', getAuth().currentUser?.uid));
    const projects = (await getDocs(myProjects)).docs;
    if(projects.length < 1){
      throw new Error("No projects found! Create one first.");
    }
    const ref = (await inquirer.prompt<{ref: [DocumentReference, string]}>([
      {
        type: 'list',
        name: 'ref',
        message: 'Select a project:',
        choices: projects.map(project=>({name: project.get('name')+' - '+project.id, value: [project.ref, project.get('name')]}))
      }
    ])).ref;
    return ref;
  }
  
  /**
  * Publish a new project.
  * @returns {Promise<DocumentReference>} The project.
  */
  private async createRun(project : DocumentReference) : Promise<DocumentReference> {
    const runid = project.id+'-'+uuid();
    const rundoc = doc(getFirestore(), project.path+'/runs/'+runid);
    setDoc(rundoc, {
      owner: getAuth().currentUser?.uid,
      deviceId: this.deviceId,
    });
    // console.log(`New run ${runid} on project ${project.id} generated successfully.`);
    return rundoc;
  }
  
  /**
  * Select a run owned by the current user under a given project.
  * @returns {Promise<DocumentReference>}
  */
  async selectRun(project : DocumentReference){
    const myRuns = query(collection(getFirestore(), project.path+'/runs'), where('owner', '==', getAuth().currentUser?.uid));
    const runs = ((await getDocs(myRuns)).docs.map(doc=>({id: doc.id,ref: doc.ref})));
    if(runs.length == 0){
      throw new Error("No runs found! Create one first.");
    }
    const ref = (await inquirer.prompt<{ref: DocumentReference}>([
      {
        type: 'list',
        name: 'ref',
        message: 'Select a run:',
        choices: runs.map(run=>({name: run.id, value: run.ref}))
      }
    ])).ref;
    return ref;
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
  * @param deviceid ID of this device.
  * @returns The MQTT client.
  */
  private async connect(servercert: string, password: string): Promise<void> {
    // Disconnect existing client
    this.disconnect();
    
    let client = mqtt.connect({
      host: 'mqtt.googleapis.com',
      port: 8883,
      clientId: `projects/${this.iotConfig.projectid}/locations/${this.iotConfig.cloudregion}/registries/${this.iotConfig.registryid}/devices/${this.deviceId}`,
      username: 'unused',
      password: password,
      protocol: 'mqtts',
      secureProtocol: 'TLSv1_2_method',
      ca: [servercert],
    });
    
    return new Promise<void>((res)=>{
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
    if(this.mqttclient && this.mqttclient.connected){
      await new Promise<void>(res=>{
        this.mqttclient?.end(true, undefined, (err)=>{
          if(err) {
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