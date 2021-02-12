import SerialPort from 'serialport';
import { PeaPodMessage } from './PeaPod';

const BAUDRATE = 9600;
const REVISION_CHECK = 0;
const REVISION_STOP = 1;

async function findSerialPath() : Promise<string> {
    let ports = await SerialPort.list();
    for(const port of ports){
        if(port.manufacturer?.toLowerCase().includes("arduino")){
            return port.path;
        }
    }
    throw new Error('Arduino not found! Check wiring.');
}

export class RevisionError extends Error {
    constructor(public serialmessage : string){
        super("Revision check failed! Message: "+serialmessage);
    }
}

export class ArduinoInstructionsError extends Error {
    constructor(public instructions : ArduinoInstructions | Object){
        super("Failed to send instructions to Arduino: "+JSON.stringify(instructions));
    }
}

export interface IPeaPodArduino {
    start(onMessage : (msg : PeaPodMessage)=> any): void;
    stop(): Promise<void>;
}

export type ArduinoInstructions = {
    [key: string]: number
}

export class PeaPodArduinoInterface implements IPeaPodArduino{
    serial : SerialPort | undefined;
    parser : SerialPort.parsers.Readline | undefined;
    private revisionchecked : boolean = false;
    private initialInstructionsSent : boolean = false;
    constructor(){}
    async start(onMessage : (msg : PeaPodMessage)=> any, arduinoRevision : number = 0, initialInstructions : ArduinoInstructions = {"scale":0}): Promise<void> {
        let serialpath = await findSerialPath();
        this.serial = new SerialPort(serialpath, {baudRate: BAUDRATE, autoOpen: true});
        this.parser = new SerialPort.parsers.Readline({ delimiter: '\n', includeDelimiter: false });
        this.serial.pipe(this.parser);
        this.parser?.on('data', msg =>{
            if(!this.revisionchecked){
                if(msg == arduinoRevision){
                    if(!this.serial?.write(REVISION_CHECK+'\n')){
                        throw new ArduinoInstructionsError(REVISION_CHECK);
                    }
                    this.revisionchecked = true;
                } else {
                    if(!this.serial?.write(REVISION_STOP+'\n')){
                        throw new ArduinoInstructionsError(REVISION_STOP);
                    }
                    // Not sure if these `destroy`s are right
                    this.parser?.destroy();
                    this.serial?.destroy();
                    throw new RevisionError(msg);
                }
            } else if (!this.initialInstructionsSent){
                if(msg == 0){
                    this.write(initialInstructions);
                    this.initialInstructionsSent = true;
                } else {
                    onMessage(msg as PeaPodMessage);
                }
            } else {
                onMessage(msg as PeaPodMessage);
            }
        });
        this.parser?.on('data', (msg : string)=>{
            return JSON.parse(msg) as PeaPodMessage;
        });
    }
    write(msg : ArduinoInstructions){
        if(!this.serial?.write(JSON.stringify(msg)+"\n")){
            throw new Error("Failed to send instructions to Arduino: "+JSON.stringify(msg));
        }
    }
    stop(): Promise<void> {
        throw new Error('Method not implemented.');
    }
}