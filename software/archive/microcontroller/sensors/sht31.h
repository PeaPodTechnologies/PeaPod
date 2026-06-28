#ifndef CSOS_INTERFACES_SHT31_H_
#define CSOS_INTERFACES_SHT31_H_

#include <Arduino.h>
#include <I2CIP.h>

#include <sensors/sensor.h>

// MACROS

// Registers
#define SHT31_MEAS_HIGHREP_STRETCH  (uint16_t)0x2C06  // Measurement High Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_MEDREP_STRETCH   (uint16_t)0x2C0D  // Measurement Medium Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_LOWREP_STRETCH   (uint16_t)0x2C10  // Measurement Low Repeatability with Clock Stretch Enabled
#define SHT31_MEAS_HIGHREP          (uint16_t)0x2400  // Measurement High Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_MEDREP           (uint16_t)0x240B  // Measurement Medium Repeatability with Clock Stretch Disabled
#define SHT31_MEAS_LOWREP           (uint16_t)0x2416  // Measurement Low Repeatability with Clock Stretch Disabled
#define SHT31_READSTATUS            (uint16_t)0xF32D  // Read Out of Status Register
#define SHT31_CLEARSTATUS           (uint16_t)0x3041  // Clear Status
#define SHT31_SOFTRESET             (uint16_t)0x30A2  // Soft Reset
#define SHT31_HEATEREN              (uint16_t)0x306D  // Heater Enable
#define SHT31_HEATERDIS             (uint16_t)0x3066  // Heater Disable
#define SHT31_REG_HEATER_BIT        (uint16_t)0x000d  // Status Register Heater Bit

// Settings
#define SHT31_ADDR  0x44  // SHT31 Default Address

using namespace I2CIP;

typedef enum {
  SHT31_HEATER_DISABLE  = 0x00,
  SHT31_HEATER_ENABLE   = 0x01,
} args_sht31_t;

typedef struct {
  float temperature;
  float humidity;
} state_sht31_t;

namespace ControlSystemsOS {

  // Interface class for the SHT31 air temperature and humidity sensor
  class SHT31 : public Device, public InputInterface<state_sht31_t, args_sht31_t>, public Sensor {
    friend Device* ControlSystemsOS::sht31Factory(const i2cip_fqa_t& fqa);

    // Note: unsigned 16-bit args are TRUNCATED to 12-bit PWM control
    private:
      const state_sht31_t default_cache = { NAN, NAN };
      const args_sht31_t default_a = SHT31_HEATER_DISABLE;

      SHT31(const i2cip_fqa_t& fqa, const i2cip_id_t& id);

    public:
      i2cip_errorlevel_t get(state_sht31_t& value, const args_sht31_t& args) override;

      const args_sht31_t& getDefaultA(void) const override;
      void clearCache(void) override;

      Datum* datumFactory(void) override;
  };
};

#endif