#ifndef PEAPOD_ACTUATORS_PWM_H_
#define PEAPOD_ACTUATORS_PWM_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

class PWM : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param pin Digital output PWM pin for controlling this PWM-capable actuator.
     * @param actuatorid ID of this actuator
     */
    PWM(uint8_t pin, actuatorid_t actuatorid);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // PWM output pin.
    uint8_t pin;
};

#endif
