#ifndef PEAPOD_SENSORS_K30_H_
#define PEAPOD_SENSORS_K30_H_

#include "Arduino.h"
#include "Wire.h"

#include "../base.h"
#include "../sensor.h"

#define K30_DEFAULT_ADDR 0x68
#define K30_DELTA 2000

// Import the default `Wire` interface provided by Arduino.
extern TwoWire Wire;

// Interface for the SenseAir K30 Extended Range CO2 sensor.
class K30 : public Sensor {
  public:
    /**
     * Constructor.
     * @param wire I2C interface. Defaults to the provided `Wire` interface (by reference).
     * @param address Address of the K30 on the I2C bus. Defaults to 0x68 (hardware default).
     */
    K30(TwoWire* wire = &Wire, uint8_t address = K30_DEFAULT_ADDR);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Address of the K30 on the I2C bus.
    uint8_t address;

    // I2C interface.
    TwoWire* wire;
};

#endif
