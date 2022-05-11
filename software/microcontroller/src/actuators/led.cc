#include <actuators/led.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>

LED::LED(uint8_t pin, led_color_t color) : Actuator(ACTUATOR_LED, 0) {
  this->pin = pin;
  this->color = color;
}

errorlevel_t LED::initialize(void) {
  pinMode(this->pin, OUTPUT);
  return ERR_NONE;
}

errorlevel_t LED::set(float target) {
  // Clamp to 0 < x < 1
  target = min(max(target, 0), 1);
  analogWrite(this->pin, target * 255);
  return ERR_NONE;
}

/**
 * UNUSED.
 * @param led Enum representation of the LED.
 * @return String representation of the LED.
 */
// static String parseLED(led_color_t led) {
//   switch (led) {
//     case LEDCOLOR_BLUE:
//       return String("Royal Blue LED");
//     case LEDCOLOR_COOL:
//       return String("Cool White LED");
//     case LEDCOLOR_WARM:
//       return String("Warm White LED");
//     case LEDCOLOR_RED:
//       return String("Photo Red LED");
//     case LEDCOLOR_FAR:
//       return String("Far Red LED");
//     default:
//       return String("Unknown LED");
//   }
// }