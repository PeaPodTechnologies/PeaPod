/**
* Error on revision check.
*/
export class RevisionError extends Error {
    constructor(expected : string, received : string){
        super(`Revision check failed! Expected ${expected}, received ${received}`);
    }
}

/**
* Error on instruction send.
*/
export class ArduinoInstructionsError extends Error {
    constructor(instructions: any){
        super("Failed to send instructions to Arduino: '"+JSON.stringify(instructions)+"'");
    }
}