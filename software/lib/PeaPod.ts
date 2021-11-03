// ROOT LIBRARY FOR ALL INTERNAL OPERATIONS

import PeaPodArduinoInterface, { IPeaPodArduino } from './PeaPodArduino'; //Library for interfacing between the main computer and the Arduino
import PeaPodPubSub, { IPeaPodPublisher } from './PeaPodPublisher';
import { ArduinoSimulator, PeaPodLogger } from './PeaPodSimulator' //Development library - simulated Arduino
import { findSerialPath } from './utils';

/**
 * PeaPod messages to Cloud.
 */
export type PeaPodMessage = {
    type: 'info' | 'data' | 'debug' | 'error',
    data: any
}

/**
 * Base class for PeaPod interfacing.
 */
export default class PeaPod {
    arduino : IPeaPodArduino;
    publisher : IPeaPodPublisher;
    constructor(simulated : boolean = true, serialpath?: string){
        if(simulated || serialpath === undefined){
            this.arduino = new ArduinoSimulator({
                air_temperature: {
                    min: 10,
                    max: 20,
                    interval: 2000
                },
                water_level: {
                    min: 0,
                    max: 1,
                    interval: 1000
                }
            });
            this.publisher = new PeaPodLogger();
        } else {
            this.arduino = new PeaPodArduinoInterface(serialpath);
            this.publisher = new PeaPodPubSub();
        }
    }
}