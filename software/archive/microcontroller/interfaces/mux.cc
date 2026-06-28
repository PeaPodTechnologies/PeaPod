#include <interfaces/mux.h>

#include <Arduino.h>

MUX::MUX(uint8_t addr, TwoWire& wire) : Interface(addr, wire) {}

errorlevel_t MUX::setBus(uint8_t bus) {
  if(bus > 0x07) {
    return ERR_SOFT;
  }
  return write((uint8_t)(0x1 << bus));
}

errorlevel_t MUX::resetBus(void) {
  return setBus(MUX_BUS_INACTIVE);
}