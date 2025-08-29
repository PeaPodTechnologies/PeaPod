#ifndef I2CIP_H_
#define I2CIP_H_

#include <Arduino.h>
#include <Wire.h>

#include <fqa.h>
#include <mux.h>
#include <device.h>
#include <eeprom.h>
#include <debug.h>
#include <bst.h>
#include <hashtable.h>

// #define I2CIP_FQA_SUBNET_MATCH(fqa, wire, module) (bool)(I2CIP_FQA_SEG_I2CBUS(fqa) == wire && I2CIP_FQA_SEG_MODULE(fqa) == module)
#define I2CIP_FQA_SUBNET_MATCH(fqa, _fqa) (bool)(I2CIP_FQA_SEG_I2CBUS(fqa) == I2CIP_FQA_SEG_I2CBUS(_fqa) && I2CIP_FQA_SEG_MODULE(fqa) == I2CIP_FQA_SEG_MODULE(_fqa))
#define I2CIP_FQA_MODULE_MATCH(fqa, wire, module) (bool)(I2CIP_FQA_SEG_I2CBUS(fqa) == (wire) && I2CIP_FQA_SEG_MODULE(fqa) == (module))
#define I2CIP_FQA_BUSADR_MATCH(fqa, bus, addr) (bool)(I2CIP_FQA_SEG_MUXBUS(fqa) == (bus) && I2CIP_FQA_SEG_DEVADR(fqa) == (addr))

namespace I2CIP {

  // Enables fundamentals subnet communication and state awareness.
  // State is reflected across two data structures:
  // 1. A BST of Device* by FQA
  // 2. A HashTable of DeviceGroup& by ID

  class Module {
    private:
      const uint8_t wire; // I2C Wire Number (Index of `wires[]`)
      const uint8_t mux;  // MUX/Module Number (0x00 - 0x07, address range 0x70 - 0x77)

      bool isFQAinSubnet(const i2cip_fqa_t& fqa);

      // Tables/trees are allocated STATICALLY, their entries are dynamic
      BST<const i2cip_fqa_t&, Device*> devices_fqabst = BST<const i2cip_fqa_t&, Device*>();
      HashTable<DeviceGroup&> devices_idgroups = HashTable<DeviceGroup&>();

      HashTableEntry<DeviceGroup&>* addEmptyGroup(const char* id);

      bool eeprom_added = false;
      
    protected:
      EEPROM* const eeprom; // EEPROM device - to be added to `devices_fqabst` and `devices_idgroups` on construction
      
    public:
      Module(const uint8_t& wire, const uint8_t& module, const uint8_t& eeprom_addr = I2CIP_EEPROM_ADDR);
      Module(const i2cip_fqa_t& eeprom_fqa);
      
      virtual ~Module();

      uint8_t getWireNum(void) const;
      uint8_t getModuleNum(void) const;

      /**
       * 1. Module Self-check
       * 1a. Check MUX - If we have lost the switch the entire subnet is down!
       * 1b. Rebuild EEPROM if necessary
       * 1c. Ping EEPROM
       * 1d. TODO: Ping Devices
      */
      i2cip_errorlevel_t operator()(void); // Module Self-check

      /**
       * 2. Device Discovery
       * 2a. Read EEPROM
       * 2b. Parse EEPROM
       *  Intended Implementation:
       *  2b i.   Read EEPROM (by Bus)
       *  2b ii.  Ping Devices (by ID)
       *  2b iii. Create (new) Devices and `add` to DeviceGroups (Made Available Internal API via `operator[]` functions)
      */

      /**
       * Discover devices on the module.
       * Side effect: Adds its own EEPROM to the proper DeviceGroup
       * Side effect: Recurses if EEPROM parse fails; attempts to overwrite with default contents and reparse
       * @param recurse {bool} - Whether to recursively parse EEPROM or not
       * @returns `false` IFF fail to add EEPROM | failed to ping EEPROM | failed to parse EEPROM; `true` otherwise
      */
      bool discover(bool recurse = true);
      virtual bool parseEEPROMContents(const char* contents);
      Device* add(Device& device, bool overwrite = false);
      // virtual DeviceGroup* deviceGroupFactory(const i2cip_id_t& id) = 0;
      virtual DeviceGroup* deviceGroupFactory(i2cip_id_t id);

      /**
       * 3. Device Lookup: HashTable<DeviceGroup&> by ID, BST<Device*> by FQA
       *  Intended Implementation:
       *  3a i.   Check if FQA is in subnet
       *  3a ii.  Check if FQA is in DeviceGroups
       * 
       *  3b i.   Check if ID is in DeviceGroups
       *  3b ii.  Return DeviceGroup
       *  
      */
      DeviceGroup* operator[](i2cip_id_t id);
      Device* operator[](const i2cip_fqa_t& fqa) const;

      /**
       * 4. Device Check
      */
      i2cip_errorlevel_t operator()(const i2cip_fqa_t& fqa, bool update = false, bool fail = false);

      void remove(Device* device, bool del = true);

      inline operator const EEPROM&() const { return *this->eeprom; }
  };
};

#endif