import { DeviceFlowUI, DeviceFlowUIOptions } from '@openformtech/firebasedeviceflow';
import firebase from 'firebase';
import { IPeaPodArduino, PeaPodArduinoInterface } from './PeaPodArduino';
import { ArduinoSimulator } from './PeaPodSimulator'

export type MessageType = 'info' | 'data' | 'debug' | 'error';

export type PeaPodMessage = {
    type: MessageType,
    msg: any
}

const firebaseConfig : Object = {
    apiKey: "AIzaSyC7iBFv4PEmWss4h_Ul01Mpkzgpu2GuXao",
    authDomain: "peapod-283416.firebaseapp.com",
    databaseURL: "https://peapod-283416.firebaseio.com",
    projectId: "peapod-283416",
    storageBucket: "peapod-283416.appspot.com",
    messagingSenderId: "513099710307",
    appId: "1:513099710307:web:bf82ec0946b233a0f79d56",
    measurementId: "G-WR33SVX7DJ"
}

const authConfig : DeviceFlowUIOptions = {
    Google : {
        clientid : '513099710307-78rqvpchfe8qissqgaugp160nsa1d4t5.apps.googleusercontent.com',
        clientsecret : 'YKCeZITc11tfDAypvT2q4Ld9',
        scopes : ['email', 'profile']
    },
    GitHub : {
        clientid : 'f982a1faefcf73eb1268',
        scopes : ['read:user', 'user:email']
    }
}

export default class PeaPod {
    arduino : IPeaPodArduino | undefined;
    constructor(readonly simulated : boolean = true){
        firebase.initializeApp(firebaseConfig);
    }

    async authenticate() : Promise<firebase.User> {
        let login = new DeviceFlowUI(firebase.app(), authConfig);
        return login.signIn();
    }

    async post() : Promise<void> {
        if(this.simulated){
            this.arduino = new ArduinoSimulator({
                air_temperature: {
                    min: 10,
                    max: 20,
                    interval: 2000
                },
                water_level: {
                    min: 0,
                    max: 1,
                    interval: 1000
                }
            });
        } else {
            this.arduino = new PeaPodArduinoInterface();
        }
    }
}