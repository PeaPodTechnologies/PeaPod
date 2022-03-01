#include "Arduino.h"
#include "../Base.h"
#include "../Sensor.h"
#include "Wire.h"
#include "K30.h"

// Data setup
static const char* labels[1] = { "co2_ppm" };
static const t_sensordatasetup datasetup = {
  1,
  labels
};

K30::K30(TwoWire* wire, uint8_t address) : Sensor(SENSOR_K30, &datasetup, K30_DELTA) {
  this->wire = wire;
  this->address = address;
}

errorlevel_t K30::initialize(void) {
  wire->begin();
  return ERR_NONE;
}

errorlevel_t K30::read(float** data, uint8_t numdata) {
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
  for(int i = 0; wire->available() && i < 4; i++) {
    recValue[i] = wire->read();
  }

  // Checksum and return
  byte checkSum = recValue[0] + recValue[1] + recValue[2];

  if (checkSum != recValue[3]) {
    return ERR_WARNING;
  }

  (*data)[0] = (float)((recValue[1] << 8) + recValue[2]);

  return ERR_NONE;
}
