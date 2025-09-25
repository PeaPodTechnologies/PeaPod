#ifndef PEAPOD_WATER_H_
#define PEAPOD_WATER_H_

#include <peapod.h>

#define PEAPOD_MODULE_WATERING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"MCP23017\":[" STR(I2CIP_MCP23017_ADDRESS) "]}]"}

namespace PeaPod {

  class PeaPodModuleWatering : public PeaPodModule {
    public:
      PeaPodModuleWatering(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_WATERING) {
        if(chronoCallbacks) {
          // TODO
        }
      }
  };
}

// #include <water.tpp>

#endif