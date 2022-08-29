#ifndef PEAPOD_ACTUATORS_TEC_H_
#define PEAPOD_ACTUATORS_TEC_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

// MACROS

// Settings
#define TEC_DEADZONE 0.01

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "Thermoelectric Controller";
#endif

// CLASS

// Interface for Thermoelectric Controllers.
class TEC : public Actuator {
  public:
    
    /**
     * Constructor.
     * @param powerpin Digital PWM output pin for controlling heat pump magnitude
     * @param relayApin Digital output pin for controlling heat pump direction relay A
     * @param relayBpin Digital output pin for controlling heat pump direction relay B
     */
    TEC(uint8_t powerpin, uint8_t relayApin, uint8_t relayBpin);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // "Magnitude" PWM output pin.
    const uint8_t power;

    // "Direction"/relay control digital output pins.
    const uint8_t relayA, relayB;
};

#endif
