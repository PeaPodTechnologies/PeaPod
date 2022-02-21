#include "LED.h"

LED::LED(uint8_t pin, led_color_t color) : Actuator(parseLED(color), String("CREE " + parseLED(color)), "Lighting"){
    this->pin = pin;
    this->color = color;
}

String parseLED(led_t led){
  switch(led){
      case LED_BLUE:
        return String("Royal Blue LED");
      case LED_COOL:
        return String("Cool White LED");
      case LED_WARM:
        return String("Warm White LED");
      case LED_RED:
        return String("Photo Red LED");
      case LED_FAR:
        return String("Far Red LED");
      default:
        return String("Unknown LED");
  }
}

bool LED::init(){
    this->target = 0;
    pinMode(this->pin, OUTPUT);
    return true;
}

void LED::update(){
    // Clamp to 0<x<1
    this->target = min(max(target, 0), 1);
    Serial.println(this->target*255);
    analogWrite(this->pin, this->target*255);
}