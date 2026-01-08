#include <air.h>
#include <water.h>
#include <light.h>

FSM::Variable PeaPod::air_temperature = FSM::Variable(FSM::Number(), "air-temperature");
FSM::Variable PeaPod::air_humidity = FSM::Variable(FSM::Number(), "air-humidity");
FSM::Variable PeaPod::air_co2 = FSM::Variable(FSM::Number(), "air-ppm-carbondioxide");

FSM::Flag PeaPod::flag_watering = FSM::Flag("flag_watering");

FSM::Variable PeaPod::adc_voltage = FSM::Variable(FSM::notanumber, "adc_voltage");
FSM::Variable PeaPod::pwm_lighting_red = FSM::Variable(FSM::notanumber, "pwm_lighting_red");
FSM::Variable PeaPod::pwm_lighting_blue = FSM::Variable(FSM::notanumber, "pwm_lighting_blue");
// FSM::Flag PeaPod::flag_lighting = FSM::Flag("flag_lighting");