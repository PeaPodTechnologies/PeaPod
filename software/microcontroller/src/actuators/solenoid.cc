#include <actuators/solenoid.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

Solenoid::Solenoid(uint8_t pin) : OnOff(pin, ACTUATOR_SOLENOID) { }