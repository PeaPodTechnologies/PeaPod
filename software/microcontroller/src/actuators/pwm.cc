#include <actuators/pwm.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

PWM::PWM(uint8_t pin, actuatorid_t actuatorid) : Actuator(actuatorid, 0) {
  this->pin = pin;
}

errorlevel_t PWM::initialize(void) {
  pinMode(this->pin, OUTPUT);
  return ERR_NONE;
}

errorlevel_t PWM::set(float target) {
  // Clamp to 0 < x < 1
  target = min(max(target, 0), 1);
  analogWrite(this->pin, target * 255);
  return ERR_NONE;
}