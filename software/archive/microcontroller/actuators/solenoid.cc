// HEADERS

#include <actuators/solenoid.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

// CONSTRUCTOR

Solenoid::Solenoid(const uint8_t pin) : OnOff(&id, pin) { }