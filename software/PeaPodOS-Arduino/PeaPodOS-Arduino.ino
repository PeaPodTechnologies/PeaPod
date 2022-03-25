#include "./src/Sensor.h"
#include "./src/SHT31.h"

#include "./src/Actuator.h"
#include "./src/LED.h"
// #include "Wire.h"
// #include "K30.h"
// #include "FloatSensor.h"
// #include "Scale.h"

//MACRO DEFINITIONS
#define NUM_SENSORS 2
#define NUM_ACTUATORS 1
// #define FLOATSENSOR_PIN 5
#define REVISION 0

// Sensors
SHT31 sht31;
SHT31_temp temp = SHT31_temp(&sht31);
SHT31_hum hum = SHT31_hum(&sht31);
// K30 k30;
// Scale scale;
// FloatSensor fs = FloatSensor(FLOATSENSOR_PIN);

Sensor* sensors [NUM_SENSORS] = {
    &temp,
    &hum,
    // &k30,
    // &fs,
    // &scale
};

//Actuators
LED led_blue(3);
LED led_cool(5);
LED led_warm(6);
LED led_red(9);
LED led_far(10);

Actuator* actuators [NUM_ACTUATORS] = {
  &led_blue,
  &led_cool,
  &led_warm,
  &led_red,
  &led_far,
}


void setup()
{
    digitalWrite(13, LOW);
    if(!post()){
        // Failed POST
        while(true){
            delay(250);
            digitalWrite(13, HIGH);
            delay(250);
            digitalWrite(13, LOW);
        }
    }

    // Tell computer 'ready', wait to receive valid program.
    Serial.print("{\"type\":\"revision\",\"data\":");
    Serial.print(REVISION);
    Serial.print("}\n");

    String ins;
    do{
        while(!Serial.available());
        ins = Serial.readString();
        ins.trim();
    }while(!handleInstructions(ins));
    digitalWrite(13, HIGH);
}

void loop()
{
    if(Serial.available()){
        String in = Serial.readString();
        in.trim(); //Whitespace or newlines
        handleInstructions(in);
    }

    for(int i = 0; i < NUM_SENSORS; i++){
        float read = sensors[i]->getRead();
        if(!isnan(read)){
            Serial.print("{\"type\":\"data\",\"data\":{\"label\":\"");
            Serial.print(sensors[i]->evname);
            Serial.print("\",\"value\":");
            Serial.print(read);
            Serial.print("}}\n");
        }
    }
    delay(10);
}

/**
 * Handles incoming instruction "var":value string.
 * @param s - Instruction string
 * @return was the instruction handled in a valid way?
 **/
bool handleInstruction(String in){
    uint8_t split = in.indexOf(":");
    if(split!=-1 && in.charAt(0) == '"' && in.charAt(split-1) == '"'){
        String var = in.substring(1, split-1); //Assumes surrounding ""
        float value = in.substring(split+1).toFloat();

        //INSTRUCTION HANDLING IF BLOCKS - EACH RETURNS TRUE
        if(var.equals("led_blue")){
            led_blue.target = value;
            return true;
        }
        if(var.equals("led_cool")){
            led_cool.target = value;
            return true;
        }
        if(var.equals("led_warm")){
            led_warm.target = value;
            return true;
        }
        if(var.equals("led_red")){
            led_red.target = value;
            return true;
        }
        if(var.equals("led_far")){
            led_far.target = value;
            return true;
        }
        Serial.print("{\"type\":\"error\",\"data\":\"Unknown instruction target '");
        Serial.print(var);
        Serial.print("'\"}\n");
        return false;
    }
    Serial.print("{\"type\":\"error\",\"data\":\"Failed to handle instruction '");
    in.replace("\"", "\\\"");
    Serial.print(in);
    Serial.print("'\"}\n");
    return false;
}

/**
 * Handles incoming instruction dictionary {"var":val,...}
 * Continues regardless of failure of a single instruction, alerts only
 * @param s - Instructions dictionary string
 * @return were all instructions handled in a valid way?
 * */
bool handleInstructions(String ins){
    if(ins.equals("{}")){
        return true;  //Empty dictionary, by default handled
    }
    if(ins.charAt(0) != '{' || ins.charAt(ins.length()-1) != '}'){
        Serial.print("{\"type\":\"error\",\"data\":\"Invalid instructions dictionary '");
        Serial.print(ins);
        Serial.print("'\"}\n");
        return false;
    }
    ins = ins.substring(1, ins.length()-1); //Strips surrounding {}

    //Handles each individual instruction

    bool result = true;
    //While there are multiple instructions
    while(ins.indexOf(',') != -1){
        //Did the leftmost one succeed?
        result &= handleInstruction(ins.substring(0, ins.indexOf(',')));
        //Truncate leftmost instruction
        ins = ins.substring(ins.indexOf(',')+1);
    }
    //Handle last/only instruction (there are no commas left)
    result &= handleInstruction(ins);
    return result;
}

bool post(){
    Serial.begin(115200);
    while(!Serial); //Waits until serial opens

    // Test sensor protocols - per-sensor tests
    bool success = true;
    for(int i = 0; i < NUM_SENSORS; i++){
        bool latest = sensors[i]->begin(); 
        success &= latest;
        if(!latest){
            Serial.print("{\"type\":\"error\",\"data\":\"Failed to initialize sensor '");
            Serial.print(sensors[i]->name);
            Serial.print("'. Check wiring.\"}\n");
        } else {
            Serial.print("{\"type\":\"debug\",\"data\":\"Sensor '");
            Serial.print(sensors[i]->name);
            Serial.print("' initialized successfully.\"}\n");

            // {type: debug, data: "Sensor 'Temperature Sensor' initialized successfully."}
        }
    }
    
    // Test actuator protocols - per-actuator tests
    for(int i = 0; i < NUM_ACTUATORS; i++){
        bool latest = actuators[i]->begin();
        success &= latest;
        if(!latest){
            Serial.print("{\"type\":\"error\",\"data\":\"Failed to initialize actuator '");
            Serial.print(actuators[i]->name);
            Serial.print("'. Check wiring.\"}\n");
        } else {
            Serial.print("{\"type\":\"debug\",\"data\":\"Actuator '");
            Serial.print(actuators[i]->name);
            Serial.print("' initialized successfully.\"}\n");
        }
    }
    return success;
}
