#ifndef PEAPOD_ACTUATORS_SUPPLY_H_
#define PEAPOD_ACTUATORS_SUPPLY_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

class SupplyPump : public OnOff {
  public:
    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     */
    SupplyPump(uint8_t pin);
};

#endif
