#ifndef PEAPOD_UTILS_JSONMESSENGER_H_
#define PEAPOD_UTILS_JSONMESSENGER_H_

#include <Arduino.h>

#define DATADECIMALS 2

;namespace JSONMessenger {

  typedef enum jsonmessagetype_t {
    MESSAGE_DEBUG,
    MESSAGE_ERROR,
  } jsonmessagetype_t;

  #ifndef UNIT_TEST

  /**
   * Send a JSON-formatted message over Serial.
   * @param type Message type
   * @param message Message content
   */
  void sendMessage(jsonmessagetype_t type, String message);

  /**
   * Send a JSON-formatted revision message over Serial.
   * @param revision Software revision
   */
  void sendRevision(uint8_t revision);

  /**
   * Send a JSON-formatted message over Serial containing sensor data.
   * @param label Environment variable label
   * @param value Sensor data value
   */
  void sendData(const char* label, float value);

  #elif

  String sendMessage(jsonmessagetype_t type, String message);

  String sendRevision(uint8_t revision);

  String sendData(const char* label, float value);

  #endif
}

#endif