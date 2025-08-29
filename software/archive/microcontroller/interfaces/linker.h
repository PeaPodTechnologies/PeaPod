#ifndef CSOS_INTERFACES_H_
#define CSOS_INTERFACES_H_

#include <Arduino.h>

#include <state.h>
#include <I2CIP.h>

// #include <interfaces/adc.h>
// #include <interfaces/gpio.h>
// #include <interfaces/pwm.h>
#include <sensors/sht31.h>

// MAPPING
#define MAP_INDEX_COUNT   2
#define MAP_INDEX_EEPROM  0
// #define MAP_INDEX_ADC     1
// #define MAP_INDEX_GPIO    2
// #define MAP_INDEX_PWM     3
#define MAP_INDEX_SHT31   1

namespace ControlSystemsOS {

  // MAP LINKING

  I2CIP::Device* eepromFactory(const i2cip_fqa_t& fqa);
  // I2CIP::Device* adcFactory(const i2cip_fqa_t& fqa);
  // I2CIP::Device* gpioFactory(const i2cip_fqa_t& fqa);
  // I2CIP::Device* pwmFactory(const i2cip_fqa_t& fqa);
  I2CIP::Device* sht31Factory(const i2cip_fqa_t& fqa);

  // const char* const adc_id_progmem PROGMEM = {"ADS1015"};
  // const char* const gpio_id_progmem PROGMEM = {"MCP23017"};
  // const char* const pwm_id_progmem PROGMEM = {"PCA9685"};

  const char PROGMEM device_id_progmem[MAP_INDEX_COUNT][I2CIP_ID_SIZE] = {
    {I2CIP_EEPROM_ID},
    // {"ADS1015\0"},
    // {"MCP23017\0"},
    // {"PCA9685\0"},
    {"SHT31"},
  };

  const I2CIP::factory_device_t device_factory[MAP_INDEX_COUNT] = {
    ControlSystemsOS::eepromFactory,
    // ControlSystemsOS::adcFactory,
    // ControlSystemsOS::gpioFactory,
    // ControlSystemsOS::pwmFactory,
    ControlSystemsOS::sht31Factory,
  };

  int getMapIndex(const i2cip_id_t& id);

  const char* getDeviceID(uint8_t index);

  // STATE LINKING

  
};

#endif