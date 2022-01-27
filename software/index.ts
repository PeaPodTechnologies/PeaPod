import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUI, DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import { initializeApp } from 'firebase/app';

import PeaPodArduinoInterface, { IPeaPodArduino } from './lib/PeaPodArduino';
import PeaPodPubSub, { IPeaPodPublisher, PeaPodDataBatch } from './lib/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './lib/PeaPodSimulator';

import { checkInternet, sleep, loadDotenv, findSerialPath } from './lib/utils'; //Utilities
import Spinner from './lib/ui'; //UI utils
import { User } from '@firebase/auth';

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

  // Seconds between data batch publications
  const batchPublishInterval = 30;

  // IN PRODUCTION: fetch serial port with findSerialPath
  const simulated = process.argv.includes('simulate');
  const offline = process.argv.includes('offline');
  
  let user: User | undefined, arduino : IPeaPodArduino, publisher : IPeaPodPublisher;

  if(simulated){
    arduino = new ArduinoSimulator({
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
    Spinner.start('Finding Arduino serial port...');
    const serialpath = await findSerialPath();
    Spinner.succeed('Arduino serial port found: '+serialpath);

    arduino = new PeaPodArduinoInterface(serialpath);
  } 
  if (offline) {
    publisher = new PeaPodLogger();
  } else {
    // Connect to Firebase
    const app = initializeApp(firebaseConfig);
    
    // Check Internet connection
    Spinner.start(`Checking for ${chalk.blue('Internet')} connection...'`);
    if(!(await checkInternet())){
      // TODO: Offline operation
      throw new Error(`Could not connect to the {${chalk.blue('Internet')}!`);
    }
    Spinner.succeed(`Connected to the ${chalk.blue('Internet')}!`);
    await sleep(1500);

    // Authenticate the user with Firebase
    let auth = new DeviceFlowUI(app, authConfig);
    user = await auth.signIn();

    // TODO: check that it matches deviceInfo.json
    
    publisher = new PeaPodPubSub({
      cloudregion: 'us-central1',
      projectid: 'cloudponics-bc383',
      registryid: 'CloudPonics',
      jwtexpiryminutes: 1440,
    });
  }

  let batch: PeaPodDataBatch = {};
  let batchInterval: NodeJS.Timer;
  
  // Initialize Arduino communications interface
  arduino.start((msg)=>{
    if(msg.type == 'data') {
      // Initialize batch array
      if(batch[msg.data.label] === undefined) batch[msg.data.label] = {batch: []};
      batch[msg.data.label].batch.push({
        timestamp: Date.now(),
        value: msg.data.value
      })
      
      // TODO: Plan, act
    }
    // TODO: publish other message types
  }).then(()=>{
    // Arduino is initialized, initialize publisher interfae
    publisher.start(config=>{
      console.log("[CONFIG] - "+config);
      // TODO: Respond to config (update instructions)
    }, command=>{
      console.log("[COMMAND] - "+command);
      // TODO: Respond to commands (immediate actions)
    }).then(()=>{
      batchInterval = setInterval(()=>{
        // Publish entire batch
        publisher.publish({
          type: 'data',
          metadata: {
            owner: user?.uid ?? 'user',
            project: 'testproject',
            run: 'testrun'
          },
          data: batch
        });
        let sum = 0;
        for(const arr of Object.values(batch)){
          sum += arr.batch.length;
        }

        // Reset batch to empty
        batch = {};

        console.log(`[PUBLISH] - Batch of ${sum} datapoints published.`);
      }, batchPublishInterval*1000)
    });
  });
}

main().catch((err: Error)=>{
  Spinner.fail(err.message);
});