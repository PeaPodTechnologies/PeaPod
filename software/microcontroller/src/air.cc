#include <air.h>

FSM::Variable PeaPod::air_temperature = FSM::Variable(FSM::Number(), "air-temperature");
FSM::Variable PeaPod::air_humidity = FSM::Variable(FSM::Number(), "air-humidity");
FSM::Variable PeaPod::air_co2 = FSM::Variable(FSM::Number(), "air-ppm-carbondioxide");