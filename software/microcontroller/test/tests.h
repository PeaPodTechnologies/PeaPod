#ifndef PEAPOD_TESTS_H_
#define PEAPOD_TESTS_H_

// HEADERS

#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <sensors/sensor.h>

// Requires `sensor` to be defined in the test source file
extern Sensor* sensor;

// DECLARATAIONS

namespace SensorTests {
  /**
   * Ensures state compliance with `begin()`
   */
  void test_begin(void);

  /**
   * Ensures state compliance with `update()`
   */
  void test_update(void);

  /**
   * Ensures state compliance when `delta` is skipped
   */
  void test_delta(void);
}

#endif