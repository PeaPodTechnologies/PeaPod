#ifndef BASE_H
#define BASE_H

#include "Arduino.h"

// Degree of error occurred
typedef enum errorlevel_t {
  ERR_NONE,
  ERR_WARNING,
  ERR_FATAL,        // NOTE: Implies debug level 'DISABLED'
} errorlevel_t;

// General state information - On error level > 0, indicates last successful state
typedef enum debuglevel_t {
  // NOTE: The various debug states imply the value of state.data as follows:
  DS_DISABLED,      // state.data[*].value == NULL
  DS_INITIALIZED,   // state.data[*].value == NULL
  DS_WAITING,       // state.data[*].value != NULL, age > 0
  DS_SUCCESS,       // state.data[*].value != NULL, age == 0
} debuglevel_t;

#endif