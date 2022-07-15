#include <actuators/led.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/pwm.h>

LED::LED(uint8_t pin, led_color_t color) : PWM(pin, ACTUATOR_LED) {
  this->color = color;
}