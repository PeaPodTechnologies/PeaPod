#ifndef PEAPOD_LIGHT_H_
#error __FILE__ should only be included AFTER <light.h>
#endif

#ifdef PEAPOD_LIGHT_H_

#ifndef PEAPOD_LIGHT_T_
#define PEAPOD_LIGHT_T_

template <i2cip_fqa_t F, unsigned char C> void PeaPod::callback_adc_read(bool _, const FSM::fsm_timestamp_t& __) {
  if(C > ADS1115_CHANNEL_3) return; // Invalid Channel
  if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr) {

    i2cip_ads1115_chsel_t channel = (i2cip_ads1115_chsel_t)C;
    i2cip_args_io_t args = { .g = true, .a = nullptr, .s = nullptr, .b = &channel };

    i2cip_errorlevel_t errlev = I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<ADS1115>(F, true, args, DebugJsonBreakpoints);

    if(errlev == I2CIP_ERR_NONE){
      Device** dptr = I2CIP::devicetree[F];
      if(dptr != nullptr && *dptr != nullptr) {
        ADS1115* ads = (ADS1115*)(*dptr);
        float voltage = ads->getCache();
        adc_voltage.set(FSM::Number((double)voltage, true, false));
      }
    }
  }
}

template <unsigned char P> void PeaPod::PeaPodModuleLighting::callback_lighting_modulate(bool _, const FSM::Number& pwm) {
  if(P > PCA9685_CH15) return; // Invalid Channel
  if(enable_lighting.get()) {
    callback_pca9685_analogWrite<PEAPOD_MODULE_LIGHTING_PWM_FQA, P>(true, pwm); // ON PWM
    // callback_pca9685_analogWrite<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL>(true, FSM::Number(4096.0 * PEAPOD_MODULE_LIGHTING_DUTY));
  } else {
    callback_pca9685_onOff<PEAPOD_MODULE_LIGHTING_PWM_FQA, P>(true, false); // OFF
  }
}

#endif
#endif