#include <interfaces/eeprom.h>

#include <Arduino.h>

#include <types.h>
#include <interfaces/interface.h>
#include <interfaces/mux.h>

EEPROM::EEPROM(MUX* mux, uint8_t addr, uint8_t bus) : Interface(addr, mux, bus), size(EEPROM_SIZE) { }

EEPROM::EEPROM(MUX* mux, size_t size, uint8_t addr, uint8_t bus) : Interface(addr, mux, bus), size(size) { }

/**
 * Reads until the first null ('/-') character, or the end of the EEPROM, whichever comes first.
 * @param buff Pointer to the buffer to copy the contents into.
 * @param len Pointer to the size_t into which to copy the number of bytes read.
 * @return Error Level
 */
errorlevel_t EEPROM::read(char* buff, size_t* len) {
  // Multiple operations being performed, reset the MUX manually
  mux.

  // Byte currently being read
  uint16_t bytenum = 0;
  for (uint16_t x = 0; x < size; x++) {
    // Request data from the EEPROM chip at byte `bytenum`
    errorlevel_t result = thisinterface.write(bytenum);
    // Store the requested data (copy as a byte pointer)
    result &= thisinterface.read((uint8_t*)buff, 1, false);
    if (result != ERR_NONE) {
      // Failed, stop
      buff[x] = '\0';
      *len = x+1;
      return ERR_SOFT;
    }
    if (buff[0] == '\0') {
      // END
      *len = x+1;
      break;
    }
    buff[x] = buff[0];

    bytenum++;
  }
  return ERR_NONE;
}