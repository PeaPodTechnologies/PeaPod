#ifndef PEAPOD_ACTUATORS_LED_H_
#define PEAPOD_ACTUATORS_LED_H_

#include "Arduino.h"

#include "../base.h"
#include "../actuator.h"

typedef enum {
  LEDCOLOR_BLUE,
  LEDCOLOR_COOL,
  LEDCOLOR_WARM,
  LEDCOLOR_RED,
  LEDCOLOR_FAR
} led_color_t;

class LED : public Actuator {
  public:
    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     * @param color Color of this LED (enum).
     */
    LED(uint8_t pin, led_color_t color);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;

    // PWM output pin.
    uint8_t pin;

    // LED color.
    led_color_t color;
};

#endif
