#ifndef UNIT_TEST

#include <main.h>

#include <Arduino.h>

#include <utils/instructions.h>
#include <utils/jsonmessenger.h>
#include <sensors/sensor.h>
#include <actuators/actuator.h>

// DECLARATIONS
bool post(void);

// GLOBAL VARIABLES
uint8_t sensor = 0, actuator = 0;

// FUNCTIONS
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

  // Await valid initial instruction set
  String in;
  do {
    // Await instructions
    while (!Serial.available());
    in = Serial.readStringUntil('\n');

    // Trim whitespace, newline terminator
    in.trim();
  } while (InstructionHandler::handleSet(in, &matrix) != ERR_NONE);

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

  // Enable the watchdog timer
  #if ENABLE_WATCHDOG
    wdt_enable(WDTO);
  #endif
  digitalWrite(PIN_STATUS, HIGH);
}

void loop(void) {
  // Check for instructions
  if (Serial.available()) {
    String in = Serial.readStringUntil('\n');
    in.trim();
    InstructionHandler::handleSet(in, &matrix);
  }

  // Read sensor
  SensorState* sstate = sensors[sensor]->update();
  if (sstate->error == ERR_NONE) {
    if (sstate->debug == DS_SUCCESS) {
      for (int j = 0; j < sstate->numdata; ++j) {
        JSONMessenger::sendData(sstate->data[j].label, sstate->data[j].value);
      }
    } // else do nothing
  } else if (sstate->error == ERR_WARNING) {
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to read from sensor " + String(sensors[sensor]->getID()) + " (non-fatal)."));
  } else if (sstate->error == ERR_FATAL) {
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to read from sensor " + String(sensors[sensor]->getID()) + " (FATAL, SENSOR DISABLED!)."));
  }

  // Update actuator
  ActuatorState* astate = actuators[actuator]->update();
  if (astate->error == ERR_WARNING) {
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to update actuator " + String(actuators[actuator]->getID()) + " (non-fatal)."));
  } else if (astate->error == ERR_FATAL) {
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to update actuator " + String(actuators[actuator]->getID()) + " (FATAL, ACTUATOR DISABLED!)."));
  }

  // Increment
  sensor = (sensor+1) % NUM_SENSORS;
  actuator = (actuator+1) % NUM_ACTUATORS;

  #if ENABLE_WATCHDOG
    wdt_enable(WDTO);
  #endif
}

bool post(void) {
  bool success = true;
  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->begin();
    bool latest = (state->debug >= DS_INITIALIZED && state->error == ERR_NONE);
    if (latest) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Sensor " + String(sensors[i]->getID()) + " initialized successfully."));
    } else {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Failed to initialize sensor " + String(sensors[i]->getID()) + ". Check wiring."));
    }
    success &= latest;
  }
  for (int i = 0; i < NUM_ACTUATORS; ++i) {
    ActuatorState* state = actuators[i]->begin();
    bool latest = (state->debug >= DS_INITIALIZED && state->error == ERR_NONE);
    if (latest) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Actuator " + String(actuators[i]->getID()) + " initialized successfully."));
    } else {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_DEBUG, String("Failed to initialize actuator " + String(actuators[i]->getID()) + ". Check wiring."));
    }
    success &= latest;
  }
  return success;
}

#endif