// HEADERS
#include "tests.h"

#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// DECLARATIONS

void SensorTests::test_begin(void) {
  SensorState* state = sensor->begin();
  TEST_ASSERT_EQUAL(DS_INITIALIZED, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}

void SensorTests::test_update(void) {
  SensorState* state = sensor->update();
  TEST_ASSERT_EQUAL(DS_SUCCESS, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}

void SensorTests::test_delta(void) {
  SensorState* state = sensor->update();
  TEST_ASSERT_EQUAL(DS_SUCCESS, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);

  // No delay

  state = sensor->update();
  TEST_ASSERT_EQUAL(DS_WAITING, state->debug);
  TEST_ASSERT_EQUAL(ERR_NONE, state->error);
}