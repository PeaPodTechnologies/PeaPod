#ifndef I2CIP_DEVICE_H_
#define I2CIP_DEVICE_H_

#include <fqa.h>
#include <mux.h>

/**
 * Shifts and masks a number's bits.
 * @param data  Data to shift/mask
 * @param lsb   LSB position
 * @param bits  Number of bits to keep
 **/
#define READ_BITS(data, lsb, bits) (((data) >> (lsb)) & ((1 << (bits)) - 1))

/**
 * Overwrites some bits in existing data.
 * @param existing  Existing data
 * @param data      New data
 * @param lsb       Position to insert (LSB)
 * @param bits      Number of bits to overwrite
 **/
#define OVERWRITE_BITS(existing, data, lsb, bits) (((existing) & ~(((1 << (bits)) - 1) << (lsb))) | (((data) & ((1 << (bits)) - 1)) << (lsb)))

#define I2CIP_DEVICES_PER_GROUP ((size_t)4)
#define I2CIP_ID_SIZE ((size_t)10)

namespace ControlSystemsOS {
  class Sensor;
}

namespace I2CIP {

  class Module;

  // typedef enum { I2CIP_ITYPE_NULL = 0b00, I2CIP_ITYPE_INPUT = 0b01, I2CIP_ITYPE_OUTPUT = 0b10, I2CIP_ITYPE_IO = 0b11 } i2cip_itype_t;

  // Barebones template-less abstract classes expose voidptr hooks for the device to be used as an input or output

  class InputGetter {
    protected:
      static const char failptr_get = '\a';

      ControlSystemsOS::Sensor* sensor = nullptr;
    public:
      virtual ~InputGetter() = 0;
      virtual i2cip_errorlevel_t get(const void* args = nullptr) { return I2CIP_ERR_HARD; } // Unimplemented; delete this device
      i2cip_errorlevel_t failGet(void) { return this->get(&failptr_get); }

      ControlSystemsOS::Sensor* getSensor(void) const { return this->sensor; }
      void setSensor(ControlSystemsOS::Sensor* sensor) { this->sensor = sensor; }
  };

  class OutputSetter {
    protected:
      static const char failptr_set = '\a';
    public:
      virtual ~OutputSetter() = 0;
      virtual i2cip_errorlevel_t set(const void* value = nullptr, const void* args = nullptr) { return I2CIP_ERR_HARD; } // Unimplemented; delete this device
      i2cip_errorlevel_t reset(const void* args = nullptr) { return this->set(&failptr_set, args); }
      i2cip_errorlevel_t failSet(const void* value) { return this->set(value, &failptr_set); }
      i2cip_errorlevel_t failSet(void) { return this->set(&failptr_set, &failptr_set); }
  };

  class Device {
    protected:
      const i2cip_fqa_t fqa;
      i2cip_id_t id;

      // // Set by public API, deleted on deconstruction
      InputGetter* input = nullptr;
      OutputSetter* output = nullptr;

      Device(const i2cip_fqa_t& fqa, i2cip_id_t id);

      /**
       * Attempt to communicate with a device. Always sets the bus.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | { resetbus? : MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param resetbus Should the bus be reset? (Default: `true`)
       * @return Hardware failure: Device lost; no ACK (check MUX). Software failure: Failed to switch MUX bus.
       */
      static i2cip_errorlevel_t ping(const i2cip_fqa_t& fqa, bool resetbus = true);

      /**
       * Attempt to communicate with a device repeatedly until timeout. Always sets the bus.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | { until ACK or timeout: DEV ADDR (7) | ACK? | } { failed? : | MUX ADDR (7) | ACK? | }{ resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? |
       * @param fqa FQA of the device
       * @param setbus Should the bus be set? (Default: `true`, set false if checking EEPROM write!)
       * @param resetbus Should the bus be reset? (Default: `true`, set false if checking EEPROM write!)
       * @param timeout Attempt duration (ms)
       * @return Hardware failure: Device unreachable, module check. Software failure: Failed to switch MUX bus
       */
      static i2cip_errorlevel_t pingTimeout(const i2cip_fqa_t& fqa, bool setbus = true, bool resetbus = true, unsigned int timeout = 100);

      /**
       * Write one byte to a device.
       * | { setbus? : MUX ADDR (7) | MUX CONFIG (8) | ACK? | } DEV ADDR (7) | DATA BYTE (8) | ACK? | { setbus? : MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param value Byte to be written
       * @param setbus Should the MUX be set and reset? (Default: `true`)
       * @return Hardware failure: Device and/or module lost. Software failure: Failed to write and/or failed to switch MUX bus
       */
      static i2cip_errorlevel_t writeByte(const i2cip_fqa_t& fqa, const uint8_t& value, bool setbus = true);

      /**
       * Write data to a device.
       * | { setbus? : MUX ADDR (7) | MUX CONFIG (8) | ACK? | } DEV ADDR (7) | DATA BYTE (8 * len) | ACK? | { setbus? : MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param buffer Bytes to be sent
       * @param len Number of bytes (Default: `1`)
       * @param setbus Should the MUX be set and reset? (Default: `true`)
       * @return Hardware failure: Device and/or module lost. Software failure: Failed to write and/or failed to switch MUX bus
       */
      static i2cip_errorlevel_t write(const i2cip_fqa_t& fqa, const uint8_t* buffer, size_t len = 1, bool setbus = true);

      /**
       * Write one byte to a device's register. Effectively adds ONE prefix byte.
       * | { setbus? : MUX ADDR (7) | MUX CONFIG (8) | ACK? | } DEV ADDR (7) | REG ADDR (16) | DATA BYTE (8) | ACK? | { setbus? : MUX ADDR (7) | MUX RESET  (8) | ACK? | }
       * @param fqa FQA of the device
       * @param reg Register address
       * @param value Byte to be written
       * @param setbus Should the MUX be reset? (Default: `true`)
       * @return Hardware failure: Device and/or module lost. Software failure: Failed to write and/or failed to switch MUX bus
       */
      static i2cip_errorlevel_t writeRegister(const i2cip_fqa_t& fqa, const uint8_t& reg, const uint8_t& value, bool setbus = true);

      /**
       * Write one byte to a device's register (16-bit register address). Effectively adds TWO prefix bytes.
       * | { setbus? : MUX ADDR (7) | MUX CONFIG (8) | ACK? | } DEV ADDR (7) | REG ADDR (16) | DATA BYTE (8) | ACK? | { setbus? : | MUX ADDR (7) | MUX RESET  (8) | ACK? | }
       * @param fqa FQA of the device
       * @param reg Register address
       * @param value Byte to be written
       * @param setbus Should the MUX be reset? (Default: `true`)
       * @return Hardware failure: Device and/or module lost. Software failure: Failed to write and/or failed to switch MUX bus
       */
      static i2cip_errorlevel_t writeRegister(const i2cip_fqa_t& fqa, const uint16_t& reg, const uint8_t& value, bool setbus = true);

      static i2cip_errorlevel_t writeRegister(const i2cip_fqa_t& fqa, const uint8_t& reg, uint8_t* buffer, size_t len = 1, bool setbus = true);

      static i2cip_errorlevel_t writeRegister(const i2cip_fqa_t& fqa, const uint16_t& reg, uint8_t* buffer, size_t len = 1, bool setbus = true);

      /**
       * Request and read in data from a device.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | READ BYTES (8*len) |
       * resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? |
       * @param fqa FQA of the device
       * @param dest Bytes to read into
       * @param len Number of bytes to read (Default: `1`)
       * @param setbus Should the MUX be reset? (Default: `true`)
       */
      static i2cip_errorlevel_t read(const i2cip_fqa_t& fqa, uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);

      /**
       * Read one byte of data from the device.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | READ BYTES (8) |
       * resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? |
       * @param fqa FQA of the device
       * @param dest Byte to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       */
      static i2cip_errorlevel_t readByte(const i2cip_fqa_t& fqa, uint8_t& dest, bool resetbus = true);

      /**
       * Read one byte of data from the device.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | READ BYTES (16) |
       * resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? |
       * @param fqa FQA of the device
       * @param dest Word to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       */
      static i2cip_errorlevel_t readWord(const i2cip_fqa_t& fqa, uint16_t& dest, bool resetbus = true);

      static i2cip_errorlevel_t readRegister(const i2cip_fqa_t& fqa, const uint8_t& reg, uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);

      static i2cip_errorlevel_t readRegister(const i2cip_fqa_t& fqa, const uint16_t& reg, uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);
      

      /**
       * Read one byte of data from the device. Effectively adds a prefix byte.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | REG ADDR (8) | READ BYTES (8*len) | { resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param dest Bytes to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       */
      static i2cip_errorlevel_t readRegisterByte(const i2cip_fqa_t& fqa, const uint8_t& reg, uint8_t& dest, bool resetbus = true);

      /**
       * Read one byte of data from the device. Effectively adds TWO prefix bytes.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | REG ADDR (16) | READ BYTES (8*len) | { resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param dest Bytes to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       **/
      static i2cip_errorlevel_t readRegisterByte(const i2cip_fqa_t& fqa, const uint16_t& reg, uint8_t& dest, bool resetbus = true);

      /**
       * Read two bytes of data from the device. Effectively adds one prefix bytes.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | REG ADDR (16) | READ BYTES (8*len) | { resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param dest Bytes to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       **/
      static i2cip_errorlevel_t readRegisterWord(const i2cip_fqa_t& fqa, const uint8_t& reg, uint16_t& dest, bool resetbus = true);

      /**
       * Read two bytes of data from the device. Effectively adds TWO prefix bytes.
       * | MUX ADDR (7) | MUX CONFIG (8) | ACK? | DEV ADDR (7) | ACK? | DEV ADDR (7) | REG ADDR (16) | READ BYTES (8*len) | { resetbus? : | MUX ADDR (7) | MUX RESET (8) | ACK? | }
       * @param fqa FQA of the device
       * @param dest Bytes to read into
       * @param setbus Should the MUX be reset? (Default: `true`)
       **/
      static i2cip_errorlevel_t readRegisterWord(const i2cip_fqa_t& fqa, const uint16_t& reg, uint16_t& dest, bool resetbus = true);

    public:
      virtual ~Device() = 0;

      void setInput(InputGetter* input);
      void setOutput(OutputSetter* output);

      void removeInput(void);
      void removeOutput(void);

      InputGetter* getInput(void) const;
      OutputSetter* getOutput(void) const;

      i2cip_errorlevel_t get(const void* args = nullptr);
      i2cip_errorlevel_t set(const void* value = nullptr, const void* args = nullptr);

      const i2cip_fqa_t& getFQA(void) const;
      const i2cip_id_t& getID(void) const;

      i2cip_errorlevel_t ping(bool resetbus = true);
      i2cip_errorlevel_t pingTimeout(bool setbus = true, bool resetbus = true, unsigned int timeout = 100);
      i2cip_errorlevel_t writeByte(const uint8_t& value, bool setbus = true);
      i2cip_errorlevel_t write(const uint8_t* buffer, size_t len = 1, bool setbus = true);
      i2cip_errorlevel_t writeRegister(const uint8_t& reg, const uint8_t& value, bool setbus = true);
      i2cip_errorlevel_t writeRegister(const uint16_t& reg, const uint8_t& value, bool setbus = true);
      i2cip_errorlevel_t writeRegister(const uint8_t& reg, uint8_t* buffer, size_t len = 1, bool setbus = true);
      i2cip_errorlevel_t writeRegister(const uint16_t& reg, uint8_t* buffer, size_t len = 1, bool setbus = true);
      i2cip_errorlevel_t read(uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);
      i2cip_errorlevel_t readByte(uint8_t& dest, bool resetbus = true);
      i2cip_errorlevel_t readWord(uint16_t& dest, bool resetbus = true);
      i2cip_errorlevel_t readRegister(const uint8_t& reg, uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);
      i2cip_errorlevel_t readRegister(const uint16_t& reg, uint8_t* dest, size_t& len, bool nullterminate = true, bool resetbus = true);
      i2cip_errorlevel_t readRegisterByte(const uint8_t& reg, uint8_t& dest, bool resetbus = true);
      i2cip_errorlevel_t readRegisterByte(const uint16_t& reg, uint8_t& dest, bool resetbus = true);
      i2cip_errorlevel_t readRegisterWord(const uint8_t& reg, uint16_t& dest, bool resetbus = true);
      i2cip_errorlevel_t readRegisterWord(const uint16_t& reg, uint16_t& dest, bool resetbus = true);

      inline operator i2cip_fqa_t() const { return this->fqa; }
  };

  typedef Device* (* factory_device_t)(const i2cip_fqa_t& fqa);

  class DeviceGroup {
    friend class Module;

    protected:
      bool add(Device& device);
      bool addGroup(Device* devices[], uint8_t numdevices);
      void remove(Device* device);

    public:
      i2cip_id_t key;
      uint8_t numdevices = 0;
      Device* devices[I2CIP_DEVICES_PER_GROUP] = { nullptr };

      factory_device_t factory;

      DeviceGroup(i2cip_id_t key, factory_device_t factory);

      ~DeviceGroup();
      
      bool contains(Device* device) const;
      bool contains(const i2cip_fqa_t& fqa) const;

      Device* operator[](const i2cip_fqa_t& fqa) const;

      // DeviceGroup& operator=(const DeviceGroup& rhs);

      Device* operator()(const i2cip_fqa_t& fqa);
  };

  /**
   * An I2CIP peripheral used for input/state "getting".
   * @param G type used for "get" variable
   * @param A type used for "get" arguments
   **/
  template <typename G, typename A> class InputInterface : public InputGetter {
    private:
      G cache;  // Last RECIEVED value
      A argsA;  // Last passed arguments

      bool argsAset = false;
    protected:
      void setCache(G value);
      void setArgsA(A args);
      
      /**
       * Gets the default arguments used for the "get" operation.
       * A constant reference.
       * To be implemented by the child class.
      */
      virtual const A& getDefaultA(void) const = 0;
    public:
      InputInterface(Device* device);
      virtual ~InputInterface() = 0;

      i2cip_errorlevel_t get(const void* args = nullptr) override;

      /**
       * Gets the last recieved value.
      */
      G getCache(void) const;

      /**
       * Sets the cache to the default "zero" value.
       * To be implemented by the child class.
      */
      virtual void clearCache(void);

      /**
       * Gets the arguments used for the last "get" operation.
      */
      A getArgsA(void) const;

      /**
       * Gets the input device's state.
       **/
      virtual i2cip_errorlevel_t get(G& dest, const A& args) { return I2CIP_ERR_HARD; } // Unimplemented; Disable this device
  };

  /**
   * An I2CIP peripheral used for output/state "setting".
   * @param S type used for "set" value
   * @param B type used for "set" arguments
   **/
  template <typename S, typename B> class OutputInterface : public OutputSetter {
    private:
      S value;  // Last SET value (not PASSED value)
      B argsB;  // Last passed arguments

      bool argsBset = false;
    protected:
      void setValue(S value);
      void setArgsB(B args);

      /**
       * Gets the default arguments used for the "set" operation.
       * To be implemented by the child class.
      */
      virtual const B& getDefaultB(void) const = 0;
    public:
      OutputInterface(Device* device);
      
      virtual ~OutputInterface() = 0;

      i2cip_errorlevel_t set(const void* value = nullptr, const void* args = nullptr) override;

      /**
       * Gets the arguments used for the last "set" operation.
      */
      B getArgsB(void) const;

      /**
       * Gets the last set value.
      */
      S getValue(void) const;

      /**
       * Gets the default "zero"/off-state value.
       * To be implemented by the child class.
      */
      virtual void resetFailsafe(void);

      /**
       * Sets the output device's state.
       **/
      virtual i2cip_errorlevel_t set(const S& value, const B& args) { return I2CIP_ERR_HARD; } // Unimplemented; Disable this device
  };

  /**
   * An I2CIP peripheral used for input/state "getting" as well as output/state "setting".
   * @param G type used for "get" variable
   * @param A type used for "get" arguments
   * @param S type used for "set" value
   * @param B type used for "set" arguments
   **/
  template <typename G, typename A, typename S, typename B> class IOInterface : public InputInterface<G, A>, public OutputInterface<S, B> {
    public:
      IOInterface(Device* device);
      virtual ~IOInterface() = 0;
  };
};

#include <device.tpp>

#endif