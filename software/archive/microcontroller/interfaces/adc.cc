#include <interfaces/adc.h>

#include <Arduino.h>
#include <I2CIP.h>

using namespace ControlSystemsOS;

ControlSystemsOS::ADC::ADC(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, id), InputInterface<float, args_adc_t>((Device*)this) { }

i2cip_errorlevel_t ControlSystemsOS::ADC::get(float& dest, const args_adc_t& args) {
  // Set config register values
  uint16_t config =
    ADC_REG_CONFIG_CQUE_1CONV   | // Set CQUE to any value other than none so we can use it in RDY mode
    ADC_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
    ADC_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low (default val)
    ADC_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
    ADC_REG_CONFIG_MODE_SINGLE  | // Single-ended discrete reading
    ADC_GAIN                    | // Gain setting
    ADC_SPS                     | // Sampling rate setting
    ADC_CHANNEL_TO_MUX(args)    | // Set MUX from channel #
    ADC_REG_CONFIG_OS_SINGLE;     // Set 'start single-conversion' bit

  // Overwrite config register
  i2cip_errorlevel_t errlev = this->writeRegister(ADC_REG_POINTER_CONFIG, config);
  I2CIP_ERR_BREAK(errlev);

  // Write threshold registers
  uint16_t instr = 0x8000;
  errlev = this->writeRegister(ADC_REG_POINTER_HITHRESH, instr);
  I2CIP_ERR_BREAK(errlev);

  instr = 0x0;
  errlev = this->writeRegister(ADC_REG_POINTER_LOWTHRESH, instr);
  I2CIP_ERR_BREAK(errlev);

  // Wait for the conversion to complete
  uint8_t timeout = 0;
  uint16_t ready = 0;
  do {
    if(timeout == ADC_TIMEOUT) {
      errlev = I2CIP_ERR_SOFT;
      break;
    }
    errlev = this->readRegisterWord(ADC_REG_POINTER_CONFIG, ready);
    timeout++;
  } while(((ready & 0x8000) == 0) && (errlev == I2CIP_ERR_NONE));
  I2CIP_ERR_BREAK(errlev);

  // Read the conversion results
  uint16_t result;
  errlev = this->readRegisterWord(ADC_REG_POINTER_CONVERT, result);
  I2CIP_ERR_BREAK(errlev);
  uint8_t buf [2] = { 0 };
  size_t readlen = 2;
  result &= this->read(buf, readlen);

  // Shift 12-bit results right 4 bits for the ADS1015, making sure we keep the sign bit intact
  uint16_t res = (((uint16_t)buf[0] << 8) | buf[1]) >> ADC_SHIFT;
  if (res > 0x07FF) {
    // negative number - extend the sign to 16th bit
    res |= 0xF000;
  }
  dest = ControlSystemsOS::ADC::computeVolts((int16_t)res);
  return errlev;
}

const args_adc_t& ControlSystemsOS::ADC::getDefaultA(void) const {
  return ControlSystemsOS::ADC::default_a;
}

void ControlSystemsOS::ADC::clearCache(void) {
  this->setCache(ControlSystemsOS::ADC::default_cache);

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(F("ADC Cache Cleared\n"));
    DEBUG_DELAY();
  #endif
}

float ControlSystemsOS::ADC::computeVolts(int16_t counts) {
  // see data sheet Table 3
  float fsRange;
  switch (ADC_GAIN) {
  case GAIN_TWOTHIRDS:
    fsRange = 6.144f;
    break;
  case GAIN_ONE:
    fsRange = 4.096f;
    break;
  case GAIN_TWO:
    fsRange = 2.048f;
    break;
  case GAIN_FOUR:
    fsRange = 1.024f;
    break;
  case GAIN_EIGHT:
    fsRange = 0.512f;
    break;
  case GAIN_SIXTEEN:
    fsRange = 0.256f;
    break;
  default:
    fsRange = 0.0f;
  }
  return counts * (fsRange / (32768 >> ADC_SHIFT));
}