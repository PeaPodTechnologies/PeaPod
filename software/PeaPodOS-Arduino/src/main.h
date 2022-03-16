#include <Arduino.h>

#include <sensors/sensor.h>
#include <sensors/sht31.h>
#include <sensors/k30.h>
#include <actuators/actuator.h>
#include <actuators/led.h>
#include <utils/instructions.h>

// Settings
#define ENABLE_WATCHDOG false

// Constants
#define NUM_SENSORS   1
#define NUM_ACTUATORS 0
#define REVISION      0
#define BAUDRATE      115200

// Pins
#define PIN_STATUS    13
// #define PIN_LEDBLUE   3
// #define PIN_LEDCOOL   5
// #define PIN_LEDWARM   6
// #define PIN_LEDRED    9
// #define PIN_LEDFAR    10

// Sensors
SHT31 sht31 = SHT31();
// K30 k30 = K30();

Sensor* sensors[NUM_SENSORS] = {
  &sht31,
  // &k30,
};

// Actuators
// LED led_blue(LEDPIN_BLUE, LEDCOLOR_BLUE);
// LED led_cool(LEDPIN_COOL, LEDCOLOR_COOL);
// LED led_warm(LEDPIN_WARM, LEDCOLOR_WARM);
// LED led_red(LEDPIN_RED, LEDCOLOR_RED);
// LED led_far(LEDPIN_FAR, LEDCOLOR_FAR);

Actuator* actuators [NUM_ACTUATORS] = {
//   &led_blue,
//   &led_cool,
//   &led_warm,
//   &led_red,
//   &led_far,
};

static const InstructionActuatorMatrix matrix = {
  .numActuators = NUM_ACTUATORS,
  .actuators = actuators,
  .instructions = {}
}