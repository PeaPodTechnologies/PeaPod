#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <sensors/k30.h>
#include "../tests.h"

Sensor* sensor = new K30();

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(SensorTests::test_begin);
}

int i = 0;

void loop(void) { 
  if (i < 3) {
    delay(K30_DELTA * 1.5);

    RUN_TEST(SensorTests::test_update);

    delay(K30_DELTA * 1.5);

    RUN_TEST(SensorTests::test_delta);
    i++;
  } else {
    UNITY_END();
  }
}