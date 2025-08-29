#include <interfaces/gpio.h>

#include <Arduino.h>
#include <I2CIP.h>

using namespace ControlSystemsOS;

GPIO::GPIO(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, id), IOInterface<state_gpio_t, args_gpio_t, state_gpio_t, args_gpio_t>((Device*)this) { }

i2cip_errorlevel_t GPIO::get(state_gpio_t& dest, const args_gpio_t& args) {
  // Set pin mode
  i2cip_errorlevel_t errlev = this->pinMode(args, GPIO_PINMODE_INPUT);
  I2CIP_ERR_BREAK(errlev);

  // Read in register
  uint8_t value;
  errlev = this->readRegisterByte(GPIO_PIN_REG(GPIO_GPIO, args), value);
  dest = (errlev > I2CIP_ERR_NONE ? GPIO_PIN_UNDEF : (state_gpio_t)READ_BITS(value, GPIO_PIN_SHIFT(args), 1));
  return errlev;
}

i2cip_errorlevel_t GPIO::set(const state_gpio_t& value, const args_gpio_t& args) {
  if(value == GPIO_PIN_UNDEF) {
    return I2CIP_ERR_SOFT;
  }

  // Set pin mode
  i2cip_errorlevel_t errlev = this->pinMode(args, GPIO_PINMODE_OUTPUT);
  I2CIP_ERR_BREAK(errlev);
  
  // Read register
  uint8_t existing;
  errlev = this->readRegisterByte(GPIO_PIN_REG(GPIO_GPIO, args), existing);
  I2CIP_ERR_BREAK(errlev);

  // Write register
  return this->writeRegister(GPIO_PIN_REG(GPIO_GPIO, args), OVERWRITE_BITS(existing, value, (args % 8), 1));
}

i2cip_errorlevel_t GPIO::pinMode(const args_gpio_t& pin, const gpio_pinmode_t& mode) {
  // Read in register
  uint8_t existing;
  i2cip_errorlevel_t errlev = this->readRegisterByte(GPIO_PIN_REG(GPIO_IODIR, pin), existing);
  I2CIP_ERR_BREAK(errlev);

  // Write to register
  return this->writeRegister(GPIO_PIN_REG(GPIO_IODIR, pin), OVERWRITE_BITS(existing, mode, (pin % 8), 1));
}

// args_gpio_t getDefaultA(void) const override;
// state_gpio_t getDefaultCache(void) const override;
// args_gpio_t getDefaultB(void) const override;
// state_gpio_t getFailsafe(void) const override;

const args_gpio_t& GPIO::getDefaultA(void) const {
  return default_a;
}

void GPIO::clearCache(void) {
  this->setCache(default_cache);
}

const args_gpio_t& GPIO::getDefaultB(void) const {
  return default_b;
}

void GPIO::resetFailsafe(void) {
  this->setValue(default_failsafe);
}