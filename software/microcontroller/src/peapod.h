#ifndef PEAPOD_H_
#define PEAPOD_H_

#include <Arduino.h>
#include <I2CIP.h>

#include <state.h>
#include <chronograph.h>

#include <HT16K33.h>
#include <JHD1313.h>
#include <MCP23017.h>
#include <Nunchuck.h>
#include <PCA9685.h>
#include <Seesaw.h>
#include <SHT45.h>

#define PEAPOD_WIRENUM 0
#define PEAPOD_MODULENUM 0

namespace PeaPod {
  class PeaPodModule : public I2CIP::JsonModule {
    private:
    protected:
    I2CIP::DeviceGroup* deviceGroupFactory(const i2cip_id_t& id) override;
    public:
    PeaPodModule();
    
    void handleCommand(JsonObject command, Print& out) override;
    
    void handleConfig(JsonObject config, Print& out) override;
  };

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