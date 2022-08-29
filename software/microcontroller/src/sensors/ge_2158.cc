// HEADERS

#include <sensors/ge_2158.h>

#include <math.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// CONSTRUCTOR

GE_2158::GE_2158(const uint8_t pin) : Sensor(&id, &datasetup, GE_2158_DELTA), pin(pin) { }

// PUBLIC METHODS

errorlevel_t GE_2158::initialize(void) {
  pinMode(pin, INPUT);
  return ERR_NONE;
}

errorlevel_t GE_2158::read(float* data, uint8_t numdata) {
  uint16_t read = analogRead(pin);
  float v = read * GE_2158_V / 1023;
  float temp = (log((v * GE_2158_R) / (GE_2158_V - v) - GE_2158_C) - GE_2158_B) / GE_2158_A;

  if (temp < 0 || temp > 100) {
    return ERR_WARNING;
  }

  data[0] = temp;

  return ERR_NONE;
}
