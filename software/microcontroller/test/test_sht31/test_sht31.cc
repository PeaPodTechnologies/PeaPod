#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <sensors/sht31.h>

SHT31 sensor = SHT31();

void test_begin(void) {
  SensorState* state = sensor.begin();
  TEST_ASSERT_EQUAL(DS_INITIALIZED, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}

void test_read(void) {
  SensorState* state = sensor.update();
  TEST_ASSERT_EQUAL(DS_SUCCESS, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}

void test_delta(void) {
  SensorState* state = sensor.update();
  TEST_ASSERT_EQUAL(DS_SUCCESS, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);

  // No delay

  state = sensor.update();
  TEST_ASSERT_EQUAL(DS_WAITING, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_begin);
}

int i = 0;

void loop(void) { 
  if (i < 3) {
    delay(1500);

    RUN_TEST(test_read);

    delay(1500);

    RUN_TEST(test_delta);
    i++;
  } else {
    UNITY_END();
  }
}