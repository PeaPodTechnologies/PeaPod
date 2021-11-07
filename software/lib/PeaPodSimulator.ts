import { IPeaPodArduino } from './PeaPodArduino';
import { IPeaPodPublisher, PeaPodMessage } from './PeaPodPublisher';
import { stringsToTuple } from './utils';
import chalk from 'chalk';

// Convert our set of strings to a union type. TypeScript nonsense.
const DataLabels = stringsToTuple('air_temperature', 'water_level');
type TDataLabels = typeof DataLabels[number];
type SimulatorParameters = {
    [key in TDataLabels]: {
        min: number,
        max: number,
        interval: number
    }
}

type ArduinoData = {
    type: 'data', data: {
        [key: string]: number
    }
};

function generateData(label: TDataLabels, min : number, max : number) : ArduinoData {
    let data : ArduinoData = {
        type: 'data',
        data: {}
    };
    data.data[label.replace('_','-')] = Math.random()*(max-min)+min;
    return data;
}

/** 
 * A simulated Arduino for generating random data.
*/
export class ArduinoSimulator implements IPeaPodArduino{
    intervals : NodeJS.Timeout[] = []
    constructor(public parameters : SimulatorParameters){}
    async stop(): Promise<void> {
        for(const interval of this.intervals){
            clearInterval(interval);
        }
    }
    start(onMessage: (msg: PeaPodMessage) => any): void {
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

export class PeaPodLogger implements IPeaPodPublisher{
    async start(): Promise<void> {}
    async publish(msg: PeaPodMessage): Promise<void> {
        console.log(`[${chalk.magenta(msg.type.toUpperCase())}] - ${JSON.stringify(msg.data)}`);
    }
}