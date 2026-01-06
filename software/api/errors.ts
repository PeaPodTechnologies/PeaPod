export class ControllerTXError extends Error {
  constructor(msg: any) {
    super("DEBUGJSON RX FAIL: '" + JSON.stringify(msg) + "'");
  }
}

// UNUSED
// export class SerialTimeoutError extends Error {
//   constructor(timeoutSeconds?: number) {
//     super(`Arduino serial communication timed out${ timeoutSeconds === undefined ? '' : ` after ${ timeoutSeconds } seconds` }.`);
//   }
// };

export class EnvFieldError extends Error {
  constructor(mode: string, missingFields: string[]) {
    super(
      `.env file is missing the following fields necessary for ${mode} functionality: ${missingFields.join(
        ', '
      )}`
    );
  }
}

export class DebugJsonSerialportError extends Error {
  constructor(err: string) {
    super(`Serialport Error: ${err}`);
  }
}