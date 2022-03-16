#include <sensors/sensor.h>

#include <stdlib.h>

#include <Arduino.h>

#include <utils/base.h>

Sensor::Sensor(sensorid_t sensorid, const SensorDataSetup* setup, uint32_t delta) {
  this->sensorid = sensorid;
  this->delta = delta;

  // Initial error and debug states
  state.error = ERR_NONE;
  state.debug = DS_DISABLED;

  // Allocate data state
  state.data = (DataPoint*)malloc(sizeof(DataPoint)*(setup->numdata));
  state.numdata = setup->numdata;
  for (int i = 0; i < setup->numdata; ++i) {
    // Since setup->labels[i] is a const char*, we can just reassign our pointer
    state.data[i].label = setup->labels[i];
  }
}

SensorState* Sensor::begin(void) {
  state.error = initialize();
  if (state.error > ERR_NONE) {
    // Failed
    state.debug = DS_DISABLED;
  } else {
    // Success
    state.debug = DS_INITIALIZED;
    // Refresh read delay
    state.timestamp = millis();
  }
  return &state;
}

SensorState* Sensor::update(void) {
  // Allocate new data buffer on stack
  float buffer[state.numdata];

  // Check state preconditions
  if (state.error < ERR_FATAL && state.debug >= DS_INITIALIZED) {
    // Check timing
    if (millis() - lastread > delta) {
      // Read and refresh read delay
      state.error = read(buffer, state.numdata);
      lastread = millis();

      switch (state.error) {
        case ERR_NONE:
          // Success!
          // Indicate that new data is available
          state.debug = DS_SUCCESS;
          state.timestamp = lastread;

          // Copy from buffer to state data
          for (int i = 0; i < state.numdata; ++i) {
            state.data[i].value = buffer[i];
          }
          break;

        case ERR_WARNING:
          // Read didn't go as planned, non-fatal
          // DO NOT UPDATE ANY STATE VALUES
          break;

        case ERR_FATAL:
          // Read failed catastrophically
          state.debug = DS_DISABLED;
          break;
      }
    } else {
      // Attempted to update between valid read cycles
      state.debug = DS_WAITING;
    }
  }
  return &state;
}

SensorState* Sensor::getState(void) {
  return &state;
}

sensorid_t Sensor::getID(void) {
  return sensorid;
}