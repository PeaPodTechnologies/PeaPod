// HEADERS

#include <actuators/led.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/pwm.h>

// CONSTRUCTOR

LED::LED(const uint8_t pin, ledwavelength_t wavelength) : PWM(&id, pin), color(String(wavelength + "nm")) { }

LED::LED(const uint8_t pin, ledtemperature_t temperature) : PWM(&id, pin), color(String(temperature + "K")) { }

String LED::toString(void) {
  return Actuator::toString() + " (" + color + ")";
}

