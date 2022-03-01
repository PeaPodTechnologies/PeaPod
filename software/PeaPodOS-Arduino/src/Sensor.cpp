#include "Base.h"
#include "Sensor.h"

Sensor::Sensor(sensorid_t sensorid, const t_sensordatasetup* setup, uint32_t delta) {
  this->sensorid = sensorid;
  this->delta = delta;

  // Initial error and debug states
  state.error = ERR_NONE;
  state.debug = DS_DISABLED;

  // Allocate data state
  state.data = (t_datapoint*)malloc(sizeof(t_datapoint)*(setup->numdata));
  state.numdata = setup->numdata;
  for (int i = 0; i < setup->numdata; i++) {
    // Since setup->labels[i] is a const char*, we can just reassign our pointer
    state.data[i].label = setup->labels[i];
    state.data[i].value = NULL;
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
  // Check state preconditions
  if (state.error < ERR_FATAL && state.debug >= DS_INITIALIZED) {
    // Check timing
    if (millis() - lastread > delta) {

      // Allocate new data buffer on heap
      float* buffer = (float*)malloc(sizeof(float)*state.numdata);

      // Read and refresh read delay
      state.error = read(&buffer, state.numdata);
      lastread = millis();

      switch (state.error) {
        case ERR_NONE:
          // Success!
          // Indicate that new data is available
          state.debug = DS_SUCCESS;
          state.timestamp = lastread;

          // Make state data pointers point to the buffer
          for (int i = 0; i < state.numdata; i++) {
            // Free old data buffer
            free(state.data[i].value);
            // Point to new data buffer
            state.data[i].value = &buffer[i];
          }
          break;

        case ERR_WARNING:
          // Read didn't go as planned, non-fatal
          // DO NOT UPDATE ANY STATE VALUES
          break;

        case ERR_FATAL:
          // Read failed catastrophically
          state.debug = DS_DISABLED;

          // Free data buffer and reset to NULL
          for (int i = 0; i < state.numdata; i++) {
            free(state.data[i].value);
            state.data[i].value = NULL;
          }
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