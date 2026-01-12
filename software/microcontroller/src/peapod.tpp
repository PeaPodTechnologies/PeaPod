#ifndef PEAPOD_H_
#error __FILE__ should only be included AFTER <peapod.h>
#endif

#ifdef PEAPOD_H_

#ifndef PEAPOD_T_
#define PEAPOD_T_

template <i2cip_fqa_t F, unsigned char P> void PeaPod::callback_mcp23017_digitalWrite(bool _, const bool& v) {
  if(P > PIN_B7) return; // Invalid Pin
  if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr) {

    i2cip_mcp23017_bitmask_t gpio_mask = 1 << P;
    i2cip_mcp23017_t gpio_data = v ? (1 << P) : 0;
    i2cip_args_io_t gpio_args = { .g = false, .a = nullptr, .s = &gpio_data, .b = &gpio_mask };

    modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<MCP23017>(F, true, gpio_args, DebugJsonBreakpoints);
  }
}

template <i2cip_fqa_t F, unsigned char P> void PeaPod::callback_pca9685_analogWrite(bool _, const FSM::Number& v) {
    if(P > PCA9685_CH15) return; // Invalid Channel
    if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr && P <= PCA9685_CH15) {

      i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
      i2cip_pca9685_t data = (uint16_t)max(0, min(4096, (int)v));
      i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

      I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<PCA9685>(F, true, args, DebugJsonBreakpoints);
    }
  }

  template <i2cip_fqa_t F, unsigned char P> void PeaPod::callback_pca9685_onOff(bool _, const bool& v) {
    if(P > PCA9685_CH15) return; // Invalid Channel
    if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr && P <= PCA9685_CH15) {

      i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
      i2cip_pca9685_t data = v ? 4096 : 0;
      i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

      I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<PCA9685>(F, true, args, DebugJsonBreakpoints);
    }
  }

#endif
#endif