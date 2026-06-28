#include <sensors/sht31.h>

static uint8_t crc8(const uint8_t *data, int len) {
  /*
   *
   * CRC-8 formula from page 14 of SHT spec pdf
   *
   * Test data 0xBE, 0xEF should yield 0x92
   *
   * Initialization data 0xFF
   * Polynomial 0x31 (x8 + x5 +x4 +1)
   * Final XOR 0x00
   */

  const uint8_t POLYNOMIAL(0x31);
  uint8_t crc(0xFF);

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

using namespace ControlSystemsOS;

SHT31::SHT31(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, id), InputInterface<state_sht31_t, args_sht31_t>((Device*)this), Sensor((InputGetter*)this) { }

i2cip_errorlevel_t SHT31::get(state_sht31_t& value, const args_sht31_t& args) {
  size_t buflen = 6;
  uint8_t buffer[buflen];

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(F("!!! SHT31 GET !!!"));
    DEBUG_DELAY();
  #endif

  // Read registers
  i2cip_errorlevel_t errlev = this->readRegister(SHT31_MEAS_HIGHREP_STRETCH, buffer, buflen, false);
  I2CIP_ERR_BREAK(errlev);

  if (buflen != sizeof(buffer)) {
    return I2CIP_ERR_SOFT; 
  }

  // Checksum
  if (buffer[2] != crc8(buffer, 2) || buffer[5] != crc8(buffer + 3, 2)) {
    return I2CIP_ERR_SOFT;
  }

  // Temperature conversion
  int32_t stemp = (int32_t)(((uint32_t)buffer[0] << 8) | buffer[1]);
  stemp = ((4375 * stemp) >> 14) - 4500;
  value.temperature = (float)stemp / 100.0f;

  // Humidity conversion
  uint32_t shum = ((uint32_t)buffer[3] << 8) | buffer[4];
  shum = (625 * shum) >> 12;
  value.humidity = (float)shum / 100.0f;

  #ifdef CSOS_DEBUG_SERIAL
    DEBUG_DELAY();
    CSOS_DEBUG_SERIAL.print(F("!!! "));
    CSOS_DEBUG_SERIAL.print(value.temperature);
    CSOS_DEBUG_SERIAL.print(F("C, "));
    CSOS_DEBUG_SERIAL.print(value.humidity);
    CSOS_DEBUG_SERIAL.print(F("% !!!"));
    DEBUG_DELAY();
  #endif

  return I2CIP_ERR_NONE;
}

const args_sht31_t& SHT31::getDefaultA(void) const {
  return this->default_a;
}

void SHT31::clearCache(void) {
  this->setCache(this->default_cache);
}

Datum* SHT31::datumFactory(void) {
  csos_data_t hum = {.f = this->getCache().humidity};
  Datum* humidity = new Datum(CSOS_FLOAT, hum, "air_humidity");

  csos_data_t temp = {.f = this->getCache().temperature};
  Datum* temperature = new Datum(CSOS_FLOAT, temp, "air_temperature", humidity);

  return temperature;
}