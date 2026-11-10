#include <Arduino.h>
#include <DHT.h>

namespace {
constexpr uint8_t dhtPin = 4;
constexpr uint8_t co2Pin = 34;
constexpr uint8_t adcPin = 35;
constexpr uint8_t wateringSwitchPin = 25;
constexpr uint8_t lightingSwitchPin = 26;
constexpr uint8_t cameraSwitchPin = 27;
constexpr uint8_t wateringLedPin = 13;
constexpr uint8_t redLedPin = 18;
constexpr uint8_t blueLedPin = 19;
constexpr uint8_t cameraLedPin = 23;

DHT dht(dhtPin, DHT22);
unsigned long lastReport = 0;

void printTelemetry(const char* key, float value) {
  Serial.print("{\"type\":\"telemetry\",\"timestamp\":");
  Serial.print(millis());
  Serial.print(",\"key\":\"");
  Serial.print(key);
  Serial.print("\",\"value\":");
  Serial.print(value, 2);
  Serial.println("}");
}
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(co2Pin, INPUT);
  pinMode(adcPin, INPUT);
  pinMode(wateringSwitchPin, INPUT);
  pinMode(lightingSwitchPin, INPUT);
  pinMode(cameraSwitchPin, INPUT);
  pinMode(wateringLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(cameraLedPin, OUTPUT);

  Serial.println("{\"type\":\"revision\",\"firmware\":\"PeaPod-Wokwi-GPIO-Sim\"}");
}

void loop() {
  const bool wateringEnabled = digitalRead(wateringSwitchPin) == HIGH;
  const bool lightingEnabled = digitalRead(lightingSwitchPin) == HIGH;
  const bool cameraEnabled = digitalRead(cameraSwitchPin) == HIGH;
  const int co2Raw = analogRead(co2Pin);
  const int adcRaw = analogRead(adcPin);
  const int lightingDuty = lightingEnabled ? analogRead(adcPin) / 16 : 0;

  digitalWrite(wateringLedPin, wateringEnabled ? HIGH : LOW);
  analogWrite(redLedPin, lightingDuty);
  analogWrite(blueLedPin, lightingDuty);
  digitalWrite(cameraLedPin, cameraEnabled ? HIGH : LOW);

  if (millis() - lastReport >= 1000) {
    lastReport = millis();

    const float temperature = dht.readTemperature();
    const float humidity = dht.readHumidity();
    if (!isnan(temperature)) printTelemetry("air-temperature", temperature);
    if (!isnan(humidity)) printTelemetry("air-humidity", humidity);

    printTelemetry("air-ppm-carbondioxide", 400.0f + (co2Raw * 1600.0f / 4095.0f));
    printTelemetry("adc_voltage", adcRaw * 3.3f / 4095.0f);
    printTelemetry("enable_watering", wateringEnabled ? 1.0f : 0.0f);
    printTelemetry("enable_lighting", lightingEnabled ? 1.0f : 0.0f);
    printTelemetry("enable_camera", cameraEnabled ? 1.0f : 0.0f);
  }

  delay(10);
}