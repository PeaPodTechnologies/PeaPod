#ifndef PEAPOD_H_
#define PEAPOD_H_

#include <Arduino.h>

// Select a Serial port for output
// #define PEAPOD_SERIAL Serial // USB
#define PEAPOD_SERIAL Serial1 // GPIO TX/RX

#define DEBUG_SERIAL PEAPOD_SERIAL // For DebugJson

#include <state.h>
#include <chronograph.h>

#include <I2CIP.h>

#include <HT16K33.h>
// #include <JHD1313.h> // Replaced with LCD-MCP23008
#include <LCD-MCP23008.h>
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
#define PEAPOD_MODULENUM_CONTROL 3

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
    HashTable<bool> locker = HashTable<bool>();
    protected:
    I2CIP::DeviceGroup* deviceGroupFactory(const i2cip_id_t& id) override;

    public:
    PeaPodModule(const uint8_t& mux) : JsonModule(PEAPOD_WIRENUM, mux) { }
    
    void handleCommand(JsonObject command, Print& out) override;
    
    void handleConfig(JsonObject config, Print& out) override;
    
    void registerFlag(FSM::Flag* flag, bool locked = false);
    void registerVariable(FSM::Variable* variable, bool locked = false);

    FSM::Flag* getFlagByIndex(uint8_t index) const;
    FSM::Variable* getVariableByIndex(uint8_t index) const;

    uint8_t getNumFlags(void) const { return this->flags.size(); }
    uint8_t getNumVariables(void) const { return this->variables.size(); }
  };

  extern PeaPodModule nomodule;

  // Global States
  extern FSM::Variable cycle;
  extern FSM::Variable fps;

  void registerCallbacks(void);
  void configRouter(JsonObject command, Print& out);

  // Callbacks
  void callback_cycle(bool _, const FSM::Number& __);
  void callback_heartbeat(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
  template <unsigned char M, class T, typename std::enable_if<std::is_base_of<PeaPod::PeaPodModule, T>::value, int>::type = 0> void callback_module(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);

  template <i2cip_fqa_t F, unsigned char P> void callback_mcp23017_digitalWrite(bool _, const bool& v);
  template <i2cip_fqa_t F, unsigned char P> void callback_pca9685_analogWrite(bool _, const FSM::Number& v);
  template <i2cip_fqa_t F, unsigned char P> void callback_pca9685_onOff(bool _, const bool& v);

  // Onboard GPIO Helpers
  extern bool pinModeSet[255];

  template <unsigned char P> void controlPin(const bool& s) {
    if(!pinModeSet[P]) { pinMode(P, OUTPUT); pinModeSet[P] = true; }
    if(s) {
      digitalWrite(P, HIGH);
    } else {
      digitalWrite(P, LOW);
    }
  }
  template <unsigned char P> void controlPWM(const uint8_t& v) {
    if(!pinModeSet[P]) { pinMode(P, OUTPUT); pinModeSet[P] = true; }
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

#include <peapod.tpp>

#endif