#ifndef PEAPOD_SENSORS_GE_2158_H_
#define PEAPOD_SENSORS_GE_2158_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// MACROS

// Settings
#define GE_2158_DELTA 100
#define GE_2158_V     5.0       // Feed voltage
#define GE_2158_R     3000      // Series resistor

// Equation: T(v) = (ln((v * GE_2158_R) / (GE_2158_V - v) - GE_2158_C) - GE_2158_B) / GE_2158_A
#define GE_2158_A     -0.055481 // A parameter
#define GE_2158_B     9.30108   // B parameter
#define GE_2158_C     122.933   // C parameter

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "GE-2158";

  // Data setup
  static const PROGMEM char* const labels[1] = { "water_temperature" };
  static const PROGMEM SensorDataSetup datasetup = {
    .numdata = 1,
    .labels = labels
  };
#endif

// CLASS

// Interface for the Amphenol GE-2158 water temperature sensor.
class GE_2158 : public Sensor {
  public:
    /**
     * Constructor.
     * @param pin Analog input pin.
     */
    GE_2158(const uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Analog input pin.
    const uint8_t pin;
};

#endif
