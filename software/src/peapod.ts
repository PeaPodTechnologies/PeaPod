import { Screen, Spinner } from './ui';
import chalk from 'chalk';

import { initializeApp, getApps } from 'firebase/app';
import { getAuth } from 'firebase/auth';

import { loadPeaPodEnv, loadAuthEnv, loadFirebaseEnv, loadIoTEnv } from './env';
import MicroController, { ControllerInstructions, Controller, CONTROLLER_REVISION } from './controller';
import OnlinePublisher, { DataBatch, DataSet, Publisher, OfflinePublisher, PublishingMode } from './publisher';
import { sleep } from './utils';

// CONSTANTS

/**
 * Seconds between data publications.
 */
const BATCH_PUBLISH_INTERVAL = 5;

/**
 * Milliseconds between target refresh.
 */
const REFRESH_INTERVAL = 1000;

/**
 * Period of the idle loop.
 */
const IDLE_PERIOD = 5000;

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
 * Currently loaded schedule.
 */
let schedule: EnvironmentSchedule;

/** 
 * Current environment schedule targets.
 */
let targets: EnvironmentTargets = {};
 
/** 
 * Latest controller instruction set.
 */
let instructions: ControllerInstructions = {};

/**
 * 
 */
let data: DataSet = {};

let controlSystems: {[key: string]: ControlSystem}

// DECLARATIONS

export type EnvironmentSchedule = {
  id: string,
  name: string,
  revision: number,
  parameters: {
    [key: string]: EnvironmentTargetPhase[]
  }
};

type EnvironmentTargets = {
  [key: string]: number
};

type EnvironmentTargetPhase = {
  type: 'piecewise',
  end: number,
  values: {
    value: number,
    timestamp: number,
  }[]
} | {
  type: 'periodic',
  period: number,
  end: number,
  values: {
    value: number,
    duration: number,
  }[]
};

/**
 * Main driver class.
 */
export default class PeaPod {

  controller: Controller;
  publisher: Publisher;
  private startTime: number = 0;

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
      const idleInstructions = {
        "led_blue": Math.sin(angle                  ) / 2 + 0.5,
        "led_cool": Math.sin(angle + 2 * Math.PI / 5) / 2 + 0.5,
        "led_warm": Math.sin(angle + 4 * Math.PI / 5) / 2 + 0.5,
        "led_red":  Math.sin(angle + 6 * Math.PI / 5) / 2 + 0.5,
        "led_far":  Math.sin(angle + 8 * Math.PI / 5) / 2 + 0.5,
      };
      this.controller.write(idleInstructions);

      angle += (2 * Math.PI) / (IDLE_PERIOD / REFRESH_INTERVAL);
      angle -= (angle > 2 * Math.PI) ? (2 * Math.PI) : 0;
    }, REFRESH_INTERVAL);
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
      Spinner.log(`[${ chalk.yellow('PUBLISHER') } | SCHEDULE] - ${ config }`);
      switch (config.type) {
        case 'schedule':
          Spinner.log(`[${ chalk.yellow('PUBLISHER') } | SCHEDULE] - ${ JSON.stringify(config.data.name) }`);

          if (config.data.revision != CONTROLLER_REVISION) {
            Spinner.fail(`Failed to load schedule '${config.data.name}', software version mismatch (Expected ${CONTROLLER_REVISION}, got ${config.data.revision}).`);
            break;
          }

          // Save new schedule
          schedule = config.data;

          break;
        default:
          Spinner.log(`[${ chalk.yellow('PUBLISHER') } | CONFIG] - ${ JSON.stringify(config.data) }`);
      }
    }, command => {
      switch (command.type) {
        case 'instructions':
          Spinner.log(`[${ chalk.yellow('PUBLISHER') } | INSTRUCTIONS] - ${ JSON.stringify(command.data) }`);
          this.controller.write(command.data);
          break;
        default:
          Spinner.log(`[${ chalk.yellow('PUBLISHER') } | COMMAND] - ${ JSON.stringify(command) }`);
      }
    });

    // START
    Spinner.log(`${ chalk.green('PeaPod') } start - Project ${ chalk.bold(projectname ?? projectid) }, Run ${ chalk.bold(run) }`);

    this.startTime = Date.now();

    // Start schedule phase 0
    
    for (const parameter of Object.keys(schedule.parameters)) {
      this.startPhase(schedule, parameter);
    }

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

    refreshInterval = set
  }

  /**
   * Start the schedule for a single parameter. Runs recursively until all phases are executed.
   * @param schedule Environment schedule
   * @param parameter Parameter to start
   * @param n Phase to start at
   */
  private startPhase(schedule: EnvironmentSchedule, parameter: string, n: number = 0): Promise<void> {
    return new Promise(res => {
      // Break once we've reached the end
      if(n >= schedule.parameters[parameter].length) {
        res();
        return;
      }

      const phase = schedule.parameters[parameter][n];

      let timeouts: NodeJS.Timeout[] = [];
      let intervals: NodeJS.Timer[] = [];

      // Extract all targets
      switch (phase.type) {
        case 'piecewise':
          // Trigger each piecewise expression at its timestamp
          for (const value of phase.values) {
            let t = setTimeout(() => {
              // Assign value
              targets[parameter] = value.value;
              // Remove this timeout if/when it self-clears
              timeouts.splice(timeouts.findIndex(v => v == t), 1);
            }, value.timestamp - (Date.now() - this.startTime));
            timeouts.push(t);
          }
          break;
        case 'periodic':
          // Retrigger each value at a fixed period, with duration offset
          let durationSum = 0;
          phase.values.forEach((value, i) => {
            // Await duration offset
            let t = setTimeout(() => {
              // Start the interval
              intervals.push(setInterval(() => {
                // Assign value
                targets[parameter] = value.value;
              }, phase.period));
              // Remove this timeout if/when it self-clears
              timeouts.splice(timeouts.findIndex(v => v == t), 1);
              // First expression? Trigger now.
              // Else: Timeout for the sum of all previous expressions' durations, minus the elapsed time in this phase
            }, i > 0 ? durationSum - (Date.now() - (n > 0 ? schedule.parameters[parameter][n-1].end : this.startTime)) : 0);
            durationSum += value.duration;
            timeouts.push(t);
          });
          break;
      }

      // End this phase once Date.now() - startTime > end, and start the next phase, waiting for its completion
      setTimeout(() => {
        // Clear all outstanding timeouts and intervals
        for (const interval of intervals) {
          clearInterval(interval);
        }
        for (const timeout of timeouts) {
          clearTimeout(timeout);
        }
        this.startPhase(schedule, parameter, n+1).then(() => { res(); });
      }, phase.end - (Date.now() - this.startTime));
    });
  }
}