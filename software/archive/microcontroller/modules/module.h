#ifndef PEAPOD_MODULES_MODULE_H_
#define PEAPOD_MODULES_MODULE_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <actuators/actuator.h>

// DECLARATIONS

// All module state info
typedef struct ModuleState {
  // Error level
  errorlevel_t error;

  // Other state information
  debuglevel_t debug;

  uint8_t numsensors;
  Sensor* sensors;

  uint8_t numactuators;
  Actuator* actuators;
} ModuleState;

// CLASS
// A "Module" is a single control system group, a collection of sensors and actuators
class Module {
  public:
    Module(const char* const* id, uint8_t numsensors, Sensor* sensors, uint8_t numactuators, Actuator* actuators);

    // @return Pointer to sensor state
    ModuleState* getState(void);

    // @return String representation
    String toString(void);

    String getID(void);
  private:
    ModuleState state;
    const char* const* _id;
};


#endif