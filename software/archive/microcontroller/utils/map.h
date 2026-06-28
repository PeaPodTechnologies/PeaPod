#ifndef PEAPOD_UTILS_MAP_H_
#define PEAPOD_UTILS_MAP_H_

#include <stdlib.h>

#include <Arduino.h>

#include <sensors/all.h>
#include <actuators/all.h>

// Returns Sensor*
typedef Sensor* (*sensorfactory_t)(void* args);

class SensorFactoryMap {
  public:
    template <class S> sensorfactory_t add(const char* const id);
    sensorfactory_t find(const char* id);
  private:
    uint8_t numsensors = 0;
    sensorfactory_t sensorFactories[NUM_SENSORS];
    const char* const* ids;
};

template <class S> sensorfactory_t SensorFactoryMap::add(const char* const id) {
  uint8_t newsensor = numsensors;
  ids[newsensor] = id;
  sensorFactories[newsensor] = (sensorfactory_t)([void* args]{ return new S(args); });
  numsensors++;
  return sensorFactories[newsensor];
}

sensorfactory_t SensorFactoryMap::find(const char* id) {
  for (uint8_t i = 0; i < numsensors; i++) {
    if (strcmp(id, ids[i]) == 0) {
      return sensorFactories[i];
    }
  }
  return nullptr;
}

// Returns Actuator*
typedef Actuator* (*actuatorfactory_t)(void* args);

class ActuatorFactoryMap {
  public:
    template <class A> actuatorfactory_t add(const char* const id);
    actuatorfactory_t find(const char* id);
  private:
    uint8_t numactuators = 0;
    actuatorfactory_t actuatorFactories[NUM_ACTUATORS];
    const char* const* ids;
};

template <class A> actuatorfactory_t ActuatorFactoryMap::add(const char* const id) {
  uint8_t newactuator = numactuators;
  ids[newactuator] = id;
  actuatorFactories[newactuator] = (actuatorfactory_t)([void* args]{ return new A(args); });
  numactuators++;
  return actuatorFactories[newactuator];
}

actuatorfactory_t ActuatorFactoryMap::find(const char* id) {
  for (uint8_t i = 0; i < numactuators; i++) {
    if (strcmp(id, ids[i]) == 0) {
      return actuatorFactories[i];
    }
  }
  return nullptr;
}

#endif