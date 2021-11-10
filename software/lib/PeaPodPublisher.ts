import * as jwt from 'jsonwebtoken';
import * as fs from 'fs';
import * as mqtt from 'mqtt';
import Spinner from './ui'; //UI utils
import { getFunctions, httpsCallable } from 'firebase/functions';
import { fetchServerCert } from './utils';

type PubSubData = {
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
  data: PubSubData
}

/**
* Abstract base class for any PeaPod message destination.
*/
export type IPeaPodPublisher = {
  start(onConfig?: (message: string)=>void, onCommand?: (message: string)=>void) : Promise<void>,
  publish(msg : PeaPodMessage) : void
}

type RegisterResponse = {
  id: string,
  name: string,
  privateKey: string
}

type IoTConfig = {
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
  constructor(readonly config: IoTConfig){}
  publish(msg: PeaPodMessage): void {
    if(!this.mqttclient || !this.mqttclient.connected){
      throw new Error('MQTT client not connected!');
    }
    const topic = `/devices/${this.deviceId}/` + (msg.type == 'data' ? 'data' : msg.type == 'info' ? 'events' : 'state');
    this.mqttclient.publish(topic, JSON.stringify(Object.fromEntries(Object.entries(msg).filter(m=>m[0]!='type'))), {qos: 1});
  }
  async start(onConfig: (message: string)=>void, onCommand: (message: string)=>void): Promise<void> {
    let privatekey = '';
    
    try {
      if(fs.existsSync('./rsa_private.pem') && fs.existsSync('./deviceInfo.json')){
        Spinner.succeed('Private key and device info found!');
        privatekey = fs.readFileSync('./rsa_private.pem').toString();
        this.deviceId = JSON.parse(fs.readFileSync('./deviceInfo.json').toString())['id'];
      } else {
        throw '';
      }
    } catch {
      Spinner.fail('Private key and/or device info not found!');
      Spinner.start('Registering device...');
      let result = await this.register();
      Spinner.succeed('Device '+(result.name ?? result.id) + ' registered!');
      
      fs.writeFileSync('./rsa_private.pem', result.privateKey);
      fs.writeFileSync('./deviceInfo.json', JSON.stringify({name: result.name, id: result.id}));
      privatekey = result.privateKey;
      this.deviceId = result.id;
    }
    
    Spinner.start('Fetching Google root CA certificates...');
    const servercert = await fetchServerCert();
    Spinner.succeed('Certificates fetched!');
    
    Spinner.start('Connecting to MQTT broker...');
    await this.connect(servercert, this.refreshToken(privatekey));
    Spinner.succeed('Device connected!');
    
    // Token Refresh
    this.tokenRefreshInterval = setInterval(async ()=>{
      Spinner.start('Refreshing token...');
      await this.connect(servercert, this.refreshToken(privatekey));
      Spinner.succeed('Token refreshed. Reconnected.');
    }, this.config.jwtexpiryminutes*60*1000);
    
    // Message listeners
    this.mqttclient?.subscribe(`/devices/${this.deviceId}/config`, {qos: 1});
    this.mqttclient?.subscribe(`/devices/${this.deviceId}/commands/#`, {qos: 0});
    
    this.mqttclient?.on('error', err => { throw err });
    this.mqttclient?.on('message', (topic, message) => {
      if (topic === `/devices/${this.deviceId}/config`) {
        onConfig(message.toString());
      } else if (topic.startsWith(`/devices/${this.deviceId}/commands`)) {
        onCommand(message.toString());
      }
    });
  }
  
  async register(): Promise<RegisterResponse> {
    const registerDevice = httpsCallable<void, RegisterResponse>(getFunctions(), 'registerDevice');
    return (await registerDevice()).data;
  }
  
  /**
  * Sign a new JWT.
  * @returns JSON Web Token string payload.
  */
  refreshToken(privatekey: string) : string {
    const now = Date.now() / 1000;
    const token = {
      iat: now,
      exp: now + this.config.jwtexpiryminutes * 60,
      aud: this.config.projectid,
    };
    return jwt.sign(token, privatekey, {algorithm: 'RS256'});
  }
  
  /**
  * Connect to the MQTT broker.
  * @param servercert Root CA certificate.
  * @param deviceid ID of this device.
  * @returns The MQTT client.
  */
  async connect(servercert: string, password: string): Promise<void> {
    // Disconnect existing client
    this.disconnect();
    
    let client = mqtt.connect({
      host: 'mqtt.googleapis.com',
      port: 8883,
      clientId: `projects/${this.config.projectid}/locations/${this.config.cloudregion}/registries/${this.config.registryid}/devices/${this.deviceId}`,
      username: 'unused',
      password: password,
      protocol: 'mqtts',
      secureProtocol: 'TLSv1_2_method',
      ca: [servercert],
    });
    
    return new Promise<void>(()=>{
      client.on('connect', packet => {
        if (!packet) {
          throw new Error('Could not connect to MQTT broker!');
        }
        this.mqttclient = client;
      });
    });
  }
  
  /**
  * If the MQTT client is connected, disconnect it.
  */
  async disconnect(): Promise<void>{
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