#ifndef UNIT_TEST
#define UNIT_TEST 1
#define IS_MAIN 1

// INCLUDES

#include <Arduino.h>

#include <peapod.h>
#include <air.h>
#include <water.h>
#include <light.h>

// #define PEAPOD_USEMENU 1 // Uncomment to enable menu functionality (requires JHD1313 LCD and Seesaw Rotary Encoder with Push Button)

#ifdef PEAPOD_USEMENU
  #include "menu.h"
#endif

using namespace PeaPod;
using namespace I2CIP;

void setup(void) {
  // Builtin LED Pinmode; Serial Begin

  pinMode(LED_BUILTIN, OUTPUT);
  PEAPOD_SERIAL.begin(115200);
  while(!PEAPOD_SERIAL) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }
  
  delay(100);
  
  // Instantiate modules ahead of time
  PeaPod::callback_module<PEAPOD_MODULENUM_AIR, PeaPodModuleAir>();
  PeaPod::callback_module<PEAPOD_MODULENUM_WATERING, PeaPodModuleWatering>();
  PeaPod::callback_module<PEAPOD_MODULENUM_LIGHTING, PeaPodModuleLighting>();

  delay(100);

  // Initialize non-module devices
  #ifdef PEAPOD_USEMENU
    if(Menu::initializeDevices() != I2CIP_ERR_NONE) {
      // TODO - print to seven segment?
    }
  #endif
  
  delay(100);
  
  DebugJson::revision(I2CIP_REVISION, PEAPOD_SERIAL);

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

  #ifdef PEAPOD_USEMENU
    if(Menu::update() != I2CIP_ERR_NONE) {
      // TODO - print to seven segment?
    }
  #endif
}

#endif