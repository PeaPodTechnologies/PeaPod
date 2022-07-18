#include <sensors/yf_b1.h>

#include <math.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// Data setup
static const char* labels[1] = { "water_flow_rate" };
static const SensorDataSetup datasetup = {
  .numdata = 1,
  .labels = labels
};

void flow(void);

volatile uint32_t flow_count;
unsigned long last_count;

YF_B1::YF_B1(uint8_t pin) : Sensor(SENSOR_YF_B1, &datasetup, YF_B1_DELTA) {
  this->pin = pin;
}

errorlevel_t YF_B1::initialize(void) {
  pinMode(this->pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(this->pin), flow, RISING);
  return ERR_NONE;
}

errorlevel_t YF_B1::read(float* data, uint8_t numdata) {
  // Get seconds between last read and now
  float diff = (millis() - this->getState()->timestamp)/1000.0;
  float freq = flow_count / diff;
  float flow_rate = freq / YF_B1_COUNT_PER_LPM;

  data[0] = flow_rate;

  flow_count = 0;

  return ERR_NONE;
}

void flow(void) {
  flow_count++;
}