#ifndef UNIT_TEST

#include <main.h>

#include <Arduino.h>

#if ENABLE_WATCHDOG
  #include <avr/wdt.h>
#endif

#include <utils/instructions.h>
#include <utils/jsonmessenger.h>
#include <sensors/sensor.h>
#include <actuators/actuator.h>

bool post(void);

void setup(void) {
  // Status LED
  pinMode(PIN_STATUS, OUTPUT);
  digitalWrite(PIN_STATUS, LOW);

  // Await serial start
  Serial.begin(BAUDRATE);
  while (!Serial) {
    delay(1);
  }

  // Send software revision first
  JSONMessenger::sendRevision(REVISION);

  // Serial communications established, initialize sensors and actuators
  if (!post()) {
    // Failed POST, blink
    while (true) {
      delay(250);
      digitalWrite(PIN_STATUS, HIGH);
      delay(250);
      digitalWrite(PIN_STATUS, LOW);
    }
  }

  // Await valid initial instruction set
  String in;
  do {
    // Await instructions
    while (!Serial.available());
    in = Serial.readStringUntil('\n');

    // Trim whitespace, newline terminator
    in.trim();
  } while (handleInstructions(in, &matrix) != ERR_NONE);

  // Enable the watchdog timer
  #if ENABLE_WATCHDOG
    wdt_enable(WDTO_1S);
  #endif
  digitalWrite(PIN_STATUS, HIGH);
}

void loop(void) {
  // Check for instructions
  if (Serial.available()) {
    String in = Serial.readStringUntil('\n');
    
    in.trim();
    handleInstructions(in, &matrix);
  }

  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->update();
    if (state->error == ERR_NONE) {
      if (state->debug == DS_SUCCESS) {
        for (int j = 0; j < state->numdata; ++j) {
          JSONMessenger::sendData(state->data[j].label, state->data[j].value);
        }
      } // else do nothing
    } else if (state->error == ERR_WARNING) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to read from sensor " + String(sensors[i]->getID()) + " (non-fatal)."));
    } else if (state->error == ERR_FATAL) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to read from sensor " + String(sensors[i]->getID()) + " (FATAL, SENSOR DISABLED!)."));
    }
  }

  #if ENABLE_WATCHDOG
    wdt_enable(WDTO_1S);
  #endif
}

bool post(void) {
  bool success = true;
  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->begin();
    bool latest = (state->debug == DS_INITIALIZED && state->error == ERR_NONE);
    if (latest) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Sensor " + String(sensors[i]->getID()) + " initialized successfully."));
    } else {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Failed to initialize sensor " + String(sensors[i]->getID()) + ". Check wiring."));
    }
    success &= latest;
  }
  for (int i = 0; i < NUM_ACTUATORS; ++i) {
    ActuatorState* state = actuators[i]->begin();
    bool latest = (state->debug == DS_INITIALIZED && state->error == ERR_NONE);
    if (latest) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Actuator " + String(actuators[i]->getID()) + " initialized successfully."));
    } else {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Failed to initialize actuator " + String(actuators[i]->getID()) + ". Check wiring."));
    }
    success &= latest;
  }
  return success;
}

#endif UNIT_TEST