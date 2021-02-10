#include "Arduino.h"
#include "Actuator.h"

/**
 * Constructor. 
 * @param name - Meaningful, readable name of the sensor.
 * @param id - Identifying name of the associated hardware.
 * @param evname - Name of the environment variable the sensor records.
 **/
Actuator::Actuator(String name, String id, String evname){
    this->id = id;
    this->name = name;
    this->evname = evname;
    //just in case
    this->evname.toLowerCase();
    this->evname.replace(' ','-');
}

/**
 * Wrapper for more complex set function.
 * @return Sensor read, or nan. Usually nan. Like 99% of the time nan.
 **/
void Actuator::setTarget(float target){
    if(this->ready){
        this->set(target);
    }
}

bool Actuator::begin(){
    this->ready = init();
    return ready;
}