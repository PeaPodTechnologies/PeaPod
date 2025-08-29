#ifndef I2CIP_MUX_H_
#define I2CIP_MUX_H_

#include <fqa.h>

// ----------------------
// MUX: I2C Multiplexers
// ----------------------

#define I2CIP_MUX_ADDR_MIN      0x70 // The lowest device address
#define I2CIP_MUX_ADDR_MAX      0x77 // The highest device address
#define I2CIP_MUX_COUNT         (I2CIP_MUX_ADDR_MAX - I2CIP_MUX_ADDR_MIN + 1)
#define I2CIP_MUX_BUS_MIN       0x00 // The lowest bus number
#define I2CIP_MUX_BUS_MAX       0x07 // The highest bus number
#define I2CIP_MUX_BUS_COUNT     (I2CIP_MUX_BUS_MAX - I2CIP_MUX_BUS_MIN + 1)
#define I2CIP_MUX_BUS_DEFAULT   0x00 // The default bus for integrated on-module interfaces.
#define I2CIP_MUX_INSTR_RST     0x00 // Disable all busses

/**
 * Converts a bus number to a MUX instruction.
 * @param bus Bus number (0-7)
 */
#define I2CIP_MUX_BUS_TO_INSTR(bus) (uint8_t)(1 << bus)

/**
 * Converts a module number to its MUX address.
 * @param num MUX number (0-7)
 */
#define I2CIP_MODULE_TO_MUXADDR(module) (module + I2CIP_MUX_ADDR_MIN)

namespace I2CIP {
  namespace MUX {
    /**
     * Pings the MUX. Begins the Wire.
     * | MUX ADDR (7) | ACK? |
     * @param fqa FQA of a device this MUX is in front of.
     * @return ACK?
     */
    bool pingMUX(const i2cip_fqa_t& fqa);

    /**
     * Pings the MUX. Begins the Wire.
     * | MUX ADDR (7) | ACK? |
     * @param fqa FQA of a device this MUX is in front of.
     * @return ACK?
     */
    bool pingMUX(const uint8_t& wire, const uint8_t& module);

    /**
     * Sets the MUX bus.
     * | MUX ADDR (7) | MUX CONFIG (8) | ACK? |
     * @param fqa FQA of a device that is on the target Subnet.
     * @return Hardware failure: No ACK; Module lost. Software failure: Failed to write to MUX.
     */
    i2cip_errorlevel_t setBus(const i2cip_fqa_t& fqa);

    /**
     * Reset the MUX to the "inactive" bus.
     * | MUX ADDR (7) | MUX RESET (8) | ACK? |
     * @param fqa FQA of a device that this MUX is in front of
     * @return Hardware failure: No ACK; Module lost. Software failure: Failed to write to MUX.
     */
    i2cip_errorlevel_t resetBus(const i2cip_fqa_t& fqa);
  };
};

#endif