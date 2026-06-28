#include "simple.h"

#include <chrono.h>
// #include "../FiniteStateMachine/debug.h"
#ifndef DEBUG_DELAY
#define DEBUG_DELAY() {delay(10);}
#endif

Flag lighting = Flag();
Flag watering = Flag();
Flag disco = Flag();

void logTimestamp(bool _, const fsm_timestamp_t& __) {
  fsm_timestamp_t timestamp = Chronos.get();

  DEBUG_DELAY();
  Serial.print(F("==== [ Time Elapsed: "));
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds -= minutes * 60;
  minutes -= hours * 60;

  if(hours < 10) Serial.print("0");
  Serial.print(hours);
  Serial.print("h : ");
  if(minutes < 10) Serial.print("0");
  Serial.print(minutes - hours * 60);
  Serial.print("m : ");
  if(seconds < 10) Serial.print("0");
  Serial.print(seconds);
  Serial.println(F("s ] ===="));
  DEBUG_DELAY();
}

void logLights(bool _, const bool& on) {
  DEBUG_DELAY();
  Serial.print(F("== [ Lights: "));
  Serial.print(on ? F("ON") : F("OFF"));
  Serial.println(F(" ] =="));
  DEBUG_DELAY();
}

void logWatering(bool _, const bool& on) {
  DEBUG_DELAY();
  Serial.print(F("== [ Watering: "));
  Serial.print(on ? F("ON") : F("OFF"));
  Serial.println(F(" ] =="));
  DEBUG_DELAY();
}

void logDisco(bool _, const bool& on) {
  DEBUG_DELAY();
  Serial.print(F("== [ Disco: "));
  Serial.print(on ? F("ON") : F("OFF"));
  Serial.println(F(" ] =="));
  DEBUG_DELAY();
}

void timerControlLights(bool on, const fsm_timestamp_t& timestamp) {
  if(on) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
}

void controlLights(bool _, const bool& lights) {
  if(lights) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
}

void controlWatering(bool _, const bool& watering) {
  if(watering) {
    digitalWrite(PIN_WATERING, HIGH);
  } else {
    digitalWrite(PIN_WATERING, LOW);
  }
}

void controlDisco(bool _, const bool& disco) {
  if(disco) {
    // Disable - don't worry about setting, disco scroll will take it from here
    lighting.disable();
  } else {
    // Enable and retrigger
    lighting.resume();
  }
}

void scrollDisco(bool _, const fsm_timestamp_t& __) {
  fsm_timestamp_t timestamp = Chronos.get();

  uint8_t discoPWM = (uint8_t)((cos(((timestamp % DISCO_CYCLE_MS) / (float)DISCO_CYCLE_MS) * 6.2831853) / 2.25 + 0.5) * 255);

  if(disco.get() && lighting.isDisabled()) {
    DEBUG_DELAY();
    Serial.print(F("== [ Disco: "));
    Serial.print(discoPWM);
    Serial.print(F(" ] ==\n"));
    DEBUG_DELAY();
    
    analogWrite(PIN_LIGHTING_HI, discoPWM);
    analogWrite(PIN_LIGHTING_LO, 255-discoPWM);
  }
}