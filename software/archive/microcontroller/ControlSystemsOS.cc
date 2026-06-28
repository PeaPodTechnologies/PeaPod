
#include <ControlSystemsOS.h>

#include <string.h>

#include <Arduino.h>
#include <ArduinoJSON.h>
#include <chrono.h>

// #define DEBUG 1

#include <interfaces/linker.h>
#include <sensors/sensor.h>
#include <debug.h>

// GROUP BY MODULE

ControlSystemsOS::CSOSModule* ControlSystemsOS::csos_modules[I2CIP_NUM_WIRES][I2CIP_MUX_COUNT] = { { nullptr } };

// GLOBAL CONSTANT MAPS

// Internal Use
char ControlSystemsOS::device_id_map[MAP_INDEX_COUNT][I2CIP_ID_SIZE] = { 
  {'\0'}, 
  {'\0'} 
};

bool ControlSystemsOS::device_id_loaded[MAP_INDEX_COUNT] = { 
  false,
  false
};

int ControlSystemsOS::getMapIndex(const i2cip_id_t& id) {
  for(unsigned char i = 0; i < MAP_INDEX_COUNT; i++) {
    // Compare strings ignoring case
    if(strcasecmp_P(id, ControlSystemsOS::device_id_progmem[i]) == 0){
      #ifdef CSOS_DEBUG_SERIAL
        const char* s = getDeviceID(i);
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("-> Map Lookup "));
        CSOS_DEBUG_SERIAL.print(i);
        CSOS_DEBUG_SERIAL.print(_F(" [ID '"));
        CSOS_DEBUG_SERIAL.print(s);
        CSOS_DEBUG_SERIAL.print(_F("' @0x"));
        CSOS_DEBUG_SERIAL.print((uint16_t)s, HEX);
        CSOS_DEBUG_SERIAL.print(_F(" | Factory @0x"));
        CSOS_DEBUG_SERIAL.print((uint16_t)device_factory[i], HEX);
        CSOS_DEBUG_SERIAL.print("]\n");
        DEBUG_DELAY();
      #endif
      return i;
    }
  }
  return -1;
}

const char* ControlSystemsOS::getDeviceID(uint8_t index) {
  // #ifdef CSOS_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   CSOS_DEBUG_SERIAL.print(_F("-> Map ID "));
  //   CSOS_DEBUG_SERIAL.print(index);
  //   CSOS_DEBUG_SERIAL.print(_F(": "));
  // #endif
  if(index >= MAP_INDEX_COUNT) {
    // #ifdef CSOS_DEBUG_SERIAL
    // CSOS_DEBUG_SERIAL.print(_F("Out of Range!"));
    // DEBUG_DELAY();
    // #endif
    return nullptr;
  }
  if(!device_id_loaded[index]) {
    // #ifdef CSOS_DEBUG_SERIAL
    // CSOS_DEBUG_SERIAL.print(_F("(Loading... "));
    // #endif
    if(index == MAP_INDEX_EEPROM) {
      return EEPROM::getStaticIDBuffer();
    } else {
      // char* s1 = (char*)malloc(I2CIP_ID_SIZE * sizeof(char));
      // char* s1 = new char[I2CIP_ID_SIZE];
      // if(s1 == nullptr) return nullptr;
      strcpy_P(device_id_map[index], ControlSystemsOS::device_id_progmem[index]);
      // device_id_map[index] = s2;
      device_id_loaded[index] = true;
    }
    // device_id_loaded[index] = (device_id_map[index] != nullptr);
    // #ifdef CSOS_DEBUG_SERIAL
    // CSOS_DEBUG_SERIAL.print(device_id_loaded[index] ? _F("Success) ") : _F("Fail!)\n"));
    // #endif
  }
  // #ifdef CSOS_DEBUG_SERIAL
  //   if(device_id_map != nullptr) {
  //     CSOS_DEBUG_SERIAL.print(_F("ID '"));
  //     CSOS_DEBUG_SERIAL.print(device_id_map[index]);
  //     CSOS_DEBUG_SERIAL.print(_F("' @0x"));
  //     CSOS_DEBUG_SERIAL.print((uint16_t)device_id_map[index], HEX);
  //     CSOS_DEBUG_SERIAL.print("]\n");
  //   }
  //   DEBUG_DELAY();
  // #endif
  return &(device_id_map[index][0]);
}

using namespace ControlSystemsOS;
using namespace I2CIP;

CSOSModule::CSOSModule(const uint8_t& wire, const uint8_t& module) : Module(wire, module) {
  // Build Maps

}

DeviceGroup* CSOSModule::deviceGroupFactory(i2cip_id_t lookup) {
  if(lookup == nullptr) { Serial.print("wtf?"); return nullptr; }
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("-> DeviceGroup Factory (ID '"));
    CSOS_DEBUG_SERIAL.print(lookup);
    CSOS_DEBUG_SERIAL.print(_F("')\n"));
    DEBUG_DELAY();
  #endif
  int index = ControlSystemsOS::getMapIndex(lookup);
  if(index < 0) {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> Map Index '"));
      CSOS_DEBUG_SERIAL.print(lookup);
      CSOS_DEBUG_SERIAL.print(_F("' DNE! Check Libraries.\n"));
      DEBUG_DELAY();
    #endif
    return nullptr;
  }

  const char* id = getDeviceID(index);

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("-> Creating DeviceGroup '"));
    CSOS_DEBUG_SERIAL.print(id);
    CSOS_DEBUG_SERIAL.print(_F("' (Map Index "));
    CSOS_DEBUG_SERIAL.print(index);
    CSOS_DEBUG_SERIAL.print(_F(", Factory @0x"));
    CSOS_DEBUG_SERIAL.print((uint16_t)device_factory[index], HEX);
    CSOS_DEBUG_SERIAL.print("): ");
    DEBUG_DELAY();
  #endif

  // Serial.print(_F("-> Creating DeviceGroup '"));
  // Serial.print(id);
  // Serial.print(_F("' (Map Index "));
  // Serial.print(index);
  // Serial.print(_F(", Factory @0x"));
  // Serial.print((uint16_t)device_factory[index], HEX);
  // Serial.print("): ");

  DeviceGroup* dg = new DeviceGroup(id, device_factory[index]);

  // if(dg == nullptr) {
  //     Serial.print(_F("Fail!\n"));
  //   } else {
  //     Serial.print(_F("Success!\n"));
  //   }

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    if(dg == nullptr) {
      CSOS_DEBUG_SERIAL.print(_F("Fail!\n"));
    } else {
      CSOS_DEBUG_SERIAL.print(_F("Success!\n"));
    }
    DEBUG_DELAY();
  #endif

  return dg;
}

bool CSOSModule::parseEEPROMContents(const char* buffer) {
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("-> Deserializing Module EEPROM to JSON: "));
    DEBUG_DELAY();
  #endif

  // 0. Buffer size
  size_t buflen = strlen(buffer);
  
  // 1. EEPROM -> JSON Deserialization
  // TODO: Buflen + 1 ?
  DeserializationError jsonerr = deserializeJson(this->eeprom_json, buffer, buflen);

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print("Code 0x");
    CSOS_DEBUG_SERIAL.print(jsonerr.code(), 16);
    CSOS_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif

  if(jsonerr.code() != DeserializationError::Code::Ok) return false;

  // 2. Schema Validation and Loading
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("-> Verifying JSON:\n"));
  #endif

  // 2a. Base Array of Busses
  if(!this->eeprom_json.is<JsonArray>() || !this->eeprom_json[0].is<JsonObject>()) {
    #ifdef CSOS_DEBUG_SERIAL
      CSOS_DEBUG_SERIAL.print(_F("Bad JSON: Invalid Structure!\n"));
      DEBUG_DELAY();
    #endif
  }

  JsonArray busses = this->eeprom_json.as<JsonArray>();

  int busnum = -1;
  for (JsonVariant bus : busses) {
    busnum++;

    #ifdef CSOS_DEBUG_SERIAL
      CSOS_DEBUG_SERIAL.print("[BUS ");
      CSOS_DEBUG_SERIAL.print(busnum+1, HEX);
      CSOS_DEBUG_SERIAL.print("]\n");
    #endif

    // 2b. Bus Root Object
    if(!bus.is<JsonObject>()) {
      #ifdef CSOS_DEBUG_SERIAL
        CSOS_DEBUG_SERIAL.print(_F("Bad JSON: Invalid Bus Structure!\n"));
        DEBUG_DELAY();
      #endif
      continue;
    }
    JsonObject root = bus.as<JsonObject>();
  
    for (JsonPair kv : root) {
      // 2c. Array of I2C Addresses
      if(!kv.value().is<JsonArray>() || kv.value().isNull()) {
        #ifdef CSOS_DEBUG_SERIAL
          CSOS_DEBUG_SERIAL.print(_F("Bad JSON: Invalid Entry Value!\n"));
          DEBUG_DELAY();
        #endif
        continue;
      }

      const char* key = kv.key().c_str();

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print("[ ID '");
        CSOS_DEBUG_SERIAL.print(key);
        CSOS_DEBUG_SERIAL.print("']\n");
      #endif

      // Get DeviceGroup
      DeviceGroup* dg = this->operator[](key);
      if(dg == nullptr) {
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Group DNE! Check Libraries.\n"));
          DEBUG_DELAY();
        #endif
        break;
      }

      uint8_t numfqas = kv.value().size();
      if(numfqas == 0) {
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Empty! (Skipping)\n"));
          DEBUG_DELAY();
        #endif
        continue;
      }
      i2cip_fqa_t fqas[numfqas];

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print("-> [");
        DEBUG_DELAY();
      #endif

      uint8_t i = 0;
      for (JsonVariant addr : kv.value().as<JsonArray>()) {
        if(!addr.is<unsigned int>()) { continue; }
        uint8_t address = addr.as<uint8_t>();
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(" 0x");
          CSOS_DEBUG_SERIAL.print(address, HEX);
          DEBUG_DELAY();
        #endif
        fqas[i] = createFQA(this->getWireNum(), this->getModuleNum(), (uint8_t)busnum, address);
      }

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(" ]\n");
        DEBUG_DELAY();
      #endif

      for (i = 0; i < numfqas; i++) {
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Adding Device "));
          CSOS_DEBUG_SERIAL.print(i+1);
          CSOS_DEBUG_SERIAL.print(" / ");
          CSOS_DEBUG_SERIAL.print(numfqas);
          CSOS_DEBUG_SERIAL.print(_F(" (Factory @0x"));
          CSOS_DEBUG_SERIAL.print((uint16_t)dg->factory, HEX);
          CSOS_DEBUG_SERIAL.print(")\n");
          DEBUG_DELAY();
        #endif

        // Invoke DeviceGroup Call Operator - Returns Matching, or Calls Factory and Adds
        i2cip_fqa_t fqa = fqas[i];
        Device* d = (*dg)(fqa); 
        if(d == nullptr) { 
          #ifdef CSOS_DEBUG_SERIAL
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> Factory Failed! (Skipping)\n"));
            DEBUG_DELAY();
          #endif
          continue;
        } else {
          #ifdef CSOS_DEBUG_SERIAL
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> Factory Success! (Adding)\n"));
            DEBUG_DELAY();
          #endif
          Device* dd = this->add(*d);
          if(dd == nullptr) {
            #ifdef CSOS_DEBUG_SERIAL
              DEBUG_DELAY();
              CSOS_DEBUG_SERIAL.print(_F("-> Couldn't Add Device!\n"));
              DEBUG_DELAY();
            #endif
            return false;
          }
        }
      }

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("-> Group Complete!\n"));
        DEBUG_DELAY();
      #endif
    }
  }
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("-> EEPROM Parsed Successfully!\n"));
    DEBUG_DELAY();
  #endif
  return true;
}

// Subnetwork state change update + rebuild from discovery
i2cip_errorlevel_t ControlSystemsOS::update(const uint8_t& wire, const uint8_t& mod, bool build) {
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("= [ NETWORK UPDATE: MODULE "));
    CSOS_DEBUG_SERIAL.print(mod, HEX);
    CSOS_DEBUG_SERIAL.print(" ] =\n");
    DEBUG_DELAY();
  #endif

  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  CSOSModule* m = csos_modules[wire][mod];
  
  if(m == nullptr) {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> Module DNE; Pinging.\n"));
      DEBUG_DELAY();
    #endif

    bool b = MUX::pingMUX(wire, mod);

    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      if(b) { CSOS_DEBUG_SERIAL.print(_F("-> Module Found! Creating.\n")); }
      else { CSOS_DEBUG_SERIAL.print(_F("-> Module Not Found.\n")); }
      DEBUG_DELAY();
    #endif
    
    if(b) {
      // New module found!
      m = new CSOSModule(wire, mod);
      if(m == nullptr) {
        Serial.println("CSOSModule ENOMEM");
        return I2CIP_ERR_SOFT;
      }
      csos_modules[wire][mod] = m;

      if (build) { 
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Discovering\n"));
          DEBUG_DELAY();
        #endif

        bool r = m->discover();

        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Discovery"));
          CSOS_DEBUG_SERIAL.print(r ? _F(" Success!\n") : _F(" Failure, Deleting Module.\n"));
          DEBUG_DELAY();
        #endif

        if (!r) {
          delete m;
          csos_modules[wire][mod] = nullptr;
          return I2CIP_ERR_SOFT;
        }
      }
    } else {
      // No net change
      return I2CIP_ERR_NONE;
    }
  } else {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> Updating Module.\n"));
      DEBUG_DELAY();
    #endif
  }

  // csos_modules_lastChecked[wire][module] = millis();

  errlev = m->operator()();
  
  if(errlev > I2CIP_ERR_NONE) {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> Fail. Deleting Module.\n"));
      DEBUG_DELAY();
    #endif
    delete m;
    csos_modules[wire][mod] = nullptr;
    return errlev;
  } 
  #ifdef CSOS_DEBUG_SERIAL
    else {
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> Pass! Updating Devices.\n"));
      DEBUG_DELAY();
    }
  #endif

  for(uint8_t i = 0; i < MAP_INDEX_COUNT; i++) {
    const char* id = getDeviceID(i);
    if(id == nullptr || id[0] == '\0') continue;
    DeviceGroup* dg = (*m)[id];
    if(dg == nullptr) continue;

    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> [ DEVICE GROUP "));
      CSOS_DEBUG_SERIAL.print(i+1, HEX);
      CSOS_DEBUG_SERIAL.print(" / ");
      CSOS_DEBUG_SERIAL.print(MAP_INDEX_COUNT, HEX);
      CSOS_DEBUG_SERIAL.print(" ]\n");
      DEBUG_DELAY();
    #endif

    for(uint8_t j = 0; j < dg->numdevices; j++) {
      Device* device = dg->devices[j];
      if(device == nullptr) continue;

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("[ DEVICE "));
        CSOS_DEBUG_SERIAL.print(j+1, HEX);
        CSOS_DEBUG_SERIAL.print(" / ");
        CSOS_DEBUG_SERIAL.print(dg->numdevices, HEX);
        CSOS_DEBUG_SERIAL.print(_F(" ]\n-> FQA: "));
        CSOS_DEBUG_SERIAL.print(I2CIP_FQA_SEG_I2CBUS(device->getFQA()), HEX);
        CSOS_DEBUG_SERIAL.print(":");
        CSOS_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MODULE(device->getFQA()), HEX);
        CSOS_DEBUG_SERIAL.print(":");
        CSOS_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MUXBUS(device->getFQA()), HEX);
        CSOS_DEBUG_SERIAL.print(":");
        CSOS_DEBUG_SERIAL.print(I2CIP_FQA_SEG_DEVADR(device->getFQA()), HEX);
        CSOS_DEBUG_SERIAL.print("\n");
        DEBUG_DELAY();
      #endif

      errlev = (*m)(*device, false);

      #ifdef CSOS_DEBUG_SERIAL
        switch(errlev) {
          case I2CIP_ERR_NONE:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x0 Pass!\n"));
            DEBUG_DELAY();
            break;
          case I2CIP_ERR_SOFT:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x1 Communication Error!\n"));
            DEBUG_DELAY();
            break;
          case I2CIP_ERR_HARD:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x2 Hardware Lost, ABORT!\n"));
            DEBUG_DELAY();
            break;
        }
      #endif

      if(errlev == I2CIP_ERR_HARD) return errlev;
    }
  }

  return errlev;
}

// Whole-network state change update
i2cip_errorlevel_t ControlSystemsOS::update(bool build) {
  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("=== [ NETWORK UPDATE ] ===\n"));
    DEBUG_DELAY();
  #endif
  // 1. Scan for Modules
  for(uint8_t wirenum = 0; wirenum < I2CIP_NUM_WIRES; wirenum++) {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("== [ WIRE "));
      CSOS_DEBUG_SERIAL.print(wirenum+1, HEX);
      CSOS_DEBUG_SERIAL.print(" / ");
      CSOS_DEBUG_SERIAL.print(I2CIP_NUM_WIRES, HEX);
      CSOS_DEBUG_SERIAL.print(_F("] ==\n"));
      DEBUG_DELAY();
    #endif
    
    for(uint8_t modnum = 0; modnum < I2CIP_MUX_COUNT; modnum++) {
      errlev = update(wirenum, modnum, build || csos_modules[wirenum][modnum] == nullptr);
      I2CIP_ERR_BREAK(errlev);
    }
  }

  return errlev;
}

// Control systems update
i2cip_errorlevel_t ControlSystemsOS::fixedUpdate(unsigned long timestamp, CSOSModule& m) {
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("= [ CONTROLSYSTEMS UPDATE: MODULE "));
    CSOS_DEBUG_SERIAL.print(m.getModuleNum(), HEX);
    CSOS_DEBUG_SERIAL.print(_F(" ] =\n"));
  #endif

  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;

  // 1. FSM Timer Functionality
  Chronos.set(timestamp);

  // 2. Control Systems Fixed Update per-ID
  for(uint8_t i = 0; i < MAP_INDEX_COUNT; i++) {
    const char* id = getDeviceID(i);

    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("-> DeviceGroup '"));
      CSOS_DEBUG_SERIAL.print(id);
      CSOS_DEBUG_SERIAL.print(_F("'\n"));
      DEBUG_DELAY();
    #endif

    DeviceGroup* dg = m[id];
    if(dg == nullptr) {
      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("-> Group '"));
        CSOS_DEBUG_SERIAL.print(id);
        CSOS_DEBUG_SERIAL.print(_F("' DNE! Check Libraries.\n"));
        DEBUG_DELAY();
      #endif
      continue;
    }

    for(uint8_t j = 0; j < dg->numdevices; j++) {
      Device* device = dg->devices[j];
      if(device == nullptr) break;

      if(device->getFQA() == ((const EEPROM&)m).getFQA()) {
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> SPRT EEPROM! Skipping...\n"));
          DEBUG_DELAY();
        #endif
        continue;
      }

      #ifdef CSOS_DEBUG_SERIAL
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("-> Updating Device "));
        CSOS_DEBUG_SERIAL.print(j+1);
        CSOS_DEBUG_SERIAL.print(" / ");
        CSOS_DEBUG_SERIAL.print(dg->numdevices);
        CSOS_DEBUG_SERIAL.print("\n");
        DEBUG_DELAY();
      #endif

      if(m[*device] == nullptr) {
        #ifdef CSOS_DEBUG_SERIAL
          DEBUG_DELAY();
          CSOS_DEBUG_SERIAL.print(_F("-> Device Not In BST! Deleting...\n"));
          DEBUG_DELAY();
        #endif
        m.remove(device);
        continue;
      }

      errlev = m(*device, true);

      #ifdef CSOS_DEBUG_SERIAL
        switch(errlev) {
          case I2CIP_ERR_NONE:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x0 Pass!\n"));
            DEBUG_DELAY();
            break;
          case I2CIP_ERR_SOFT:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x1 Communication Error!\n"));
            DEBUG_DELAY();
            break;
          case I2CIP_ERR_HARD:
            DEBUG_DELAY();
            CSOS_DEBUG_SERIAL.print(_F("-> 0x2 Hardware Lost, ABORT!\n"));
            DEBUG_DELAY();
            break;
        }
      #endif

      if(errlev == I2CIP_ERR_HARD) return errlev;

      // Sensor JSON Output
      if(device->getInput() != nullptr && device->getInput()->getSensor() != nullptr) {
        StaticJsonDocument<100> data;
        data["id"] = device->getID();
        data["timestamp"] = timestamp;
        data.createNestedObject("data");

        Datum* datum = device->getInput()->getSensor()->datumFactory();
        do {
          if(datum == nullptr) break;

          JsonObject d = data["data"].as<JsonObject>();

          datum->addToJSON(d);

          if(datum->next != nullptr) {
            Serial.print(',');
          }
          datum = datum->next;
        } while(datum != nullptr);
        size_t l = serializeJson(data, Serial);
        Serial.print(l);
        Serial.println(F(" Bytes"));
        // data.remove("id");
        // data.remove("timestamp");
        // data.remove("data");
        data.garbageCollect();
        delay(10);
      } 
      #ifdef CSOS_DEBUG_SERIAL
      else {
        DEBUG_DELAY();
        CSOS_DEBUG_SERIAL.print(_F("-> No Sensor, Skipping!\n"));
        DEBUG_DELAY();
      }
      #endif
    }
  }

  return errlev;

  // RPi: "air-temperature" -> Variable-Sensor(s) Linker -> Sensors (i.e. SHT31_Temperature)
  // uC: "sht31" -> ID-Interface Linker -> FQA[], SHT31 -> SHT31_Temperature::read(SHT31(fqa), nullptr, dest);
}

i2cip_errorlevel_t ControlSystemsOS::fixedUpdate(unsigned long timestamp) {
  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(_F("=== [ CONTROLSYSTEMS UPDATE T + "));
    // Chronograph::debugTimestamp(timestamp);
    CSOS_DEBUG_SERIAL.print(_F(" ] ===\n"));
    DEBUG_DELAY();
  #endif

  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;

  // 1. Scan for Modules
  for(uint8_t wirenum = 0; wirenum < I2CIP_NUM_WIRES; wirenum++) {
    #ifdef CSOS_DEBUG_SERIAL
      DEBUG_DELAY();
      CSOS_DEBUG_SERIAL.print(_F("== [ WIRE "));
      CSOS_DEBUG_SERIAL.print(wirenum+1, HEX);
      CSOS_DEBUG_SERIAL.print(" / ");
      CSOS_DEBUG_SERIAL.print(I2CIP_NUM_WIRES, HEX);
      CSOS_DEBUG_SERIAL.print(_F("] ==\n"));
      DEBUG_DELAY();
    #endif
    
    for(uint8_t modnum = 0; modnum < I2CIP_MUX_COUNT; modnum++) {
      CSOSModule* m = csos_modules[wirenum][modnum];
      if(m == nullptr) continue;
      errlev = fixedUpdate(timestamp, *m);
      
      if(errlev == I2CIP_ERR_HARD) return errlev;
    }
  }

  return errlev;

}

// void ControlSystemsOS::deleteDevice(Device* device) {
//   if(device == nullptr) return;

//   // Pick module from FQA
//   uint8_t wirenum = I2CIP_FQA_SEG_I2CBUS(device->getFQA());
//   uint8_t modnum = I2CIP_FQA_SEG_MODULE(device->getFQA());
//   CSOSModule* m = csos_modules[wirenum][modnum];
//   if(m == nullptr) return;

//   m->remove(device);
// }

// template <typename G, typename A> i2cip_errorlevel_t ControlSystemsOS::handleInputDevice(Device* device, const A& args) {
//   InputGetter* input = device->getInput();
//   if(input == nullptr) return I2CIP_ERR_SOFT;

//   InputInterface<G, A>* interface = (InputInterface<G, A>*) input;
//   G value = interface->getCache();
//   i2cip_errorlevel_t errlev = interface->get(value, args);
//   Serial.print("Value: ");
//   Serial.print(value);
//   return errlev;
// }

// template <typename S, typename B>  i2cip_errorlevel_t ControlSystemsOS::handleOutputDevice(Device* device, const S& value, const B& args) {
//   OutputSetter* output = device->getOutput();
//   if(output == nullptr) return I2CIP_ERR_SOFT;

//   OutputInterface<S, B>* interface = (OutputInterface<S, B>*) output;
//   i2cip_errorlevel_t errlev = interface->set(value, args);
//   return errlev;
// }