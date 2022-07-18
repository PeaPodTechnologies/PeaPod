#include <sensors/ge_2158.h>

#include <math.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// Data setup
static const char* labels[1] = { "water_temperature" };
static const SensorDataSetup datasetup = {
  .numdata = 1,
  .labels = labels
};

GE_2158::GE_2158(uint8_t pin) : Sensor(SENSOR_GE_2158, &datasetup, GE_2158_DELTA) {
  this->pin = pin;
}

errorlevel_t GE_2158::initialize(void) {
  pinMode(this->pin, INPUT);
  return ERR_NONE;
}

errorlevel_t GE_2158::read(float* data, uint8_t numdata) {
  uint16_t read = analogRead(this->pin);
  float v = read * GE_2158_V / 1023;
  float temp = (log((v * GE_2158_R) / (GE_2158_V - v) - GE_2158_C) - GE_2158_B) / GE_2158_A;

  if (temp < 0 || temp > 100) {
    return ERR_WARNING;
  }

  data[0] = temp;

  return ERR_NONE;
}
