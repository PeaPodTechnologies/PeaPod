#ifndef PEAPOD_SENSORS_SEN0257_H_
#define PEAPOD_SENSORS_SEN0257_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// MACROS

// Settings
#define SEN0257_DELTA 100
#define SEN0257_VMIN  0.5   // Min output voltage
#define SEN0257_VMAX  4.5   // Max output voltage
#define SEN0257_PMIN  0.0   // Min output pressure (PSI)
#define SEN0257_PMAX  261.1 // Max output pressure (PSI)

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "SEN0257";

  // Data setup
  static const PROGMEM char* const labels[1] = { "water_pressure" };
  static const PROGMEM SensorDataSetup datasetup = {
    .numdata = 1,
    .labels = labels
  };
#endif

// CLASS

// Interface for the DFRobot SEN0257 water pressure sensor.
class SEN0257 : public Sensor {
  public:
    /**
     * Constructor.
     * @param pin Analog input pin.
     */
    SEN0257(const uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Analog input pin.
    const uint8_t pin;
};

#endif
