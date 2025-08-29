#ifndef CSOS_SIMPLE_H_
#define CSOS_SIMPLE_H_

#include <chrono.h>

// Watering

#define PIN_WATERING        2
#define DURATION_WATERING   2000     // 10 seconds
#define PERIOD_WATERING  200000   // 30 minutes

// Lighting

#define DELAY_START 1000

#define PIN_LIGHTING_LO    5
#define PIN_LIGHTING_HI   6
// #define PIN_LIGHTING_PHOTO  5
#define DURATION_LIGHTING (TWENTYFOURHRS_MILLIS*3/4)
#define PWM_LIGHTING_LO    (unsigned char)(255*0.3)
#define PWM_LIGHTING_HI   (unsigned char)(255*0.6)

// Fun Stuff
#define PIN_DISCO_BUTTON  10
#define DELTA_DISCO  10
#define DISCO_CYCLE_MS 1000

void logTimestamp(bool _, const fsm_timestamp_t& timestamp);
void logLights(bool _, const bool& on);
void logWatering(bool _, const bool& on);
void logDisco(bool _, const bool& on);

// void timerControlLights(bool on, const fsm_timestamp_t& timestamp);
void controlLights(bool _, const bool& lights);
void controlWatering(bool _, const bool& watering);
void controlDisco(bool _, const bool& __);

void scrollDisco(bool _, const fsm_timestamp_t& __);

extern Flag lighting;
extern Flag watering;
extern Flag disco;

#endif