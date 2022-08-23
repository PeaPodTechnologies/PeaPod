// HEADERS

#include <actuators/tec.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

// CONSTRUCTOR

TEC::TEC(uint8_t powerpin, uint8_t relayApin, uint8_t relayBpin) : Actuator(&id, 0), power(powerpin), relayA(relayApin), relayB(relayBpin) { }

// PUBLIC METHODS

errorlevel_t TEC::initialize(void) {
  pinMode(power, OUTPUT);
  pinMode(relayA, OUTPUT);
  pinMode(relayB, OUTPUT);
  return ERR_NONE;
}

errorlevel_t TEC::set(float target) {
  // Clamp to -1 < x < 1
  target = min(max(target, -1), 1);
  analogWrite(power, abs(target) * 255);
  analogWrite(relayA, 255 * (target >= TEC_DEADZONE ? 1 : 0));
  analogWrite(relayB, 255 * (target <= -TEC_DEADZONE ? 1 : 0));
  return ERR_NONE;
}