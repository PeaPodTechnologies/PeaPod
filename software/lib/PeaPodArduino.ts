import { PeaPodMessage } from './PeaPodPublisher';
import { SerialPort, ReadlineParser } from 'serialport';
import { ArduinoInstructionsError, SerialTimeoutError } from './errors';
import Spinner from './ui';
import { updateArduino } from './utils';
import chalk from 'chalk';

const BAUDRATE = 115200;
const ARDUINO_REVISION = "1";
const TIMEOUT_SECONDS = 5;

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
  * Halt communications.
  */
  stop(): void;
};

/**
* Expanded message type to include all types of messages from the Arduino
*/
export type ArduinoMessage = {
  type: 'revision' | 'info' | 'debug' | 'error',
  data: any
} | {
  type: 'data',
  data: {
    label: string,
    value: number
  }
}

type ArduinoInstructions = {
  [key: string]: number;
}

/**
* Interface between this computer and the Arduino.
*/
export default class PeaPodArduinoInterface implements IPeaPodArduino {
  serial : SerialPort;
  parser : ReadlineParser;
  constructor(readonly serialpath: string, private initialInstructions: ArduinoInstructions = {}) {
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
  async start(onMessage : (msg : ArduinoMessage) => void): Promise<void> {
    // Open the serial port
    this.serial.open(err => { if(err){ throw err }; });

    // Reusable timeout, enabled at serial open (fails if no messages recieved after starting)
    let timeout: NodeJS.Timeout;
    const resetTimeout = (timeoutSeconds: number = TIMEOUT_SECONDS) => {
      clearTimeout(timeout);
      timeout = setTimeout(() => {
        this.stop();
        throw new SerialTimeoutError(timeoutSeconds)
      }, timeoutSeconds*1000);
    }
    // Initial timeout
    resetTimeout();
    
    // Set up the listener
    this.parser.on('data', msgtxt => {
      clearTimeout(timeout);
      try {
        // Attempt to parse the raw text as a valid JSON object
        const msg = JSON.parse(msgtxt) as ArduinoMessage;
        // Handle all message types except: 'info', 'data', 'debug', 'error' 
        switch (msg.type) {
          case 'revision':
            onMessage(msg);
            if((msg.data as typeof ARDUINO_REVISION) == ARDUINO_REVISION) {
              this.write(this.initialInstructions);
            } else {
              Spinner.fail(`Arduino revision check failed! Expected ${ARDUINO_REVISION}, recieved ${msg.data}`);
              // Attempt to update the Arduino, and then restart
              this.update().finally(() => {
                process.exit();
              });
            }
            break;
          default:
            onMessage(msg);
            break;
        }
      } catch (err) {
        throw err;
      }
    });
  }
  async update() {
    Spinner.start('Compiling Arduino software and flashing to board...');
    await updateArduino(this.serialpath).catch(e => {
      Spinner.fail(e);
    }).then(() => {
      Spinner.succeed('Updated Arduino software successfully!');
    })
  }
  write(msg : any) {
    Spinner.info(`[${chalk.yellow('WRITE')}] - ${JSON.stringify(msg)}`);
    if(!this.serial?.write(JSON.stringify(msg)+'\n')){
      throw new ArduinoInstructionsError(JSON.stringify(msg));
    }
  }
  stop() {
    this.serial.close();
  }
}