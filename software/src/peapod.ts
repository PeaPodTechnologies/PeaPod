import { Screen, Spinner } from './ui';
import chalk from 'chalk';

import { initializeApp, getApps } from 'firebase/app';
import { getAuth } from 'firebase/auth';

import { loadPeaPodEnv, loadAuthEnv, loadFirebaseEnv, loadIoTEnv } from './env';
import MicroController, { ControllerInstructions, Controller } from './controller';
import OnlinePublisher, { DataBatch, DataSet, Publisher, OfflinePublisher, PublishingMode } from './publisher';
import { sleep } from './utils';

// CONSTANTS

/**
 * Seconds between data publications.
 */
const BATCH_PUBLISH_INTERVAL = 5;

/**
 * Milliseconds between refresh during idle.
 */
const IDLE_INTERVAL = 100;

// GLOBAL VARIABLES
 
/**
 * Batch of data to be published.
 */
let batch: DataBatch = { };
 
/**
 * JS interval to trigger batch publishing.
 */
let batchInterval: NodeJS.Timer;
 
/** 
 * Latest controller instruction set.
 */
let instructions: ControllerInstructions = {};

/**
 * 
 */
let data: DataSet = {};

/**
 * Main driver class.
 */
export default class PeaPod {

  controller: Controller;
  publisher: Publisher;

  constructor(pm: PublishingMode) {
    const ENV_PEAPOD = loadPeaPodEnv();

    // CREATE CONTROLLER
    this.controller = new MicroController(ENV_PEAPOD.serialport);

    // CREATE PUBLISHER
    Spinner.info(`Running ${ chalk.green('PeaPod') } in ${ pm } mode.`);
    switch (pm) {
      
      case PublishingMode.ONLINE:
        initializeApp(loadFirebaseEnv());
        if (getApps().length === 0) {
          throw new Error("Failed to initialize Firebase app! Check .env fields.");
        } else {
          Spinner.succeed(`Initialized Firebase app!`);
        }
        this.publisher = new OnlinePublisher(loadIoTEnv(), loadAuthEnv());
        break;

      case PublishingMode.OFFLINE:
      default:
        this.publisher = new OfflinePublisher();
        break;
    }
  }

  async idle(): Promise<NodeJS.Timer> {
    await this.controller.start(msg => {
      switch (msg.type) {
        case "data":
          data[msg.data.label] = msg.data.value;
          Screen.setData(data);
          Screen.render();
          break;
        case "revision":
          this.controller.write(instructions);
          break;
        default:
          // TODO: Console box?
          Spinner.log(`[${ chalk.blueBright('CONTROLLER') } | ${ msg.type.toUpperCase() }] - ${ JSON.stringify(msg.data) }`);
      }
    });

    // Angle to "wheel" the lights
    let angle = 0;

    const idleInterval = setInterval(() => {
      instructions = {
        "led_blue": Math.sin(angle)/2+0.5,
        "led_cool": Math.sin(angle + 2*Math.PI/5)/2+0.5,
        "led_warm": Math.sin(angle + 4*Math.PI/5)/2+0.5,
        "led_red": Math.sin(angle + 6*Math.PI/5)/2+0.5,
        "led_far": Math.sin(angle + 8*Math.PI/5)/2+0.5,
      };
      this.controller.write(instructions);

      // One revolution every 5 seconds
      angle += 2*Math.PI/(5000/IDLE_INTERVAL);
      angle -= angle > 2*Math.PI ? 2*Math.PI : 0;
    }, IDLE_INTERVAL);
    return idleInterval;
  }

  async start(): Promise<void> {
    // INITIALIZE CONTROLLER
    await this.controller.start(msg => {
      // Handle different message types
      // TODO: Handle error, etc. types
      switch (msg.type) {
        case "data":
          // Initialize batch array
          if (batch[msg.data.label] === undefined) batch[msg.data.label] = [];
          // Accumulate data into batches
          batch[msg.data.label].push({
            timestamp: Date.now(),
            value: msg.data.value,
          });
          break;
        case "revision":
          this.controller.write(instructions);
          break;
        default:
          Spinner.log(`[${ chalk.blueBright('CONTROLLER') } | ${ msg.type.toUpperCase() }] - ${ JSON.stringify(msg.data) }`);
      }
    });

    // Wait for all POST messages
    await sleep(1000);

    // INITIALIZE PUBLISHER
    let { projectid, projectname, run } = await this.publisher.start(config => {
      // Hot-swap programs
      // TODO: This
      Spinner.log(`[${ chalk.yellow('PUBLISHER') } | CONFIG] - ${ config }`);

    }, command => {
      switch (command.type) {
        case 'instructions':
          this.controller.write(command.data);
        default:
          Spinner.log(`[${ chalk.yellow('PUBLISHER') } | COMMAND] - ${ JSON.stringify(command) }`);
      }
    });
    // Todo: get instructions

    // START
    Spinner.log(`${ chalk.green('PeaPod') } start - Project ${ chalk.bold(projectname ?? projectid) }, Run ${ chalk.bold(run) }`);

    // Reset
    batch = { };

    batchInterval = setInterval(() => {
      // Publish entire batch
      try {
        this.publisher.publish({
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
      
      Spinner.log(`[${ chalk.magenta('PUBLISH') }] - Batch of ${ Object.values(batch).reduce((sum, entry) => { return sum+entry.length }, 0) } datapoints published.`);
      
      // Reset batch to empty
      batch = { };
    }, BATCH_PUBLISH_INTERVAL*1000);
  }
}