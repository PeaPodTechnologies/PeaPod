#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <peapod.h>
#include <air.h>

using namespace I2CIP;

bool run_temperature = false;
void callback_run_temperature() { run_temperature = true; }

bool run_co2 = false;
void callback_run_co2() { run_co2 = true; }

void test_eeprom_ping(void) {
  EEPROM& eeprom = (I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator EEPROM &());
  i2cip_fqa_t eeprom_fqa = eeprom.getFQA();

  char msg[30];
  sprintf(msg, "Device unreachable (%01X.%01X.%01X.%02X)", I2CIP_FQA_SEG_I2CBUS(eeprom_fqa), I2CIP_FQA_SEG_MODULE(eeprom_fqa), I2CIP_FQA_SEG_MUXBUS(eeprom_fqa), I2CIP_FQA_SEG_DEVADR(eeprom_fqa));

  i2cip_errorlevel_t result = eeprom.ping(true, true);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, msg);
}

void test_eeprom_overwrite(void) {
  EEPROM& eeprom = (I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator EEPROM &());

  i2cip_errorlevel_t result = I2CIP_ERR_NONE;
  const char* msg = PEAPOD_MODULE_AIR_EEPROM_CONTENTS;
  size_t len = strlen(msg);

  result = eeprom.getOutput()->set(&msg, &len);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, "EEPROM Output Setter (Default Value, Args)");

  result = eeprom.getInput()->get(nullptr);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, result, "EEPROM Input Getter (Default Args)");

  const char* cache = eeprom.getCache();
  TEST_ASSERT_EQUAL_STRING_MESSAGE(msg, cache, "EEPROM Cache (Match)");
}

void test_module_check(void) {
  i2cip_errorlevel_t errlev = I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator()();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Module Check");
}

void test_devicegroups(void) {
  // SHT45
  DeviceGroup* dg_sht45 = I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator[]("SHT45");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_sht45, "SHT45 Device Group (Exists)");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, dg_sht45->getNumDevices(), "SHT45 Device Group (Non-Empty)");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_sht45->getDevice(0), "SHT45 Device Group (Valid Device)");

  // K30
  DeviceGroup* dg_k30 = I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator[]("K30");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_k30, "K30 Device Group (Exists)");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, dg_k30->getNumDevices(), "K30 Device Group (Non-Empty)");
  TEST_ASSERT_NOT_NULL_MESSAGE(dg_k30->getDevice(0), "K30 Device Group (Valid Device)");
}

void test_sht45_mean(void) {
  run_temperature = false;
  PeaPod::PeaPodModuleAir::callback_sht45_mean<PEAPOD_MODULENUM_AIR>();
  TEST_ASSERT_TRUE_MESSAGE(run_temperature, "SHT45 Temperature Callback Execution");
}

void test_k30_mean(void) {
  run_co2 = false;
  PeaPod::PeaPodModuleAir::callback_k30_mean<PEAPOD_MODULENUM_AIR>();
  TEST_ASSERT_TRUE_MESSAGE(run_co2, "K30 Mean Callback Execution");
}

void test_sevenseg(void) {
  i2cip_ht16k33_mode_t seg_mode = SEG_ASCII;
  i2cip_ht16k33_data_t seg_data = { .h = ('T') + ('E' << 8) + ('S' << 16) + ('T' << 24) };
  i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };
  i2cip_errorlevel_t errlev = I2CIP::modules[PEAPOD_MODULENUM_AIR]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "7-Segment Display");
}

void setup() {
  Serial.begin(115200); 

  I2CIP::modules[PEAPOD_MODULENUM_AIR] = new PeaPod::PeaPodModuleAir(false);

  PeaPod::air_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_run_temperature);
  PeaPod::air_co2.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_run_co2);

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
  delay(1000);
  RUN_TEST(test_sht45_mean);
  delay(1000);
  RUN_TEST(test_k30_mean);
  delay(1000);
  RUN_TEST(test_sevenseg);

  UNITY_END();
}

void loop() {

}