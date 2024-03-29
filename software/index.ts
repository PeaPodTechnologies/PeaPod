#!/usr/bin/env node
import chalk from 'chalk'; //Colored CLI text
import { DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow'; //Firebase Auth via OAuth2 'Device Flow'
import { initializeApp, getApps } from 'firebase/app';
import { getAuth } from 'firebase/auth';
import PiCamera from 'pi-camera';

import PeaPodArduinoInterface, { IPeaPodArduino } from './lib/PeaPodArduino';
import PeaPodPubSub, { IoTConfig, IPeaPodPublisher, PeaPodDataBatch } from './lib/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './lib/PeaPodSimulator';

import { checkInternet, sleep, loadDotenv, findSerialPath } from './lib/utils'; //Utilities
import Spinner from './lib/ui'; //UI utils
import PeaPodCamera, { IPeaPodCamera } from './lib/PeaPodCamera';

// Global State

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

  const RTCServers = {
    iceServers: [
      {
        urls: ['stun:stun1.l.google.com:19302', 'stun:stun2.l.google.com:19302'],
      },
    ],
    iceCandidatePoolSize: 10,
  };
  
  // Seconds between data batch publications
  const batchPublishInterval = 5;
  
  // IN PRODUCTION: fetch serial port with findSerialPath
  const simulated = process.argv.includes('simulate');
  const offline = process.argv.includes('offline');
  
  let arduino : IPeaPodArduino, publisher : IPeaPodPublisher, camera: IPeaPodCamera | null;
  
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

      camera = null;
    } else {
      let serialpath;
      if(process.env.SERIALPORT){
        Spinner.info('Using serial port: '+process.env.SERIALPORT);
        serialpath = process.env.SERIALPORT;
      } else {
        Spinner.start('Finding Arduino serial port...');
        serialpath = await findSerialPath();
        Spinner.succeed('Arduino serial port found: '+serialpath);
      }
      
      arduino = new PeaPodArduinoInterface(serialpath);
      camera = new PeaPodCamera;
    } 
    if (offline) {
      publisher = new PeaPodLogger();
    } else {
      // Check Internet connection
      Spinner.start(`Checking for ${chalk.blue('Internet')} connection...'`);
      if(!(await checkInternet())){
        Spinner.fail(`Could not connect to the ${chalk.blue('Internet')}! Running in ${chalk.bold('Offline')} mode.`);
        publisher = new PeaPodLogger();
      } else {
        Spinner.succeed(`Connected to the ${chalk.blue('Internet')}!`);

        // Connect to Firebase
        initializeApp(firebaseConfig);
        
        publisher = new PeaPodPubSub(iotConfig, authConfig);
      }
      await sleep(1500);
    }
    
    let batch: PeaPodDataBatch = {};
    let batchInterval: NodeJS.Timer;

    // Initialize publisher (ready for Arduino), fetch initial instructions
    publisher.start(config=>{
      console.log("[CONFIG] - "+config);
      // TODO: Respond to config (update instructions)
    }, command=>{
      console.log("[COMMAND] - "+command);
      // TODO: Respond to commands (immediate actions)
      if(command.type == 'livestreamoffer'){

        // Assumes webcam hardware has been initialized

        // 1. Create answer from command data
        // 2. Call cloud function with answer
        // 3. Enable video feed

        // camera?.stream().pipe();

        /**
         * {
         *  type: 'livestreamoffer',
         *  data: {...}
         * }
         */

      }
    }).then(({projectid, projectname, run})=>{
      // Get program

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
        } else {
          Spinner.info(`[${chalk.blueBright('ARDUINO')} | ${msg.type.toUpperCase()}] - ${JSON.stringify(msg.data)}`)
        }
        // TODO: publish other message types
      }).then(()=>{
        Spinner.info(`${chalk.green('PeaPod')} start - Project ${chalk.bold(projectname ?? projectid)}, Run ${chalk.bold(run)}`);
        batchInterval = setInterval(()=>{
          // Publish entire batch
          try{
            publisher.publish({
              type: 'data',
              metadata: {
                owner: (getAuth().currentUser?.uid ?? 'user'),
                project: projectid,
                run
              },
              data: batch
            });
          } catch {
            Spinner.fail('Batch publish failed, will retry...');
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
  process.exit(1);
});