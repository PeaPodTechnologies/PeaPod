import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import firebase from 'firebase';

import PeaPodArduinoInterface, { IPeaPodArduino } from './lib/PeaPodArduino';
import PeaPodPubSub, { IPeaPodPublisher } from './lib/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './lib/PeaPodSimulator';
import { findSerialPath } from './lib/utils';

import { checkInternet, sleep, loadDotenv } from './lib/utils'; //Utilities
import { failSpinner, startSpinner, succeedSpinner } from './lib/ui'; //UI utils

async function main(){

    // SETUP

    // Load environment variables
    loadDotenv();

    // CONSTANTS
    const firebaseConfig = {
        apiKey: process.env.FIREBASE_APIKEY,
        authDomain: process.env.FIREBASE_AUTHDOMAIN,
        databaseURL: process.env.FIREBASE_DATABASEURL,
        projectId: process.env.FIREBASE_PROJECTID,
        storageBucket: process.env.FIREBASE_STORAGEBUCKET,
        messagingSenderId: process.env.FIREBASE_MESSAGINGSENDERID,
        appId: process.env.FIREBASE_APPID,
        measurementId: process.env.FIREBASE_MEASUREMENTID
    };

    const authConfig : DeviceFlowUIOptions = {
        Google : {
            scopes : process.env.GOOGLE_SCOPES?.split(' '),
            clientid : process.env.GOOGLE_CLIENTID,
            clientsecret : process.env.GOOGLE_CLIENTSECRET
        },
        GitHub : {
            scopes : process.env.GITHUB_SCOPES?.split(' '),
            clientid : process.env.GITHUB_CLIENTID,
            clientsecret : process.env.GITHUB_CLIENTSECRET
        }
    };

    // Check Internet connection
    startSpinner(`Checking for ${chalk.blue('Internet')} connection...'`)
    if(!(await checkInternet())){
        // TODO: Offline operation
        throw new Error(`Could not connect to the {${chalk.blue('Internet')}!`);
    }
    succeedSpinner(`Connected to the ${chalk.blue('Internet')}!`);
    await sleep(1500);

    // ONLINE OPERATION

    // Connect to Firebase
    firebase.initializeApp(firebaseConfig);

    // firebase.functions().useEmulator('localhost', 5001);
    // firebase.auth().useEmulator('http://localhost:9099');
    // firebase.firestore().useEmulator('localhost', 8080);

    // Authenticate the user with Firebase
    let auth = new DeviceFlowUI(firebase.app(), authConfig);
    const user = await auth.signIn();

    // IN PRODUCTION: fetch serial port with findSerialPath
    const simulated = process.argv.includes('simulate');

    let serialpath: string = '';

    if(!simulated){
      serialpath = await findSerialPath();
    }

    // INTERNAL SETUP
    const arduino : IPeaPodArduino = simulated ? new ArduinoSimulator({
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
    }) : new PeaPodArduinoInterface(serialpath);

    const publisher : IPeaPodPublisher = simulated ? new PeaPodLogger() : new PeaPodPubSub();

    // Establish communications
    arduino.start((msg)=>{
        publisher.publish(msg);
    });
}

main().catch((err: Error)=>{
    failSpinner(err.message)
});