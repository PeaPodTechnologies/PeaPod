#include <air.h>

void PeaPod::callback_temperature(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_temperature.getKey());

  #ifdef PEAPOD_SEVENSEG_TEMPERATURE
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

void PeaPod::callback_humidity(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_humidity.getKey());

  #ifdef PEAPOD_SEVENSEG_HUMIDITY
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

void PeaPod::callback_co2(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (unsigned)v, air_co2.getKey());

  #ifdef PEAPOD_SEVENSEG_CO2
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_UINT;
    i2cip_ht16k33_data_t seg_data = { .h = (unsigned)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

PeaPod::PeaPodModuleAir::PeaPodModuleAir(bool chronoCallbacks) : PeaPodModule(PEAPOD_MODULENUM_AIR) {
  if(chronoCallbacks) {
    interval_sht45 = FSM::Chronos.addInterval(PEAPOD_SHT45_DELTA, PeaPodModuleAir::callback_sht45_mean<PEAPOD_MODULENUM_AIR>);
    interval_k30 = FSM::Chronos.addInterval(PEAPOD_K30_DELTA, PeaPodModuleAir::callback_k30_mean<PEAPOD_MODULENUM_AIR>);
  }

  air_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_temperature);
  air_humidity.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_humidity);
  air_co2.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_co2);

  registerVariable(&air_temperature, true); // LOCKED
  registerVariable(&air_humidity, true); // LOCKED
  registerVariable(&air_co2, true); // LOCKED
}

PeaPod::PeaPodModuleAir::~PeaPodModuleAir() {
  FSM::Chronos.removeInterval(interval_sht45);
  FSM::Chronos.removeInterval(interval_k30);
  interval_sht45 = nullptr;
  interval_k30 = nullptr;
}