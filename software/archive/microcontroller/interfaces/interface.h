#ifndef CSOS_INTERFACES_INTERFACE_H_
#define CSOS_INTERFACES_INTERFACE_H_

// Forward declaration to fix circular dependancy
class Interface;

// HEADERS

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>

#include <types.h>
#include <interfaces/mux.h>

extern TwoWire Wire;

// CLASS
// An Interface is a single I2C peripheeral
// It is used as a generic I2C communications base class for all device classes
class Interface {
  protected:
    // Kept protected in case inherited classes need direct MUX access.
    MUX* mux;
    uint8_t bus;
    // Reusable reference reference to this object as an Adafruit_I2CDevice. All methods/fields inaccessible.
    Adafruit_I2CDevice& thisdevice = (Adafruit_I2CDevice&)(*this);

    // Reusable reference reference to this object as an Adafruit_I2CDevice. All methods/fields inaccessible.
    Interface& thisinterface = (*this);
  public:
    /**
     * Constructor.
     * @param addr Device address
     * @param bus Module MUX's bus that this device is on
     * @param mux Module's MUX
     * @param wire I2C bus
     */
    Interface(uint8_t addr, MUX* mux, uint8_t bus, TwoWire& wire = Wire);

    /**
     * Constructor. Used when the device is not on a module/behind a MUX.
     * @param addr Device address
     * @param wire I2C bus
     */
    Interface(uint8_t addr, TwoWire& wire = Wire);

    /**
     * Attempt to communicate with the device.
     */
    errorlevel_t begin(void);

    /**
     * Attempt to communicate with the device. Effectively the same as begin, just looks better sometimes :)
     */
    bool ping(void);

    /**
     * Write data to the device.
     * @param buffer Bytes to send
     * @param len Number of bytes
     * @param reset Should the MUX be reset? (Default: `true`)
     * @return ERR_FAIL: Device lost. ERR_WARN: Detected, failed to write.
     */
    errorlevel_t write(const uint8_t* buffer, size_t len, bool reset = true);

    /**
     * Write one byte of data to the device.
     * @param b Byte to send
     * @param reset Should the MUX be reset? (Default: `true`)
     * @return ERR_FAIL: Device lost. ERR_WARN: Detected, failed to write.
     */
    errorlevel_t write(const uint8_t b, bool reset = true);

    /**
     * Write two bytes of data to the device.
     * Effectively equivalent to write({b >> 8, b && 0xFF}, 2), but makes sensor interfacing a little easier :)
     * @param b 2 bytes to send
     * @param reset Should the MUX be reset? (Default: `true`)
     * @return ERR_FAIL: Device lost. ERR_WARN: Detected, failed to write.
     */
    errorlevel_t write(const uint16_t b, bool reset = true);


    /**
     * Write one byte of data to a specific register.
     * Effectively equivalent to write({reg, value}, 2), but makes sensor interfacing a little easier :)
     * @param reg Register to write to
     * @param value Byte to write
     */
    errorlevel_t writeRegister(uint8_t reg, uint8_t value);

    /**
     * Write two bytes of data to a specific register.
     * Effectively equivalent to write({reg, value >> 8, value & 0xFF }, 3), but makes sensor interfacing a little easier :)
     * @param reg Register to write to
     * @param value Two bytes to write
     */
    errorlevel_t writeRegister(uint8_t reg, uint16_t value);

    /**
     * Read data from the device.
     * @param buffer Bytes to read to
     * @param len Number of bytes to read (Default: `1`)
     * @param reset Should the MUX be reset? (Default: `true`)
     */
    errorlevel_t read(uint8_t* buffer, size_t len = 1, bool reset = true);
};

template <class S> static S* I2CSensorFactory(Interface* i, JsonObject* args);

#endif