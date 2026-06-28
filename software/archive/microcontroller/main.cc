#ifndef UNIT_TEST
#ifndef MAIN
#define MAIN true

#include <Arduino.h>

#include <debug.h>

#include <ControlSystemsOS.h>
#include <chrono.h>

#include <avr/wdt.h>

#define TIMESTAMP_LOG_DELTA_MS 5000 // Default

#define FIXED_UPDATE_DELTA 1000

#include "simple.h"

fsm_timestamp_t start = 0;
fsm_timestamp_t ms_last = 0;

bool rebuild = false;

void setup(void) {
  // Serial
  Serial.begin(115200);
  while(!Serial);

  // Pin Modes
  pinMode(PIN_LIGHTING_LO, OUTPUT);
  pinMode(PIN_LIGHTING_HI, OUTPUT);
  pinMode(PIN_WATERING, OUTPUT);
  pinMode(PIN_DISCO_BUTTON, INPUT);

  lighting.addLoggerCallback(&logLights);
  watering.addLoggerCallback(&logWatering);

  lighting.addLatchingConditional(true, false, &controlLights);
  watering.addLatchingConditional(true, false, &controlWatering);
  disco.addLatchingConditional(true, false, &controlDisco);

  // Timestamp output
  Chronos.addInterval(TIMESTAMP_LOG_DELTA_MS, &logTimestamp);

  // Timer Flag Event - Lighting ON (No-Invert)
  Chronos.addEventFlag(DELAY_START, &lighting);

  // Timer Flag Event - Lighting OFF (Inverted)
  Chronos.addEventFlag(DELAY_START + DURATION_LIGHTING, &lighting, true);

  // Timer Flag Interval - Watering ON (No-Invert)
  Chronos.addIntervalFlag(PERIOD_WATERING, &watering);

  // Timer Flag Interval - Watering OFF (Invert)
  Chronos.addIntervalFlag(PERIOD_WATERING, DURATION_WATERING, &watering, true);

  // Disco Scroller Callback - passes interval timestamp
  // discoScroller = 
  Chronos.addInterval(DELTA_DISCO, &scrollDisco);

  // Disco Controller - Dis-/En-ables Disco Scroller Callback

  start = millis();

  // Serial.println(F("==== [ CYCLE -1 (SIZEOF) ] ===="));
  // Serial.print(F("Module: "));
  // Serial.print(sizeof(ControlSystemsOS::CSOSModule));
  // Serial.print(F("\nHashTable: "));
  // Serial.print(sizeof(HashTable<DeviceGroup&>));
  // Serial.print(F("\nHashTableEntry: "));
  // Serial.print(sizeof(HashTableEntry<DeviceGroup&>));
  // Serial.print(F("\nBST: "));
  // Serial.print(sizeof(BST<i2cip_fqa_t, Device*>));
  // Serial.print(F("\nBSTNode: "));
  // Serial.print(sizeof(BSTNode<i2cip_fqa_t, Device*>));
  // Serial.print(F("\nDeviceGroup: "));
  // Serial.print(sizeof(DeviceGroup));
  // Serial.print(F("\nEEPROM: "));
  // Serial.print(sizeof(EEPROM));
  // Serial.println();
  Serial.println(F("==== [ CYCLE 0 (BUILD) ] ===="));

  delay(1000);

  i2cip_errorlevel_t errlev = ControlSystemsOS::update(true);
  if(errlev > I2CIP_ERR_NONE) {
    Serial.println(F("==== [ BUILD FAILED, FREEZING ] ===="));
    while(true) { // Blink
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }

  #ifdef _AVR_WDT_H_
    wdt_enable(WDTO_4S);
  #endif
}

unsigned long cycle = 0;

void loop(void) {
  #ifdef _AVR_WDT_H_
    wdt_reset();
  #endif

  cycle++;
  Serial.print(F("\n\n==== [ CYCLE "));
  Serial.print(cycle);
  Serial.println(F(" ] ===="));

  fsm_timestamp_t ms_start = millis();
  // Watchdog Timer 24Hr Kickout
  if(ms_start > TWENTYFOURHRS_MILLIS) { while(true) { delay(1); } }

  // 0. Module State-Change Forward-Propagation
  // TODO: Move to `Module`?

  // 0a. Delete Devices Associated with Lost Modules
  // if(ControlSystemsOS::stateChange) {

  //   for(uint8_t i = 0; i < I2CIP_NUM_WIRES; i++) {
  //     for(uint8_t x = 0; x < I2CIP_MUX_COUNT; x++) {

  //       // Check if module exists
  //       Module* m = ControlSystemsOS::csos_modules[i][x];
  //       if(m == nullptr) {
  //         for(uint8_t j = 0; j < NUM_DEVICE_TYPES; j++) {
  //           const char* key = ControlSystemsOS::csos_map_device_id[j];
  //           I2CIP::DeviceGroup * group = (*m)[key];
  //           if(group == nullptr) continue;

  //           for(uint8_t k = 0; k < group->numdevices; k++) {
  //             Device* device = group->devices[k];
  //             if(device != nullptr) {
  //               // Delete Device
  //               m->remove(device);
  //             }
  //           }
  //         }
  //       }


  //     }
  //   }

  //   ControlSystemsOS::stateChange = false;
  // }
  
  // 1. Refresh Update - Network Status Handling & Rebuild

  // 1a. Module "Roll-Call" Check
  i2cip_errorlevel_t errlev = ControlSystemsOS::update(rebuild);
  // If we lose hardware, return and do FixedUpdate ASAP
  rebuild = (errlev == I2CIP_ERR_HARD);
  if(errlev > I2CIP_ERR_NONE) return;

  // 1b. Device Checking Per-Group
  fsm_timestamp_t ms_now = millis();
  if((ms_now - ms_last) > FIXED_UPDATE_DELTA) {
    errlev = ControlSystemsOS::fixedUpdate(ms_now);
    ms_last = millis();
  }

  // 2. Fixed Update - Instruction and Control Handling

  Chronos.set(ms_start);

  // Other Stuff

  int discoPin = digitalRead(PIN_DISCO_BUTTON);
  disco.set(discoPin == HIGH ? true : false);

  delay(100);

}

// Overview:
//
// 1. Module State-Change Forward-Propagation
// 2. Refresh Update - Network Status Handling & Rebuild
// 3. Fixed Update - Instruction and Control Handling
//
// 1. Module State-Change Forward-Propagation
//
// 1a. Delete Devices Associated with Lost Modules
// 1b. Delete Modules
//
// 2. Refresh Update - Network Status Handling & Rebuild
//
// 2a. Module "Roll-Call" Check
// 2b. Device Checking Per-Group
//
// 3. Fixed Update - Instruction and Control Handling

#endif
#endif