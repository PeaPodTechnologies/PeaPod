#include <interfaces/linker.h>

// 0. EEPROM
#include "../I2CIP/eeprom.h"
I2CIP::Device* ControlSystemsOS::eepromFactory(const i2cip_fqa_t& fqa) {
  if(getDeviceID(MAP_INDEX_EEPROM) == nullptr) {
    // First time
    return I2CIP::EEPROM::eepromFactory(fqa);
  }
  return I2CIP::EEPROM::eepromFactory(fqa, getDeviceID(MAP_INDEX_EEPROM));
}

// 1. ADC
// #include <interfaces/adc.h>
// I2CIP::Device* ControlSystemsOS::adcFactory(const i2cip_fqa_t& fqa) {
//   return new ControlSystemsOS::ADC(fqa, getDeviceID(MAP_INDEX_ADC));
// }

// 2. GPIO
// #include <interfaces/gpio.h>
// I2CIP::Device* ControlSystemsOS::gpioFactory(const i2cip_fqa_t& fqa) {
//   return new ControlSystemsOS::GPIO(fqa, getDeviceID(MAP_INDEX_GPIO));
// }

// 3. PWM
// #include <interfaces/pwm.h>
// I2CIP::Device* ControlSystemsOS::pwmFactory(const i2cip_fqa_t& fqa) {
//   return new ControlSystemsOS::PWM(fqa, getDeviceID(MAP_INDEX_PWM));
// }

// 4. SHT31
#include <sensors/sht31.h>
I2CIP::Device* ControlSystemsOS::sht31Factory(const i2cip_fqa_t& fqa) {
  return new ControlSystemsOS::SHT31(fqa, getDeviceID(MAP_INDEX_SHT31));
}
