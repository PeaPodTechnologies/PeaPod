#include <Arduino.h>

#define LED_PIN 3
#define STEP 0.15

void setup(){
    Serial.begin(9600);
    while(!Serial);

    pinMode(LED_PIN, OUTPUT);
}

float angle = 0;

void loop(){
    angle += STEP;    //Step the angle
    if(angle > TWO_PI){ angle = 0;}
    Serial.print(angle);
    Serial.println("rad");
    analogWrite(LED_PIN, (-sin(angle)/2+0.5)*255*0.25);    //Sinusoid between 0 and .25
    delay(100);
}
