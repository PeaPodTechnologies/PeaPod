// HEADERS

#include <sensors/k30.h>

#include <Arduino.h>
#include <Wire.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// CONSTRUCTOR

K30::K30(TwoWire* wire, uint8_t address) : Sensor(&id, &datasetup, K30_DELTA), address(address), wire(wire) { }

// PUBLIC METHODS

errorlevel_t K30::initialize(void) {
  wire->begin();
  return ERR_NONE;
}

errorlevel_t K30::read(float* data, uint8_t numdata) {
  byte recValue[4] = {0,0,0,0};

  // Instruct to take a reading
  wire->beginTransmission(address);
  wire->write(0x22);
  wire->write(0x00);
  wire->write(0x08);
  wire->write(0x2A);
  wire->endTransmission();
  delay(30);
  
  // Request reading
  wire->requestFrom(address, (uint8_t)4);
  delay(20);
  
  // Store reading
  for(int i = 0; wire->available() && i < 4; ++i) {
    recValue[i] = wire->read();
  }

  // Checksum and return
  byte checkSum = recValue[0] + recValue[1] + recValue[2];

  if (checkSum != recValue[3]) {
    return ERR_WARNING;
  }

  data[0] = (float)((recValue[1] << 8) + recValue[2]);

  return ERR_NONE;
}
