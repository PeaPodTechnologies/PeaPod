export abstract class ControlSystem {
	protected target?: number;
	protected value?: number;
	constructor(readonly actuator: string) {}
	abstract refresh(): number;
	setTarget(target: number) {
		this.target = target;
	}
	setValue(value: number) {
		this.value = value;
	}
}

// MAIN CLASS

/**
 * Interface for publishing data to and receiving instructions from Google Cloud/Firebase via IoT Core PubSub.
 */
export class BangBang extends ControlSystem {
	constructor(readonly actuator: string, readonly deadzone: number = 0) {
		super(actuator);
	}
	refresh(): number {
		if (this.target === undefined || this.value === undefined) {
			return NaN;
		}
		if (this.value > this.target + this.deadzone) {
			return -1;
		}
		if (this.value < this.target - this.deadzone) {
			return 1;
		}
		return 0;
	}
}

/**
 * Interface for publishing data to and receiving instructions from Google Cloud/Firebase via IoT Core PubSub.
 */
export class PID extends ControlSystem {
	private valueLastSet: number = NaN;
	private lasterr: number = NaN;
	private integral: number = 0;
	constructor(
		readonly actuator: string,
		readonly Kp: number,
		readonly Ki: number,
		readonly Kd: number
	) {
		super(actuator);
	}
	refresh(): number {
		if (this.target === undefined || this.value === undefined) {
			return NaN;
		}
		const err = this.target - this.value;
		let derivative = 0;
		if (this.valueLastSet != NaN || this.lasterr != NaN) {
			// Trapezoidal approximation
			this.integral += ((this.lasterr + err) / 2) * (Date.now() - this.valueLastSet);
			derivative = (err - this.lasterr) / (Date.now() - this.valueLastSet);
		}
		return this.Kp * err + this.Ki * this.integral + this.Kd * derivative;
	}

	override setValue(value: number): void {
		this.value = value;
		this.valueLastSet = Date.now();
	}
}
