#ifndef PEAPOD_AEROPONICS_H_
#define PEAPOD_AEROPONICS_H_

#include <peapod.h>

#define PEAPOD_MODULE_AEROPONICS_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"MCP23017\":[" STR(I2CIP_MCP23XXX_ADDRESS) "," STR(I2CIP_MCP23XXX_ADDRESS + 1) "],\"ADS1115\":[" STR(I2CIP_ADS1115_ADDRESS) "]}]"}
#define PEAPOD_MODULE_AEROPONICS_GPIO_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_AEROPONICS, 0, I2CIP_MCP23XXX_ADDRESS)
#define PEAPOD_MODULE_AEROPONICS_GPIO_PUMP PIN_B1 // Uncomment to link pump relay to MCP23017 GPIO
#define PEAPOD_MODULE_AEROPONICS_GPIO_SOLENOID PIN_B0 // Uncomment to link solenoid relay to MCP23017 GPIO
#define PEAPOD_MODULE_AEROPONICS_ADC_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_AEROPONICS, 0, I2CIP_ADS1115_ADDRESS)
#define PEAPOD_MODULE_AEROPONICS_ADC_PRESSURE ADS1115_CHANNEL_0 // Uncomment to link pressure sensor to ADC channel 0
#define PEAPOD_MODULE_AEROPONICS_GPIOFLOW_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_AEROPONICS, 0, I2CIP_MCP23XXX_ADDRESS + 1)
#define PEAPOD_MODULE_AEROPONICS_PRESSURE_DELTA 100
#define PEAPOD_MODULE_AEROPONICS_FLOW_DELTA 100
#define PEAPOD_MODULE_AEROPONICS_WATERING_DELTA 600000 // 10 minutes
#define PEAPOD_MODULE_AEROPONICS_WATERING_PHASE 10000 // 10 seconds
#define PEAPOD_MODULE_AEROPONICS_PRESSURE_THRESHOLD 110

namespace PeaPod {
  
  extern FSM::Flag enable_pump;
  extern FSM::Flag enable_watering;
  extern FSM::Variable aeroponics_pressure;
  extern FSM::Variable aeroponics_flow;
  extern FSM::Variable aeroponics_temperature;

  void callback_aeroponics_pressure(bool _, const FSM::Number& v);
  void callback_aeroponics_flow(bool _, const FSM::Number& v);
  void callback_aeroponics_temperature(bool _, const FSM::Number& v);

  class PeaPodModuleAeroponics : public PeaPodModule {
    FSM::IntervalCallback* interval_watering_on = nullptr;
    FSM::IntervalCallback* interval_watering_off = nullptr;
    FSM::IntervalCallback* interval_read_pressure = nullptr;
    FSM::IntervalCallback* interval_read_flow = nullptr;

    template <i2cip_fqa_t F, i2cip_ads1115_chsel_t C> static void callback_ads1115_pressure(bool _, const FSM::fsm_timestamp_t& __) {
      if(C < ADS1115_CHANNEL_0 || C > ADS1115_CHANNEL_3) return;
      Device** dptr = I2CIP::devicetree[F];
      if(dptr == nullptr) return;
      ADS1115* adc = (ADS1115*)(*dptr);

      WaterPressure wp(adc, C);
      float pressure = 0.0f;
      i2cip_errorlevel_t errlev_wp = wp.get(pressure);
      
      if(errlev_wp == I2CIP_ERR_NONE) {
        aeroponics_pressure.set(pressure);
      }
    }

    template <i2cip_fqa_t F> static void callback_mcp23017_cd4040_waterFlow(bool _, const FSM::fsm_timestamp_t& __) {
      Device** dptr = I2CIP::devicetree[F];
      if(dptr == nullptr) return;
      MCP23017* mcp = (MCP23017*)(*dptr);

      WaterFlow wf(mcp);
      float flow = 0;
      i2cip_errorlevel_t errlev_wf = wf.get(flow);
      if(errlev_wf == I2CIP_ERR_NONE) {
        aeroponics_flow.set(flow);
      }
    }

    public:
      PeaPodModuleAeroponics(bool chronoCallbacks = true);

      ~PeaPodModuleAeroponics();

      i2cip_errorlevel_t initialize() override {
        enable_pump.set(true);
        enable_watering.set(false);

        i2cip_errorlevel_t errlev_adc = modules[PEAPOD_MODULENUM_AEROPONICS]->operator()<ADS1115>(PEAPOD_MODULE_AEROPONICS_ADC_FQA, false, _i2cip_args_io_default, DebugJsonOut);

        i2cip_errorlevel_t errlev_mcp = modules[PEAPOD_MODULENUM_AEROPONICS]->operator()<MCP23017>(PEAPOD_MODULE_AEROPONICS_GPIOFLOW_FQA, false, _i2cip_args_io_default, DebugJsonOut);

        return (errlev_adc != I2CIP_ERR_NONE) ? errlev_adc : errlev_mcp;
      }; 
  };
}

#endif