#ifndef BASE_H
#define BASE_H

#include "Arduino.h"

// Degree of error occurred
typedef enum errorlevel_t {
  ERR_NONE,
  ERR_WARNING,
  ERR_FATAL
} errorlevel_t;

// General state information - On error level > 0, indicates last successful state
typedef enum debuglevel_t {
  DS_DISABLED,
  DS_INITIALIZED,
  DS_WAITING,
  DS_SUCCESS
} debuglevel_t;

#endif