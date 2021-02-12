import { PeaPodMessage } from './PeaPod';
import * as iot from '@google-cloud/iot';
import {PubSub} from '@google-cloud/pubsub';
import firebase from 'firebase';
import {v4 as uuid} from 'uuid';
import { exec } from 'child_process';
import * as fs from 'fs';
import { default as namegen } from 'project-name-generator';

const shell = (command : string) : Promise<string> => {
    return new Promise<string>((resolve, reject)=>{
        exec(command, (error, stdout, stderr)=>{
            if(error){
                reject(error);
            }else if(stderr.length > 0){
                reject(stderr);
            } else {
                resolve(stdout);
            }
        });
    });
};

export interface IPeaPodPublisher {
    initialize() : Promise<void>,
    publish(msg : PeaPodMessage) : Promise<void>
}

type IoTConfig = {
    projectid: string,
    cloudregion: string,
    registryid: string
}

type Keypair = {
    public: string,
    private: string
}

export default class PeaPodFirebase implements IPeaPodPublisher {
    private iotClient : iot.DeviceManagerClient;
    private pubsub : PubSub;
    constructor(private config: IoTConfig, private deviceid? : string){
        this.iotClient = new iot.v1.DeviceManagerClient();
        this.pubsub = new PubSub();
        this.deviceid = deviceid || 'peapod-'+uuid();
    }
    publish(msg: PeaPodMessage): Promise<void> {
        throw new Error('Method not implemented.');
    }
    async initialize(): Promise<void> {
        // ASSUMPTION: New device IFF no existing keypair
        if(!fs.existsSync('./openssl/rsa_public.pem')){
            await this.register();
        }
    }
    async register(): Promise<{response: any, name: string}> {
        const keypair = await generateRSAKeypair();
        //Generate name
        const name = namegen().raw.map((word)=>{
            return (word as string).slice(0,1).toUpperCase()+(word as string).slice(1)
        }).join(' ');
        const request : iot.protos.google.cloud.iot.v1.ICreateDeviceRequest = {
            parent: this.iotClient.registryPath(this.config.projectid, this.config.cloudregion, this.config.registryid),
            device : {
                id: this.deviceid,
                credentials: [
                    {
                        publicKey: {
                            format: 'RSA_PEM',
                            key: keypair.public,
                        },
                    },
                ],
            },
        };
        const func = firebase.functions().httpsCallable('createDevice');
        const result = await func({request, name: name});
        return {response: result.data, name: name};
    }
}

async function generateRSAKeypair() : Promise<Keypair>{
    await shell('openssl genpkey -algorithm RSA -out rsa_private.pem -pkeyopt rsa_keygen_bits:2048');
    await shell('openssl rsa -in rsa_private.pem -pubout -out rsa_public.pem');
    return {
        public: fs.readFileSync('./openssl/rsa_public.pem').toString(),
        private: fs.readFileSync('./openssl/rsa_private.pem').toString()
    } as Keypair;
}