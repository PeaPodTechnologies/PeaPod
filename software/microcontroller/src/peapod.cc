#include <peapod.h>

#include <air.h>
#include <water.h>
#include <light.h>

#include <DebugJson.h>

FSM::Variable PeaPod::cycle = FSM::Variable(FSM::Number(0, false, false), "cycle");
FSM::Variable PeaPod::fps = FSM::Variable(FSM::Number(0, false, false), "fps");
bool PeaPod::pinModeSet[255] = { false };
PeaPod::PeaPodNoModule PeaPod::notamodule(I2CIP_WIRENUM_PRIMARY);

using namespace PeaPod;
using namespace I2CIP;

DeviceGroup* PeaPodDeviceGroupsInterface::deviceGroupFactory(const i2cip_id_t& id) {
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
  // dg = DeviceGroup::create<JHD1313>(id);
  // if(dg != nullptr) return dg;
  dg = DeviceGroup::create<MCP23008>(id);
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

PeaPodModule::~PeaPodModule() { }

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
  doc["errlev"] = I2CIP_ERR_NONE; // TODO: better error level indication

  bool list = false;
  for (JsonPair kv : config) {
    const char* key = kv.key().c_str();

    if(strcmp(key, "timestamp") == 0) continue; // Reserved key
    if(strcmp(key, "list") == 0) { list = true; continue; } // Reserved key

    if(!doc["data"][key].isNull()) continue; // Already handled

    JsonVariant value = kv.value();

    FSM::Flag* flag = flags[key];
    if(flag != nullptr) {
      bool* locked = locker_flags[key];
      if(locked && *locked) continue; // Locked, skip
      if(value.is<bool>()) {
        flag->set(value.as<bool>());
      }
      doc["data"][key] = flag->get();
      continue;
    }

    FSM::Variable* variable = variables[key];
    if(variable != nullptr) {
      bool* locked = locker_variables[key];
      if(locked && *locked) continue; // Locked, skip
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

void PeaPodModule::handleInterval(JsonObject interval, Print& out) {
  JsonDocument doc;
  doc["timestamp"] = millis();
  doc["type"] = "interval";
  
  FSM::IntervalCallback* theInterval = nullptr;
  bool interval_set = false;
  FSM::Flag* flag = nullptr;
  
  bool invert = false; // OFF by default
  // bool flagstate_set = false;
  FSM::fsm_timestamp_t phase = 0;
  FSM::fsm_timestamp_t delta = 0;
  bool phase_set = false;
  bool delta_set = false;
  
  bool list = false;
  for (JsonPair kv : interval) {
    const char* key = kv.key().c_str();

    if(strcmp(key, "timestamp") == 0) continue; // Reserved key
    if(strcmp(key, "list") == 0) { list = true; continue; } // Reserved key

    if(!doc["data"][key].isNull()) continue; // Already handled

    JsonVariant value = kv.value();

    if(strcmp(key, "flag") == 0) {
      flag = flags[value.as<const char*>()];
      continue;
    }

    if(strcmp(key, "interval") == 0) {
      theInterval = intervals[value.as<const char*>()];
      interval_set = true;
      continue;
    }

    if(strcmp(key, "delta") == 0) {
      delta = value.as<FSM::fsm_timestamp_t>();
      delta_set = true;
      continue;
    }

    if(strcmp(key, "phase") == 0) {
      phase = value.as<FSM::fsm_timestamp_t>();
      phase_set = true;
      continue;
    }

    if(strcmp(key, "invert") == 0) {
      invert = value.as<bool>();
      // flagstate_set = true;
      continue;
    }
  }

  if(theInterval != nullptr) {
    // Interval exists
    bool* locked = locker_intervals[theInterval->getKey()];
    if(locked == nullptr || !*locked) {
      if(delta_set) theInterval->setDelta(delta);
      if(phase_set) theInterval->setPhase(phase);
    }
    doc["data"]["key"] = theInterval->getKey();
    doc["data"]["delta"] = theInterval->getDelta();
    doc["data"]["phase"] = theInterval->getPhase();
    doc["data"]["invert"] = theInterval->getInvert();
    if(flag != nullptr) doc["data"]["flag"] = flag->getKey();
    doc["msg"] = "INTERVAL UPDATE";
  } else if(flag != nullptr && !interval_set && delta_set && phase_set) {
    // Interval does not yet exist and was not requested, create it (matching flag key)
    bool* locked = locker_flags[flag->getKey()];
    if(locked == nullptr || !*locked) {
      String newkey = "i_" + String(flag->getKey()) + "_" + String(millis() % 1000000);
      FSM::IntervalCallback* newInterval = FSM::Chronos.addIntervalFlag(newkey.c_str(), delta, phase, flag, invert); // Invert defaults to false unless specified true
      intervals.set(newInterval->getKey(), newInterval); // Add to hashtable
      doc["data"]["key"] = newkey;
      doc["data"]["delta"] = delta;
      doc["data"]["phase"] = phase;
      doc["data"]["invert"] = invert;
    }
    doc["msg"] = "INTERVAL CREATE";
  } else if (!list) {
    // Neither interval nor flag, ENOENT
    doc["type"] = "error";
    doc["msg"] = "INTERVAL EINVAL";
    doc["errlev"] = I2CIP_ERR_SOFT;
    DebugJson::jsonPrintln(doc, out);
    return; // Stop further processing since the interval was not found
  }

  if(list) {
    String* intervalkeys = intervals.keys();
    uint8_t intervalcount = intervals.size();

    for(uint8_t i = 0; i < intervalcount; i++) {
      if(doc["data"][intervalkeys[i]].isNull()) {
        doc["data"][intervalkeys[i]]["phase"] = intervals[intervalkeys[i].c_str()]->getPhase();
        doc["data"][intervalkeys[i]]["delta"] = intervals[intervalkeys[i].c_str()]->getDelta();
      }
    }

    delete[] intervalkeys;
  }

  doc["errlev"] = I2CIP_ERR_NONE;

  DebugJson::jsonPrintln(doc, out);
}

void PeaPodModule::handleEvent(JsonObject event, Print& out) {
  JsonDocument doc;
  doc["timestamp"] = millis();
  doc["type"] = "event";

  FSM::ChronoEvent* theEvent = nullptr;
  bool event_set = false;
  FSM::Flag* flag = nullptr;
  
  bool invert = false; // OFF by default
  // bool flagstate_set = false;
  FSM::fsm_timestamp_t delta = 0;
  bool delta_set = false;
  
  bool list = false;
  for (JsonPair kv : event) {
    const char* key = kv.key().c_str();

    if(strcmp(key, "timestamp") == 0) continue; // Reserved key
    if(strcmp(key, "list") == 0) { list = true; continue; } // Reserved key

    if(!doc["data"][key].isNull()) continue; // Already handled

    JsonVariant value = kv.value();

    if(strcmp(key, "flag") == 0) {
      flag = flags[value.as<const char*>()];
      continue;
    }

    if(strcmp(key, "event") == 0) {
      theEvent = events[value.as<const char*>()];
      event_set = true;
      continue;
    }

    if(strcmp(key, "delta") == 0) {
      delta = value.as<FSM::fsm_timestamp_t>();
      delta_set = true;
      continue;
    }

    if(strcmp(key, "invert") == 0) {
      invert = value.as<bool>();
      // flagstate_set = true;
      continue;
    }
  }

  if(theEvent != nullptr) {
    // Interval exists, don't change behaviour
    bool* locked = locker_intervals[theEvent->getKey()];
    if(locked == nullptr && !*locked) {
      if(delta_set) theEvent->setTimestamp(delta);
    }
    doc["data"]["key"] = theEvent->getKey();
    doc["data"]["delta"] = theEvent->getTimestamp();
    doc["data"]["invert"] = theEvent->getInvert();
    if(flag != nullptr) doc["data"]["flag"] = flag->getKey();
    doc["msg"] = "EVENT UPDATE";
  } else if(flag != nullptr && !event_set && delta_set) {
    // Interval does not yet exist and was not requested, create it (matching flag key)
    bool* locked = locker_flags[flag->getKey()];
    if(locked == nullptr || !*locked) {
      String newkey = "e_" + String(flag->getKey()) + "_" + String(millis() % 1000000);
      FSM::ChronoEvent* newEvent = FSM::Chronos.addEventFlag(newkey.c_str(), delta, flag, invert); // Invert defaults to false unless specified true
      events.set(newEvent->getKey(), newEvent);
      doc["data"]["key"] = newkey;
      doc["data"]["delta"] = delta;
      doc["data"]["invert"] = invert;
    }
    doc["msg"] = "EVENT CREATE";
  } else if (!list) {
    // Neither event nor flag, ENOENT
    doc["type"] = "error";
    doc["msg"] = "EVENT EINVAL";
    doc["errlev"] = I2CIP_ERR_SOFT;
    DebugJson::jsonPrintln(doc, out);
    return; // Stop further processing since the event was not found
  }

  if(list) {
    String* eventkeys = events.keys();
    uint8_t eventcount = events.size();

    for(uint8_t i = 0; i < eventcount; i++) {
      if(doc["data"][eventkeys[i]].isNull()) {
        doc["data"][eventkeys[i]] = events[eventkeys[i].c_str()]->getTimestamp();
      }
    }

    delete[] eventkeys;
  }

  doc["errlev"] = I2CIP_ERR_NONE;

  DebugJson::jsonPrintln(doc, out);
}

void PeaPodModule::registerFlag(FSM::Flag* flag, bool locked) {
  if(flag == nullptr) return;
  this->flags.set(flag->getKey(), flag);
  this->locker_flags.set(flag->getKey(), new bool(locked)); // TODO: Memory leak?
}
void PeaPodModule::registerVariable(FSM::Variable* variable, bool locked) {
  if(variable == nullptr) return;
  this->variables.set(variable->getKey(), variable);
  this->locker_variables.set(variable->getKey(), new bool(locked)); // TODO: Memory leak?
}
void PeaPodModule::registerInterval(FSM::IntervalCallback* interval, bool locked) {
  if(interval == nullptr) return;
  this->intervals.set(interval->getKey(), interval);
  this->locker_intervals.set(interval->getKey(), new bool(locked)); // TODO: Memory leak?
}
void PeaPodModule::registerEvent(FSM::ChronoEvent* event, bool locked) {
  if(event == nullptr) return;
  this->events.set(event->getKey(), event);
  this->locker_events.set(event->getKey(), new bool(locked)); // TODO: Memory leak?
}

void PeaPodModule::unregisterFlag(FSM::Flag* flag) {
  if(flag == nullptr) return;
  this->flags.remove(flag->getKey());
  this->locker_flags.remove(flag->getKey());
}
void PeaPodModule::unregisterVariable(FSM::Variable* variable) {
  if(variable == nullptr) return;
  this->variables.remove(variable->getKey());
  this->locker_variables.remove(variable->getKey());
}
void PeaPodModule::unregisterInterval(FSM::IntervalCallback* interval) {
  if(interval == nullptr) return;
  this->intervals.remove(interval->getKey());
  this->locker_intervals.remove(interval->getKey());
}
void PeaPodModule::unregisterEvent(FSM::ChronoEvent* event) {
  if(event == nullptr) return;
  this->events.remove(event->getKey());
  this->locker_events.remove(event->getKey());
}

FSM::Flag* PeaPodModule::getFlagByIndex(uint8_t index) const {
  return this->flags.getByFirstOccurrence(index);
}
FSM::Variable* PeaPodModule::getVariableByIndex(uint8_t index) const {
  return this->variables.getByFirstOccurrence(index);
}
FSM::IntervalCallback* PeaPodModule::getIntervalByIndex(uint8_t index) const {
  return this->intervals.getByFirstOccurrence(index);
}
FSM::ChronoEvent* PeaPodModule::getEventByIndex(uint8_t index) const {
  return this->events.getByFirstOccurrence(index);
}

void PeaPod::eventRouter(JsonObject event, Print& out) {
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleEvent(event, out);
    }
  }
}

void PeaPod::intervalRouter(JsonObject interval, Print& out) {
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleInterval(interval, out);
    }
  }
}

void PeaPod::configRouter(JsonObject command, Print& out) {
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleConfig(command, out);
    }
  }
}

void PeaPod::rebuildStates(Print& out) {
  JsonDocument doc;
  doc["list"] = true;
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleConfig(doc.as<JsonObject>(), out);
    }
  }
}
void PeaPod::rebuildEvents(Print& out) {
  JsonDocument doc;
  doc["list"] = true;
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleEvent(doc.as<JsonObject>(), out);
    }
  }
}

void PeaPod::rebuildIntervals(Print& out) {
  JsonDocument doc;
  doc["list"] = true;
  for(unsigned int i = 0; i < I2CIP_MUX_COUNT; i++) {
    if(I2CIP::modules[i] != nullptr) {
      I2CIP::modules[i]->handleInterval(doc.as<JsonObject>(), out);
    }
  }
}

void PeaPod::registerCallbacks(void)  {
  cycle.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_cycle);

  FSM::Chronos.addInterval(PEAPOD_DELTA_HEARTBEAT, PeaPod::callback_heartbeat);
  FSM::Chronos.addInterval(PEAPOD_DELTA_REFRESH, PeaPod::callback_rebuild);
  
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_AIR, PeaPodModuleAir>);
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_WATERING, PeaPodModuleWatering>);
  FSM::Chronos.addInterval(PEAPOD_DELTA_MODULECHECK, PeaPod::callback_module<PEAPOD_MODULENUM_LIGHTING, PeaPodModuleLighting>);
}

unsigned long last = 0;
void PeaPod::callback_cycle(bool _, const FSM::Number& cycle) {
  FSM::Chronos.set(millis());

  unsigned long delta = millis() - last;
  PeaPod::fps.set(((unsigned)PeaPod::fps.get() + 1000.f / max(1.f, (float)delta))/2); // Old FPS plus new FPS over two (moving average)
  last = millis();

  while(PEAPOD_SERIAL.available() > 0) { // With baud 115200, this should not block
    DebugJson::update(PEAPOD_SERIAL, I2CIP::commandRouter, PeaPod::configRouter, PeaPod::intervalRouter, PeaPod::eventRouter);
  }
}

void PeaPod::callback_heartbeat(bool _, const FSM::fsm_timestamp_t& __) {
  DebugJson::heartbeat(millis(), PEAPOD_SERIAL);
  DebugJson::revision(I2CIP_REVISION, PEAPOD_SERIAL);
  DebugJson::telemetry(millis(), (unsigned)PeaPod::fps.get(), "fps", PEAPOD_SERIAL);
  DebugJson::telemetry(millis(), (unsigned)PeaPod::cycle.get(), "cycle", PEAPOD_SERIAL);
}

void PeaPod::callback_rebuild(bool _, const FSM::fsm_timestamp_t& __) {
  I2CIP::rebuildTree(PEAPOD_SERIAL, true);
  rebuildStates(PEAPOD_SERIAL);
  rebuildIntervals(PEAPOD_SERIAL);
  rebuildEvents(PEAPOD_SERIAL);
}