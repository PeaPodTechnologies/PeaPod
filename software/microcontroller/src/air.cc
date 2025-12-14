#include <air.h>

FSM::Variable PeaPod::air_temperature = FSM::Variable(FSM::Number(), "air-temperature");
FSM::Variable PeaPod::air_humidity = FSM::Variable(FSM::Number(), "air-humidity");
FSM::Variable PeaPod::air_co2 = FSM::Variable(FSM::Number(), "air-ppm-carbondioxide");

void PeaPod::Callbacks::callback_temperature(bool _, const FSM::Number& v) {
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

void PeaPod::Callbacks::callback_humidity(bool _, const FSM::Number& v) {
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

void PeaPod::Callbacks::callback_co2(bool _, const FSM::Number& v) {
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