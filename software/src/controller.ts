import chalk from 'chalk';
import { ReadlineParser, SerialPort } from 'serialport';
import { ControllerInstructionsError } from './errors';
import * as Spinner from './ui';
import { gpioUnexport, gpioWrite, sleep, updateMicrocontroller } from './utils';

// CONSTANTS

/** 
 * Baud rate for serial communication. Must match that found in microcontroller code.
 */
const BAUDRATE = 115200;

/**
 * Microcontroller software revision matching this software.
 */
const CONTROLLER_REVISION = 0;

/**
 * Seconds to wait between messages before timing out.
 */
const SERIAL_TIMEOUT_SECONDS = 10;

/** 
 * Linux `sysfs` GPIO pin attached to the active-low reset pin on the microcontroller.
 */
const RESET_PIN = 5;

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
  start(onMessage: (msg : ControllerMessage) => void): Promise<void>,

  /**
   * Write instructions to the Controller.
   * @param instructions Instruction set.
   */
  write(instructions: ControllerInstructions): void,

  /**
   * Halt communications with the Controller.
   */
  stop(): void,
};

/**
 * Messages FROM the controller
 */
export type ControllerMessage = {
  type: 'info' | 'debug' | 'error',
  data: string,
} | {
  type: 'data',
  data: {
    label: string,
    value: number,
  },
} | {
  type: 'revision',
  data: number,
};

/**
 * Messages TO the controller
 */
export type ControllerInstructions = {
  [key: string]: number,
};

/**
 * Simulated controller parameters
 */
export type SimulatorConfig = {
  [key: string]: {
    min: number,
    max: number,
    interval: number,
  },
};

// CLASSES

/**
 * Interface between this computer and the microcontroller.
 */
export default class MicroController implements Controller {

  serial: SerialPort;
  parser: ReadlineParser;
  timeout?: NodeJS.Timeout;

  constructor(readonly serialport: string) {

    // Create the serial port interface
    this.serial = new SerialPort({
      path: serialport,
      baudRate: BAUDRATE,
      autoOpen: false
    });
    
    // Create the newline parser
    this.parser = this.serial.pipe(new ReadlineParser({ 
      delimiter: '\n',
      includeDelimiter: false
    }));
  }

  start(onMessage: (msg: ControllerMessage) => void): Promise<void> {
    // Explicit promise construction so we can resolve only on valid comms AND revision check
    return new Promise<void>(async (res, rej) => {
      // Reset the microcontroller
      await this.reset();

      // Open the serial port
      await new Promise<void>((reso, reje) => {
        Spinner.start('Establishing serial communications with the microcontroller...');
        this.serial.open(err => {
          if (err) {
            reje(err);
          } else {
            Spinner.succeed('Microcontroller serial communications established!');
            reso();
          }
        });
      });

      Spinner.start('Awaiting microcontroller software revision number...');
      this.resetTimeout();
      
      // Set up the listener
      this.parser.on('data', async msgtxt => {

        // Attempt to parse the raw text as a valid JSON object
        let msg: ControllerMessage;
        try {
          msg = JSON.parse(msgtxt);
        } catch (err) {
          rej(err);
          return;
        }

        // Microcontroller-specific pre-handling
        switch (msg.type) {
          case 'revision':
            // Software update
            if(msg.data === CONTROLLER_REVISION) {
              Spinner.succeed('Microcontroller software up to date!');
              res();  //Successful start sequence
            } else {
              Spinner.fail(`Microcontroller software out of date! Got ${msg.data}, expected ${CONTROLLER_REVISION}.`);
              // Attempt to update the microcontroller, and then restart
              this.stop();
              Spinner.start('Compiling latest microcontroller software and flashing...');
              await updateMicrocontroller();
              Spinner.succeed('Updated microcontroller software successfully!');
            }
            // Do not break
          default:
            onMessage(msg);
            break;
        }
        this.resetTimeout();
      });
    });
  }

  /**
   * Clear the serial timeout.
   */
  private clearTimeout(): void {
    if(this.timeout) clearTimeout(this.timeout);
  }

  /**
   * Refresh (or start) the serial timeout.
   */
  private resetTimeout(timeoutSeconds: number = SERIAL_TIMEOUT_SECONDS): void {
    this.clearTimeout();
    this.timeout = setTimeout(() => {
      this.reset();
    }, timeoutSeconds*1000);
  }

  write(msg: ControllerInstructions): void {
    Spinner.info(`[${ chalk.yellow('WRITE') }] - ${ JSON.stringify(msg) }`);
    this.serial.write(JSON.stringify(msg)+'\n', undefined, err => {
      if (err) throw new ControllerInstructionsError(JSON.stringify(msg));
    });
  }

  stop(): void {
    this.clearTimeout();
    this.serial.close();
    this.reset();
  }

  /**
   * Resets the microcontroller (active-low).
   */
  private async reset(): Promise<void> {
    gpioWrite(RESET_PIN, 0);
    await sleep(10);
    gpioWrite(RESET_PIN, 1);
    gpioUnexport(RESET_PIN);
  }
}

/** 
* A simulated controller for generating random data.
*/
export class SimulatedController implements Controller {

  intervals: NodeJS.Timeout[] = [];

  constructor(readonly parameters: SimulatorConfig) { }

  async start(onMessage: (msg: ControllerMessage) => any): Promise<void> {
    for(const label of Object.keys(this.parameters)) {
      this.intervals.push(setInterval(() => {
        onMessage(this.generateData(label, this.parameters[label].min, this.parameters[label].max));
      }, this.parameters[label].interval));
    }
  }
  write() {}
  async stop(): Promise<void> {
    for (const interval of this.intervals) {
      clearInterval(interval);
    }
  }

  /**
   * Generate a single data point
   * @param label Dataset label
   * @param min Minimum value
   * @param max Maximum value
   */
  private generateData(label: string, min: number, max: number): ControllerMessage {
    return {
      type: 'data',
      data: {
        label,
        value: Math.random()*(max-min)+min
      }
    };
  }
}