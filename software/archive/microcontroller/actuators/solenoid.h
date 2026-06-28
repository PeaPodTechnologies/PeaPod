#ifndef PEAPOD_ACTUATORS_SOLENOID_H_
#define PEAPOD_ACTUATORS_SOLENOID_H_

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
  static const PROGMEM char* const id = "Solenoid";
#endif

// CLASS

// Interface for relay-controlled solenoids
class Solenoid : public OnOff {
  public:
    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     */
    Solenoid(const uint8_t pin);
};

#endif
