#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>

#include <I2CIP.h>
#include "../config.h"

// GLOBALS
i2cip_fqa_t eeprom_fqa = I2CIP::createFQA(WIRENUM, MUXNUM, CSOS_MUX_BUS_DEFAULT, CSOS_EEPROM_ADDR);
char buffer[I2CIP_TEST_BUFFERSIZE] = { '\0' };
StaticJsonDocument<I2CIP_TEST_BUFFERSIZE> eeprom_json;  // EEPROM JSON doc
uint8_t totaldevices = 0; // Number of devices in EEPROM
i2cip_fqa_t* devices; // Array of EEPROM device FQAs
uint8_t d = 0;  // Loop variable

/**
 * Read and deserialize the EEPROM.
 **/
void test_sprt_json_deser(void) {
  // READ EEPROM
  uint16_t size = 0;
  I2CIP::i2cip_errorlevel_t result = I2CIP::EEPROM::readContents(eeprom_fqa, (uint8_t*)buffer, size, CSOS_TEST_BUFFERSIZE);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, result, "Failed to read EEPROM");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, size, "EEPROM empty!");

  // DESERIALIZE
  DeserializationError jsonerr = deserializeJson(eeprom_json, buffer);

  TEST_ASSERT_TRUE_MESSAGE(jsonerr.code() == DeserializationError::Code::Ok, buffer);
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, eeprom_json.memoryUsage(), buffer);
  TEST_ASSERT_FALSE_MESSAGE(eeprom_json.overflowed(), buffer);
}

/**
 * Parse the JSON, count devices and busses.
 **/
void test_sprt_json_valid(void) {
  JsonArray arr = eeprom_json.as<JsonArray>();

  uint8_t buscount = 0;
  for (JsonObject bus : arr) {
    // Count reachable devices in each device group
    uint8_t devicecount = 0;
    for (JsonPair device : bus) {
      buscount++;

      // Device addresses
      JsonArray addresses = device.value().as<JsonArray>();

      for (JsonVariant address : addresses) {
        devicecount++;
      }
    }

    // Add the number of devices on this bus to the tally
    totaldevices += devicecount;
  }

  TEST_ASSERT_NOT_EQUAL(0, buscount);

  TEST_ASSERT_NOT_EQUAL(0, totaldevices);
}

/**
 * Attempt to reach a device.
 **/
void test_device_reachable(void) {
  char msg[30];
  sprintf(msg, "Device unreachable (%01X.%01X.%01X.%02X)", CSOS_FQA_SEG_I2CBUS(devices[d]), CSOS_FQA_SEG_MUXNUM(devices[d]), CSOS_FQA_SEG_MUXBUS(devices[d]), CSOS_FQA_SEG_DEVADR(devices[d]));
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP::I2CIP_ERR_NONE, I2CIP::Device::ping(devices[d]), msg);
}

void setup() {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_sprt_json_deser);

  RUN_TEST(test_sprt_json_valid);

  devices = new i2cip_fqa_t[totaldevices];

  JsonArray arr = eeprom_json.as<JsonArray>();

  uint8_t busnum = 0, i = 0;
  for (JsonObject bus : arr) {
    for (JsonPair device : bus) {

      // Device addresses
      JsonArray addresses = device.value().as<JsonArray>();

      for (JsonVariant address : addresses) {
        devices[i] = I2CIP::createFQA(WIRENUM, MUXNUM, busnum, address.as<uint8_t>());
        i++;
      }

      busnum++;
    }
  }
}

void loop() {
  if(d >= totaldevices) {
    UNITY_END();
    delete devices;
  }
  RUN_TEST(test_device_reachable);
  d++;
}