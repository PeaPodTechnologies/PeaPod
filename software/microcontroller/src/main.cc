#ifndef UNIT_TEST
#define UNIT_TEST 1
#define IS_MAIN 1

#include <Arduino.h>

#include <peapod.h>
#include <air.h>
#include <water.h>
#include <light.h>

#include <DebugJson.h>

// #define SEVENSEG_TEMPERATURE 1 // Uncomment to enable 7-segment display for temperature
// #define SEVENSEG_HUMIDITY 1 // Uncomment to enable 7-segment display for humidity
// #define SEVENSEG_CO2 1 // Uncomment to enable 7-segment display for CO2
#define SEVENSEG_ADC 1

#define PEAPOD_MODULENUM_PCA9685 2
#define PEAPOD_MODULENUM_MCP23017 1
#define PEAPOD_MODULENUM_SEVENSEG 0
#define PEAPOD_MODULENUM_ADS1115  2

void callback_temperature(bool _, const FSM::Number& v);
void callback_humidity(bool _, const FSM::Number& v);
void callback_co2(bool _, const FSM::Number& v);

template <unsigned char P> void callback_mcp23017_digitalWrite(bool _, const bool& v);
template <unsigned char P> void callback_pca9685_analogWrite(bool _, const FSM::Number& v);
template <unsigned char P> void callback_pca9685_onOff(bool _, const bool& v);
template <unsigned char C> void callback_adc_read(bool _, const FSM::fsm_timestamp_t& __);

void callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __);
void callback_adc_voltage(bool _, const FSM::Number& v);

using namespace PeaPod;
using namespace I2CIP;

// SETUP GLOBALS

// i2cip_fqa_t fqa_sht45 = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SHT45_ADDRESS);
// i2cip_fqa_t fqa_lcd = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 1, I2CIP_JHD1313_ADDRESS);
// i2cip_fqa_t fqa_rotary = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SEESAW_ADDRESS);
// i2cip_fqa_t fqa_nunchuck = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_NUNCHUCK_ADDRESS);
i2cip_fqa_t fqa_gpio = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM_MCP23017, 0, I2CIP_MCP23017_ADDRESS);
i2cip_fqa_t fqa_pwm = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM_PCA9685, 0, I2CIP_PCA9685_ADDRESS);
i2cip_fqa_t fqa_adc = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM_ADS1115, 0, I2CIP_ADS1115_ADDRESS);

FSM::Flag led_zero = FSM::Flag("led0");
FSM::Flag led_one = FSM::Flag("led1");
FSM::Variable led_pwm = FSM::Variable(FSM::notanumber, "led_pwm");
FSM::Variable adc_voltage = FSM::Variable(FSM::notanumber, "adc_volts");

void setup(void) {
  // 0. Builtin LED Pinmode; Serial Begin

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }
  
  delay(100);

  DebugJson::revision(I2CIP_REVISION, Serial);

  delay(100);

  // Instantiate modules ahead of time
  PeaPod::callback_module<PEAPOD_MODULENUM_AIR, PeaPodModuleAir>();
  PeaPod::callback_module<PEAPOD_MODULENUM_WATERING, PeaPodModuleWatering>();
  PeaPod::callback_module<PEAPOD_MODULENUM_LIGHTING, PeaPodModuleLighting>();

  // // Print all devices
  // delay(100);
  // DEBUG_JSON(I2CIP::devicetree.toString());
  // DEBUG_JSON(modules[PEAPOD_MODULENUM]->toString());

  delay(100);

  PeaPod::registerCallbacks();

  delay(100);

  // Load Devices
  // modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<PCA9685>(fqa_pca9685, false, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<SHT45>(fqa_sht45, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<JHD1313>(fqa_lcd, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<Seesaw>(fqa_rotary, true, _i2cip_args_io_default, DebugJsonBreakpoints);

  PeaPod::air_humidity.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_humidity);
  PeaPod::air_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_temperature);
  PeaPod::air_co2.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_co2);

  FSM::Chronos.addIntervalFlag(2000, 0, &led_zero, false);
  FSM::Chronos.addIntervalFlag(2000, 1000, &led_zero, true);
  led_zero.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PIN_A0>);

  FSM::Chronos.addIntervalFlag(10000, 0, &led_one, false);
  FSM::Chronos.addIntervalFlag(10000, 5000, &led_one, true);
  led_one.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PIN_B0>);

  FSM::Chronos.addInterval(50, 0, callback_pwm_cycle, false);
  led_pwm.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_pca9685_analogWrite<PCA9685_CH0>);

  FSM::Chronos.addInterval(100, 0, callback_adc_read<ADS1115_CHANNEL_0>, false);
  adc_voltage.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_adc_voltage);
}

// LOOP GLOBALS

void loop(void) {

  // I2CIP_DEBUG_SERIAL.println(I2CIP::devicetree.toString());
  // I2CIP_DEBUG_SERIAL.println(modules[PEAPOD_MODULENUM]->toString());

  PeaPod::cycle.set(PeaPod::cycle.get() + FSM::Number(1, false, false));
}

// HELPER FUNCTIONS

void callback_temperature(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_temperature.getKey());

  #ifdef SEVENSEG_TEMPERATURE
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

void callback_humidity(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_humidity.getKey());

  #ifdef SEVENSEG_HUMIDITY
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_1F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

void callback_co2(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (unsigned)v, air_co2.getKey());

  #ifdef SEVENSEG_CO2
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_UINT;
    i2cip_ht16k33_data_t seg_data = { .h = (unsigned)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

template <unsigned char P> void callback_mcp23017_digitalWrite(bool _, const bool& v) {
  if(P > PIN_B7) return; // Invalid Pin
  if(I2CIP::errlev[PEAPOD_MODULENUM_MCP23017] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_MCP23017] != nullptr) {

    i2cip_mcp23017_bitmask_t gpio_mask = 1 << P;
    i2cip_mcp23017_t gpio_data = v ? (1 << P) : 0;
    i2cip_args_io_t gpio_args = { .g = false, .a = nullptr, .s = &gpio_data, .b = &gpio_mask };

    modules[PEAPOD_MODULENUM_MCP23017]->operator()<MCP23017>(fqa_gpio, true, gpio_args, DebugJsonBreakpoints);
  }
}

template <unsigned char P> void callback_pca9685_analogWrite(bool _, const FSM::Number& v) {
  if(P > PCA9685_CH15) return; // Invalid Channel
  if(I2CIP::errlev[PEAPOD_MODULENUM_PCA9685] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_PCA9685] != nullptr && P <= PCA9685_CH15) {

    i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
    i2cip_pca9685_t data = (uint16_t)max(0, min(4096, (int)v));
    i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

    I2CIP::modules[PEAPOD_MODULENUM_PCA9685]->operator()<PCA9685>(fqa_pwm, true, args, DebugJsonBreakpoints);
  }
}

template <unsigned char P> void callback_pca9685_onOff(bool _, const bool& v) {
  if(P > PCA9685_CH15) return; // Invalid Channel
  if(I2CIP::errlev[PEAPOD_MODULENUM_PCA9685] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_PCA9685] != nullptr && P <= PCA9685_CH15) {

    i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
    i2cip_pca9685_t data = v ? 4096 : 0;
    i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

    I2CIP::modules[PEAPOD_MODULENUM_PCA9685]->operator()<PCA9685>(fqa_pwm, true, args, DebugJsonBreakpoints);
  }
}

void callback_pwm_cycle(bool _, const FSM::fsm_timestamp_t& __) {
  double pwm = ((cos((FSM::Chronos.get() % 5000) / 5000.0 * 2.0 * M_PI) + 1.0) * 2048.0); // 0 to 1 over 5s

  led_pwm.set(FSM::Number(pwm, true, false));
}

template <unsigned char C> void callback_adc_read(bool _, const FSM::fsm_timestamp_t& __) {
  if(C > ADS1115_CHANNEL_3) return; // Invalid Channel
  if(I2CIP::errlev[PEAPOD_MODULENUM_ADS1115] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_ADS1115] != nullptr) {

    i2cip_ads1115_chsel_t channel = (i2cip_ads1115_chsel_t)C;
    i2cip_args_io_t args = { .g = true, .a = nullptr, .s = nullptr, .b = &channel };

    i2cip_errorlevel_t errlev = I2CIP::modules[PEAPOD_MODULENUM_ADS1115]->operator()<ADS1115>(fqa_adc, true, args, DebugJsonBreakpoints);

    if(errlev == I2CIP_ERR_NONE){
      Device** dptr = I2CIP::devicetree[fqa_adc];
      if(dptr != nullptr) {
        ADS1115* ads = (ADS1115*)(*dptr);
        float voltage = ads->getCache();
        adc_voltage.set(FSM::Number((double)voltage, true, false));
      }
    }
  }
}
void callback_adc_voltage(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, adc_voltage.getKey());

  #ifdef SEVENSEG_ADC
  if(I2CIP::errlev[PEAPOD_MODULENUM_SEVENSEG] == I2CIP_ERR_NONE && I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG] != nullptr) {
    i2cip_ht16k33_mode_t seg_mode = SEG_2F;
    i2cip_ht16k33_data_t seg_data = { .f = (float)v };
    i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };

    I2CIP::modules[PEAPOD_MODULENUM_SEVENSEG]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
  }
  #endif
}

#endif