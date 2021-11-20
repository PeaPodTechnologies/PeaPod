import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import { initializeApp } from 'firebase/app';
import { getAuth } from 'firebase/auth';

import PeaPodArduinoInterface, { IPeaPodArduino } from './lib/PeaPodArduino';
import PeaPodPubSub, { IoTConfig, IPeaPodPublisher, PeaPodDataBatch } from './lib/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './lib/PeaPodSimulator';

import { checkInternet, sleep, loadDotenv, findSerialPath } from './lib/utils'; //Utilities
import Spinner from './lib/ui'; //UI utils

function main(): Promise<void> {
  
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

  const iotConfig : IoTConfig = {
    cloudregion: 'us-central1',
    projectid: 'cloudponics-bc383',
    registryid: 'CloudPonics',
    jwtexpiryminutes: 1440,
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
  
  let arduino : IPeaPodArduino, publisher : IPeaPodPublisher;
  
  Spinner.info(`Running in ${chalk.bold(simulated ? 'Simulated' : 'Live')} mode with ${chalk.bold(offline ? 'Local Filesystem' : 'Google Cloud')} publishing.`);
  
  return new Promise(async (res, rej)=>{

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
      // Check Internet connection
      Spinner.start(`Checking for ${chalk.blue('Internet')} connection...'`);
      if(!(await checkInternet())){
        // TODO: Offline operation
        rej(new Error(`Could not connect to the {${chalk.blue('Internet')}!`));
      }
      Spinner.succeed(`Connected to the ${chalk.blue('Internet')}!`);
      await sleep(1500);

      // Connect to Firebase
      initializeApp(firebaseConfig);
      
      publisher = new PeaPodPubSub(iotConfig, authConfig);
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
      }).then(({projectid, projectname, run})=>{
        Spinner.info(`${chalk.green('PeaPod')} start - Project ${chalk.bold(projectname ?? projectid)}, Run ${chalk.bold(run)}`);
        batchInterval = setInterval(()=>{
          // Publish entire batch
          try{
            publisher.publish({
              type: 'data',
              metadata: {
                owner: getAuth().currentUser?.uid ?? 'user',
                project: projectid,
                run
              },
              data: batch
            });
          } catch {
            // Publish failed, do not clear cache
            return;
          }
          
          console.log(`[${chalk.magenta('PUBLISH')}] - Batch of ${Object.values(batch).reduce((sum, entry)=>{return sum+entry.batch.length}, 0)} datapoints published.`);
          
          // Reset batch to empty
          batch = {};
        }, batchPublishInterval*1000);
      }).catch(e=>{rej(e)});
    }).catch(e=>{rej(e)});
  });
}

main().catch((err: Error)=>{
  Spinner.fail(err.message);
});