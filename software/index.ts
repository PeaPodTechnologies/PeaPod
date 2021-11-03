import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import firebase from 'firebase';

import PeaPod from './lib/PeaPod'; //MAIN LIB

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

    // Authenticate the user with Firebase
    let auth = new DeviceFlowUI(firebase.app(), authConfig);
    const user = await auth.signIn();

    // INTERNAL SETUP
    const peapod = new PeaPod();

    // IN PRODUCTION: fetch serial port with findSerialPath

    // Establish communications
    peapod.arduino.start((msg)=>{
        peapod.publisher.publish(msg);
    });
}

main().catch((err: Error)=>{
    failSpinner(err.message)
});