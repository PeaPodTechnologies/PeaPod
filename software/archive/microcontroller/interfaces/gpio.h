#ifndef CSOS_INTERFACES_GPIO_H_
#define CSOS_INTERFACES_GPIO_H_

#include <Arduino.h>
#include <I2CIP.h>

// MACROS
// Registers
#define GPIO_IODIR    0x00 // I/O direction register
#define GPIO_IPOL     0x02 // Input polarity register
#define GPIO_GPINTEN  0x04 // Interrupt-on-change control register
#define GPIO_DEFVAL   0x06 // Default compare register for interrupt-on-change
#define GPIO_INTCON   0x08 // Interrupt control register
#define GPIO_IOCON    0x0A // Configuration register
#define GPIO_GPPU     0x0C // Pull-up resistor configuration register
#define GPIO_INTF     0x0E // Interrupt flag register
#define GPIO_INTCAP   0x10 // Interrupt capture register
#define GPIO_GPIO     0x12 // Port register
#define GPIO_OLAT     0x14 // Output latch register

// Constants
#define GPIO_NUMPINS  16

// Settings
#define GPIO_ADDR     0x20 // Default address

/**
 * Determine shifted register address from pin number.
 **/
#define GPIO_PIN_REG(reg, pin) (uint8_t)((pin < 8) ? reg : reg+1)

#define GPIO_PIN_SHIFT(pin) (pin % 8)

typedef enum {
  GPIO_PIN_NULL = 0xFF,
  GPIO_PIN_A0 = 0,
  GPIO_PIN_A1,
  GPIO_PIN_A2,
  GPIO_PIN_A3,
  GPIO_PIN_A4,
  GPIO_PIN_A5,
  GPIO_PIN_A6,
  GPIO_PIN_A7,
  GPIO_PIN_B0,
  GPIO_PIN_B1,
  GPIO_PIN_B2,
  GPIO_PIN_B3,
  GPIO_PIN_B4,
  GPIO_PIN_B5,
  GPIO_PIN_B6,
  GPIO_PIN_B7,
} args_gpio_t;

typedef enum {
  GPIO_PIN_UNDEF = 0xFF,
  GPIO_PIN_LOW = LOW,
  GPIO_PIN_HIGH = HIGH
} state_gpio_t;

typedef enum {
  // GPIO_PINMODE_UNDEF = 0xFF,
  GPIO_PINMODE_OUTPUT = 0,
  GPIO_PINMODE_INPUT = 1
} gpio_pinmode_t;

using namespace I2CIP;

namespace ControlSystemsOS {

  // Interface class for the MCP23017 16-pin GPIO IC
  class GPIO : public Device, public IOInterface<state_gpio_t, args_gpio_t, state_gpio_t, args_gpio_t> {
    friend Device* gpioFactory(const i2cip_fqa_t& fqa);
    
    private:
      const state_gpio_t default_cache = GPIO_PIN_UNDEF;
      const args_gpio_t default_a = GPIO_PIN_NULL;
      const state_gpio_t default_failsafe = GPIO_PIN_LOW;
      const args_gpio_t default_b = GPIO_PIN_NULL;
      /**
       * @param fqa
       * @param pin Pin number
       * @param mode Pin mode
       **/
      i2cip_errorlevel_t pinMode(const args_gpio_t& pin, const gpio_pinmode_t& mode);

      GPIO(const i2cip_fqa_t& fqa, const i2cip_id_t& id);

    public:
      /**
       * Read a GPIO pin.
       * @param dest Pin state
       * @param args Pin number
       **/
      i2cip_errorlevel_t get(state_gpio_t& dest, const args_gpio_t& args) override;

      /**
       * Write to a GPIO pin.
       * @param dest Pin state
       * @param args Pin number
       **/
      i2cip_errorlevel_t set(const state_gpio_t& value, const args_gpio_t& args) override;

      const args_gpio_t& getDefaultA(void) const override;
      void clearCache(void) override;
      const args_gpio_t& getDefaultB(void) const override;
      void resetFailsafe(void) override;
  };
};

#endif