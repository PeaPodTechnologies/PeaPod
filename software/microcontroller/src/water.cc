#include <water.h>

PeaPod::PeaPodModuleWatering::PeaPodModuleWatering(bool chronoCallbacks) : PeaPodModule(PEAPOD_MODULENUM_WATERING) {
  if(chronoCallbacks) {
    #ifdef PEAPOD_PROGRAM_DEFAULT
    interval_watering_on = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_WATERING_DELTA, 0, &enable_watering, false);
    interval_watering_off = FSM::Chronos.addIntervalFlag(PEAPOD_MODULE_WATERING_DELTA, PEAPOD_MODULE_WATERING_PHASE, &enable_watering, true);
    #endif
  }

  enable_watering.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PEAPOD_MODULE_WATERING_GPIO_FQA, PEAPOD_MODULE_WATERING_GPIO_PIN>);

  enable_watering.set(false);

  registerFlag(&enable_watering);
}

PeaPod::PeaPodModuleWatering::~PeaPodModuleWatering() {
  enable_watering.set(false);
  
  FSM::Chronos.removeInterval(interval_watering_on);
  FSM::Chronos.removeInterval(interval_watering_off);
  interval_watering_on = nullptr;
  interval_watering_off = nullptr;
}