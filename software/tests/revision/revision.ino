#define REVISION 0

void setup() {
    digitalWrite(13, LOW);
    if (!post()) {
        // Failed POST
        while(true) {
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
    do {
        while(!Serial.available());
        ins = Serial.readString();
        ins.trim();
    } while(!handleInstructions(ins));
    digitalWrite(13, HIGH);
}

void loop() {
    if (Serial.available()) {
        String in = Serial.readString();
        in.trim(); //Whitespace or newlines
        handleInstructions(in);
    }

    delay(100);
}

/**
 * Handles incoming instruction "var":value string.
 * @param s - Instruction string
 * @return was the instruction handled in a valid way?
 **/
bool handleInstruction(String in) {
    uint8_t split = in.indexOf(":");
    if (split!=-1 && in.charAt(0) == '"' && in.charAt(split-1) == '"') {
        String var = in.substring(1, split-1); //Assumes surrounding ""
        float value = in.substring(split+1).toFloat();

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
bool handleInstructions(String ins) {
    if (ins.equals("{}")) {
        return true;  //Empty dictionary, by default handled
    }
    if (ins.charAt(0) != '{' || ins.charAt(ins.length()-1) != '}') {
        Serial.print("{\"type\":\"error\",\"data\":\"Invalid instructions dictionary '");
        Serial.print(ins);
        Serial.print("'\"}\n");
        return false;
    }
    ins = ins.substring(1, ins.length()-1); //Strips surrounding {}

    //Handles each individual instruction

    bool result = true;
    //While there are multiple instructions
    while (ins.indexOf(',') != -1) {
        //Did the leftmost one succeed?
        result &= handleInstruction(ins.substring(0, ins.indexOf(',')));
        //Truncate leftmost instruction
        ins = ins.substring(ins.indexOf(',')+1);
    }
    //Handle last/only instruction (there are no commas left)
    result &= handleInstruction(ins);
    return result;
}

bool post() {
    Serial.begin(115200);
    while(!Serial); //Waits until serial opens
    return true;
}
