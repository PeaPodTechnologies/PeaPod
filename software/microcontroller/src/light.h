#ifndef PEAPOD_LIGHT_H_
#define PEAPOD_LIGHT_H_

#include <peapod.h>

#define PEAPOD_MODULE_LIGHTING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"PCA9685\":[" STR(I2CIP_PCA9685_ADDRESS) "],\"ADS1115\":[" STR(I2CIP_ADS1115_ADDRESS) "]}]"}

#define PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_RED PCA9685_CH0
#define PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_BLUE PCA9685_CH1
#define PEAPOD_MODULE_LIGHTING_PWM_CHANNEL_CAMERA PCA9685_CH2
#define PEAPOD_MODULE_LIGHTING_PWM_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_LIGHTING, 0, I2CIP_PCA9685_ADDRESS)

#define PEAPOD_MODULE_LIGHTING_ADC_CHANNEL ADS1115_CHANNEL_0
#define PEAPOD_MODULE_LIGHTING_ADC_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_LIGHTING, 0, I2CIP_ADS1115_ADDRESS)

#define PEAPOD_MODULE_LIGHTING_ADC_DELTA 100
#define PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA 2500
#define PEAPOD_MODULE_LIGHTING_DELTA 86400000 // 24 hours
#define PEAPOD_MODULE_LIGHTING_PHASE 57600000 // 16 hours on
#define PEAPOD_MODULE_LIGHTING_DUTY_RED 0.5 // 50% Duty Cycle
#define PEAPOD_MODULE_LIGHTING_DUTY_BLUE 0.5 // 50% Duty Cycle
#define PEAPOD_MODULE_LIGHTING_DUTY_CAMERA 0.5 // 50% Duty Cycle

namespace PeaPod {
  extern FSM::Variable adc_voltage;
  extern FSM::Variable pwm_lighting_red;
  extern FSM::Variable pwm_lighting_blue;
  extern FSM::Flag enable_lighting;
  extern FSM::Flag enable_camera;

  template <i2cip_fqa_t F, unsigned char C> void callback_adc_read(bool _, const FSM::fsm_timestamp_t& __);
  void callback_adc_voltage(bool _, const FSM::Number& v);

  class PeaPodModuleLighting : public PeaPodModule {
    FSM::IntervalCallback* interval_adc = nullptr;
    FSM::IntervalCallback* interval_lighting_on = nullptr;
    FSM::IntervalCallback* interval_lighting_off = nullptr;
    // FSM::IntervalCallback* interval_pwmcycle = nullptr;

    template <unsigned char P> static void callback_lighting_modulate(bool _, const FSM::Number& pwm);

    static void callback_lighting_gate(bool _, const bool& onoff);

    static void callback_lighting_camera(bool _, const bool& onoff);

    // static void callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __);

    public:
      PeaPodModuleLighting(bool chronoCallbacks = true);

      ~PeaPodModuleLighting();
  };
}

#include <light.tpp>

#endif