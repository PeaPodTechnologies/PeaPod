#!/usr/bin/env node
import chalk from 'chalk';
import { DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow';
import { initializeApp, getApps } from 'firebase/app';
import { getAuth } from 'firebase/auth';

import PeaPodArduinoInterface, { ArduinoInstructions, IPeaPodArduino } from './src/PeaPodArduino';
import PeaPodPubSub, { IoTConfig, IPeaPodPublisher, PeaPodDataBatch } from './src/PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './src/PeaPodSimulator';
import { loadDotenv, checkInternet, sleep, findSerialPath } from './src/utils';
import Spinner from './src/ui';

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
  cloudregion: process.env.IOT_CLOUDREGION,
  projectid: process.env.FIREBASE_PROJECTID,
  registryid: process.env.IOT_REGISTRY,
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
const batchPublishInterval = 5;
const simulated = process.argv.includes('simulate');
const offline = process.argv.includes('offline');

function main(): Promise<void> {
  
  let arduino : IPeaPodArduino, publisher : IPeaPodPublisher;
  
  Spinner.log(`Running in ${ chalk.bold(simulated ? 'Simulated' : 'Live') } mode with ${ chalk.bold(offline ? 'Local Filesystem' : 'Google Cloud') } publishing.`);
  
  return new Promise(async (res, rej) => {

    if (simulated) {
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
      let serialpath;
      if (process.env.SERIALPORT) {
        Spinner.log('Using serial port: '+process.env.SERIALPORT);
        serialpath = process.env.SERIALPORT;
      } else {
        Spinner.start('Finding Arduino serial port...');
        serialpath = await findSerialPath();
        Spinner.succeed('Arduino serial port found: '+serialpath);
      }
      
      arduino = new PeaPodArduinoInterface(serialpath);
      // camera = new PeaPodCamera;
    } 
    if (offline) {
      publisher = new PeaPodLogger();
    } else {
      // Check Internet connection
      Spinner.start(`Checking for ${ chalk.blue('Internet') } connection...'`);
      if (!(await checkInternet())) {
        Spinner.fail(`Could not connect to the ${ chalk.blue('Internet') }! Running in ${ chalk.bold('Offline') } mode.`);
        publisher = new PeaPodLogger();
      } else {
        Spinner.succeed(`Connected to the ${ chalk.blue('Internet') }!`);

        // Connect to Firebase
        initializeApp(firebaseConfig);
        
        publisher = new PeaPodPubSub(iotConfig, authConfig);
      }
      await sleep(1500);
    }
    
    let batch: PeaPodDataBatch = { }, batchInterval: NodeJS.Timer, instructions: ArduinoInstructions = {};

    // Initialize Arduino communications interface first
    arduino.start((msg) => {
      switch (msg.type) {
        case "data":
          // Initialize batch array
          if (batch[msg.data.label] === undefined) batch[msg.data.label] = [];
          // Accumulate data into batches
          batch[msg.data.label].push({
            timestamp: Date.now(),
            value: msg.data.value
          });
          break;
        case "revision":
          arduino.write(instructions);
          break;
        default:
          Spinner.log(`[${ chalk.blueBright('ARDUINO') } | ${ msg.type.toUpperCase() }] - ${ JSON.stringify(msg.data) }`)
      }
      // TODO: publish other message types
    }).then(() => {
      // Initialize publisher
      publisher.start(config => {
        // Hot-swap programs
        Spinner.log("[CONFIG] - "+config);

        // TODO: Handle this
      }, command => {
        Spinner.log("[COMMAND] - "+command);
        switch (command.type) {
          case 'instructions':
            arduino.write(command.data);
            break;
        }
      }).then(({projectid, projectname, run}) => {
        // Todo: get instructions

        // START
        Spinner.log(`${ chalk.green('PeaPod') } start - Project ${ chalk.bold(projectname ?? projectid) }, Run ${ chalk.bold(run) }`);

        // Reset
        batch = { };

        batchInterval = setInterval(() => {
          // Publish entire batch
          try {
            publisher.publish({
              type: 'data',
              metadata: {
                owner: (getApps().length ? (getAuth().currentUser?.uid ?? 'user') : 'user'),
                project: projectid,
                run
              },
              data: batch
            });
          } catch {
            Spinner.fail('Batch publish failed, will retry...');
            return;
          }
          
          Spinner.log(`[${chalk.magenta('PUBLISH')}] - Batch of ${ Object.values(batch).reduce((sum, entry) => { return sum+entry.length }, 0) } datapoints published.`);
          
          // Reset batch to empty
          batch = { };
        }, batchPublishInterval*1000);
      }).catch(e => { rej(e) });
    }).catch(e => { rej(e) });
  });
}

main().catch((err: Error) => {
  Spinner.fail(err.message);
  process.exit(1);
});