#include <actuators/tec.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

TEC::TEC(uint8_t powerpin, uint8_t relayApin, uint8_t relayBpin) : Actuator(ACTUATOR_TEC, 0) {
  this->power = powerpin;
  this->relayA = relayApin;
  this->relayB = relayBpin;
}

errorlevel_t TEC::initialize(void) {
  pinMode(this->power, OUTPUT);
  pinMode(this->relayA, OUTPUT);
  pinMode(this->relayB, OUTPUT);
  return ERR_NONE;
}

errorlevel_t TEC::set(float target) {
  // Clamp to -1 < x < 1
  target = min(max(target, -1), 1);
  analogWrite(this->power, abs(target) * 255);
  analogWrite(this->relayA, 255 * (target >= TEC_DEADZONE ? 1 : 0));
  analogWrite(this->relayB, 255 * (target <= -TEC_DEADZONE ? 1 : 0));
  return ERR_NONE;
}