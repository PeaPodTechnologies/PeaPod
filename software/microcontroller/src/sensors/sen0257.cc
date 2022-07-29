// HEADERS

#include <sensors/sen0257.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// CONSTRUCTOR

SEN0257::SEN0257(const uint8_t pin) : Sensor(&id, &datasetup, SEN0257_DELTA), pin(pin) { }

// PUBLIC METHODS

errorlevel_t SEN0257::initialize(void) {
  pinMode(this->pin, INPUT);
  return ERR_NONE;
}

errorlevel_t SEN0257::read(float* data, uint8_t numdata) {
  uint16_t read = analogRead(this->pin);
  float v = read * 5.0 / 1023;

  float pres = (float)((v - SEN0257_VMIN) * (SEN0257_PMAX - SEN0257_PMIN) / (SEN0257_VMAX - SEN0257_VMIN) + SEN0257_PMIN);

  if (pres < SEN0257_PMIN || pres > SEN0257_PMAX) {
    return ERR_WARNING;
  }

  data[0] = pres;

  return ERR_NONE;
}
