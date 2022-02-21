#ifndef LED_H
#define LED_H

#include "Actuator.h"

typedef enum {
  LED_BLUE,
  LED_COOL,
  LED_WARM,
  LED_RED,
  LED_FAR
} led_color_t;

class LED : public Actuator {
    public:
        LED(uint8_t pin, led_color_t color);
    private:
        void update() override;
        bool init() override;
        /**
         * PWM output pin.
         * */
        uint8_t pin;
        led_color_t color;
};

#endif