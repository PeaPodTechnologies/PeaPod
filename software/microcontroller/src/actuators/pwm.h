#ifndef PEAPOD_ACTUATORS_PWM_H_
#define PEAPOD_ACTUATORS_PWM_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

// CLASS

// Abstract interface for any PWM-controllable actuator
class PWM : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param id ID of this actuator
     * @param pin Digital output PWM pin for controlling this PWM-capable actuator.
     */
    PWM(const char* const* id, const uint8_t pin);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // PWM output pin.
    const uint8_t pin;
};

#endif
