#ifndef PEAPOD_SENSORS_GE_2158_H_
#define PEAPOD_SENSORS_GE_2158_H_

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

#define GE_2158_DELTA 100
#define GE_2158_V     5.0       // Feed voltage
#define GE_2158_R     3000      // Series resistor

// Equation: T(v) = (ln((v * GE_2158_R) / (GE_2158_V - v) - GE_2158_C) - GE_2158_B) / GE_2158_A
#define GE_2158_A     -0.055481 // A parameter
#define GE_2158_B     9.30108   // B parameter
#define GE_2158_C     122.933   // C parameter

// Interface for the SenseAir K30 Extended Range CO2 sensor.
class GE_2158 : public Sensor {
  public:
    /**
     * Constructor.
     * @param pin Analog input pin.
     */
    GE_2158(uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Analog input pin.
    uint8_t pin;
};

#endif
