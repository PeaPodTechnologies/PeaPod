#ifndef PEAPOD_ACTUATORS_SUPPLY_H_
#define PEAPOD_ACTUATORS_SUPPLY_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "Supply Pump";
#endif

// CLASS

// Interface for the supply pump.
class SupplyPump : public OnOff {
  public:
    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     */
    SupplyPump(const uint8_t pin);
};

#endif
