#include <air.h>
#include <water.h>
#include <light.h>

FSM::Variable PeaPod::air_temperature = FSM::Variable(FSM::notanumber, "air-temperature");
FSM::Variable PeaPod::air_humidity = FSM::Variable(FSM::notanumber, "air-humidity");
FSM::Variable PeaPod::air_co2 = FSM::Variable(FSM::notanumber, "air-ppm-carbondioxide");

FSM::Flag PeaPod::enable_watering = FSM::Flag(false, "enable_watering");

FSM::Variable PeaPod::adc_voltage = FSM::Variable(FSM::notanumber, "adc_voltage");
FSM::Variable PeaPod::pwm_lighting_red = FSM::Variable(FSM::Number(0, true), "pwm_lighting_red");
FSM::Variable PeaPod::pwm_lighting_blue = FSM::Variable(FSM::Number(0, true), "pwm_lighting_blue");
FSM::Flag PeaPod::enable_lighting = FSM::Flag("enable_lighting");
FSM::Flag PeaPod::enable_camera = FSM::Flag("enable_camera");