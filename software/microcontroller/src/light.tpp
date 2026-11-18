#ifndef PEAPOD_LIGHT_H_
#error __FILE__ should only be included AFTER <light.h>
#endif

#ifdef PEAPOD_LIGHT_H_

#ifndef PEAPOD_LIGHT_T_
#define PEAPOD_LIGHT_T_

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