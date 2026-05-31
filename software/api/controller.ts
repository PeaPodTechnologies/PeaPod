import chalk from 'chalk';
import { SerialPort, ReadlineParser } from 'serialport';
import { ControllerTXError, DebugJsonSerialportError } from './errors';
import { DebugJsonConsole as ui } from './ui';
import { DebugJsonMessage, DebugJsonMessageTypes } from './types';
import { updateMicrocontroller } from './utils';
import { config } from 'process';

// import { Gpio } from 'onoff';

// CONSTANTS

/**
 * Baud rate for serial communication. Must match that found in microcontroller code.
 */
const BAUDRATE = 115200;

/**
 * Microcontroller software revision matching this software.
 */
export const CONTROLLER_REVISION = 0; // Just zero for now

/**
 * Seconds to wait between messages before timing out.
 */
const SERIAL_TIMEOUT_SECONDS = 5;

const RESET_INTERVAL = 60000;
const BATCH_INTERVAL = 100; 

/**
 * GPIO pin attached to the reset grounding circuit
 */
// const RESET_PIN = 26;

export function findSerialPort(path?: string, matchExact: boolean = false): Promise<string[]> {
  return SerialPort.list().then(ports => {
    let exact = false;
    return ports.reduce((acc, port) => {
      // console.log(JSON.stringify(port, null, 2));
      if(port && port['path']) {
        if(!path) {
          acc.push(port.path as string);
          return acc;
        }
        if(matchExact && (port['path'] as string).toLowerCase() === path.toLowerCase()) {
          exact = true;
          acc = [port.path as string];
        } else if(!exact && (port['path'] as string).toLowerCase().includes(path.toLowerCase())) {
          acc.push(port.path as string);
        }
      }
      return acc;
    }, [] as string[]);
  });
}

export function getSerialPorts(): Promise<string[]> {
  return SerialPort.list().then(ports => {
    return ports.map(port => port.path as string);
  });
}

// TYPES

/**
 * Base type for any controller.
 */
export type Controller = {
  /**
   * Establish communications with the Controller.
   * @param onMessage Pipe all received messages.
   * @throws If received message is invalid (JSON parsing fails).
   */
  // start(onMessage: (msg: ControllerMessage) => void): Promise<void>;
  start(onMessages: (messages: DebugJsonMessage[]) => void): Promise<void>;

  /**
   * Write instructions to the Controller.
   * @param instructions Instruction set.
   */
  // write(instructions: ControllerInstructions): void;
  write(instructions: DebugJsonMessage): void;

  /**
   * Halt communications with the Controller.
   */
  stop(): Promise<void>;
};

/**
 * Messages FROM the controller
 */
// export type ControllerMessage =
// 	| {
// 			type: 'info' | 'debug' | 'error';
// 			data: string;
// 	  }
// 	| {
// 			type: 'data';
// 			data: {
// 				label: string;
// 				value: number;
// 			};
// 	  }
// 	| {
// 			type: 'revision';
// 			data: number;
// 	  };

/**
 * Messages TO the controller
 */
// export type ControllerInstructions = {
// 	[key: string]: number;
// };

/**
 * Simulated controller parameters
 */
export type SimulatorConfig = {
  [key: string]: {
    min: number;
    max: number;
    interval: number;
  };
};

// CLASSES

/**
 * Interface between this computer and the microcontroller.
 */
export class MicroController implements Controller {
  serial: SerialPort;
  parser: ReadlineParser;
  #timedout: boolean = false;
  #count: number = 0;
  #started: boolean = false;
  #batch: DebugJsonMessage[] = [];
  private timeout?: NodeJS.Timeout;
  private resetInterval?: NodeJS.Timeout;
  #lastBatch = Date.now();
  // private resetpin: Gpio;

  constructor(readonly serialport: string, readonly passRevision: boolean = true) {
    // Reset pin GPIO interface
    // this.resetpin = new Gpio(RESET_PIN, 'out');

    // Create the serial port interface
    this.serial = new SerialPort({
      path: serialport,
      baudRate: BAUDRATE,
      autoOpen: false,
    });

    this.serial.on('error', async (err) => {
      // ui.fail(`CONTROLLER SERIAL: ${err}`);
      throw new DebugJsonSerialportError(`${err.name} - ${err.message}`);
    });

    // Create the newline parser
    this.parser = this.serial.pipe(
      new ReadlineParser({
        delimiter: '\n',
        includeDelimiter: false,
      })
    );
  }

  // Starts serial (newline parser) and resolves when RX revision is correct
  async start(onMessage: (messages: DebugJsonMessage[]) => void): Promise<void> {
    this.pauseTimeout(true); // Don't want it interrupting the start sequence
    // Reset listeners
    this.parser.removeAllListeners('data');

    // resolve only on valid comms AND revision check
      
    // Reset the microcontroller (opens the serial port)
    await this.reset();

    // Reset the serial timeout interval
    this.resetTimeout();

    // Set up the reset interval
    this.resetInterval = setInterval(() => {
      ui.info('CONTROLLER RESET INTERVAL');
      this.reset();
    }, RESET_INTERVAL);

    this.parser.on('error', async (_err) => {
      await this.reset().catch(() => {throw new DebugJsonSerialportError(`${_err.name} - ${_err.message}`)});
    });

    // Set up the data listener
    ui.start('CONTROLLER REVISION...');
    await new Promise<void>((resolve) => {
      this.parser.on('data', async (msgtxt) => {
        this.resetTimeout();
        this.#count++;
        
        // Attempt to parse the raw text as a valid JSON object
        const msg: DebugJsonMessage = JSON.parse(msgtxt);

        // Microcontroller-specific pre-handling
        switch (msg.type) {
        case 'revision':
          // Software update
          if (msg.data && Object.keys(msg.data).includes('revision') && msg.data['revision'] === CONTROLLER_REVISION) {
            if(ui.spinning()) {
              ui.succeed(
                `CONTROLLER REVISION PASS! ${msg.data['revision']} === ${CONTROLLER_REVISION}`
              );
              this.#started = true;
              resolve();
            }
            if(this.passRevision) {
              onMessage([msg]);
            }
            return;
          } else {
            ui.fail(
              `CONTROLLER REVISION FAIL: ${msg.data['revision'] ?? 'NULL'} !== ${CONTROLLER_REVISION}`
            );
            // Attempt to update the microcontroller, and then restart
            await this.stop();
            // ui.start('CONTROLLER FLASH...');
            // await updateMicrocontroller();
            // ui.succeed('CONTROLLER FLASH PASS!');
          }
          break;
        default:
          if(this.#started) {
            this.#batch.push(msg);
            if(Date.now() - this.#lastBatch >= BATCH_INTERVAL) {
              onMessage(this.#batch);
              this.#batch = [];
              this.#lastBatch = Date.now();
            }
          }
          break;
        }
      });
    });
  }

  /**
   * Clear the serial timeout.
   */
  private pauseTimeout(force: boolean = false): void {
    if (this.timeout && (this.#timedout || force)) {clearTimeout(this.timeout);}
  }

  /**
   * Refresh (or start) the serial timeout.
   */
  private resetTimeout(timeoutSeconds: number = SERIAL_TIMEOUT_SECONDS): void {
    this.pauseTimeout(true);
    this.timeout = setTimeout(() => {
      ui.fail(
        `CONTROLLER TIMEOUT: ${timeoutSeconds}s`
      );
      this.#timedout = true;
      // this.reset().catch((err) => {if(cb) {cb(err);}});
      this.reset();
    }, timeoutSeconds * 1000);
  }

  write(msg: DebugJsonMessage): void {
    ui.info(`[${chalk.yellow('WRITE')}] - ${JSON.stringify(msg)}`);
    this.serial.write(JSON.stringify(msg) + '\n', undefined, (err) => {
      if (err) throw new ControllerTXError(JSON.stringify(msg));
    });
  }

  stop(): Promise<void> {
    this.pauseTimeout(true);
    if (!this.serial.isOpen) return Promise.resolve();
    return new Promise<void>((resolve, reject) => {
      this.serial.close((err) => {
        if (err) {
          reject(new DebugJsonSerialportError(`${err.name} - ${err.message}`));
        } else {
          resolve();
        }
      });
    });
    // Stop listening for data
    // this.parser.removeAllListeners('data');
  }

  /**
   * Resets the microcontroller by closing and re-opening serial.
   */
  private reset(): Promise<void> {
    // Stop and reset
    return this.stop().then(() => {
      // this.resetpin.writeSync(1);

      this.#count = 0;

      // this.#started = false;

      // Wait, then stop resetting
      // await new Promise<void>((r) => setTimeout(r, 1000));
      // this.resetpin.writeSync(0);

      // (Re-)open serial
      return new Promise<void>((resolve, reject) => {
        ui.start('CONTROLLER RESET...');
        this.serial.open((err) => {
          if (err) {
            ui.fail('CONTROLLER RESET FAIL!');
            reject(new DebugJsonSerialportError(`${err.name} - ${err.message}`));
          } else {
            ui.succeed('CONTROLLER RESET PASS!');
            this.resetTimeout();
            resolve();
          }
        });
      });
    });
  }
}

/**
 * A simulated controller for generating random data.
 */
export class SimulatedController implements Controller {
  private intervals: NodeJS.Timeout[] = [];
  private readonly startDate: number = Date.now();
  private output?: (msg: DebugJsonMessage[]) => void;

  constructor(readonly parameters: SimulatorConfig) {}

  async start(onMessage: (msg: DebugJsonMessage[]) => void): Promise<void> {
    this.output = onMessage;
    for (const label of Object.keys(this.parameters)) {
      this.intervals.push(
        setInterval(() => {
          onMessage(
            [this.generateData(
              label,
              this.parameters[label].min,
              this.parameters[label].max
            )]
          );
        }, this.parameters[label].interval)
      );
    }
  }
  write(instructions: DebugJsonMessage): void {
    switch(instructions.type) {
    case 'command':
      if(instructions.data && Object.keys(instructions.data).includes('rebuild')) {
        ui.info('SIMULATED CONTROLLER REBUILD TREE');
        if(this.output) {
          this.output([
            this.generateTree()
          ]);
        }
        break;
      }
    case 'config':
      if(instructions.data && Object.keys(instructions.data).includes('list')) {
        ui.info('SIMULATED CONTROLLER LIST STATES');
        if(this.output) {
          this.output([
            this.parameters ? {
              type: 'config' as DebugJsonMessageTypes,
              timestamp: Date.now() - this.startDate,
              data: {'onoff': true, ...Object.keys(this.parameters).reduce<{[key: string]: number | boolean}>((acc, key) => {
                const max = this.parameters[key].max;
                const min = this.parameters[key].min;
                acc[key] = Math.random() * (max - min) + min;
                return acc;
              }, {})},
            } : {
              type: 'config' as DebugJsonMessageTypes,
              timestamp: Date.now() - this.startDate,
              data: {},
            }
          ]);
        }
        break;
      }
    default:
      ui.info(`SIMULATED CONTROLLER WRITE: ${JSON.stringify(instructions)}`);
      break;
    }
  }
  stop(): Promise<void> {
    for (const interval of this.intervals) {
      clearInterval(interval);
    }
    return Promise.resolve();
  }

  /**
   * Generate a single data point
   * @param label Dataset label
   * @param min Minimum value
   * @param max Maximum value
   */
  private generateData(
    label: string,
    min: number,
    max: number
  ): DebugJsonMessage {
    const d = (Math.random() * (max - min) + min);
    return {
      type: 'event',
      timestamp: Date.now() - this.startDate,
      data: {
        [label]: d,
      },
    };
  }

  private generateTree() {
    // modules: {devices: {id: string, fqa: number}[]}[]
    return {type: 'tree' as DebugJsonMessageTypes, timestamp: Date.now() - this.startDate, data: Object.keys(this.parameters).reduce<{[key: string]: number[]}[]>((acc, label, idx) => { acc[0][label] = [idx]; return acc; }, [{}])};
  }
}
