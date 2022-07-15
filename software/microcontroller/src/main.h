#include <Arduino.h>

#include <sensors/sensor.h>
#include <sensors/sht31.h>
#include <sensors/k30.h>
#include <actuators/actuator.h>
#include <actuators/led.h>
#include <actuators/supply.h>
#include <actuators/solenoid.h>
#include <utils/instructions.h>

// Settings
#define ENABLE_WATCHDOG false

// Constants
#define NUM_SENSORS   2
#define NUM_ACTUATORS 7
#define REVISION      0
#define BAUDRATE      115200

// Pins
#define PIN_STATUS    13
#define PIN_LEDBLUE   3
#define PIN_LEDCOOL   5
#define PIN_LEDWARM   6
#define PIN_LEDRED    9
#define PIN_LEDFAR    10
#define PIN_SUPPLY    8
#define PIN_SOLENOID  7

// Sensors
SHT31 sht31 = SHT31();
K30 k30 = K30();

Sensor* sensors[NUM_SENSORS] = {
  &sht31,
  &k30,
};

// Actuators
LED led_blue = LED(PIN_LEDBLUE, LED::LEDCOLOR_BLUE);
LED led_cool = LED(PIN_LEDCOOL, LED::LEDCOLOR_COOL);
LED led_warm = LED(PIN_LEDWARM, LED::LEDCOLOR_WARM);
LED led_red = LED(PIN_LEDRED, LED::LEDCOLOR_RED);
LED led_far = LED(PIN_LEDFAR, LED::LEDCOLOR_FAR);
SupplyPump supply = SupplyPump(PIN_SUPPLY);
Solenoid solenoid = Solenoid(PIN_SOLENOID);

Actuator* actuators [NUM_ACTUATORS] = {
  &led_blue,
  &led_cool,
  &led_warm,
  &led_red,
  &led_far,
  &supply,
  &solenoid,
};

const char* instr [NUM_ACTUATORS] = { "led_blue", "led_cool", "led_warm", "led_red", "led_far", "supply", "solenoid" };

static const InstructionActuatorMatrix matrix = {
  NUM_ACTUATORS,
  actuators,
  instr
};