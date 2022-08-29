#ifndef PEAPOD_SENSORS_SHT31_H_
#define PEAPOD_SENSORS_SHT31_H_

// HEADERS

#include <Arduino.h>
#include <Wire.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// MACROS

// Registers
#define SHT31_MEAS_HIGHREP_STRETCH  0x2C06  // Measurement High Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_MEDREP_STRETCH   0x2C0D  // Measurement Medium Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_LOWREP_STRETCH   0x2C10  // Measurement Low Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_HIGHREP          0x2400  // Measurement High Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_MEDREP           0x240B  // Measurement Medium Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_LOWREP           0x2416  // Measurement Low Repeatability with Clock Stretch Disabled
#define SHT31_READSTATUS            0xF32D  // Read Out of Status Register
#define SHT31_CLEARSTATUS           0x3041  // Clear Status
#define SHT31_SOFTRESET             0x30A2  // Soft Reset
#define SHT31_HEATEREN              0x306D  // Heater Enable
#define SHT31_HEATERDIS             0x3066  // Heater Disable
#define SHT31_REG_HEATER_BIT        0x000d  // Status Register Heater Bit

// Settings
#define SHT31_ADDR  0x44  // SHT31 Default Address
#define SHT31_DELTA 1000  // read() once every delta milliseconds


// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "SHT31";

  // Data setup
  static const PROGMEM char* const labels[2] = { "air_temperature", "air_humidity" };
  static const PROGMEM SensorDataSetup datasetup = {
    .numdata = 2,
    .labels = labels
  };
#endif

// Import the default I2C interface provided by Arduino.
extern TwoWire Wire;

// CLASS

// Interface for the Adafruit SHT31-D Temperature and Humidity breakout board.
class SHT31 : public Sensor {
  public:
    /**
     * Constructor.
     * @param wire I2C interface. Defaults to the provided `Wire` interface (by reference).
     * @param address I2C address. Defaults to the default SHT31 address
     */
    SHT31(TwoWire* wire = &Wire, uint8_t address = SHT31_ADDR);

    /**
     * UNUSED
     * Enables or disabled the heating element.
     * @param h True to enable the heater, False to disable it.
     */
    // void heater(bool h);

    /**
     * UNUSED
     * Gets the current status register heater bit.
     * @return Boolean value, True = enabled, False = disabled.
     */
    // bool isHeaterEnabled(void);

  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    /**
     * Internal function to perform and I2C write.
     * @param cmd   The 16-bit command ID to send.
     */
    void writeCommand(uint16_t cmd);

    /**
     * Gets the current status register contents.
     * @return The 16-bit status register.
     */
    uint16_t readStatus(void);

    // Performs a reset of the sensor to put it into a known state.
    void reset(void);

    // I2C address
    const uint8_t address;

    // Wire object
    TwoWire* wire; 
};

#endif
