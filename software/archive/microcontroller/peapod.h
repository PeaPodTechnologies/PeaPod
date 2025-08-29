#ifndef CSOS_CSOS_H_
#define CSOS_CSOS_H_

// HEADERS

#include <Arduino.h>
#include <Wire.h>

#include <modules/module.h>
#include <interfaces/interface.h>
#include <sensors/all.h>
#include <actuators/all.h>

#include <utils/hashtable.h>

extern TwoWire wire;

// CLASS
// An Interface is a single I2C device, wrapping an Adafruit_I2CDevice.
class PeaPod {
  private:
    const TwoWire* wire;

    HashTable<Interface> interfaces = HashTable<Interface>(100);

    Module* modules[MODULE_COUNT] = { nullptr };
  public:
    /**
     * Constructor.
     */
    PeaPod(TwoWire* wire = &Wire);

    errorlevel_t post(void);
    void scan(void);
};

#endif