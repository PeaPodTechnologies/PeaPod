// IMPORTS
#include "./src/Sensor.h"
#include "./src/sensors/SHT31.h"
// #include "./src/sensors/K30.h"
// #include "./src/Actuator.h"
// #include "./src/actuators/led.h"

// CONSTANTS
#define NUM_SENSORS   1
// #define NUM_ACTUATORS 0
#define REVISION      0
#define BAUDRATE      115200
#define DEBUG         true

// PINS
#define PIN_STATUS    13
// #define PIN_LEDBLUE   3
// #define PIN_LEDCOOL   5
// #define PIN_LEDWARM   6
// #define PIN_LEDRED    9
// #define PIN_LEDFAR    10

// SENSORS
SHT31 sht31 = SHT31();
// K30 k30 = K30();

Sensor* sensors[NUM_SENSORS] = {
  &sht31,
  // &k30,
};

// ACTUATORS
// LED led_blue(LEDPIN_BLUE, LEDCOLOR_BLUE);
// LED led_cool(LEDPIN_COOL, LEDCOLOR_COOL);
// LED led_warm(LEDPIN_WARM, LEDCOLOR_WARM);
// LED led_red(LEDPIN_RED, LEDCOLOR_RED);
// LED led_far(LEDPIN_FAR, LEDCOLOR_FAR);

// Actuator* actuators [NUM_ACTUATORS] = {
//   &led_blue,
//   &led_cool,
//   &led_warm,
//   &led_red,
//   &led_far,
// }

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
  Serial.print("{\"type\":\"revision\",\"data\":");
  Serial.print(REVISION);
  Serial.print("}\n");

  // Serial communications established, initialize sensors and actuators
  if (!post()) {
    // Failed POST
    while (true) {
      delay(250);
      digitalWrite(PIN_STATUS, HIGH);
      delay(250);
      digitalWrite(PIN_STATUS, LOW);
    }
  }

  // Await valid initial instruction set
  // String ins;
  // do {
  //   // Await instructions
  //   while (!Serial.available());
  //   ins = Serial.readString();

  //   // Trim whitespace, newline terminator
  //   ins.trim();
  // } while (!handleInstructions(ins));
  digitalWrite(PIN_STATUS, HIGH);
}

void loop(void) {
  // Check for instructions
  // if (Serial.available()) {
  //   String in = Serial.readString();
    
  //   // Trim whitespace, newline terminator
  //   in.trim();
  //   handleInstructions(in);
  // }

  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->update();

    #if DEBUG
      Serial.print(F("{\"type\":\"debug\",\"data\":\"Sensor "));
      Serial.print(sensors[i]->getID());
      Serial.print(F(" state: DS = "));
      Serial.print(state->debug);
      Serial.print(F(", ERR = "));
      Serial.print(state->error);
      Serial.print(F("\"}\n"));
    #endif

    if (state->error == ERR_NONE) {
      if (state->debug == DS_SUCCESS) {
        for (int j = 0; j < state->numdata; ++j) {
          Serial.print(F("{\"type\":\"data\",\"data\":{\"label\":\""));
          Serial.print(state->data[j].label);
          Serial.print(F("\",\"value\":"));
          Serial.print(state->data[j].value);
          Serial.print(F("}}\n"));
        }
      } // else do nothing
    } else if (state->error == ERR_WARNING) {
      Serial.print(F("{\"type\":\"debug\",\"data\":\"Failed to read from sensor "));
      Serial.print(sensors[i]->getID());
      Serial.print(F(" (non-fatal)\"}\n"));
    } else if (state->error == ERR_FATAL) {
      Serial.print(F("{\"type\":\"error\",\"data\":\"Failed to read from sensor "));
      Serial.print(sensors[i]->getID());
      Serial.print(F(" (FATAL, SENSOR DISABLED!)\"}\n"));
    }
  }
  delay(500);
}

bool post(void) {
  bool success = true;
  for (int i = 0; i < NUM_SENSORS; ++i) {
    SensorState* state = sensors[i]->begin();
    bool latest = (state->debug == DS_INITIALIZED && state->error == ERR_NONE);
    if (latest) {
      Serial.print("{\"type\":\"debug\",\"data\":\"Sensor ");
      Serial.print(sensors[i]->getID());
      Serial.print(" initialized successfully.\"}\n");
    } else {
      Serial.print("{\"type\":\"error\",\"data\":\"Failed to initialize sensor ");
      Serial.print(sensors[i]->getID());
      Serial.print(". Check wiring.\"}\n");
    }
    success &= latest;
  }
  return success;
}

/**
 * Handles incoming instruction dictionary {"var":val,...}
 * Continues regardless of failure of a single instruction, alerts only
 * @param s - Instructions dictionary string
 * @return were all instructions handled in a valid way?
 */
bool handleInstructions(String ins) {
  if(ins.equals("{}")){
    return true;
  }
  if (ins.charAt(0) != '{' || ins.charAt(ins.length()-1) != '}' || ins.indexOf(':') == -1) {
      Serial.print("{\"type\":\"error\",\"data\":\"Invalid instructions dictionary '");
      Serial.print(ins);
      Serial.print("', bad formatting.\"}\n");
      return false;
  }
  ins = ins.substring(1, ins.length()-1); // Strips surrounding {}

  // Handle each individual instruction
  bool result = true;
  // While there are multiple instructions:
  while (ins.indexOf(',') != -1) {
    // Did the leftmost one succeed?
    result &= handleInstruction(ins.substring(0, ins.indexOf(',')));
    //T runcate leftmost instruction
    ins = ins.substring(ins.indexOf(',')+1);
  }
  //Handle last/only instruction (there are no commas left)
  result &= handleInstruction(ins);
  return result;
}

/**
 * Handles incoming instruction "var":value string.
 * @param s - Instruction string
 * @return was the instruction handled in a valid way?
 */
bool handleInstruction(String in) {
  uint8_t split = in.indexOf(":");
  if (split!=-1 && in.charAt(0) == '"' && in.charAt(split-1) == '"') {
    String var = in.substring(1, split-1); //Assumes surrounding ""
    float value = in.substring(split+1).toFloat();

    //INSTRUCTION HANDLING IF BLOCKS - EACH RETURNS TRUE
    // if(var.equals("led_blue")){
    //   led_blue.target = value;
    //   return true;
    // }
    // if(var.equals("led_cool")){
    //   led_cool.target = value;
    //   return true;
    // }
    // if(var.equals("led_warm")){
    //   led_warm.target = value;
    //   return true;
    // }
    // if(var.equals("led_red")){
    //   led_red.target = value;
    //   return true;
    // }
    // if(var.equals("led_far")){
    //   led_far.target = value;
    //   return true;
    // }
    Serial.print("{\"type\":\"debug\",\"data\":\"Unknown instruction target '");
    Serial.print(var);
    Serial.print("'\"}\n");
    return false;
  }
  Serial.print("{\"type\":\"debug\",\"data\":\"Failed to handle instruction '");
  in.replace("\"", "\\\"");
  Serial.print(in);
  Serial.print("'\"}\n");
  return false;
}
