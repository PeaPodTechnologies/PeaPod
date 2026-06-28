#ifndef CSOS_INTERFACES_EEPROM_H_
#define CSOS_INTERFACES_EEPROM_H_

#define EEPROM_ADDR 0x50 // Default device address
#define EEPROM_SIZE 200  // Bytes

#include <Arduino.h>

#include <types.h>
#include <interfaces/interface.h>
#include <interfaces/mux.h>

// Interface class for EEPROM
class EEPROM : public Interface {
  private:
    const size_t size;
  public:
    EEPROM(MUX* mux, uint8_t addr = EEPROM_ADDR, uint8_t bus = MUX_BUS_DEFAULT);
    EEPROM(MUX* mux, size_t size, uint8_t addr = EEPROM_ADDR, uint8_t bus = MUX_BUS_DEFAULT);

    /**
     * Read from the EEPROM into a buffer, until '\0'
     * @param buffer The buffer to read into
     * @param len Pointer to a variable for how many bytes were read
     */
    errorlevel_t read(char* buffer, size_t* len);
};

#endif