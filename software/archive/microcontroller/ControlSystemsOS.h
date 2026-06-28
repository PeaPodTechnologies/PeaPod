#ifndef CSOS_CSOS_H_
#define CSOS_CSOS_H_

#include <Arduino.h>
#include <I2CIP.h>
#include <chrono.h>

#include <ArduinoJson.h>

#include <interfaces/linker.h>

#ifndef MAP_INDEX_COUNT
#define MAP_INDEX_COUNT 1
#endif

#include <sensors/sensor.h>
// #include <actuators/actuator.h>

#define CSOS_MODULE_JSON_DOCSIZE (size_t)(I2CIP_EEPROM_SIZE * 3 / 2)

using namespace I2CIP;

typedef i2cip_errorlevel_t (* interfaceHandler_t)(Device*);

namespace ControlSystemsOS {

  // MODULE DEFINITION

  class CSOSModule : public Module {
    private:
      StaticJsonDocument<CSOS_MODULE_JSON_DOCSIZE> eeprom_json;

    protected:
      DeviceGroup* deviceGroupFactory(i2cip_id_t id) override;

      bool parseEEPROMContents(const char* eeprom_contents) override;

    public:
      CSOSModule(const uint8_t& wire, const uint8_t& module);

      ~CSOSModule() { }
  };


  i2cip_errorlevel_t update(bool build = false);
  i2cip_errorlevel_t update(const uint8_t& wire, const uint8_t& mod, bool build = true);
  i2cip_errorlevel_t fixedUpdate(unsigned long timestamp);
  i2cip_errorlevel_t fixedUpdate(unsigned long timestamp, CSOSModule& m);
      
  extern CSOSModule* csos_modules[I2CIP_NUM_WIRES][I2CIP_MUX_COUNT];
  extern char device_id_map[MAP_INDEX_COUNT][I2CIP_ID_SIZE];
  extern bool device_id_loaded[MAP_INDEX_COUNT];

  // static Linker linker;
  
  // extern fsm_timestamp_t csos_modules_lastChecked[I2CIP_NUM_WIRES][I2CIP_MUX_COUNT];

  // extern const csos_types_t csos_map_interface_state[];
  // extern const csos_types_t csos_map_interface_args[];

  // extern bool stateChange;

  // new SHT31(fqa)

  // void initialize(void);
  

  // i2cip_errorlevel_t handleDevice(Device* device);
  // template <typename G, typename A> i2cip_errorlevel_t handleInputDevice(Device* device, const A& args);
  // template <typename S, typename B> i2cip_errorlevel_t handleOutputDevice(Device* device, const S& value, const B& args);

  /**
   * Find a device group by ID.
   * @param id
   * @returns Pointer to the device group (`nullptr` if none)
   */
  // DeviceGroup* operator[](const char* id);

  /**
   * Find a device ID by FQA.
   * @param fqa
   * @returns Pointer to the device ID (`nullptr` if none)
   */
  // const char* operator[](const i2cip_fqa_t& fqa);

  // const i2cip_devicetree_t& getDevices(void);

  // const i2cip_devicetable_t& getDeviceGroups(void);
};

#endif