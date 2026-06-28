#include <interfaces/pwm.h>

#include <Arduino.h>

using namespace ControlSystemsOS;

PWM::PWM(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, id), OutputInterface<uint16_t, args_pwm_t>((Device*)this) { }

i2cip_errorlevel_t PWM::set(const uint16_t& value, const args_pwm_t& args) {
  // Encode LED on/off values
  uint16_t on = 0, off = 0;
  if (value == 0x0000) {
    // OFF
    off = 0x1000;
  } else if (value > 0xFFF) {
    // ON
    on = 0x1000;
  } else {
    // Truncate
    off = value & 0xFFF;
  }

  // Write registers
  i2cip_errorlevel_t errlev = this->writeRegister(PWM_CHANNEL_TO_LEDREG(args), (uint8_t)(on & 0xFF));
  I2CIP_ERR_BREAK(errlev);
  errlev = this->writeRegister((uint8_t)(PWM_CHANNEL_TO_LEDREG(args) + 0x1), (uint8_t)(on >> 8));
  I2CIP_ERR_BREAK(errlev);
  errlev = this->writeRegister((uint8_t)(PWM_CHANNEL_TO_LEDREG(args) + 0x2), (uint8_t)(off & 0xFF));
  I2CIP_ERR_BREAK(errlev);
  errlev = this->writeRegister((uint8_t)(PWM_CHANNEL_TO_LEDREG(args) + 0x3), (uint8_t)(off >> 8));
  return errlev;
}

const args_pwm_t& PWM::getDefaultB(void) const {
  return default_b;
}

void PWM::resetFailsafe(void) {
  this->setValue(default_failsafe);
}