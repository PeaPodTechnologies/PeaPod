#ifndef PEAPOD_ACTUATORS_SOLENOID_H_
#define PEAPOD_ACTUATORS_SOLENOID_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

class Solenoid : public OnOff {
  public:
    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     */
    Solenoid(uint8_t pin);
};

#endif
