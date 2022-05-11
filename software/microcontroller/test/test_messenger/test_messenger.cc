#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <utils/jsonmessenger.h>

void test_revision(void) {
  const char* result = JSONMessenger::sendRevision(63).c_str();
  TEST_ASSERT_EQUAL_STRING("{\"type\":\"revision\",\"data\":63}\n", result);
}

void test_debug(void) {
  const char* result = JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, "hello world").c_str();
  TEST_ASSERT_EQUAL_STRING("{\"type\":\"debug\",\"data\":\"hello world\"}\n", result);
}

void test_error(void) {
  const char* result = JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, "oh no").c_str();
  TEST_ASSERT_EQUAL_STRING("{\"type\":\"error\",\"data\":\"oh no\"}\n", result);
}

void test_data(void) {
  const char* result = JSONMessenger::sendData("testlabel", 32.4).c_str();
  TEST_ASSERT_EQUAL_STRING("{\"type\":\"data\",\"data\":{\"label\":\"testlabel\",\"value\":32.40}}\n", result);
}

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_revision);
  RUN_TEST(test_debug);
  RUN_TEST(test_error);
  RUN_TEST(test_data);

  UNITY_END();
}

void loop(void) { }