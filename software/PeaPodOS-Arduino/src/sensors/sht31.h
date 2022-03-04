#ifndef PEAPOD_SENSORS_SHT31_H_
#define PEAPOD_SENSORS_SHT31_H_

#include "Arduino.h"
#include "Wire.h"

#include "../base.h"
#include "../sensor.h"

#define SHT31_DEFAULT_ADDR 0x44           // SHT31 Default Address
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06 // Measurement High Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_MEDREP_STRETCH 0x2C0D  // Measurement Medium Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_LOWREP_STRETCH 0x2C10  // Measurement Low Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_HIGHREP 0x2400         // Measurement High Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_MEDREP 0x240B          // Measurement Medium Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_LOWREP 0x2416          // Measurement Low Repeatability with Clock Stretch Disabled
#define SHT31_READSTATUS 0xF32D           // Read Out of Status Register
#define SHT31_CLEARSTATUS 0x3041          // Clear Status
#define SHT31_SOFTRESET 0x30A2            // Soft Reset
#define SHT31_HEATEREN 0x306D             // Heater Enable
#define SHT31_HEATERDIS 0x3066            // Heater Disable
#define SHT31_REG_HEATER_BIT 0x0d         // Status Register Heater Bit
#define SHT31_DELTA 1000

// Import the default `Wire` interface provided by Arduino.
extern TwoWire Wire;

// Interface for the Adafruit SHT31-D Temperature and Humidity breakout board. Does NOT inherit from `Sensor`.
class SHT31 : public Sensor {
  public:
    /**
     * Constructor.
     * @param wire I2C interface. Defaults to the provided `Wire` interface (by reference).
     */
    SHT31(TwoWire* wire = &Wire, uint8_t address = SHT31_DEFAULT_ADDR);

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
    uint8_t address;

    // Wire object
    TwoWire* wire; 
};

#endif
