// HEADERS

#include <modules/module.h>

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <actuators/actuator.h>

// CONSTRUCTOR

Module::Module(const char* const* id, uint8_t numsensors, Sensor* sensors, uint8_t numactuators, Actuator* actuators) : _id(id) {
  state.numsensors = numsensors;
  state.sensors = sensors;
  state.numactuators = numactuators;
  state.actuators = actuators;
}

ModuleState* Module::getState(void) {
  return &state;
}

String Module::toString(void) {
  String s = String((const char*)(_id)) + " (";
  for (int i = 0; i < state.numsensors; i++) {
    s += state.sensors[i].getID();
    if (i < state.numsensors - 1 || state.numactuators > 0) {
      s += ", ";
    }
  }
  for (int i = 0; i < state.numactuators; i++) {
    s += state.actuators[i].getID();
    if (i < state.numactuators - 1) {
      s += ", ";
    }
  }
  return s + ")";
}

String Module::getID(void) {
  return String((const char*)_id);
}