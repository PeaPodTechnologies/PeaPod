#ifndef PEAPOD_H_
#define PEAPOD_H_

#include <Arduino.h>

#include <state.h>
#include <chronograph.h>

#include <I2CIP.h>

#include <HT16K33.h>
#include <JHD1313.h>
#include <MCP23017.h>
#include <Nunchuck.h>
#include <PCA9685.h>
#include <Seesaw.h>
#include <SHT45.h>
#include <K30.h>
#include <ADS1115.h>

#include <DebugJson.h>

#define PEAPOD_WIRENUM 0

#define PEAPOD_MODULENUM_AIR 0
#define PEAPOD_MODULENUM_WATERING 1
#define PEAPOD_MODULENUM_LIGHTING 2

#define PEAPOD_DELTA_HEARTBEAT 5000
#define PEAPOD_DELTA_MODULECHECK 100

#define PEAPOD_MODULENUM_SEVENSEG 0
#define PEAPOD_SEVENSEG_TEMPERATURE 1 // Uncomment to enable 7-segment display for temperature
// #define PEAPOD_SEVENSEG_HUMIDITY 1 // Uncomment to enable 7-segment display for humidity
// #define PEAPOD_SEVENSEG_CO2 1 // Uncomment to enable 7-segment display for CO2
// #define PEAPOD_SEVENSEG_ADC 1 // Uncomment to enable 7-segment display for ADC voltage

// #define PEAPOD_PROGRAM_DEFAULT 1 // Uncomment to enable default program on startup

namespace PeaPod {
  class PeaPodModule : public I2CIP::JsonModule {
    private:
    HashTable<FSM::Flag> flags = HashTable<FSM::Flag>();
    HashTable<FSM::Variable> variables = HashTable<FSM::Variable>();
    protected:
    I2CIP::DeviceGroup* deviceGroupFactory(const i2cip_id_t& id) override;
    public:
    PeaPodModule(const uint8_t& mux) : JsonModule(PEAPOD_WIRENUM, mux) { }
    
    void handleCommand(JsonObject command, Print& out) override;
    
    void handleConfig(JsonObject config, Print& out) override;

    void registerFlag(FSM::Flag* flag);
    void registerVariable(FSM::Variable* variable);

    void setFlag(const char* key, const bool& value);
    void setVariable(const char* key, const FSM::Number& value);
  };

  // Global States
  extern FSM::Variable cycle;
  extern FSM::Variable fps;

  void registerCallbacks(void);
  void configRouter(JsonObject command, Print& out);

  // Callbacks
  void callback_cycle(bool _, const FSM::Number& __);
  void callback_heartbeat(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
  template <unsigned char M, class T, typename std::enable_if<std::is_base_of<PeaPod::PeaPodModule, T>::value, int>::type = 0> void callback_module(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);

  template <i2cip_fqa_t F, unsigned char P> void callback_mcp23017_digitalWrite(bool _, const bool& v) {
    if(P > PIN_B7) return; // Invalid Pin
    if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr) {

      i2cip_mcp23017_bitmask_t gpio_mask = 1 << P;
      i2cip_mcp23017_t gpio_data = v ? (1 << P) : 0;
      i2cip_args_io_t gpio_args = { .g = false, .a = nullptr, .s = &gpio_data, .b = &gpio_mask };

      modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<MCP23017>(F, true, gpio_args, DebugJsonBreakpoints);
    }
  }

  template <i2cip_fqa_t F, unsigned char P> void callback_pca9685_analogWrite(bool _, const FSM::Number& v) {
    if(P > PCA9685_CH15) return; // Invalid Channel
    if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr && P <= PCA9685_CH15) {

      i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
      i2cip_pca9685_t data = (uint16_t)max(0, min(4096, (int)v));
      i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

      I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<PCA9685>(F, true, args, DebugJsonBreakpoints);
    }
  }

  template <i2cip_fqa_t F, unsigned char P> void callback_pca9685_onOff(bool _, const bool& v) {
    if(P > PCA9685_CH15) return; // Invalid Channel
    if(I2CIP::errlev[I2CIP_FQA_SEG_MODULE(F)] == I2CIP_ERR_NONE && I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)] != nullptr && P <= PCA9685_CH15) {

      i2cip_pca9685_chsel_t channel = (i2cip_pca9685_chsel_t)P;
      i2cip_pca9685_t data = v ? 4096 : 0;
      i2cip_args_io_t args = { .g = false, .a = nullptr, .s = &data, .b = &channel };

      I2CIP::modules[I2CIP_FQA_SEG_MODULE(F)]->operator()<PCA9685>(F, true, args, DebugJsonBreakpoints);
    }
  }

  // Onboard GPIO Helpers
  extern bool pinModeSet[255];

  template <unsigned char P> void controlPin(const bool& s) {
    if(!pinModeSet[P]) { pinMode(P, OUTPUT); }
    if(s) {
      digitalWrite(P, HIGH);
    } else {
      digitalWrite(P, LOW);
    }
  }
  template <unsigned char P> void controlPWM(const uint8_t& v) {
    if(!pinModeSet[P]) { pinMode(P, OUTPUT); }
    analogWrite(P, v);
  }

  template <unsigned char P> void controlPin(bool _, const bool& s) { controlPin<P>(s); }

  template <unsigned char P> void controlPWM(bool _, const FSM::Number& v) { 
    if(v.isFloating) {
      // 0.0 - 1.0
      controlPWM<P>(min(255, max(0, (int)((double)v * 255.f))));
    } else {
      // 0 - 255
      controlPWM<P>(min(255, max(0, (int)(v))));
    }
  }
}

#endif