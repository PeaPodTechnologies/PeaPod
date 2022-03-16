#include <Arduino.h>

#include <utils/base.h>
#include <utils/jsonmessenger.h>

void test_revision(void) {
  TEST_ASSERT_EQUAL_STRING(JSONMessenger::sendRevision(63).c_str(), "{\"type\":\"revision\",\"data\":63}\n")
}

void test_debug(void) {
  TEST_ASSERT_EQUAL_STRING(JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, "hello world").c_str(), "{\"type\":\"debug\",\"data\":\"hello world\"}\n");
}

void test_error(void) {
  TEST_ASSERT_EQUAL_STRING(JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, "oh no").c_str(), "{\"type\":\"error\",\"data\":\"oh no\"}\n");
}

void test_data(void) {
  TEST_ASSERT_EQUAL_STRING(JSONMessenger::sendData("testlabel", 32.4).c_str(), "{\"type\":\"data\",\"data\":{\"label\":\"testlabel\",\"value\":32.40}}\n");
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