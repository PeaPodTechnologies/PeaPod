#ifndef PEAPOD_ACTUATORS_ONOFF_H_
#define PEAPOD_ACTUATORS_ONOFF_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

class OnOff : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param pin Digital output pin for controlling this on/off actuator.
     * @param actuatorid ID of this actuator
     */
    OnOff(uint8_t pin, actuatorid_t actuatorid);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // Digital output pin.
    uint8_t pin;
};

#endif
