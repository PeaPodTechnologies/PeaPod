#include <water.h>

PeaPod::PeaPodModuleWatering::PeaPodModuleWatering(bool chronoCallbacks) : PeaPodModule(PEAPOD_MODULENUM_WATERING) {
  if(chronoCallbacks) {
    #ifdef PEAPOD_PROGRAM_DEFAULT
      interval_watering_on = FSM::Chronos.addIntervalFlag("i_watering_on", PEAPOD_MODULE_WATERING_DELTA, 0, &enable_watering, false);
      interval_watering_off = FSM::Chronos.addIntervalFlag("i_watering_off", PEAPOD_MODULE_WATERING_DELTA, PEAPOD_MODULE_WATERING_PHASE, &enable_watering, true);
      registerInterval(interval_watering_on);
      registerInterval(interval_watering_off);
    #endif
  }
  
  #ifdef PEAPOD_MODULE_WATERING_GPIO_PUMP
  enable_pump.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PEAPOD_MODULE_WATERING_GPIO_FQA, PEAPOD_MODULE_WATERING_GPIO_PUMP>);
  #endif
  enable_pump.set(true);

  enable_watering.addLatchingConditional(true, false, callback_mcp23017_digitalWrite<PEAPOD_MODULE_WATERING_GPIO_FQA, PEAPOD_MODULE_WATERING_GPIO_SOLENOID>);
  enable_watering.set(false);

  registerFlag(&enable_watering);
  registerFlag(&enable_pump);
}

PeaPod::PeaPodModuleWatering::~PeaPodModuleWatering() {
  enable_watering.set(false);
  enable_pump.set(false);
  
  unregisterInterval(interval_watering_on);
  unregisterInterval(interval_watering_off);
  FSM::Chronos.removeInterval(interval_watering_on);
  FSM::Chronos.removeInterval(interval_watering_off);
  interval_watering_on = nullptr;
  interval_watering_off = nullptr;
}