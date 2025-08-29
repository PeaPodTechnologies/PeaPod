#include <peapod.h>

bool PeaPod::pinModeSet[255] = { false };

using namespace PeaPod;
using namespace I2CIP;

PeaPodModule::PeaPodModule() : JsonModule(PEAPOD_WIRENUM, PEAPOD_MODULENUM) { }

DeviceGroup* PeaPodModule::deviceGroupFactory(const i2cip_id_t& id) {
  DeviceGroup* dg = DeviceGroup::create<EEPROM>(id);
  if(dg != nullptr) return dg;
  dg = DeviceGroup::create<SHT45>(id);
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
  // TODO: Implement - something with EEPROM? or FSM?
}