#include "sht31.h"

#include "Arduino.h"
#include "Wire.h"

#include "../base.h"
#include "../sensor.h"

static uint8_t crc8(const uint8_t *data, int len);

// Data setup
static const char* labels[2] = { "air_temperature", "air_humidity" };
static const t_sensordatasetup datasetup = {
  2,
  labels
};

// CONSTRUCTOR

SHT31::SHT31(TwoWire *wire, uint8_t address) : Sensor(SENSOR_SHT31, &datasetup, SHT31_DELTA) {
  this->wire = wire;
  this->address = address;
}

// PUBLIC METHODS

errorlevel_t SHT31::initialize(void) {
  wire->begin(address);
  reset();
  if (readStatus() == 0xFFFF) {
    return ERR_FATAL;
  } else {
    return ERR_NONE;
  }
}

errorlevel_t SHT31::read(float* data, uint8_t numdata) {
  uint8_t buffer[6];

  // Ready up
  writeCommand(SHT31_MEAS_HIGHREP);
  delay(20);
  wire->requestFrom(address, sizeof(buffer));

  // Invalid size
  if (wire->available() != sizeof(buffer)) {
    return ERR_WARNING;
  }

  // Read into buffer
  for (size_t i = 0; i < sizeof(buffer); i++) {
    buffer[i] = wire->read();
  }

  // Checksum
  if (buffer[2] != crc8(buffer, 2) || buffer[5] != crc8(buffer + 3, 2)) {
    return ERR_WARNING;
  }

  // Temperature conversion
  int32_t stemp = (int32_t)(((uint32_t)buffer[0] << 8) | buffer[1]);
  stemp = ((4375 * stemp) >> 14) - 4500;
  data[0] = (float)stemp / 100.0f;

  // Humidity conversion
  uint32_t shum = ((uint32_t)buffer[3] << 8) | buffer[4];
  shum = (625 * shum) >> 12;
  data[1] = (float)shum / 100.0f;

  return ERR_NONE;
}

// UTILITY FUNCTIONS

void SHT31::writeCommand(uint16_t cmd) {
  wire->beginTransmission(address);
  wire->write(cmd >> 8);
  wire->write(cmd & 0xFF);
  wire->endTransmission();
}

uint16_t SHT31::readStatus(void) {
  writeCommand(SHT31_READSTATUS);
  wire->requestFrom(address, (uint8_t)3);
  uint16_t stat = wire->read();
  stat <<= 8;
  stat |= wire->read();
  return stat;
}

void SHT31::reset(void) {
  writeCommand(SHT31_SOFTRESET);
  delay(10);
}

/**
 * Performs a CRC8 calculation on the supplied values.
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return The computed CRC8 value.
 */
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

// UNUSED

// void SHT31::heater(bool h) {
//   if (h) {
//     writeCommand(SHT31_HEATEREN);
//   } else {
//     writeCommand(SHT31_HEATERDIS);
//   }
//   delay(1);
// }

/**
 *  @brief  Return sensor heater state
 *  @return heater state (TRUE = enabled, FALSE = disabled)
 */
// bool SHT31::isHeaterEnabled() {
//   uint16_t regValue = readStatus();
//   return (bool)bitRead(regValue, SHT31_REG_HEATER_BIT);
// }
