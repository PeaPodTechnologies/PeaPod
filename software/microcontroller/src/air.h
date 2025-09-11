#ifndef PEAPOD_AIR_H_
#define PEAPOD_AIR_H_

#include <peapod.h>

#define PEAPOD_SHT45_DELTA 250
#define PEAPOD_K30_DELTA 3000

#define PEAPOD_MODULE_AIR_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"SHT45\":[" STR(I2CIP_SHT45_ADDRESS) "]},{\"SHT45\":[" STR(I2CIP_SHT45_ADDRESS) "],\"K30\":[" STR(I2CIP_K30_ADDRESS) "]},{\"SHT45\":[" STR(I2CIP_SHT45_ADDRESS) "]}]"}

namespace PeaPod {
  extern FSM::Variable air_temperature;
  extern FSM::Variable air_humidity;
  extern FSM::Variable air_co2;

  class PeaPodModuleAir : public PeaPodModule {
    public:
      PeaPodModuleAir(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_AIR) {
        if(chronoCallbacks) {
          FSM::Chronos.addInterval(PEAPOD_SHT45_DELTA, PeaPodModuleAir::callback_sht45_mean<PEAPOD_MODULENUM_AIR>);
          FSM::Chronos.addInterval(PEAPOD_K30_DELTA, PeaPodModuleAir::callback_k30_mean<PEAPOD_MODULENUM_AIR>);
        }
      }
      
      template <unsigned char M> static void callback_sht45_mean(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
      template <unsigned char M> static void callback_k30_mean(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
  };
}

#include <air.tpp>

#endif