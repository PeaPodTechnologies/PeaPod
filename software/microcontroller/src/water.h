#ifndef PEAPOD_WATER_H_
#define PEAPOD_WATER_H_

#include <peapod.h>

#define PEAPOD_MODULE_WATERING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"MCP23017\":[" STR(I2CIP_MCP23017_ADDRESS) "]}]"}
#define PEAPOD_MODULE_WATERING_GPIO_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_WATERING, 0, I2CIP_MCP23017_ADDRESS)
// #define PEAPOD_MODULE_WATERING_GPIO_PUMP PIN_A0 // Uncomment to link pump to MCP23017
#define PEAPOD_MODULE_WATERING_GPIO_SOLENOID PIN_B0
#define PEAPOD_MODULE_WATERING_DELTA 600000 // 10 minutes
#define PEAPOD_MODULE_WATERING_PHASE 10000 // 10 seconds

namespace PeaPod {
  
  extern FSM::Flag enable_pump;
  extern FSM::Flag enable_watering;

  class PeaPodModuleWatering : public PeaPodModule {
    FSM::IntervalCallback* interval_watering_on = nullptr;
    FSM::IntervalCallback* interval_watering_off = nullptr; 

    public:
      PeaPodModuleWatering(bool chronoCallbacks = true);

      ~PeaPodModuleWatering();
  };
}

// #include <water.tpp>

#endif