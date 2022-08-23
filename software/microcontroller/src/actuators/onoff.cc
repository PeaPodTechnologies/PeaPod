// HEADERS

#include <actuators/onoff.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

// CONSTRUCTOR

OnOff::OnOff(const char* const* id, const uint8_t pin) : Actuator(id, 0), pin(pin) { }

// PUBLIC METHODS

errorlevel_t OnOff::initialize(void) {
  pinMode(pin, OUTPUT);
  return ERR_NONE;
}

errorlevel_t OnOff::set(float target) {
  // Round to 0 or 1
  digitalWrite(pin, target >= 0.5 ? 1 : 0);
  return ERR_NONE;
}