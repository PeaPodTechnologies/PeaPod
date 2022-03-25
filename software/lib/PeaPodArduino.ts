import { PeaPodMessage } from './PeaPodPublisher';
import SerialPort from 'serialport';
import { ArduinoInstructionsError, RevisionError } from './errors';
import Spinner from './ui';
import chalk from 'chalk';

const BAUDRATE = 115200;
const ARDUINO_REVISION = "0";

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
export default class PeaPodArduinoInterface implements IPeaPodArduino{
  serial : SerialPort;
  parser : SerialPort.parsers.Readline;
  constructor(readonly serialpath: string, private initialInstructions: ArduinoInstructions = {}){
    // Open the serial port
    this.serial = new SerialPort(serialpath, {
      baudRate: BAUDRATE,
      autoOpen: false
    });
    
    // Create the newline parser
    this.parser = this.serial.pipe(new SerialPort.parsers.Readline({ 
      delimiter: '\n',
      includeDelimiter: false
    }));
  }
  async start(onMessage : (msg : ArduinoMessage) => void): Promise<void> {
    this.serial.open(err=>{if(err){throw err};});
    
    // Set up the listener
    this.parser.on('data', msgtxt =>{
      try{
        // Parse the raw text as a JSON object.
        const msg = JSON.parse(msgtxt) as ArduinoMessage;
        // Handle all message types except: 'info', 'data', 'debug', 'error' 
        switch (msg.type) {
          case 'revision':
            onMessage(msg);
            if((msg.data as typeof ARDUINO_REVISION) == ARDUINO_REVISION){
              this.write(this.initialInstructions);
            } else {
              // TODO: Update Arduino if revision does not match
              throw new RevisionError(ARDUINO_REVISION, msg.data as string);
            }
            break;
          default:
            onMessage(msg);
            break;
        }
      } catch (err) {
      }
    });
  }
  write(msg : any){
    Spinner.info(`[${chalk.yellow('WRITE')}] - ${JSON.stringify(msg)}`);
    if(!this.serial?.write(JSON.stringify(msg)+'\n')){
      throw new ArduinoInstructionsError(JSON.stringify(msg));
    }
  }
  stop() {
    this.serial.close();
  }
}