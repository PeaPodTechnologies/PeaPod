#include <eeprom.h>

#include <fqa.h>
#include <device.h>
#include <debug.h>

bool I2CIP::EEPROM::_id_set;
char I2CIP::EEPROM::_id[I2CIP_ID_SIZE];
bool I2CIP::EEPROM::_failsafe_set;
char I2CIP::EEPROM::_failsafe[I2CIP_EEPROM_SIZE];
uint16_t I2CIP::EEPROM::_failsafe_b;

using namespace I2CIP;

// Handles ID pointer assignment too
// NEVER returns nullptr, unless out of memory
Device* EEPROM::eepromFactory(const i2cip_fqa_t& fqa, const i2cip_id_t& id) {
  if(!EEPROM::_id_set) {
    uint8_t idlen = strlen_P(i2cip_eeprom_id_progmem);

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.print(F("PROGMEM ID EEPROM Loaded: char["));
      I2CIP_DEBUG_SERIAL.print(idlen+1);
      I2CIP_DEBUG_SERIAL.print(F("] = {'"));
    #endif

    // Read in PROGMEM
    for (uint8_t k = 0; k < idlen; k++) {
      char c = pgm_read_byte_near(i2cip_eeprom_id_progmem + k);
      #ifdef I2CIP_DEBUG_SERIAL
        DEBUG_SERIAL.print(c);
      #endif
      EEPROM::_id[k] = c;
    }

    EEPROM::_id[idlen] = '\0';
    EEPROM::_id_set = true;

    #ifdef I2CIP_DEBUG_SERIAL
      I2CIP_DEBUG_SERIAL.print("'} @0x");
      I2CIP_DEBUG_SERIAL.print((uint16_t)(&(EEPROM::_id[0])), HEX);
      DEBUG_SERIAL.print('\n');
      DEBUG_DELAY();
    #endif
  }

  return (Device*)(new EEPROM(fqa, id));
}

Device* EEPROM::eepromFactory(const i2cip_fqa_t& fqa) { return eepromFactory(fqa, EEPROM::_id); }

EEPROM::EEPROM(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, &(EEPROM::_id[0])), IOInterface<char*, uint16_t, const char*, uint16_t>((Device*)this) {
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("EEPROM Constructed (ID '"));
    I2CIP_DEBUG_SERIAL.print(this->id);
    I2CIP_DEBUG_SERIAL.print(F("' @0x"));
    I2CIP_DEBUG_SERIAL.print((uint16_t)&(this->id[0]), HEX);
    I2CIP_DEBUG_SERIAL.print(F("; FQA "));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_I2CBUS(fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MODULE(fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MUXBUS(fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_DEVADR(fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(")\n"));
    DEBUG_DELAY();
  #endif
}

EEPROM::~EEPROM() {
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.print(F("EEPROM Destructed (ID '"));
  //   I2CIP_DEBUG_SERIAL.print(this->id);
  //   I2CIP_DEBUG_SERIAL.print(F("' @0x"));
  //   I2CIP_DEBUG_SERIAL.print((uint16_t)&(this->id[0]), HEX);
  //   I2CIP_DEBUG_SERIAL.print(F("; FQA "));
  //   I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_I2CBUS(this->fqa), HEX);
  //   I2CIP_DEBUG_SERIAL.print(F(":"));
  //   I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MODULE(this->fqa), HEX);
  //   I2CIP_DEBUG_SERIAL.print(F(":"));
  //   I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MUXBUS(this->fqa), HEX);
  //   I2CIP_DEBUG_SERIAL.print(F(":"));
  //   I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_DEVADR(this->fqa), HEX);
  //   I2CIP_DEBUG_SERIAL.print(F(")\n"));
  //   DEBUG_DELAY();
  // #endif

  // Cleanup
  if(this->getValue() != nullptr && this->getValue() != _failsafe) delete this->getValue();
}

// EEPROM::EEPROM(const uint8_t& wire, const uint8_t& module, const uint8_t& addr) : EEPROM(I2CIP_FQA_CREATE(wire, module, I2CIP_MUX_BUS_DEFAULT, addr)) { }

i2cip_errorlevel_t EEPROM::readContents(uint8_t* dest, size_t& num_read, size_t max_read) {
  size_t bytes_read = max_read;
  i2cip_errorlevel_t errlev = Device::readRegister(fqa, (uint16_t)0, dest, bytes_read, true, false);
  num_read = bytes_read;
  return errlev;
}

i2cip_errorlevel_t EEPROM::clearContents(bool setbus, uint16_t numbytes) {
  i2cip_errorlevel_t errlev = pingTimeout(setbus, false);
  I2CIP_ERR_BREAK(errlev);

  uint8_t zeroes[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (uint16_t bytes_written = 0; bytes_written < numbytes; bytes_written+=8) {
    const uint8_t pagelen = min(numbytes - bytes_written, 8);
    
    errlev = writeRegister(bytes_written, zeroes, pagelen, false);
    if(errlev == I2CIP_ERR_SOFT) {
      // Actual failed write
      return errlev;
    }

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.print(F("Cleared EEPROM bytes "));
      I2CIP_DEBUG_SERIAL.print(bytes_written);
      I2CIP_DEBUG_SERIAL.print(F(" - "));
      I2CIP_DEBUG_SERIAL.println((bytes_written + pagelen - 1));
      DEBUG_DELAY();
    #endif

    // Note: Timeout ping before each byte write to await completion of last write cycle
    errlev = pingTimeout(false, false);
    I2CIP_ERR_BREAK(errlev);
  }

  return errlev;
}

i2cip_errorlevel_t EEPROM::overwriteContents(const char* contents, bool clear, bool setbus) {
  for(size_t i = 0; i < I2CIP_EEPROM_SIZE; i++) {
    if(contents[i] == '\0') {
      return overwriteContents((uint8_t*)contents, i, clear, setbus);
    }
  }
  return I2CIP_ERR_SOFT;
}

i2cip_errorlevel_t EEPROM::overwriteContents(uint8_t* buffer, size_t len, bool clear, bool setbus) {
  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  if(clear) {
    errlev = clearContents(setbus, len);
  } else if(setbus) {
    errlev = MUX::setBus(this->fqa);
  }
  I2CIP_ERR_BREAK(errlev);

  for (uint16_t bytes_written = 0; bytes_written < len; bytes_written+=8) {
    const uint8_t pagelen = min(len - bytes_written, 8);
    errlev = writeRegister(bytes_written, buffer+bytes_written, pagelen, false);
    if(errlev == I2CIP_ERR_SOFT) {
      // Actual failed write
      return errlev;
    }

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.print(F("Bytes "));
      I2CIP_DEBUG_SERIAL.print(bytes_written);
      I2CIP_DEBUG_SERIAL.print(F(" - "));
      I2CIP_DEBUG_SERIAL.print(bytes_written + pagelen);
      I2CIP_DEBUG_SERIAL.print(F(" written '"));
      for(int i = 0; i < pagelen; i++) {
        I2CIP_DEBUG_SERIAL.print((char)((buffer+bytes_written)[i]));
      }
      I2CIP_DEBUG_SERIAL.print(F("'... "));
      DEBUG_DELAY();
    #endif

    // Note: Timeout ping before each byte write to await completion of last write cycle
    errlev = pingTimeout(false, false);
    I2CIP_ERR_BREAK(errlev);

    #ifdef I2CIP_DEBUG_SERIAL
      I2CIP_DEBUG_SERIAL.println("... ACK'd");
      DEBUG_DELAY();
    #endif

    if(buffer[bytes_written] == '\0') {
      // Null terminator
      #ifdef I2CIP_DEBUG_SERIAL
        DEBUG_DELAY();
        I2CIP_DEBUG_SERIAL.println("Overwrite stopped - null terminator");
        DEBUG_DELAY();
      #endif
      break;
    }
  }

  return errlev;
}

i2cip_errorlevel_t EEPROM::get(char*& dest, const uint16_t& args) {
  // 0. Check args
  if(args > I2CIP_EEPROM_SIZE) {
    return I2CIP_ERR_SOFT;
  }

  // 1. Read register (until null terminator or max bytes, arg-dependant) into a local buffer
  size_t len = args == 0 ? I2CIP_EEPROM_SIZE-1 : args;
  uint8_t buffer[len];

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("EEPROM Get (up to "));
    I2CIP_DEBUG_SERIAL.print(len);
    I2CIP_DEBUG_SERIAL.print(F(" bytes)\n"));
    DEBUG_DELAY();
  #endif

  i2cip_errorlevel_t errlev = readRegister((uint16_t)0, buffer, len);
  I2CIP_ERR_BREAK(errlev);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(len);
    I2CIP_DEBUG_SERIAL.print(F("+'\\0' bytes read from EEPROM\n"));
    DEBUG_DELAY();
  #endif

  // if((uint16_t)len != args) return I2CIP_ERR_SOFT;

  // 2. Copy local buffer to heap buffer (null-terminated)  
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("Reading to static heap buffer @0x"));
    I2CIP_DEBUG_SERIAL.print((uint16_t)(&this->readBuffer[0]), HEX);
    I2CIP_DEBUG_SERIAL.print(F(" ("));
    I2CIP_DEBUG_SERIAL.print(len+1);
    I2CIP_DEBUG_SERIAL.print(F(" bytes) '"));
  #endif

  for(size_t i = 0; i < len; i++) {
    this->readBuffer[i] = (char)buffer[i];
  }
  this->readBuffer[len] = '\0';

  #ifdef I2CIP_DEBUG_SERIAL
    I2CIP_DEBUG_SERIAL.print(this->readBuffer);
    I2CIP_DEBUG_SERIAL.print("'\n");
    DEBUG_DELAY();
  #endif

  if(errlev == I2CIP_ERR_NONE) {
    dest = &this->readBuffer[0];
  }
  return errlev;
}

i2cip_errorlevel_t EEPROM::set(const char * const& value, const uint16_t& args) {
  if(args > I2CIP_EEPROM_SIZE) {
    return I2CIP_ERR_SOFT;
  }

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("EEPROM Set ("));
    I2CIP_DEBUG_SERIAL.print(args);
    I2CIP_DEBUG_SERIAL.print(F(" bytes @0x"));
    I2CIP_DEBUG_SERIAL.print((uint16_t)value, HEX);
    I2CIP_DEBUG_SERIAL.print(F(") '"));
    I2CIP_DEBUG_SERIAL.print(value);
    I2CIP_DEBUG_SERIAL.print("'...\n");
  #endif

  // Write register
  size_t len = args;
  i2cip_errorlevel_t errlev = writeRegister((uint16_t)0, (uint8_t*)value, len);
  I2CIP_ERR_BREAK(errlev);

  // Pre-caching Cleanup
  if(this->getValue() != value && this->getValue() != nullptr && this->getValue()) { delete this->getValue(); }

  // if((uint16_t)len != args) return I2CIP_ERR_SOFT;
  return errlev;
}

// Args: len (2 bytes)
// int EEPROM::read(char*& dest, uint8_t argc, uint8_t[] argv) {
//   size_t len = 0;
//   if(argc == 0 || argv == nullptr) {
//     this->readContents((uint8_t*&)dest, len);
//   } else {
//     this->readContents((uint8_t*&)dest, len, argc > 1 ? ((argv[1] << 8) & argv[0]) : argv[0]);

//     argv[0] = (len & 0xFF);
//     if(argc > 1) argv[1] = (len >> 8);
//   }

//   return (len > 0 ? 0 : 1);
// }

// G - Getter type: char* (null-terminated; writable heap)
void EEPROM::clearCache(void) {
  this->setCache(nullptr);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("EEPROM Cache Cleared\n"));
    DEBUG_DELAY();
  #endif
}

// A - Getter argument type: uint16_t (max bytes to read)
const uint16_t& EEPROM::getDefaultA(void) const {
  return i2cip_eeprom_capacity;
}

// S - Setter type: const char* (null-terminated; immutable)
void EEPROM::resetFailsafe(void) {
  if(_failsafe_set && this->getValue() == _failsafe) return; // Already set
  if(this->getValue() != nullptr && this->getValue() != _failsafe) delete this->getValue();

  // Load from PROGMEM
  if(!_failsafe_set) {
    uint8_t len = strlen_P(i2cip_eeprom_default);

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.print(F("Loading Failsafe PROGMEM Static Heap @0x"));
      I2CIP_DEBUG_SERIAL.print((uint16_t)(&_failsafe[0]), HEX);
      I2CIP_DEBUG_SERIAL.print(F(" ("));
      I2CIP_DEBUG_SERIAL.print(len+1);
      I2CIP_DEBUG_SERIAL.print(F(" bytes) '"));
    #endif

    // Read in PROGMEM
    for (uint8_t k = 0; k < len; k++) {
      char c = pgm_read_byte_near(i2cip_eeprom_default + k);
      #ifdef I2CIP_DEBUG_SERIAL
        I2CIP_DEBUG_SERIAL.print(c);
      #endif
      _failsafe[k] = c;
    }

    _failsafe[len] = '\0';

    _failsafe_b = len;
    _failsafe_set = true;

    #ifdef I2CIP_DEBUG_SERIAL
      I2CIP_DEBUG_SERIAL.print("'\n");
      DEBUG_DELAY();
    #endif
  }


  this->setValue(_failsafe);
  this->setArgsB(_failsafe_b);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("EEPROM Value Reset\n"));
    DEBUG_DELAY();
  #endif
}

// B - Setter argument type: uint16_t (max bytes to write)
const uint16_t& EEPROM::getDefaultB(void) const {
  return i2cip_eeprom_capacity;
}