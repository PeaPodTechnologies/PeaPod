import { PeaPodMessage } from './PeaPodPublisher';
import SerialPort from 'serialport';
import { ArduinoInstructionsError, RevisionError } from './errors';

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
    start(onMessage : (msg : ArduinoMessage)=> void): void;
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
            autoOpen: true
        });

        // Create the newline parser
        this.parser = new SerialPort.parsers.Readline({ 
            delimiter: '\n',
            includeDelimiter: false
        });
    }
    async start(onMessage : (msg : ArduinoMessage) => void): Promise<void> {
        // Pipe all serial messages to the newline parser
        this.serial.pipe(this.parser);

        // Set up the listener
        this.parser?.on('data', msgtxt =>{
            // Parse the raw text as a JSON object.
            const msg = JSON.parse(msgtxt) as ArduinoMessage;
            // Handle all message types except: 'info', 'data', 'debug', 'error' 
            switch (msg.type) {
                case 'revision':
                    if((msg.data as string) == ARDUINO_REVISION){
                        this.write({
                            type: 'instructions',
                            data: {...this.initialInstructions}
                        });
                    } else {
                        // TODO: Update Arduino if revision does not match
                        throw new RevisionError(ARDUINO_REVISION, msg.data as string);
                    }
                    break;
                default:
                    onMessage(msg);
                    break;
            }
        });
    }
    write(msg : any){
        if(!this.serial?.write(JSON.stringify(msg)+'\n')){
            throw new ArduinoInstructionsError(JSON.stringify(msg));
        }
    }
    stop() {
       this.serial.close();
    }
}