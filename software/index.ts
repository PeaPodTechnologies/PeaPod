import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import { initializeApp } from 'firebase/app';

import PeaPodArduinoInterface, { IPeaPodArduino } from './lib/PeaPodArduino';
import PeaPodPubSub, { IPeaPodPublisher } from './lib/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './lib/PeaPodSimulator';

import { checkInternet, sleep, loadDotenv, findSerialPath } from './lib/utils'; //Utilities
import Spinner from './lib/ui'; //UI utils

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
  Spinner.start(`Checking for ${chalk.blue('Internet')} connection...'`)
  if(!(await checkInternet())){
    // TODO: Offline operation
    throw new Error(`Could not connect to the {${chalk.blue('Internet')}!`);
  }
  Spinner.succeed(`Connected to the ${chalk.blue('Internet')}!`);
  await sleep(1500);
  
  // ONLINE OPERATION
  
  // Connect to Firebase
  const app = initializeApp(firebaseConfig);
  
  // firebase.functions().useEmulator('localhost', 5001);
  // firebase.auth().useEmulator('http://localhost:9099');
  // firebase.firestore().useEmulator('localhost', 8080);
  
  // Authenticate the user with Firebase
  let auth = new DeviceFlowUI(app, authConfig);
  const user = await auth.signIn();
  
  // IN PRODUCTION: fetch serial port with findSerialPath
  const simulated = process.argv.includes('simulate');
  
  let serialpath: string = '';
  
  if(!simulated){
    Spinner.start('Finding Arduino serial port...');
    serialpath = await findSerialPath();
    Spinner.succeed('Arduino serial port found: '+serialpath);
  }
  
  // ARDUINO INTERFACE
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
  
  // PUBLISHER INTERFACE
  const publisher : IPeaPodPublisher = simulated ? new PeaPodLogger() : new PeaPodPubSub({
    cloudregion: 'us-central1',
    projectid: 'cloudponics-bc383',
    registryid: 'CloudPonics',
    jwtexpiryminutes: 1440,
  });
  
  // Establish communications
  console.log('===== START =====');
  arduino.start((msg)=>{
    if(msg.type == 'revision'){
      throw new Error('Invalid PeaPod message type: revision')
    } else if(msg.type == 'data') {
      publisher.publish({
        type: 'data',
        metadata: {
          owner: user.uid,
          project: 'testproject',
          run: 'testrun'
        },
        data: Object.fromEntries([[msg.data.label, {batch: [{timestamp: Date.now(), value: msg.data.value}]}]])
      });
    }
  });
}

main().catch((err: Error)=>{
  Spinner.fail(err.message)
});