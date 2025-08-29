#ifndef PEAPOD_MODULES_MCP23017_H_
#define PEAPOD_MODULES_MCP23017_H_

// HEADERS

#include <Arduino.h>

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>

// MACROS

// Registers
#define MCP23XXX_IODIR 0x00   //!< I/O direction register
#define MCP23XXX_IPOL 0x01    //!< Input polarity register
#define MCP23XXX_GPINTEN 0x02 //!< Interrupt-on-change control register
#define MCP23XXX_DEFVAL                                                        \
  0x03 //!< Default compare register for interrupt-on-change
#define MCP23XXX_INTCON 0x04 //!< Interrupt control register
#define MCP23XXX_IOCON 0x05  //!< Configuration register
#define MCP23XXX_GPPU 0x06   //!< Pull-up resistor configuration register
#define MCP23XXX_INTF 0x07   //!< Interrupt flag register
#define MCP23XXX_INTCAP 0x08 //!< Interrupt capture register
#define MCP23XXX_GPIO 0x09   //!< Port register
#define MCP23XXX_OLAT 0x0A   //!< Output latch register

// Constants
#define MCP23XXX_ADDR 0x20 //!< Default I2C Address

#define MCP_PORT(pin) ((pin < 8) ? 0 : 1) //!< Determine port from pin number

class MCP23017 {
  private:
    int x;
  
};

uint8_t _width;
uint8_t _addrwidth;
uint16_t _address;
uint8_t _byteorder;
uint8_t *_buffer;



#endif