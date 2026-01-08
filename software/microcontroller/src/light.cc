#include <light.h>

void PeaPod::callback_adc_voltage(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, adc_voltage.getKey());

  #ifdef PEAPOD_SEVENSEG_ADC
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_2F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}