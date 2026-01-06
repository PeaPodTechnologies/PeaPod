import { Ora, Spinner } from 'ora';
import ora from 'ora';
// import * as blessed from 'blessed';

const I2CIP_DEFAULT_SPINNER: Spinner = {
  interval: 50,
  frames: [
    '▁▁▁▁▁▁▁▁▁▁▁',
    '█▁▁▁▁▁▁▁▁▁▁',
    '██▁▁▁▁▁▁▁▁▁',
    '███▁▁▁▁▁▁▁▁',
    '████▁▁▁▁▁▁▁',
    '█████▁▁▁▁▁▁',
    '▁█████▁▁▁▁▁',
    '▁▁█████▁▁▁▁',
    '▁▁▁█████▁▁▁',
    '▁▁▁▁█████▁▁',
    '▁▁▁▁▁█████▁',
    '▁▁▁▁▁▁█████',
    '▁▁▁▁▁▁▁████',
    '▁▁▁▁▁▁▁▁███',
    '▁▁▁▁▁▁▁▁▁██',
    '▁▁▁▁▁▁▁▁▁▁█',
  ],
};

// class ConsoleStream implements NodeJS.WritableStream, NodeJS.EventEmitter {
//   out: (message?: any, ...optionalParams: any[]) => void;
//   constructor(
//     readonly con: Console,
//     readonly writable = true
//   ) {
//     // Capture console.log
//     this.out = con.log;
//   }
//   // Writeable Stream
//   write(
//     buffer: Uint8Array | string,
//     encodingOrCb?: BufferEncoding | ((err?: Error | null) => void),
//     cb?: (err?: Error | null) => void
//   ): boolean {
//     if (typeof encodingOrCb === 'function') {
//       this.out(buffer);
//       encodingOrCb();
//     } else {
//       if (encodingOrCb) this.out(Buffer.from(buffer as string, encodingOrCb));
//       else this.out(buffer);
//       if (cb) cb();
//     }
//     return true;
//   }
//   end(
//     dataOrCb?: string | Uint8Array | (() => void),
//     encodingOrCb?: BufferEncoding | (() => void),
//     cb?: () => void
//   ): this {
//     if (typeof dataOrCb === 'function') {
//       this.out('\n');
//       dataOrCb();
//     } else if (typeof encodingOrCb === 'function') {
//       this.out(dataOrCb as string | Uint8Array);
//       if ((dataOrCb as string | Uint8Array).toString().slice(-1) !== '\n')
//         this.out('\n');
//       encodingOrCb();
//     } else {
//       if (dataOrCb) this.out(dataOrCb);
//       if (encodingOrCb) this.out('\n');
//       if (cb) cb();
//     }
//     return this;
//   }

//   // Readable (EventEmitter) - NOP FOR NOw
//   on<K>(eventName: string | symbol, listener: (...args: any[]) => void): this {
//     return this;
//   }
//   once<K>(
//     eventName: string | symbol,
//     listener: (...args: any[]) => void
//   ): this {
//     return this;
//   }
//   off<K>(eventName: string | symbol, listener: (...args: any[]) => void): this {
//     return this;
//   }
//   emit(eventName: string | symbol, ...args: any[]): boolean {
//     return true;
//   }
//   // addListener, removeListener, removeAllListeners, setMaxListeners
//   // listeners, rawListeners, eventNames
//   addListener<K>(
//     eventName: string | symbol,
//     listener: (...args: any[]) => void
//   ): this {
//     return this;
//   }
//   removeListener<K>(
//     eventName: string | symbol,
//     listener: (...args: any[]) => void
//   ): this {
//     return this;
//   }
//   removeAllListeners<K>(eventName?: string | symbol): this {
//     return this;
//   }
//   setMaxListeners(n: number): this {
//     return this;
//   }
//   getMaxListeners(): number {
//     return 0;
//   }
//   listeners<K>(eventName: string | symbol): Function[] {
//     return [];
//   }
//   rawListeners<K>(eventName: string | symbol): Function[] {
//     return [];
//   }
//   eventNames(): (string | symbol)[] {
//     return [];
//   }
//   listenerCount<K>(
//     eventName: string | symbol,
//     listener?: Function | undefined
//   ): number {
//     return 0;
//   }
//   prependListener<K>(
//     eventName: string | symbol,
//     listener: (...args: any[]) => void
//   ): this {
//     return this;
//   }
//   prependOnceListener<K>(
//     eventName: string | symbol,
//     listener: (...args: any[]) => void
//   ): this {
//     return this;
//   }
// }

// class DebugJsonUI extends ConsoleStream {
class DebugJsonUI {
  spinner: Ora = this.reset();
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  private _log: (...args: any[]) => void;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  private _err: (...args: any[]) => void;
  constructor(con: Console = console) {
    // super(con);
    // Create and start the spinner
    // this.spinner.start('Loading...');
    this._log = con.log;
    this._err = con.error;
  }

  reset(): Ora {
    if(this.spinning()) { 
      this.spinner.stop();
    } else {
      this.spinner = ora({ 
        // stream: this, 
        spinner: I2CIP_DEFAULT_SPINNER
      });
    }
    return this.spinner;
  }

  spinning(): boolean {
    return this.spinner?.isSpinning;
  }

  /**
   * Start the loading spinner.
   * @param text Text to display.
   * @param spinner Spinner to use. Defaults to the default spinner.
   */
  start(text: string = '') {
    // If it's already spinning, just change the text
    if (this.spinning()) {
      this.spinner.text = text;
    } else {
      // Otherwise, start a new one
      this.reset();
      this.spinner.start(text);
    }
  }

  /**
   * Fail the loading spinner.
   * @param text Text to display.
   */
  fail(text: string = '') {
    // If it's spinning, change the text and fail
    if (!this.spinning()) {
      this.reset();
    }
    this.spinner.fail(text);
  }

  /**
   * Succeed the loading spinner.
   * @param text Text to display.
   */
  succeed(text: string = '') {
    // If it's spinning, change the text and succeed
    if (!this.spinning()) {
      this.reset();
    }
    this.spinner.succeed(text);
  }

  /**
   * Complete the loading spinner with info (blue `i`)
   * @param text Text to display.
   */
  info(text: string = '') {
    if (!this.spinning()) {
      this.reset();
    }
    this.spinner.info(text);
  }

  /**
   * If spinning: stop and clear the current spinner, log some text, then restart the spinner
   * Else: Just log
   *
   * @param text Text to log
   */
  log(...args: unknown[]) {
    if (this.spinning()) {
      const oldtext = this.spinner.text;
      this.spinner.stop(); // stop spinner
      this._log(...args); // log text
      this.spinner.start(oldtext); // restart spinner
    } else {
      this._log(...args); // log text
    }
  }

  /**
   * If spinning: stop and clear the current spinner, log some text, then restart the spinner
   * Else: Just log
   *
   * @param text Text to log
   */
  err(...args: unknown[]) {
    if (this.spinning()) {
      const oldtext = this.spinner.text;
      this.spinner.stop(); // stop spinner
      this._err(...args); // log text
      this.spinner.start(oldtext); // restart spinner
    } else {
      this._err(...args); // log text
    }
  }
}

export const DebugJsonConsole = new DebugJsonUI();
export default DebugJsonConsole;

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function _logRedirect(...message: any[]): void { 
  DebugJsonConsole.log(...message);
}

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function _errRedirect(...message: any[]): void { 
  DebugJsonConsole.err(...message);
}

// // Use yargs to get a flag "--spawn"
// const _IS_SPAWN = yargs(process.argv.slice(2)).boolean('spawn').argv;

// // If spawn, spawn a new process
// if (_IS_SPAWN) {
// 	// Import the child_process module
// 	import { spawn } from 'child_process';
// 	// Spawn a new process, with that console used as a stream for the UI
// 	const n = spawn('node', [], {});
// 	// Make writablestream from n.stdout

// } else {

// }

// If not spawn, this is the spawned process. Run the UI

// Create a screen object.
// let screen = blessed.screen({
// 	smartCSR: true
// });

// let box = blessed.box({
// 	top: 'top',
// 	left: 'left',
// 	width: '50%',
// 	height: '50%',
// 	border: {
// 		type: 'line'
// 	},
// 	style: {
// 		fg: 'white',
// 		bg: 'black',
// 		border: {
// 			fg: '#ffffff'
// 		}
// 	},
// 	align: 'left'
// });

// screen.append(box);

// export namespace Screen {
// 	export function render(): void {
// 		box.focus();
// 		screen.render();
// 	}
// 	export function hide(): void {
// 		box.hide();
// 	}
// 	export function setData(data: DebugJsonMessage): void {
// 		let s = '[' + (data.t ?? parseType(data.type)) + ']';
// 		if (data.timestamp) s += '@' + data.timestamp + 'ms';
// 		if (data.data) {
// 			s += Object.entries(data.data).map(datum => {
// 				return (
// 					datum[0] // label
// 						.split('-') // multiword delimiter; replace with space
// 						.map(word => word.slice(0, 1).toUpperCase() + word.slice(1).toLowerCase())
// 						.join(' ') +
// 					': ' +
// 					parseDatum(datum[1]) // value, to string
// 				);
// 			}).join(', ') + ' }';
// 		}
// 		box.setContent(s);
// 	}
// }
