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

// void PeaPod::PeaPodModuleLighting::callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __) {
//   double pwm = ((cos((FSM::Chronos.get() % PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA) / ((double)PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA) * 2.0 * M_PI) + 1.0) * 2048.0 * PEAPOD_MODULE_LIGHTING_DUTY); // 0 to PEAPOD_MODULE_LIGHTING_DUTY over PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA milliseconds

//   pwm_lighting_red.set(FSM::Number(pwm, true, false));
// }

void PeaPod::PeaPodModuleLighting::callback_lighting_gate(bool _, const bool& onoff) {
  pwm_lighting_blue.resume();
  pwm_lighting_red.resume();
}

void PeaPod::PeaPodModuleLighting::callback_lighting_camera(bool _, const bool& onoff) {
  // Turn off all lights, then turn on the camera light if `onoff` is true
  // enable_lighting.set(!onoff);
  if(onoff) {
    callback_pca9685_analogWrite<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_CAMERA>(true, FSM::Number(4096.0 * PEAPOD_MODULE_LIGHTING_DUTY_CAMERA, true));
  } else {
    callback_pca9685_onOff<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_CAMERA>(true, false); // OFF
  }
}

PeaPod::PeaPodModuleLighting::PeaPodModuleLighting(bool chronoCallbacks) : PeaPodModule(PEAPOD_MODULENUM_LIGHTING) {
  if(chronoCallbacks) {
    // #ifdef PEAPOD_PROGRAM_DEFAULT
    // interval_pwmcycle = FSM::Chronos.addInterval(50, 0, callback_pwm_cycle, false);
    // #endif

    #ifdef PEAPOD_PROGRAM_DEFAULT
    interval_lighting_on = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_LIGHTING_DELTA, 0, &enable_lighting, false);
    interval_lighting_off = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_LIGHTING_DELTA, PEAPOD_MODULE_LIGHTING_PHASE, &enable_lighting, true);
    pwm_lighting_blue.set(FSM::Number(4096 * PEAPOD_MODULE_LIGHTING_DUTY_BLUE, true));
    pwm_lighting_red.set(FSM::Number(4096 * PEAPOD_MODULE_LIGHTING_DUTY_RED, true));
    #endif

    // ADC
    interval_adc = FSM::Chronos.addInterval(PEAPOD_MODULE_LIGHTING_ADC_DELTA, 0, callback_adc_read<PEAPOD_MODULE_LIGHTING_ADC_FQA, PEAPOD_MODULE_LIGHTING_ADC_CHANNEL>, false);
  }

  // enable_lighting.addLatchingConditional(true, false, callback_lighting_onoff);
  
  adc_voltage.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_adc_voltage);
  enable_lighting.addLatchingConditional(true, false, callback_lighting_gate);
  enable_camera.addLatchingConditional(true, false, callback_lighting_camera);
  pwm_lighting_red.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_lighting_modulate<PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_RED>);
  pwm_lighting_blue.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_lighting_modulate<PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_BLUE>);

  /* A note on how the lighting gating and modulation logic works:
  * The lighting modulation conditional callbacks added to pwm_lighting_red and pwm_lighting_blue execute each update (if the value is not NaN) and set the PWM of their respective channels according to the value of the variable DEPENDING ON whether enable_lighting is true or false.
  * The lighting gate conditional callback added to enable_lighting executes each time enable_lighting changes state. Its only function is to resume/"refresh" pwm_lighting_red and pwm_lighting_blue, which retriggers the modulation callbacks.
  */

  // pwm_lighting_red.set(FSM::Number(0, true));
  // pwm_lighting_blue.set(FSM::Number(0, true));
  enable_lighting.set(false);
  enable_camera.set(false);

  registerVariable(&adc_voltage, true); // LOCKED
  registerVariable(&pwm_lighting_red);
  registerVariable(&pwm_lighting_blue);
  registerFlag(&enable_lighting);
  registerFlag(&enable_camera);
}

PeaPod::PeaPodModuleLighting::~PeaPodModuleLighting() {
  enable_lighting.set(false);
  enable_camera.set(false);

  FSM::Chronos.removeInterval(interval_adc);
  FSM::Chronos.removeInterval(interval_lighting_on);
  FSM::Chronos.removeInterval(interval_lighting_off);
  // FSM::Chronos.removeInterval(interval_pwmcycle);
  interval_adc = nullptr;
  interval_lighting_on = nullptr;
  interval_lighting_off = nullptr;
  // interval_pwmcycle = nullptr;
}