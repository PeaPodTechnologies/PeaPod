import { PeaPodMessage } from './PeaPod';
import {PubSub} from '@google-cloud/pubsub';
import * as fs from 'fs';

/**
 * Abstract base class for any PeaPod message destination.
 */
export interface IPeaPodPublisher {
    start() : Promise<void>,
    publish(msg : PeaPodMessage) : Promise<void>
}

export default class PeaPodPubSub implements IPeaPodPublisher {
    private pubsub : PubSub;
    constructor(){
        this.pubsub = new PubSub();
    }
    publish(msg: PeaPodMessage): Promise<void> {
        // TODO
        throw new Error('Method not implemented.');
    }
    async start(): Promise<void> {
        // ASSUMPTION: New device IFF no existing key
        if(!fs.existsSync('./openssl/rsa_private.pem')){
            await this.register();
        }
    }
    async register(): Promise<void> {
        //TODO
        throw new Error('Method not implemented.');
    }
}