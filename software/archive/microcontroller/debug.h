#ifndef CSOS_DEBUG_SERIAL

#include <Arduino.h>

// #define DEBUG 1 // Uncomment to enable debug

// BASIC DEBUG MACRO
#ifdef DEBUG
#if DEBUG == 1
#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL Serial
#endif
#endif
#endif

// CROSS-LIBRARY DEBUG COMPATIBILITY
#ifdef DEBUG_SERIAL
#define CSOS_DEBUG_SERIAL DEBUG_SERIAL
#endif

// DEBUG DELAY MACRO FOR SERIAL OUTPUT STABILITY (OPTIONAL)
#ifdef CSOS_DEBUG_SERIAL
#ifndef DEBUG_DELAY
#define DEBUG_DELAY() {delay(10);}
#endif
#endif

// #define DEBUG_DISABLE_FSTRINGS 1
#ifdef DEBUG_DISABLE_FSTRINGS
#define _F(x) x
#else
#define _F(x) F(x)
#endif

#endif

// #include <../I2CIP/debug.h>
// #include <../FiniteStateMachine/debug.h>