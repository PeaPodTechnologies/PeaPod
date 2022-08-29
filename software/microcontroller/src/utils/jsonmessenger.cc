#include <utils/jsonmessenger.h>

#include <Arduino.h>

String parseMessageLevel(JSONMessenger::jsonmessagetype_t type) {
  switch (type) {
    case JSONMessenger::MESSAGE_DEBUG:
      return String("debug");
    case JSONMessenger::MESSAGE_ERROR:
      return String("error");
  }
  return String("msg");
}

namespace JSONMessenger {

  #ifdef UNIT_TEST

  String sendMessage(jsonmessagetype_t type, String message) {
    // Replace quotes with escaped quotes
    message.replace("\"", "\\\"");
    String out = String("{\"type\":\"" + parseMessageLevel(type) + "\",\"data\":\"" + message + "\"}\n");
    return out;
  }

  String sendRevision(uint8_t revision) {
    String out = String("{\"type\":\"revision\",\"data\":" + String(revision) + "}\n");
    return out;
  }

  String sendData(const char* label, float value) {
    String out = String("{\"type\":\"data\",\"data\":{\"label\":\"" + String(label) + "\",\"value\":" + String(value, DATADECIMALS) + "}}\n");
    return out;
  }

  #else

  void sendMessage(jsonmessagetype_t type, String message) {
    // Replace quotes with escaped quotes
    message.replace("\"", "\\\"");
    String out = String("{\"type\":\"" + parseMessageLevel(type) + "\",\"data\":\"" + message + "\"}\n");
    Serial.print(out);
  }

  void sendRevision(uint8_t revision) {
    String out = String("{\"type\":\"revision\",\"data\":" + String(revision) + "}\n");
    Serial.print(out);
  }

  void sendData(const char* label, float value) {
    String out = String("{\"type\":\"data\",\"data\":{\"label\":\"" + String(label) + "\",\"value\":" + String(value, DATADECIMALS) + "}}\n");
    Serial.print(out);
  }

  #endif
}