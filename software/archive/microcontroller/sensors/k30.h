#ifndef PEAPOD_SENSORS_K30_H_
#define PEAPOD_SENSORS_K30_H_

// HEADERS

#include <Arduino.h>
#include <Wire.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// MACROS

// Settings
#define K30_DEFAULT_ADDR 0x68
#define K30_DELTA 2000

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "K30";

  // Data setup
  static const PROGMEM char* const labels[1] = { "co2_ppm" };
  static const PROGMEM SensorDataSetup datasetup = {
    .numdata = 1,
    .labels = labels
  };
#endif

// Import the default I2C interface provided by Arduino.
extern TwoWire Wire;

// CLASS

// Interface for the SenseAir K30 Extended Range CO2 sensor.
class K30 : public Sensor {
  public:
    /**
     * Constructor.
     * @param wire I2C interface. Defaults to the provided `Wire` interface (by reference).
     * @param address Address of the K30 on the I2C bus. Defaults to 0x68 (hardware default).
     */
    K30(TwoWire* wire = &Wire, const uint8_t address = K30_DEFAULT_ADDR);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Address of the K30 on the I2C bus.
    const uint8_t address;

    // I2C interface.
    TwoWire* wire;
};

#endif
