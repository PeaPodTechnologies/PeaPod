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

  void callback_temperature(bool _, const FSM::Number& v);
  void callback_humidity(bool _, const FSM::Number& v);
  void callback_co2(bool _, const FSM::Number& v);

  class PeaPodModuleAir : public PeaPodModule {
    private:
      FSM::IntervalCallback* interval_sht45 = nullptr;
      FSM::IntervalCallback* interval_k30 = nullptr;
    public:
      PeaPodModuleAir(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_AIR) {
        if(chronoCallbacks) {
          interval_sht45 = FSM::Chronos.addInterval(PEAPOD_SHT45_DELTA, PeaPodModuleAir::callback_sht45_mean<PEAPOD_MODULENUM_AIR>);
          interval_k30 = FSM::Chronos.addInterval(PEAPOD_K30_DELTA, PeaPodModuleAir::callback_k30_mean<PEAPOD_MODULENUM_AIR>);
        }

        air_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_temperature);
        air_humidity.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_humidity);
        air_co2.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_co2);

        registerVariable(&air_temperature);
        registerVariable(&air_humidity);
        registerVariable(&air_co2);
      }

      ~PeaPodModuleAir() {
        FSM::Chronos.removeInterval(interval_sht45);
        FSM::Chronos.removeInterval(interval_k30);
        interval_sht45 = nullptr;
        interval_k30 = nullptr;
      }
      
      template <unsigned char M> static void callback_sht45_mean(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
      template <unsigned char M> static void callback_k30_mean(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
  };
}

#include <air.tpp>

#endif