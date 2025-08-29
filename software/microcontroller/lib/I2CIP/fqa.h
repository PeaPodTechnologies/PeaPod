#ifndef I2CIP_FQA_H_
#define I2CIP_FQA_H_

#include <Arduino.h>
#include <Wire.h>

// --------------------------------
// FQA: Fully Qualified Addressing
// --------------------------------

// Two useful typedefs
typedef uint16_t i2cip_fqa_t;
typedef const char* i2cip_id_t;

// Address segments: Least Significant Bit positions, lengths, and maximum values
#define I2CIP_FQA_I2CBUS_LSB  13
#define I2CIP_FQA_I2CBUS_LEN  3
#define I2CIP_FQA_I2CBUS_MAX  ((1 << I2CIP_FQA_I2CBUS_LEN) - 1)
#define I2CIP_FQA_MODULE_LSB  10
#define I2CIP_FQA_MODULE_LEN  3
#define I2CIP_FQA_MODULE_MAX  ((1 << I2CIP_FQA_MODULE_LEN) - 1)
#define I2CIP_FQA_MUXBUS_LSB  7
#define I2CIP_FQA_MUXBUS_LEN  3
#define I2CIP_FQA_MUXBUS_MAX  ((1 << I2CIP_FQA_MUXBUS_LEN) - 1)
#define I2CIP_FQA_DEVADR_LSB  0
#define I2CIP_FQA_DEVADR_LEN  7
#define I2CIP_FQA_DEVADR_MAX  ((1 << I2CIP_FQA_DEVADR_LEN) - 1)

#define I2CIP_FQA_CREATE(wire, module, bus, addr) (i2cip_fqa_t)((wire << (16 - I2CIP_FQA_I2CBUS_LEN)) | (module << (16 - I2CIP_FQA_I2CBUS_LEN - I2CIP_FQA_MODULE_LEN)) | (bus << (16 - I2CIP_FQA_I2CBUS_LEN - I2CIP_FQA_MODULE_LEN - I2CIP_FQA_MUXBUS_LEN)) | addr)

/**
 * Segment extraction. Shifts all bits so LSB is at index 0. Returns as uint8_t.
 * @param fqa FQA of interest
 * @param lsb Right-most (least significant) bit position
 * @param len Length of the segment
 */
#define I2CIP_FQA_SEG(fqa, lsb, len) ((uint8_t)((fqa >> lsb) & (0xFFFF >> (16 - len))))

// Shorthands
#define I2CIP_FQA_SEG_DEVADR(fqa) I2CIP_FQA_SEG(fqa, I2CIP_FQA_DEVADR_LSB, I2CIP_FQA_DEVADR_LEN) // Extracts the device address segment from an FQA
#define I2CIP_FQA_SEG_MUXBUS(fqa) I2CIP_FQA_SEG(fqa, I2CIP_FQA_MUXBUS_LSB, I2CIP_FQA_MUXBUS_LEN) // Extracts the MUX bus number segment from an FQA
#define I2CIP_FQA_SEG_MODULE(fqa) I2CIP_FQA_SEG(fqa, I2CIP_FQA_MODULE_LSB, I2CIP_FQA_MODULE_LEN) // Extracts the MUX number segment from an FQA
#define I2CIP_FQA_SEG_I2CBUS(fqa) I2CIP_FQA_SEG(fqa, I2CIP_FQA_I2CBUS_LSB, I2CIP_FQA_I2CBUS_LEN) // Extracts the I2C bus number segment from an FQA

// I2C Wire Implementation
#define I2CIP_MAXBUFFER 32  // I2C buffer size
#define I2CIP_NUM_WIRES 1   // Number of I2C wires - TODO: autodetect and populate `wires[]` based on hardware spec macros

extern TwoWire Wire;

#define I2CIP_FQA_TO_WIRE(fqa) (I2CIP::wires[I2CIP_FQA_SEG_I2CBUS(fqa)])

#define I2CIP_ERR_BREAK(errlev) if((errlev) != I2CIP::I2CIP_ERR_NONE) { return (errlev); }

namespace I2CIP {
  /**
   * Errorlevels for I2CIP communication.
   */
  typedef enum {
    I2CIP_ERR_NONE = 0x0, // No error
    I2CIP_ERR_SOFT = 0x1, // Communications error, device still reachable
    I2CIP_ERR_HARD = 0x2, // Device unreachable
  } i2cip_errorlevel_t;
  
  /**
   * Create an FQA from segments. Validates.
   * @param wire I2C bus number
   * @param mux MUX number
   * @param bus MUX bus number
   * @param addr Device address
   * @return A valid FQA, or 0 if any segment has an invalid value.
   */
  i2cip_fqa_t createFQA(const uint8_t& wire, const uint8_t& mux, const uint8_t& bus, const uint8_t& addr);

  extern TwoWire* const wires[I2CIP_NUM_WIRES];

  /**
   * Initialize an I2C interface (if it has not already been initialized)
   * @param wire
   */
  void beginWire(const uint8_t& wire);
};

#endif