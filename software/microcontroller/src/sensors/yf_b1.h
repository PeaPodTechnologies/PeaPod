#ifndef PEAPOD_SENSORS_YF_B1_H_
#define PEAPOD_SENSORS_YF_B1_H_

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

#define YF_B1_DELTA         1000  // Calculate frequency every second
#define YF_B1_COUNT_PER_LPM 7.5   // 7.5 counts is one liter per minute

// Interface for the SenseAir K30 Extended Range CO2 sensor.
class YF_B1 : public Sensor {
  public:
    /**
     * Constructor.
     * @param pin Digital input pin. MUST BE 2 OR 3
     */
    YF_B1(uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Digital input pin.
    uint8_t pin;
};

#endif
