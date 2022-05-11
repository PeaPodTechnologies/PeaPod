#include <actuators/actuator.h>

#include <Arduino.h>

#include <utils/base.h>

Actuator::Actuator(actuatorid_t actuatorid, float failtarget) {
  this->actuatorid = actuatorid;
  state.error = ERR_NONE;
  state.debug = DS_DISABLED;

  // Default start target
  target = this->failtarget = failtarget;
}

ActuatorState* Actuator::begin(void) {
  state.error = initialize();
  if (state.error > ERR_NONE) {
    // Failed
    state.debug = DS_DISABLED;
  } else {
    // Success
    state.debug = DS_INITIALIZED;
    // Perform first update
    update();
  }
  return &state;
}

ActuatorState* Actuator::update(void) {
  // Check state preconditions, and do not attempt set if the target hasn't changed
  if (state.error < ERR_FATAL && state.debug >= DS_INITIALIZED) {
    if (target != state.lasttarget) {
      // Attempt to set actuator state from target
      state.error = set(target);

      switch (state.error) {
        case ERR_NONE:
          // Success!
          state.debug = DS_SUCCESS;
          state.lasttarget = target;
          break;

        case ERR_WARNING:
          // Set didn't go as planned, non-fatal
          // DO NOT UPDATE STATE VALUES
          break;

        case ERR_FATAL:
          // Set failed catastrophically
          state.debug = DS_DISABLED;

          // Failsafe
          set(failtarget);
          break;
      }
    } else {
      // No `target` updates since last `update()` call
      state.debug = DS_WAITING;
    }
  }
  return &state;
}

ActuatorState* Actuator::getState(void) {
  return &state;
}

actuatorid_t Actuator::getID(void) {
  return actuatorid;
}

void Actuator::setTarget(float target) {
  this->target = target;
}