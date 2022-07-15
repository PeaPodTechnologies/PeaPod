#include <actuators/onoff.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

OnOff::OnOff(uint8_t pin, actuatorid_t actuatorid) : Actuator(actuatorid, 0) {
  this->pin = pin;
}

errorlevel_t OnOff::initialize(void) {
  pinMode(this->pin, OUTPUT);
  return ERR_NONE;
}

errorlevel_t OnOff::set(float target) {
  // Round to 0 or 1
  digitalWrite(this->pin, target >= 0.5 ? 1 : 0);
  return ERR_NONE;
}