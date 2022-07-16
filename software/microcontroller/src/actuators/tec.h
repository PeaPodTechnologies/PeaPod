#ifndef PEAPOD_ACTUATORS_TEC_H_
#define PEAPOD_ACTUATORS_TEC_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

#define TEC_DEADZONE 0.01

class TEC : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param powerpin Digital PWM output pin for controlling heat pump magnitude
     * @param directionpin Digital output pin for controlling heat pump direction
     */
    TEC(uint8_t powerpin, uint8_t relayApin, uint8_t relayBpin);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // "Magnitude" PWM output pin.
    uint8_t power;

    // "Direction"/relay control digital output pin.
    uint8_t relayA, relayB;
};

#endif
