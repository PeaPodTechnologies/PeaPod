#include <peapod.h>

#include <air.h>
#include <water.h>
#include <light.h>

#include <DebugJson.h>

FSM::Variable PeaPod::cycle = FSM::Variable(FSM::Number(0, false, false), "cycle");
FSM::Variable PeaPod::fps = FSM::Variable(FSM::Number(0, false, false), "fps");
bool PeaPod::pinModeSet[255] = { false };

using namespace PeaPod;
using namespace I2CIP;

DeviceGroup* PeaPodModule::deviceGroupFactory(const i2cip_id_t& id) {
  DeviceGroup* dg = DeviceGroup::create<EEPROM>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<SHT45>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<K30>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<HT16K33>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<PCA9685>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<JHD1313>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<RotaryEncoder>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<MCP23017>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<Nunchuck>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<ADS1115>(id);
  return dg;
}

void PeaPodModule::handleCommand(JsonObject command, Print& out) {
  JsonDocument doc;
  doc["timestamp"] = millis();

  i2cip_fqa_t fqa = command["fqa"].as<i2cip_fqa_t>();
  
  Device** dptr = I2CIP::devicetree[fqa];
  if(dptr != nullptr && *dptr != nullptr) {

    Device* d = *dptr;

    DeviceGroup* dg = this->operator[](d->getID());

    if(dg != nullptr && dg->handler != nullptr) {
      i2cip_args_io_t args = _i2cip_args_io_default;

      JsonVariant argsG = command["g"];
      JsonVariant argsA = command["a"];
      JsonVariant argsS = command["s"];
      JsonVariant argsB = command["b"];
      
      dg->handler(args, argsA, argsS, argsB);

      // DebugJson::StringWriter sw;
      // i2cip_errorlevel_t errlev = this->operator()(d, true, args, sw);
      unsigned long start = millis();
      String msg = String(d->getID()) + ' ' + fqaToString(fqa) + ' ';
      bool spacer = false;

      i2cip_errorlevel_t errlev = MUX::setBus(fqa);
      if(errlev == I2CIP_ERR_NONE) {
        if(d->getOutput() != nullptr && !argsS.isNull()) {
          errlev = d->set(args.s, args.b);

          // Print output cache
          msg += "OUTSET ";
          msg += d->getOutput()->valueToString();
          spacer = true;
        }
        if(errlev == I2CIP_ERR_NONE && d->getInput() != nullptr && !argsG.isNull()) {
          errlev = d->get(args.a);

          // Print input cache
          if(spacer) msg += "; ";
          else spacer = true;
          msg += "INPGET ";
          msg += d->getInput()->printCache();

          if(errlev == I2CIP_ERR_NONE) {
            DebugJson::telemetryJsonString(d->getInput()->getLastRX(), d->getInput()->cacheToString());
          }
        }
        if(errlev == I2CIP_ERR_NONE && argsG.isNull() && argsS.isNull()) {
          errlev = d->pingTimeout(false, true);
          msg = "PING";
        }
      }

      dg->cleanup(args);

      msg += " DELTA ";
      msg += String(millis() - start);
      msg += "ms";

      msg += errlev == I2CIP_ERR_NONE ? " OK" : (errlev == I2CIP_ERR_SOFT ? " EINVAL" : " EIO");


      // TODO: Print to out with sw, errlev, timestamp
      doc["type"] = "info";
      doc["id"] = d->getID();
      doc["fqa"] = d->getFQA();
      doc["errlev"] = errlev;
      // doc["msg"] = sw.operator String();
      doc["msg"] = msg;
    } else {
      doc["type"] = "error";
      doc["msg"] = "LIBRARY ENOENT";
      doc["errlev"] = I2CIP_ERR_SOFT;
    }
  } else {
    doc["type"] = "error";
    doc["msg"] = "DEVICE ENOENT";
    doc["errlev"] = I2CIP_ERR_SOFT;
  }

  DebugJson::jsonPrintln(doc, out);
}

void PeaPodModule::handleConfig(JsonObject config, Print& out) { 
  JsonDocument doc;
  doc["timestamp"] = millis();
  doc["type"] = "config";

  bool list = false;
  for (JsonPair kv : config) {
    const char* key = kv.key().c_str();

    if(strcmp(key, "timestamp") == 0) continue; // Reserved key
    if(strcmp(key, "list") == 0) { list = true; continue; } // Reserved key

    if(!doc["data"][key].isNull()) continue; // Already handled

    bool* locked = locker[key];
    if(locked && *locked) continue; // Locked, skip

    JsonVariant value = kv.value();

    FSM::Flag* flag = flags[key];
    if(flag != nullptr) {
      if(value.is<bool>()) {
        flag->set(value.as<bool>());
      }
      doc["data"][key] = flag->get();
      continue;
    }

    FSM::Variable* variable = variables[key];
    if(variable != nullptr) {
      if(value.is<unsigned>()) {
        variable->set(FSM::Number(value.as<double>(), false, false));
      } else if(value.is<int>()) {
        variable->set(FSM::Number(value.as<double>(), false, true));
      } else if(value.is<float>()) {
        variable->set(FSM::Number(value.as<double>(), true, true));
      }
      doc["data"][key] = variable->get().operator double();
      continue;
    }
  }

  if(list) {
    String* flagkeys = flags.keys();
    uint8_t flagcount = flags.size();

    for(uint8_t i = 0; i < flagcount; i++) {
      if(doc["data"][flagkeys[i]].isNull()) doc["data"][flagkeys[i]] = flags[flagkeys[i].c_str()]->get();
    }

    delete[] flagkeys;

    String* varkeys = variables.keys();
    uint8_t varcount = variables.size();

    for(uint8_t i = 0; i < varcount; i++) {
      if(doc["data"][varkeys[i]].isNull()) doc["data"][varkeys[i]] = variables[varkeys[i].c_str()]->get().operator double();
    }

    delete[] varkeys;
  }

  DebugJson::jsonPrintln(doc, out);
}

void PeaPodModule::registerFlag(FSM::Flag* flag, bool locked) {
  this->flags.set(flag->getKey(), flag);
  this->locker.set(flag->getKey(), new bool(locked)); // TODO: Memory leak?
}
void PeaPodModule::registerVariable(FSM::Variable* variable, bool locked) {
  this->variables.set(variable->getKey(), variable);
  this->locker.set(variable->getKey(), new bool(locked)); // TODO: Memory leak?
}

FSM::Flag* PeaPodModule::getFlagByIndex(uint8_t index) const {
  return this->flags.getByFirstOccurrence(index);
}
FSM::Variable* PeaPodModule::getVariableByIndex(uint8_t index) const {
  return this->variables.getByFirstOccurrence(index);
}

void PeaPod::configRouter(JsonObject command, Print& out) {
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleConfig(command, out);
    }
  }
}

void PeaPod::registerCallbacks(void)  {
  cycle.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_cycle);

  FSM::Chronos.addInterval(PEAPOD_DELTA_HEARTBEAT, PeaPod::callback_heartbeat);
  
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_AIR, PeaPodModuleAir>);
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_WATERING, PeaPodModuleWatering>);
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_LIGHTING, PeaPodModuleLighting>);
}

unsigned long last = 0;
void PeaPod::callback_cycle(bool _, const FSM::Number& __) {
  FSM::Chronos.set(millis());

  unsigned long delta = millis() - last;
  PeaPod::fps.set(((unsigned)PeaPod::fps.get() + 1000.f / max(1.f, (float)delta))/2); // Old FPS plus new FPS over two (moving average)
  last = millis();

  while(Serial.available() > 0) { // With baud 115200, this should not block
    DebugJson::update(Serial, I2CIP::commandRouter, PeaPod::configRouter);
  }
}

void PeaPod::callback_heartbeat(bool _, const FSM::fsm_timestamp_t& __) {
  DebugJson::heartbeat(millis(), Serial);
  DebugJson::revision(I2CIP_REVISION, Serial);
  DebugJson::telemetry(millis(), (unsigned)PeaPod::fps.get(), "fps", Serial);
  DebugJson::telemetry(millis(), (unsigned)PeaPod::cycle.get(), "cycle", Serial);
}

template <unsigned char M, class T, typename std::enable_if<std::is_base_of<PeaPod::PeaPodModule, T>::value, int>::type = 0> void PeaPod::callback_module(bool _, const FSM::fsm_timestamp_t& __) {
  if(M >= I2CIP_MUX_COUNT) return;

  if(I2CIP::MUX::pingMUX(PEAPOD_WIRENUM, M)) {
    if(I2CIP::modules[M] == nullptr) {
      I2CIP::modules[M] = new T();

      // First Module - Add HT16K33
      if(M == 0) {
        I2CIP::modules[0]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, _i2cip_args_io_default, NullStream);
      }
    }

    I2CIP::errlev[M] = I2CIP::modules[M]->operator()();
  } else {
    I2CIP::errlev[M] = I2CIP_ERR_HARD;
  }

  if(I2CIP::modules[M] != nullptr && I2CIP::errlev[M] == I2CIP_ERR_HARD) {
    delete I2CIP::modules[M];
    I2CIP::modules[M] = nullptr;
  }
}