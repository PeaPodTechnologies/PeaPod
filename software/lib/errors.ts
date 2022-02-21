/**
* Error on instruction send.
*/
export class ArduinoInstructionsError extends Error {
    constructor(instructions: any) {
        super("Failed to send instructions to Arduino: '"+JSON.stringify(instructions)+"'");
    }
}

export class SerialTimeoutError extends Error {
  constructor(timeoutSeconds?: number) {
    super(`Arduino serial communication timed out${timeoutSeconds === undefined ? '' : ` after ${timeoutSeconds} seconds`}.`);
  }
}