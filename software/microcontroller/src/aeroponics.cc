#include <aeroponics.h>

PeaPod::PeaPodModuleAeroponics::PeaPodModuleAeroponics(bool chronoCallbacks) : PeaPodModule(PEAPOD_MODULENUM_AEROPONICS) {
  if(chronoCallbacks) {
    #ifdef PEAPOD_PROGRAM_DEFAULT
      interval_watering_on = FSM::Chronos.addIntervalFlag("i_aeroponics_watering_on", PEAPOD_MODULE_AEROPONICS_WATERING_DELTA, 0, &enable_watering, false);
      interval_watering_off = FSM::Chronos.addIntervalFlag("i_aeroponics_watering_off", PEAPOD_MODULE_AEROPONICS_WATERING_DELTA, PEAPOD_MODULE_AEROPONICS_WATERING_PHASE, &enable_watering, true);
      registerInterval(interval_watering_on);
      registerInterval(interval_watering_off);
    #endif

    #ifdef PEAPOD_MODULE_AEROPONICS_ADC_FQA
    #ifdef PEAPOD_MODULE_AEROPONICS_ADC_PRESSURE
      interval_read_pressure = FSM::Chronos.addInterval("i_aeroponics_read_pressure", PEAPOD_MODULE_AEROPONICS_PRESSURE_DELTA, 0, callback_ads1115_pressure<PEAPOD_MODULE_AEROPONICS_ADC_FQA, PEAPOD_MODULE_AEROPONICS_ADC_PRESSURE>);
      registerInterval(interval_read_pressure);
    #endif
    #endif

    #ifdef PEAPOD_MODULE_AEROPONICS_GPIOFLOW_FQA
      interval_read_flow = FSM::Chronos.addInterval("i_aeroponics_read_flow", PEAPOD_MODULE_AEROPONICS_FLOW_DELTA, 0, callback_mcp23017_cd4040_waterFlow<PEAPOD_MODULE_AEROPONICS_GPIOFLOW_FQA>);
    #endif
  }

  aeroponics_pressure.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_aeroponics_pressure);
  aeroponics_pressure.addConditionalSetFlag(FSM::CMP_GTR, FSM::Number((float)PEAPOD_MODULE_AEROPONICS_PRESSURE_THRESHOLD), &enable_pump, true);

  aeroponics_flow.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_aeroponics_flow);

  aeroponics_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_aeroponics_temperature);
  
  #ifdef PEAPOD_MODULE_AEROPONICS_GPIO_PUMP
  enable_pump.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PEAPOD_MODULE_AEROPONICS_GPIO_FQA, PEAPOD_MODULE_AEROPONICS_GPIO_PUMP>);
  #endif

  #ifdef PEAPOD_MODULE_AEROPONICS_GPIO_SOLENOID
  enable_watering.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PEAPOD_MODULE_AEROPONICS_GPIO_FQA, PEAPOD_MODULE_AEROPONICS_GPIO_SOLENOID>);
  #endif

  registerFlag(&enable_watering);
  registerFlag(&enable_pump);
  registerVariable(&aeroponics_pressure, true);
  registerVariable(&aeroponics_flow, true);
  registerVariable(&aeroponics_temperature, true);
}

PeaPod::PeaPodModuleAeroponics::~PeaPodModuleAeroponics() {
  enable_watering.set(false);
  enable_pump.set(false);
  
  unregisterInterval(interval_watering_on);
  unregisterInterval(interval_watering_off);
  unregisterInterval(interval_read_pressure);
  unregisterInterval(interval_read_flow);
  FSM::Chronos.removeInterval(interval_watering_on);
  FSM::Chronos.removeInterval(interval_watering_off);
  FSM::Chronos.removeInterval(interval_read_pressure);
  FSM::Chronos.removeInterval(interval_read_flow);
  interval_watering_on = nullptr;
  interval_watering_off = nullptr;
  interval_read_pressure = nullptr;
  interval_read_flow = nullptr;
}

void PeaPod::callback_aeroponics_pressure(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (float)v, aeroponics_pressure.getKey());

  #ifdef PEAPOD_SEVENSEG_WATERPRES
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    notamodule.operator()<HT16K33>(fqa_sevenseg, true, seg_args, DebugJsonOut);
  }
  #endif
}

void PeaPod::callback_aeroponics_flow(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (float)v, aeroponics_flow.getKey());

  #ifdef PEAPOD_SEVENSEG_WATERFLOW
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    notamodule.operator()<HT16K33>(fqa_sevenseg, true, seg_args, DebugJsonOut);
  }
  #endif
}

void PeaPod::callback_aeroponics_temperature(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (float)v, aeroponics_temperature.getKey());

  #ifdef PEAPOD_SEVENSEG_WATERTEMP
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .h = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    notamodule.operator()<HT16K33>(fqa_sevenseg, true, seg_args, DebugJsonOut);
  }
  #endif
}