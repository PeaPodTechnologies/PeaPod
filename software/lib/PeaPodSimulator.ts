import { ArduinoMessage, IPeaPodArduino } from './PeaPodArduino';

function stringTuple<T extends [string] | string[]>(...data: T): T {
    return data;
}

const DataLabels = stringTuple('air_temperature', 'water_level');
type TDataLabels = typeof DataLabels[number];
type SimulatorParameters = {
    [key in TDataLabels]: {
        min: number,
        max: number,
        interval: number
    }
}

type ArduinoData = ArduinoMessage & {
    type: 'data', msg: {
        [key: string]: number
    }
};

function generateData(label: TDataLabels, min : number, max : number) : ArduinoData {
    let data : ArduinoData = {
        type: 'data',
        msg: {}
    };
    data.msg[label.replace('_','-')] = Math.random()*(max-min)+min;
    return data;
}

export class ArduinoSimulator implements IPeaPodArduino{
    intervals : NodeJS.Timeout[] = []
    constructor(public parameters : SimulatorParameters){}
    async stop(): Promise<void> {
        for(const interval of this.intervals){
            clearInterval(interval);
        }
    }
    start(onMessage: (msg: ArduinoMessage) => any): void {
        for(const label in this.parameters){
            this.intervals.push(setInterval(()=>{
                onMessage(generateData(
                    label as TDataLabels, 
                    this.parameters[label as TDataLabels].min, 
                    this.parameters[label as TDataLabels].max));
            }, this.parameters[label as TDataLabels].interval));
        }
    }
}