#include <peapod.h>

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_I2CDevice.h>

#include <modules/module.h>
#include <interfaces/all.h>
#include <sensors/all.h>
#include <actuators/all.h>
#include <utils/factory.h>
#include <utils/hashtable.h>

PeaPod::PeaPod(TwoWire* wire) : wire(wire) {

}

void PeaPod::scan(void) {
  // Module Ping
  uint8_t nmodules = 0;
  bool moduleFound [MODULE_COUNT] = { false };
  for (uint8_t i = 0; i < MODULE_COUNT; i++) {
    if (modules[i] != nullptr) {
      moduleFound[i] = true;
    }

    // Acknowledge?
    uint8_t addr = MODULE_ADDR_MIN + i;
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      // Module found! Mark 'true'
      // sendMessage(MESSAGE_DEBUG, "Module found at 0x" + String(addr < 16 ? "0" : "") + String(addr, HEX));
      moduleFound[i] = true;
      nmodules++;
    } else if (error == 4) {
      // Wierd error; retry
      // sendMessage(MESSAGE_ERROR, "Error at 0x" + String(addr < 16 ? "0" : "") + String(addr, HEX));
      i--;
      continue;
    } else if (moduleFound[i] == true) {
      // Module not found; previously found module has been lost; deallocate
      delete(modules[i]);
      modules[i] = nullptr;
    }
  }

  // Read moduleinfo EEPROM
  StaticJsonDocument<EEPROM_SIZE> json;
  for (uint8_t i = 0; i < MODULE_COUNT; i++)  {
    if (!moduleFound[i]) break;
    uint8_t modaddr = MODULE_ADDR_MIN + i;

    // PING EEPROM
    EEPROM eeprom(modaddr);
    // Attempt; uncache if failed
    if (eeprom.begin() != ERR_NONE) {
      // sendMessage(MESSAGE_ERROR, "Error pinging EEPROM at Module 0x" + String(modaddr < 16 ? "0" : "") + String(modaddr, HEX));
      moduleFound[i] = false;
      nmodules--;
      continue;
    }

    // READ EEPROM
    char buffer[EEPROM_SIZE] = { '\0' };
    size_t len = 0;
    eeprom.read(buffer, &len);

    // SWITCH MUX TO INACTIVE BUS
    Wire.beginTransmission(modaddr);
    Wire.write(1 << MODULE_BUS_INACTIVE);
    Wire.endTransmission();

    // EEPROM dump
    // sendMessage(MESSAGE_DEBUG, "Read from EEPROM: " + String(buffer));

    // Attempt to parse JSON
    DeserializationError error = deserializeJson(json, buffer);
    if (error) {
      // sendMessage(MESSAGE_ERROR, "Error parsing EEPROM to JSON at Module 0x" + String(modaddr < 16 ? "0" : "") + String(modaddr, HEX));
      moduleFound[i] = false;
      nmodules--;
      continue;
    }

    // Initialize Module
    modules[i] = new Module(json["id"], json["numsensors"], json["numactuators"]);

    // Set up interfaces
    // GPIO, ADC, etc.

    // ALLOCATE SENSORS, ACTUATORS TO HEAP
    for (uint8_t i = 0; i < MODULE_COUNT; i++) {
      ModuleState* state = modules[i]->getState();
      uint8_t j = 0;
      for (JsonPair sensor : json["sensors"].as<JsonArray>()) {

        // Check interface hashtable
        Factory<Sensor>* sf = sensorFactories[(const char*)(sensor.key().c_str())];

        if (sf == nullptr) {
          // ERROR: Unrecognized sensor. Flash module EEPROM.
          break;
        }

        Sensor* s = sf->operator()(&(sensor.value().to<JsonObject>()));

        if (s == nullptr) {
          // ERROR: constructor arguments invalid
          break;
        }

        state->sensors[j] = s;
        j++;
      }
      state->numsensors = j;
      j = 0;
      for (JsonPair actuator : json["actuators"].as<JsonObject>()) {
        // Check interface hashtable
        Factory<Actuator>* af = actuatorFactories[(const char*)(actuator.key().c_str())];

        if (af == nullptr) {
          // ERROR: Unrecognized sensor. Flash module EEPROM.
          break;
        }

        Actuator* a = af->operator()(&(actuator.value().to<JsonObject>()));

        if (a == nullptr) {
          // ERROR: constructor arguments invalid
          break;
        }

        state->actuators[j] = a;
        j++;
      }
      state->numactuators = j;
    }
  }
}

errorlevel_t PeaPod::post(void) {
  errorlevel_t success = ERR_NONE;
  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->begin();
    errorlevel_t latest = (state->error == ERR_HARD ? ERR_HARD : (state->debug == ISTATE_OFF || state->error == ERR_SOFT ? ERR_SOFT : ERR_NONE));
    if (latest) {
      // sendMessage(MESSAGE_DEBUG, String("Sensor " + sensors[i]->toString() + " initialized successfully."));
    } else {
      // sendMessage(MESSAGE_ERROR, String("Failed to initialize sensor " + sensors[i]->toString() + ". Check wiring."));
    }
    success &= latest;
  }
  for (int i = 0; i < NUM_ACTUATORS; ++i) {
    ActuatorState* state = actuators[i]->begin();
    errorlevel_t latest = (state->debug >= ISTATE_ON && state->error == ERR_NONE);
    if (latest) {
      // sendMessage(MESSAGE_DEBUG, String("Actuator " + actuators[i]->toString() + " initialized successfully."));
    } else {
      // sendMessage(MESSAGE_DEBUG, String("Failed to initialize actuator " + actuators[i]->toString() + ". Check wiring."));
    }
    success &= latest;
  }
  return success;
}