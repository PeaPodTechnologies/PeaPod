// import { PeaPodMessage } from './PeaPodPublisher';
import chalk from 'chalk';
import { ReadlineParser, SerialPort } from 'serialport';
import { ArduinoInstructionsError } from './errors';
import Spinner from './ui';
import { gpioUnexport, gpioWrite, sleep, updateArduino } from './utils';

const BAUDRATE = 115200;
const ARDUINO_REVISION = 0;
// const TIMEOUT_SECONDS = 5;
const RESET_PIN = 5;

/**
 * Abstract base type for any PeaPod message source.
 */
export type IPeaPodArduino = {
  /**
   * Establish communications with the Arduino.
   * @param onMessage Pipe recieved messages.
   */
  start(onMessage : (msg : ArduinoMessage) => void): Promise<void>

  /**
   * Write new instructions to the Arduino.
   * @param instructions 
   */
  write(instructions: ArduinoInstructions): void;

  /**
   * Halt communications with the Arduino.
   */
  stop(): void;
};

/**
 * Expanded message type to include all types of messages from the Arduino
 */
export type ArduinoMessage = {
  type: 'info' | 'debug' | 'error',
  data: string
} | {
  type: 'data',
  data: {
    label: string,
    value: number
  }
} | {
  type: 'revision',
  data: number
}

export type ArduinoInstructions = {
  [key: string]: number;
}

/**
 * Interface between this computer and the Arduino.
 */
export default class PeaPodArduinoInterface implements IPeaPodArduino {
  serial : SerialPort;
  parser : ReadlineParser;
  constructor(readonly serialpath: string) {
    // Create the serial port interface
    this.serial = new SerialPort({
      path: serialpath,
      baudRate: BAUDRATE,
      autoOpen: false
    });
    
    // Create the newline parser
    this.parser = this.serial.pipe(new ReadlineParser({ 
      delimiter: '\n',
      includeDelimiter: false
    }));
  }
  start(onMessage : (msg : ArduinoMessage) => void): Promise<void> {
    return new Promise<void>(async (res, rej) => {
      // Reset the Arduino
      await this.reset();

      // Open the serial port
      await new Promise<void>((reso, reje) => {
        Spinner.start('Establishing serial communications with the Arduino...');
        this.serial.open(err => { 
          if (err) {
            reje(err);
          } else {
            Spinner.succeed('Arduino serial communications established!');
            reso();
          }
        });
      });

      Spinner.start('Awaiting Arduino software revision number...');

      // Reusable timeout, enabled at serial open (fails if no messages recieved after starting)
      // let timeout: NodeJS.Timeout;
      // const resetTimeout = (timeoutSeconds: number = TIMEOUT_SECONDS) => {
      //   clearTimeout(timeout);
      //   timeout = setTimeout(() => {
      //     this.stop();
      //     throw new SerialTimeoutError(timeoutSeconds)
      //   }, timeoutSeconds*1000);
      // }
      // Initial timeout
      // resetTimeout();
      
      // Set up the listener
      this.parser.on('data', msgtxt => {
        // Attempt to parse the raw text as a valid JSON object'
        var msg : ArduinoMessage;
        try {
          msg = JSON.parse(msgtxt) as ArduinoMessage;
        } catch (err) {
          rej(err);
          return;
        }
        // Handle all message types except: 'info', 'data', 'debug', 'error' 
        switch (msg.type) {
          case 'revision':
            if(msg.data == ARDUINO_REVISION) {
              Spinner.succeed('Arduino software up to date!');
              res();
              // Do not break
            } else {
              Spinner.fail(`Arduino software out of date! Expected ${ARDUINO_REVISION}.`);
              // Attempt to update the Arduino, and then restart
              this.stop();
              // clearTimeout(timeout);
              this.update().finally(() => {
                process.exit();
              });
              // Do not reset timeout
              return;
            }
            break;
          default:
            onMessage(msg);
            break;
        }
        // resetTimeout();
      });
    });
  }
  async update() {
    Spinner.start('Compiling latest Arduino software and flashing to board...');
    await updateArduino().then(() => {
      Spinner.succeed('Updated Arduino software successfully! Rebooting in 3 seconds...');
    }).catch(e => {
      Spinner.fail(e);
    })
    await sleep(3000);
  }
  write(msg : ArduinoInstructions) {
    Spinner.info(`[${chalk.yellow('WRITE')}] - ${JSON.stringify(msg)}`);
    this.serial.write(JSON.stringify(msg)+'\n', undefined, (err)=>{
      if(err) throw new ArduinoInstructionsError(JSON.stringify(msg));
    });
  }
  stop() {
    this.serial.close();
  }
  async reset() {
    gpioWrite(RESET_PIN, 1);
    await sleep(100);
    gpioWrite(RESET_PIN, 0);
    gpioUnexport(5);
  }
}