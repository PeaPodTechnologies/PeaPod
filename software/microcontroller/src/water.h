#ifndef PEAPOD_WATER_H_
#define PEAPOD_WATER_H_

#include <peapod.h>

#define PEAPOD_MODULE_WATERING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"MCP23017\":[" STR(I2CIP_MCP23017_ADDRESS) "]}]"}
#define PEAPOD_MODULE_WATERING_GPIO_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_WATERING, 0, I2CIP_MCP23017_ADDRESS)
#define PEAPOD_MODULE_WATERING_GPIO_PIN PIN_A0
#define PEAPOD_MODULE_WATERING_DELTA 1800000 // 30 minutes
#define PEAPOD_MODULE_WATERING_PHASE 5000 // 5 seconds

namespace PeaPod {

  class PeaPodModuleWatering : public PeaPodModule {
    FSM::Flag flag_watering = FSM::Flag("flag_watering");
    FSM::IntervalCallback* interval_watering_on = nullptr;
    FSM::IntervalCallback* interval_watering_off = nullptr;

    public:
      PeaPodModuleWatering(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_WATERING) {
        if(chronoCallbacks) {
          interval_watering_on = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_WATERING_DELTA, 0, &flag_watering, false);
          interval_watering_off = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_WATERING_DELTA, PEAPOD_MODULE_WATERING_PHASE, &flag_watering, true);

          flag_watering.addLatchingConditional(true, false, Callbacks::callback_mcp23017_digitalWrite<PEAPOD_MODULE_WATERING_GPIO_FQA, PEAPOD_MODULE_WATERING_GPIO_PIN>);
        }
      }

      ~PeaPodModuleWatering() {
        FSM::Chronos.removeInterval(interval_watering_on);
        FSM::Chronos.removeInterval(interval_watering_off);
        interval_watering_on = nullptr;
        interval_watering_off = nullptr;
      }
  };
}

// #include <water.tpp>

#endif