#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>
#include "../config.h"

#define CSOS_TEST_EEPROM_BYTE '[' // This should be the first character of ANY valid SPRT EEPROM

i2cip_fqa_t eeprom_fqa = I2CIP::createFQA(WIRENUM, MUXNUM, CSOS_MUX_BUS_DEFAULT, CSOS_EEPROM_ADDR);
char buffer[I2CIP_TEST_BUFFERSIZE] = { '\0' };
uint16_t bufferlen = 0;

// ALL MACROS

void test_eeprom_ping(void) {
  char msg[30];
  sprintf(msg, "Device unreachable (%01X.%01X.%01X.%02X)", CSOS_FQA_SEG_I2CBUS(eeprom_fqa), CSOS_FQA_SEG_MUXNUM(eeprom_fqa), CSOS_FQA_SEG_MUXBUS(eeprom_fqa), CSOS_FQA_SEG_DEVADR(eeprom_fqa));
  I2CIP::i2cip_errorlevel_t result = I2CIP::Device::ping(eeprom_fqa);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, msg);
}

void test_eeprom_read_byte(void) {
  uint8_t c = '\0';
  I2CIP::i2cip_errorlevel_t result = I2CIP::Device::readRegisterByte(eeprom_fqa, (uint16_t)0, c);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, "EEPROM Read Byte");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_TEST_EEPROM_BYTE, c, "EEPROM Read Byte (Match)");
}

void test_eeprom_write_byte(void) {
  I2CIP::i2cip_errorlevel_t result = I2CIP::Device::writeRegister(eeprom_fqa, (uint16_t)0, CSOS_TEST_EEPROM_BYTE);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, "EEPROM Write Byte");
}

void test_eeprom_read_contents(void) {
  I2CIP::i2cip_errorlevel_t result = I2CIP::EEPROM::readContents(eeprom_fqa, (uint8_t*)buffer, bufferlen, CSOS_TEST_BUFFERSIZE);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, "EEPROM Read Contents");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, bufferlen, "EEPROM Read Contents (Empty)");
}

void test_eeprom_overwrite_contents(void) {
  I2CIP::i2cip_errorlevel_t result = I2CIP::EEPROM::overwriteContents(eeprom_fqa, (uint8_t*)buffer, bufferlen);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, "EEPROM Overwrite Contents");
}

void setup() {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_eeprom_ping);
  RUN_TEST(test_eeprom_read_byte);
  // RUN_TEST(test_eeprom_write_byte);
  RUN_TEST(test_eeprom_read_contents);
  // RUN_TEST(test_eeprom_overwrite_contents);

  UNITY_END();
}

void loop() {w

}