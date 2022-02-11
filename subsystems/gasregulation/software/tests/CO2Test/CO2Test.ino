#include "Sensor.h"
#include "Wire.h"
#include "K30.h"

K30 k30;

void setup()
{
    digitalWrite(13, LOW);

    Serial.begin(115200);
    while(!Serial); //Waits until serial opens

    // Test sensor protocols - per-sensor tests
    bool success = k30.begin(); 
    if(!success){
        Serial.print("{\"type\":\"error\",\"data\":\"Failed to initialize CO2 sensor. Check wiring. Freezing...\"}\n");
        while(true){
            delay(250);
            digitalWrite(13, HIGH);
            delay(250);
            digitalWrite(13, LOW);
        }
    } else {
        Serial.print("{\"type\":\"debug\",\"data\":\"CO2 sensor initialized successfully.\"}\n");
    }

    digitalWrite(13, HIGH);
}

void loop()
{
    float read = k30.getRead();
    if(!isnan(read)){
        Serial.print("{\"type\":\"data\",\"data\":{\"label\":\"");
        Serial.print(k30.evname);
        Serial.print("\",\"value\":");
        Serial.print(read);
        Serial.print("}}\n");
    }
    delay(10);
}
