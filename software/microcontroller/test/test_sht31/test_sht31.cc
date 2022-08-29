#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <sensors/sht31.h>
#include "../tests.h"

Sensor* sensor = new SHT31();

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(SensorTests::test_begin);
}

int i = 0;

void loop(void) { 
  if (i < 3) {
    delay(SHT31_DELTA * 1.5);

    RUN_TEST(SensorTests::test_update);

    delay(SHT31_DELTA * 1.5);

    RUN_TEST(SensorTests::test_delta);
    i++;
  } else {
    UNITY_END();
  }
}