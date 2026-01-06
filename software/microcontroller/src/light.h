#ifndef PEAPOD_LIGHT_H_
#define PEAPOD_LIGHT_H_

#include <peapod.h>

#define PEAPOD_MODULE_LIGHTING_EEPROM_CONTENTS {"[{\"24LC32\":[80],\"PCA9685\":[" STR(I2CIP_PCA9685_ADDRESS) "],\"ADS1115\":[" STR(I2CIP_ADS1115_ADDRESS) "]}]"}

#define PEAPOD_MODULE_LIGHTING_PWM_CHANNEL PCA9685_CH0
#define PEAPOD_MODULE_LIGHTING_PWM_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_LIGHTING, 0, I2CIP_PCA9685_ADDRESS)

#define PEAPOD_MODULE_LIGHTING_ADC_CHANNEL ADS1115_CHANNEL_0
#define PEAPOD_MODULE_LIGHTING_ADC_FQA I2CIP_FQA_CREATE(PEAPOD_WIRENUM, PEAPOD_MODULENUM_LIGHTING, 0, I2CIP_ADS1115_ADDRESS)

#define PEAPOD_MODULE_LIGHTING_ADC_DELTA 100
#define PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA 2500
#define PEAPOD_MODULE_LIGHTING_DELTA 86400000 // 24 hours
#define PEAPOD_MODULE_LIGHTING_PHASE 57600000 // 16 hours on
#define PEAPOD_MODULE_LIGHTING_DUTY 0.5 // 50% Duty Cycle

namespace PeaPod {
  extern FSM::Variable adc_voltage;
  extern FSM::Variable pwm_lighting;
  extern FSM::Flag flag_lighting;

  namespace Callbacks {
    template <i2cip_fqa_t F, unsigned char C> void callback_adc_read(bool _, const FSM::fsm_timestamp_t& __);
    void callback_adc_voltage(bool _, const FSM::Number& v);
  }

  class PeaPodModuleLighting : public PeaPodModule {
    FSM::IntervalCallback* interval_adc = nullptr;
    FSM::IntervalCallback* interval_lighting_on = nullptr;
    FSM::IntervalCallback* interval_lighting_off = nullptr;
    FSM::IntervalCallback* interval_pwmcycle = nullptr;

    static void callback_lighting_modulate(bool _, const FSM::Number& pwm) {
      if(flag_lighting.get()) {
        Callbacks::callback_pca9685_analogWrite<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL>(true, pwm);
        // Callbacks::callback_pca9685_analogWrite<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL>(true, FSM::Number(4096.0 * PEAPOD_MODULE_LIGHTING_DUTY));
      } else {
        Callbacks::callback_pca9685_onOff<PEAPOD_MODULE_LIGHTING_PWM_FQA, PEAPOD_MODULE_LIGHTING_PWM_CHANNEL>(true, false);
      }
    }

    static void callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __) {
      double pwm = ((cos((FSM::Chronos.get() % PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA) / ((double)PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA) * 2.0 * M_PI) + 1.0) * 2048.0 * PEAPOD_MODULE_LIGHTING_DUTY); // 0 to PEAPOD_MODULE_LIGHTING_DUTY over PEAPOD_MODULE_LIGHTING_PWMCYCLE_DELTA milliseconds

      pwm_lighting.set(FSM::Number(pwm, true, false));
    }

    public:
      PeaPodModuleLighting(bool chronoCallbacks = true) : PeaPodModule(PEAPOD_MODULENUM_LIGHTING) {
        if(chronoCallbacks) {
          interval_pwmcycle = FSM::Chronos.addInterval(50, 0, callback_pwm_cycle, false);

          // flag_lighting.addLatchingConditional(true, false, callback_lighting_modulate);

          pwm_lighting.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_lighting_modulate);

          interval_lighting_on = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_LIGHTING_DELTA, 0, &flag_lighting, false);
          interval_lighting_off = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_LIGHTING_DELTA, PEAPOD_MODULE_LIGHTING_PHASE, &flag_lighting, true);

          // ADC
          interval_adc = FSM::Chronos.addInterval(PEAPOD_MODULE_LIGHTING_ADC_DELTA, 0, Callbacks::callback_adc_read<PEAPOD_MODULE_LIGHTING_ADC_FQA, PEAPOD_MODULE_LIGHTING_ADC_CHANNEL>, false);
          adc_voltage.addConditional(FSM::CMP_NEQ, FSM::notanumber, Callbacks::callback_adc_voltage);
        }
      }

      ~PeaPodModuleLighting() {
        FSM::Chronos.removeInterval(interval_adc);
        FSM::Chronos.removeInterval(interval_lighting_on);
        FSM::Chronos.removeInterval(interval_lighting_off);
        FSM::Chronos.removeInterval(interval_pwmcycle);
        interval_adc = nullptr;
        interval_lighting_on = nullptr;
        interval_lighting_off = nullptr;
        interval_pwmcycle = nullptr;
      }
  };
}

#include <light.tpp>

#endif