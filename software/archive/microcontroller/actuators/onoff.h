#ifndef PEAPOD_ACTUATORS_ONOFF_H_
#define PEAPOD_ACTUATORS_ONOFF_H_

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

class OnOff : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param id Actuator ID
     * @param pin Digital output pin for controlling this on/off actuator.
     */
    OnOff(const char* const* id, const uint8_t pin);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // Digital output pin.
    const uint8_t pin;
};

#endif
