#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <peapod.h>
#include <water.h>

using namespace I2CIP;

void test_eeprom_ping(void) {
  EEPROM& eeprom = (I2CIP::modules[PEAPOD_MODULENUM_WATERING]->operator EEPROM &());
  i2cip_fqa_t eeprom_fqa = eeprom.getFQA();

  char msg[30];
  sprintf(msg, "Device unreachable (%01X.%01X.%01X.%02X)", I2CIP_FQA_SEG_I2CBUS(eeprom_fqa), I2CIP_FQA_SEG_MODULE(eeprom_fqa), I2CIP_FQA_SEG_MUXBUS(eeprom_fqa), I2CIP_FQA_SEG_DEVADR(eeprom_fqa));

  i2cip_errorlevel_t result = eeprom.ping(true, true);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, msg);
}

void test_eeprom_overwrite(void) {
  EEPROM& eeprom = (I2CIP::modules[PEAPOD_MODULENUM_WATERING]->operator EEPROM &());

  i2cip_errorlevel_t result = I2CIP_ERR_NONE;
  const char* msg = PEAPOD_MODULE_WATERING_EEPROM_CONTENTS;
  size_t len = strlen(msg);

  result = eeprom.getOutput()->set(&msg, &len);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, "EEPROM Output Setter (Default Value, Args)");

  result = eeprom.getInput()->get(nullptr);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, "EEPROM Input Getter (Default Args)");

  const char* cache = eeprom.getCache();
  TEST_ASSERT_EQUAL_STRING_MESSAGE(msg, cache, "EEPROM Cache (Match)");
}

void test_module_check(void) {
  i2cip_errorlevel_t errlev = I2CIP::modules[PEAPOD_MODULENUM_WATERING]->operator()();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Module Check");
}

void test_devicegroups(void) {
  // MCP23017
  DeviceGroup* dg_gpio = I2CIP::modules[PEAPOD_MODULENUM_WATERING]->operator[]("MCP23017");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_gpio, "MCP23017 Device Group (Exists)");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, dg_gpio->getNumDevices(), "MCP23017 Device Group (Non-Empty)");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_gpio->getDevice(0), "MCP23017 Device Group (Valid Device)");
}

void setup() {
  Serial.begin(115200); 

  I2CIP::modules[PEAPOD_MODULENUM_WATERING] = new PeaPod::PeaPodModuleWatering(false);

  delay(2000);

  UNITY_BEGIN();

  delay(1000);
  RUN_TEST(test_eeprom_ping);
  delay(1000);
  RUN_TEST(test_eeprom_overwrite);
  delay(1000);
  RUN_TEST(test_module_check);
  delay(1000);
  RUN_TEST(test_devicegroups);

  // TODO: More tests

  UNITY_END();
}

void loop() {

}