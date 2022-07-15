#include <actuators/supply.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/onoff.h>

SupplyPump::SupplyPump(uint8_t pin) : OnOff(pin, ACTUATOR_SUPPLY) { }