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

#define PEAPOD_WIRENUM 0

#define PEAPOD_MODULENUM_AIR 0
#define PEAPOD_MODULENUM_WATERING 1
#define PEAPOD_MODULENUM_LIGHTING 2

#define PEAPOD_DELTA_HEARTBEAT 5000
#define PEAPOD_DELTA_MODULECHECK 100

namespace PeaPod {
  class PeaPodModule : public I2CIP::JsonModule {
    private:
    protected:
    I2CIP::DeviceGroup* deviceGroupFactory(const i2cip_id_t& id) override;
    public:
    PeaPodModule(const uint8_t& mux) : JsonModule(PEAPOD_WIRENUM, mux) { }
    
    void handleCommand(JsonObject command, Print& out) override;
    
    void handleConfig(JsonObject config, Print& out) override;
  };

  // Global States
  extern FSM::Variable cycle;
  extern FSM::Variable fps;

  void registerCallbacks(void);

  // Callbacks
  void callback_cycle(bool _, const FSM::Number& __);
  void callback_heartbeat(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
  template <unsigned char M, class T, typename std::enable_if<std::is_base_of<PeaPod::PeaPodModule, T>::value, int>::type = 0> void callback_module(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);

  // GPIO Helpers
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