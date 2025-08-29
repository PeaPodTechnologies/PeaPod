#ifndef FSM_DEBUG_SERIAL

#include <Arduino.h>

// Uncomment to enable debug
// #define DEBUG 1

// CROSS-LIBRARY DEBUG COMPATIBILITY
#ifdef DEBUG
#if DEBUG == true
#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL Serial
#endif
#endif
#endif

#ifdef DEBUG_SERIAL
// Just once
#define FSM_DEBUG_SERIAL DEBUG_SERIAL

#ifndef DEBUG_DELAY
#define DEBUG_DELAY() { DEBUG_SERIAL.flush(); delay(10); }
#endif
#endif

// #define DEBUG_DISABLE_FSTRINGS 1
#ifdef DEBUG_DISABLE_FSTRINGS
#define _F(x) x
#else
#define _F(x) F(x)
#endif

#endif