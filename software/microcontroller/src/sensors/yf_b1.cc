// HEADERS

#include <sensors/yf_b1.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// CONSTRUCTOR

YF_B1::YF_B1(const uint8_t pin) : Sensor(&id, &datasetup, YF_B1_DELTA), InterruptHandler(&(this->pin)), pin(pin) { }

// PUBLIC METHODS

errorlevel_t YF_B1::initialize(void) {
  pinMode(this->pin, INPUT);
  // Attach rising-edge interrupt to our digital pin, triggers flow()
  attachInstanceInterrupt(RISING);
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

void YF_B1::handleInterrupt(void) {
  this->flow_count++;
}