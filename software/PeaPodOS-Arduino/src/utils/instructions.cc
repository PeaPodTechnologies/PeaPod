#include <utils/instructions.h>

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <utils/jsonmessenger.h>

/**
 * Checks a non-empty instruction set for formatting validity
 * @param instructionSet JSON-formatted instruction set
 * @return Validity
 */
bool isValidSet(String instructionSet) {
  return (instructionSet.charAt(0) == '{' && instructionSet.charAt(instructionSet.length()-1) == '}' && instructionSet.indexOf(':') != -1);
}

/**
 * Handles incoming instruction set according to an instruction-actuator matrix
 * @param instructionSet JSON-formatted instruction set
 * @param matrix Instruction-actuator matrix
 * @return Error level - `ERR_FATAL`: invalid instruction set formatting; `ERR_WARNING`: one or more instruction labels were unknown
 */
errorlevel_t handleInstructions(String instructionSet, const InstructionActuatorMatrix* matrix) {
  // If empty, by default handled
  if(instructionSet.equals("{}")){
    return ERR_NONE;
  }

  // If invalid, throw error
  if (!isValidSet(instructionSet)) {
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Invalid instruction set '" + instructionSet + "', bad formatting."));
    return ERR_FATAL;
  }

  // Valid and non-empty, strip curly braces
  instructionSet = instructionSet.substring(1, instructionSet.length()-1);

  errorlevel_t result = ERR_NONE;
  // While there are instructions:
  while (instructionSet.indexOf(':') != -1) {
    // Find end of leftmost instruction
    int stop = instructionSet.indexOf(',');
    if (stop == -1) {
      stop = instructionSet.length();
    }

    // Handle leftmost instruction
    String instruction = instructionSet.substring(0, stop);
    errorlevel_t latest = handleInstruction(instruction, matrix);

    // Raise result errorlevel?
    result = max(result, latest);

    // Truncate leftmost instruction
    instructionSet = instructionSet.substring(stop+1);
  }
  return result;
}

errorlevel_t handleInstruction(String instruction, const InstructionActuatorMatrix* matrix) {
  uint8_t split = instruction.indexOf(":");

  // Check formatting
  if (split != -1 && instruction.charAt(0) == '"' && instruction.charAt(split-1) == '"') {
    // Get label (remove quotations)
    String label = instruction.substring(1, split-1);

    // Get value
    float value = instruction.substring(split+1).toFloat();

    if(value == 0 && !(instruction.charAt(split+1) == '0' || (instruction.charAt(split+1) == '.' && instruction.charAt(split+2) == '0'))) {
      JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Invalid target value '" + instruction.substring(split+1) + "'."));
      return ERR_FATAL;
    }

    for (int i = 0; i < matrix->numActuators; ++i) {
      if (label.equals(matrix->instructions[i])) {
        matrix->actuators[i]->setTarget(value);
        return ERR_NONE;
      }
    }
    JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Unknown instruction label '" + label + "'."));
    return ERR_WARNING;
  }
  JSONMessenger::sendMessage(JSONMessenger::MESSAGE_ERROR, String("Invalid instruction '" + instruction + String("', bad formatting.")));
  return ERR_FATAL;
}