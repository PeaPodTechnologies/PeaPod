#ifndef PEAPOD_LIGHT_H_
#define PEAPOD_LIGHT_H_

#include <peapod.h>

#define PEAPOD_MODULE_LIGHTING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"PCA9685\":[" STR(I2CIP_PCA9685_ADDRESS) "]}]"}

namespace PeaPod {

  class PeaPodModuleLighting : public PeaPodModule {
    public:
      PeaPodModuleLighting(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_LIGHTING) {
        if(chronoCallbacks) {
          // TODO
        }
      }
  };
}

// #include <water.tpp>

#endif