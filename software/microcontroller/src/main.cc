#ifndef UNIT_TEST
#define UNIT_TEST 1
#define IS_MAIN 1

#include <Arduino.h>

#include <peapod.h>
#include <air.h>
#include <water.h>
#include <light.h>

#include <DebugJson.h>

// void callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __);

using namespace PeaPod;
using namespace I2CIP;

void setup(void) {
  // Builtin LED Pinmode; Serial Begin

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }
  
  delay(100);
  
  // Instantiate modules ahead of time
  PeaPod::callback_module<PEAPOD_MODULENUM_AIR, PeaPodModuleAir>();
  PeaPod::callback_module<PEAPOD_MODULENUM_WATERING, PeaPodModuleWatering>();
  PeaPod::callback_module<PEAPOD_MODULENUM_LIGHTING, PeaPodModuleLighting>();
  
  delay(100);
  
  DebugJson::revision(I2CIP_REVISION, Serial);

  // // Print all devices
  // delay(100);
  // DEBUG_JSON(I2CIP::devicetree.toString());
  // DEBUG_JSON(modules[PEAPOD_MODULENUM]->toString());

  delay(100);

  PeaPod::registerCallbacks();

  delay(100);
}

// LOOP GLOBALS

void loop(void) {

  // I2CIP_DEBUG_SERIAL.println(I2CIP::devicetree.toString());
  // I2CIP_DEBUG_SERIAL.println(modules[PEAPOD_MODULENUM]->toString());

  PeaPod::cycle.set(PeaPod::cycle.get()++);
}

#endif