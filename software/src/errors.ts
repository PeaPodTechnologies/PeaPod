export class ControllerInstructionsError extends Error {
	constructor(instructions: any) {
		super("Failed to send instructions to Arduino: '" + JSON.stringify(instructions) + "'");
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
