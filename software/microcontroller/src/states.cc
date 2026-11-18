#include <air.h>
#include <aeroponics.h>
#include <light.h>

FSM::Variable PeaPod::air_temperature = FSM::Variable(FSM::notanumber, "air-temperature");
FSM::Variable PeaPod::air_humidity = FSM::Variable(FSM::notanumber, "air-humidity");
FSM::Variable PeaPod::air_co2 = FSM::Variable(FSM::notanumber, "air-ppm-carbondioxide");

FSM::Flag PeaPod::enable_watering = FSM::Flag(false, "enable_watering");
FSM::Flag PeaPod::enable_pump = FSM::Flag(false, "enable_pump");
FSM::Variable PeaPod::aeroponics_pressure = FSM::Variable(FSM::notanumber, "aeroponics_pressure");
FSM::Variable PeaPod::aeroponics_flow = FSM::Variable(FSM::notanumber, "aeroponics_flow");
FSM::Variable PeaPod::aeroponics_temperature = FSM::Variable(FSM::notanumber, "aeroponics_temperature");

// FSM::Variable PeaPod::adc_voltage = FSM::Variable(FSM::notanumber, "adc_voltage");
FSM::Variable PeaPod::pwm_lighting_red = FSM::Variable(FSM::Number(0, true), "pwm_lighting_red");
FSM::Variable PeaPod::pwm_lighting_blue = FSM::Variable(FSM::Number(0, true), "pwm_lighting_blue");
FSM::Flag PeaPod::enable_lighting = FSM::Flag("enable_lighting");
FSM::Flag PeaPod::enable_camera = FSM::Flag("enable_camera");