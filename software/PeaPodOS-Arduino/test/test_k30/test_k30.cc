#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <sensors/k30.h>

K30 sensor = K30();

void test_begin(void) {
  SensorState* state = sensor.begin();
  TEST_ASSERT_EQUAL(state->debug, DS_INITIALIZED);
  TEST_ASSERT_EQUAL(state->error, ERR_NONE);
}

void test_read(void) {
  SensorState* state = sensor.update();
  TEST_ASSERT_EQUAL(state->debug, DS_SUCCESS);
  TEST_ASSERT_EQUAL(state->error, ERR_NONE);
}

void test_delta(void) {
  SensorState* state = sensor.update();
  TEST_ASSERT_EQUAL(state->debug, DS_INITIALIZED);
  TEST_ASSERT_EQUAL(state->error, ERR_NONE);

  // No delay

  state = sensor.update();
  TEST_ASSERT_EQUAL(state->debug, DS_WAITING);
  TEST_ASSERT_EQUAL(state->error, ERR_NONE);
}

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_begin);
}

int i = 0;

void loop(void) { 
  if (i < 5) {
    delay(2000);

    RUN_TEST(test_read);

    delay(2000);

    RUN_TEST(test_delta);
    i++;
  } else {
    UNITY_END();
  }
}