#include <Arduino.h>
#include <unity.h>

#include "../config.h"

#include <debug.h>
#include <ControlSystemsOS.h>

using namespace ControlSystemsOS;

CSOSModule* m;  // to be initialized in setup()

void test_csos_map(void) { }
void test_csos_build(void) {
  #ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("==== [ BUILD ] ===="));
    DEBUG_DELAY();
  #endif

  i2cip_errorlevel_t errlev = ControlSystemsOS::update(true);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Build failed!");

  // TODO: Test Module Contents! (Against what?)
}

void test_csos_update(void) {
  #ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("==== [ UPDATE ] ===="));
    DEBUG_DELAY();
  #endif

  i2cip_errorlevel_t errlev = ControlSystemsOS::update(false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Update failed!");
}

void setup(void) {
  Serial.begin(115200);

  delay(2000);

  UNITY_BEGIN();
  
  RUN_TEST(test_csos_build);

  delay(1000);
}

uint8_t count = 1;
void loop(void) {

  RUN_TEST(test_csos_update);

  if(count > 10) {
    UNITY_END();
    while(true);
  }

  delay(1000);
  count++;
}
