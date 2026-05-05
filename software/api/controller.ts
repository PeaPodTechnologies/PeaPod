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

export function findSerialPort(path: string): Promise<string[]> {
  return SerialPort.list().then(ports => {
    return ports.reduce((acc, port) => {
      // console.log(JSON.stringify(port, null, 2));
      if(port && port['path'] && (port['path'] as string).toLowerCase().includes(path.toLowerCase())) {
        acc.push(port.path as string);
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
  stop(): void;
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
  start(onMessage: (messages: DebugJsonMessage[]) => void): Promise<void> {
    this.pauseTimeout(true); // Don't want it interrupting the start sequence
    // Reset listeners
    this.parser.removeAllListeners('data');

    // Explicit promise construction so we can resolve only on valid comms AND revision check
    return new Promise<void>(async (res, rej) => {
      
      // Reset the microcontroller (opens the serial port)
      this.reset().catch((err) => rej(err)).then(() => {
        this.resetTimeout(rej);
        this.resetInterval = setInterval(() => {
          ui.info('CONTROLLER RESET INTERVAL');
          this.reset().catch((err) => rej(err));
        }, RESET_INTERVAL);
        ui.start('CONTROLLER REVISION...');
      });

      this.parser.on('error', async (_err) => {
        await this.reset().catch(() => rej(new DebugJsonSerialportError(`${_err.name} - ${_err.message}`)));
      });

      // Set up the listener
      this.parser.on('data', async (msgtxt) => {
        this.resetTimeout(rej);
        this.#count++;
        
        // Attempt to parse the raw text as a valid JSON object
        let msg: DebugJsonMessage;
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
          if (msg.data?.revision === CONTROLLER_REVISION) {
            if(ui.spinning()) {
              ui.succeed(
                `CONTROLLER REVISION PASS! ${msg.data.revision}`
              );
            }
            res(); //Successful start sequence
          } else {
            ui.fail(
              `CONTROLLER REVISION FAIL: ${msg.data?.revision ?? 'NULL'} != ${CONTROLLER_REVISION}`
            );
            // Attempt to update the microcontroller, and then restart
            this.stop();
            ui.start('Compiling latest microcontroller software and flashing...');
            await updateMicrocontroller();
            ui.succeed('Updated microcontroller software successfully!');
          }
          if(this.passRevision === false) { break; }
          // onMessage([msg]); // Uncomment to pass revision messages to the rest of the app (unbatched)
          break;
        default:
          this.#batch.push(msg);
          if(Date.now() - this.#lastBatch >= BATCH_INTERVAL) {
            onMessage(this.#batch);
            this.#batch = [];
            this.#lastBatch = Date.now();
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
  private resetTimeout(cb?: (err?: Error) => void, timeoutSeconds: number = SERIAL_TIMEOUT_SECONDS): void {
    this.pauseTimeout(true);
    this.timeout = setTimeout(() => {
      ui.fail(
        `CONTROLLER TIMEOUT: ${timeoutSeconds}s`
      );
      this.#timedout = true;
      this.reset().catch((err) => {if(cb) {cb(err);}});
    }, timeoutSeconds * 1000);
  }

  write(msg: DebugJsonMessage): void {
    ui.info(`[${chalk.yellow('WRITE')}] - ${JSON.stringify(msg)}`);
    this.serial.write(JSON.stringify(msg) + '\n', undefined, (err) => {
      if (err) throw new ControllerTXError(JSON.stringify(msg));
    });
  }

  stop(): void {
    this.pauseTimeout();
    if (this.serial.isOpen) this.serial.close();
    // Stop listening for data
    // this.parser.removeAllListeners('data');
  }

  /**
   * Resets the microcontroller by closing and re-opening serial.
   */
  private reset(): Promise<void> {
    // Stop and reset
    this.stop();
    // this.resetpin.writeSync(1);

    this.#count = 0;

    // Wait, then stop resetting
    // await new Promise<void>((r) => setTimeout(r, 1000));
    // this.resetpin.writeSync(0);

    // (Re-)open serial
    return new Promise<void>((reso, reje) => {
      ui.start('CONTROLLER...');
      this.serial.open((err) => {
        if (err) {
          reje(err);
        } else {
          ui.succeed('CONTROLLER!');
          this.resetTimeout(reje);
          reso();
        }
      });
    });

    // Restart timeout
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
