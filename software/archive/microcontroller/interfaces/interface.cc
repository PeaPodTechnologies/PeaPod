#include <interfaces/interface.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>

#include <types.h>
#include <utils/hashtable.h>
#include <interfaces/mux.h>

Interface::Interface(uint8_t addr, MUX* mux, uint8_t bus, TwoWire& wire = Wire) : Adafruit_I2CDevice(addr, &wire), mux(mux), bus(bus) { }

Interface::Interface(uint8_t addr, TwoWire& wire = Wire) : Adafruit_I2CDevice(addr, &wire), mux(nullptr), bus(0x0) {}

errorlevel_t Interface::begin(void) {
  return ping() ? ERR_NONE : ERR_HARD;
}

bool Interface::ping(void) {
  // `ping()` just looks better okayyy
  return thisdevice.begin();
}

errorlevel_t Interface::write(const uint8_t* buffer, size_t len, bool reset) {
  errorlevel_t status = ERR_NONE;

  // If this interface is on a module, switch the MUX to the correct bus
  if (mux != nullptr) {
    status &= mux->setBus(bus);
  }

  // Device alive?
  if(!ping()){
    // Device lost!
    return ERR_HARD;
  }

  if(!thisdevice.write(buffer, len)) {
    status = ERR_SOFT;
  }

  // If this interface is on a module, switch the module's MUX to the "inactive" bus (to avoid unintentional messaging)
  if (mux != nullptr) {
    status &= mux->resetBus();
  }

  return status;
}

errorlevel_t Interface::write(const uint8_t b, bool reset) {
  return write(&b, 1, reset);
}

errorlevel_t Interface::write(const uint16_t b, bool reset) {
  return writeRegister((uint8_t)(b >> 8), (uint8_t)(b & 0xFF));
}

errorlevel_t Interface::writeRegister(uint8_t reg, uint8_t value) {
  uint8_t buf[2] = { reg, value };
  return write(buf, 2);
}

errorlevel_t Interface::writeRegister(uint8_t reg, uint16_t value) {
  uint8_t buf[3] = { reg, value >> 8, value & 0xFF };
  return write(buf, 3);
}

errorlevel_t Interface::read(uint8_t* buffer, size_t len, bool reset) {
  errorlevel_t status = ERR_NONE;

  // If this interface is on a module, switch the MUX to the correct bus
  if (mux != nullptr) {
    status &= mux->setBus(bus);
  }

  // Device alive?
  if(!ping()){
    // Device lost!
    return ERR_HARD;
  }

  // Attempt to read from the device
  if(!thisdevice.read(buffer, len)) {
    status &= ERR_SOFT;
  }

  // If this interface is on a module, switch the module's MUX to the "inactive" bus (to avoid unintentional messaging)
  if (mux != nullptr) {
    mux->resetBus();
  }

  return status;
}

template <class S> static S* I2CSensorFactory(Interface* i, JsonObject* args) {
  // I2C device; `i` is ignored
  // Check for all keys
  if(args->containsKey("addr") && args->containsKey("bus") && args->containsKey("moduleaddr")) {
    // Parse
    uint8_t addr = args->getMember("addr").as<uint8_t>();
    uint8_t bus = args->getMember("bus").as<uint8_t>();
    uint8_t moduleaddr = args->getMember("moduleaddr").as<uint8_t>();
    // Construct
    return new S(addr, bus, moduleaddr);
  } else {
    return nullptr;
  }
}