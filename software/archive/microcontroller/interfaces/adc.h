#ifndef CSOS_INTERFACES_ADC_H_
#define CSOS_INTERFACES_ADC_H_

#include <Arduino.h>
#include <I2CIP.h>

// Not sure if this will break something. Let's try it and find out
#ifdef ADC
#undef ADC
#endif

// REGISTERS

// Pointer register
#define ADC_REG_POINTER_MASK        (uint8_t)0x03 // Point mask
#define ADC_REG_POINTER_CONVERT     (uint8_t)0x00 // Conversion
#define ADC_REG_POINTER_CONFIG      (uint8_t)0x01 // Configuration
#define ADC_REG_POINTER_LOWTHRESH   (uint8_t)0x02 // Low threshold
#define ADC_REG_POINTER_HITHRESH    (uint8_t)0x03 // High threshold

// Config register: OS bit
#define ADC_REG_CONFIG_OS_MASK      (uint16_t)0x8000 // OS Mask
#define ADC_REG_CONFIG_OS_SINGLE    (uint16_t)0x8000 // Write: Set to start a single-conversion
#define ADC_REG_CONFIG_OS_BUSY      (uint16_t)0x0000 // Read: Bit = 0 when conversion is in progress
#define ADC_REG_CONFIG_OS_NOTBUSY   (uint16_t)0x8000 // Read: Bit = 1 when device is not performing a conversion

// Config register: MUX bits
#define ADC_REG_CONFIG_MUX_MASK     (uint16_t)0x7000 // Mux Mask
#define ADC_REG_CONFIG_MUX_DIFF_0_1 (uint16_t)0x0000 // Differential P = AIN0, N = AIN1 (default)
#define ADC_REG_CONFIG_MUX_DIFF_0_3 (uint16_t)0x1000 // Differential P = AIN0, N = AIN3
#define ADC_REG_CONFIG_MUX_DIFF_1_3 (uint16_t)0x2000 // Differential P = AIN1, N = AIN3
#define ADC_REG_CONFIG_MUX_DIFF_2_3 (uint16_t)0x3000 // Differential P = AIN2, N = AIN3
#define ADC_REG_CONFIG_MUX_SINGLE_0 (uint16_t)0x4000 // Single-ended AIN0
#define ADC_REG_CONFIG_MUX_SINGLE_1 (uint16_t)0x5000 // Single-ended AIN1
#define ADC_REG_CONFIG_MUX_SINGLE_2 (uint16_t)0x6000 // Single-ended AIN2
#define ADC_REG_CONFIG_MUX_SINGLE_3 (uint16_t)0x7000 // Single-ended AIN3

// Config register: Gain bits
#define ADC_REG_CONFIG_PGA_MASK     (uint16_t)0x0E00 // PGA Mask
#define ADC_REG_CONFIG_PGA_6_144V   (uint16_t)0x0000 // +/-6.144V range = Gain 2/3
#define ADC_REG_CONFIG_PGA_4_096V   (uint16_t)0x0200 // +/-4.096V range = Gain 1
#define ADC_REG_CONFIG_PGA_2_048V   (uint16_t)0x0400 // +/-2.048V range = Gain 2 (default)
#define ADC_REG_CONFIG_PGA_1_024V   (uint16_t)0x0600 // +/-1.024V range = Gain 4
#define ADC_REG_CONFIG_PGA_0_512V   (uint16_t)0x0800 // +/-0.512V range = Gain 8
#define ADC_REG_CONFIG_PGA_0_256V   (uint16_t)0x0A00 // +/-0.256V range = Gain 16

// Config register: ADC mode bit
#define ADC_REG_CONFIG_MODE_MASK    (uint16_t)0x0100 // Mode Mask
#define ADC_REG_CONFIG_MODE_CONTIN  (uint16_t)0x0000 // Continuous conversion mode
#define ADC_REG_CONFIG_MODE_SINGLE  (uint16_t)0x0100 // Power-down single-shot mode (default)

// Config register: sample rate bits
#define ADC_REG_CONFIG_RATE_MASK    (uint16_t)0x00E0 // Data Rate Mask
#define ADC_RATE_128SPS             (uint16_t)0x0000 // 128 samples per second
#define ADC_RATE_250SPS             (uint16_t)0x0020 // 250 samples per second
#define ADC_RATE_490SPS             (uint16_t)0x0040 // 490 samples per second
#define ADC_RATE_920SPS             (uint16_t)0x0060 // 920 samples per second
#define ADC_RATE_1600SPS            (uint16_t)0x0080 // 1600 samples per second (default)
#define ADC_RATE_2400SPS            (uint16_t)0x00A0 // 2400 samples per second
#define ADC_RATE_3300SPS            (uint16_t)0x00C0 // 3300 samples per second

// Config register: Comparator mode bit
#define ADC_REG_CONFIG_CMODE_MASK   (uint16_t)0x0010 // CMode Mask
#define ADC_REG_CONFIG_CMODE_TRAD   (uint16_t)0x0000 // Traditional comparator with hysteresis (default)
#define ADC_REG_CONFIG_CMODE_WINDOW (uint16_t)0x0010 // Window comparator

// Config register: Comparator polarity bit
#define ADC_REG_CONFIG_CPOL_MASK    (uint16_t)0x0008 // CPol Mask
#define ADC_REG_CONFIG_CPOL_ACTVLOW (uint16_t)0x0000 // ALERT/RDY pin is low when active (default)
#define ADC_REG_CONFIG_CPOL_ACTVHI  (uint16_t)0x0008 // ALERT/RDY pin is high when active

// Config register: Comparator latching bit
#define ADC_REG_CONFIG_CLAT_MASK    (uint16_t)0x0004 // Determines if ALERT/RDY pin latches once asserted
#define ADC_REG_CONFIG_CLAT_NONLAT  (uint16_t)0x0000 // Non-latching comparator (default)
#define ADC_REG_CONFIG_CLAT_LATCH   (uint16_t)0x0004 // Latching comparator

// Config register: Comparator queue bits
#define ADC_REG_CONFIG_CQUE_MASK    (uint16_t)0x0003 // CQue Mask
#define ADC_REG_CONFIG_CQUE_1CONV   (uint16_t)0x0000 // Assert ALERT/RDY after one conversions
#define ADC_REG_CONFIG_CQUE_2CONV   (uint16_t)0x0001 // Assert ALERT/RDY after two conversions
#define ADC_REG_CONFIG_CQUE_4CONV   (uint16_t)0x0002 // Assert ALERT/RDY after four conversions
#define ADC_REG_CONFIG_CQUE_NONE    (uint16_t)0x0003 // Disable the comparator and put ALERT/RDY in high state (default)

// HELPERS
#define ADC_CHANNEL_TO_MUX(channel) (0x4000 + (channel) * 0x1000)

// SETTINGS
#define ADC_ADDR    0x48
#define ADC_SHIFT   4
#define ADC_GAIN    ADC::GAIN_TWOTHIRDS // +/-6.144V for 0-5V single-ended analogRead
#define ADC_SPS     ADC_RATE_1600SPS
#define ADC_TIMEOUT 100

using namespace I2CIP;

typedef enum {
  ADC_CHANNEL_NULL = 0xFF,
  ADC_CHANNEL_0 = 0x0,
  ADC_CHANNEL_1,
  ADC_CHANNEL_2,
  ADC_CHANNEL_3
} args_adc_t;

namespace ControlSystemsOS {

  // Interface class for the ADS1015 12-bit ADC IC. Reads analog voltage (range: +/-6.144V)
  class ADC : public Device, public InputInterface<float, args_adc_t> {
    friend Device* ControlSystemsOS::adcFactory(const i2cip_fqa_t& fqa);
    
    private:
      const float default_cache = 0.0f;
      const args_adc_t default_a = ADC_CHANNEL_NULL;

      // Gain settings
      typedef enum {
        GAIN_TWOTHIRDS = ADC_REG_CONFIG_PGA_6_144V,
        GAIN_ONE = ADC_REG_CONFIG_PGA_4_096V,
        GAIN_TWO = ADC_REG_CONFIG_PGA_2_048V,
        GAIN_FOUR = ADC_REG_CONFIG_PGA_1_024V,
        GAIN_EIGHT = ADC_REG_CONFIG_PGA_0_512V,
        GAIN_SIXTEEN = ADC_REG_CONFIG_PGA_0_256V
      } adc_gain_t;

      static float computeVolts(int16_t counts);
      
      ADC(const i2cip_fqa_t& fqa, const i2cip_id_t& id);

    public:
      /**
       * Read an ADC channel.
       * @param fqa
       * @param dest Pin state
       * @param args Pin number
       **/
      i2cip_errorlevel_t get(float& dest, const args_adc_t& args) override;

      const args_adc_t& getDefaultA(void) const override;
      void clearCache(void) override;
  };
};

#endif