#ifndef CSOS_INTERFACES_MUX_H_
#define CSOS_INTERFACES_MUX_H_

#include <Arduino.h>
#include <Wire.h>

#include <types.h>
#include <interfaces/interface.h>

#define MUX_ADDR_MIN      0x70 // The lowest device address
#define MUX_ADDR_MAX      0x77 // The highest device address
#define MUX_BUS_MIN       0x00 // The lowest bus
#define MUX_BUS_MAX       0x07 // The highest bus
#define MUX_BUS_DEFAULT   0x00 // The default bus for "base" interfaces.
#define MUX_BUS_INACTIVE  0x07 // Bus to switch to when "inactive". This bus should be left vacant.

extern TwoWire Wire;

// Interface class for the TCA9548A I2C multiplexer IC
class MUX : private Interface {
  private:
    const uint8_t addr;
  public:
    /**
     * Constructor
     * @param addr MUX device address
     * @param wire I2C bus
     */
    MUX(uint8_t addr, TwoWire& wire = Wire);

    /**
     * Set the MUX bus
     * @param bus 0-7
     * @return Success?
     */
    errorlevel_t setBus(uint8_t bus);

    /**
     * Reset the MUX bus to inactive
     * @return Success?
     */
    errorlevel_t resetBus(void);
};

#endif