import * as dns from 'dns';
import { existsSync, mkdirSync, writeFileSync, readFileSync } from 'fs';
import axios from 'axios';
import { spawn } from 'child_process';
import { GPIOError } from './errors';

/**
 * Checks the internet connectivity.
 * @param timeout Timeout in milliseconds. Default: 5000
 * @param url The URL to ping. Default: 'www.google.com'
 */
export function checkInternet(timeout: number = 5000, url: string = 'www.google.com'): Promise<boolean> {
  const resolver = new dns.Resolver({ timeout });
  return new Promise<boolean>(ret => {
    resolver.resolve(url, err => {
      if (err) {
        ret(false);
      } else {
        ret(true);
      }
    });
  });
};

/**
 * Sleep a given number of milliseconds. 
 * 
 * Usage: `await sleep(x)`
 * @param millis Number of milliseconds to sleep.
 * @returns 
 */
export function sleep(millis: number): Promise<void> {
  return new Promise(resolve => {
    setTimeout(resolve, millis);
  });
};

/**
 * Fetch the Google Root CA certificate.
 * @returns The very same certificate.
 */
export async function fetchServerCert(): Promise<string> {
  return String((await axios.get('https://pki.goog/roots.pem')).data);
}

/**
 * Checks that the microcontroller is connected properly and accessible for flashing.
 */
export function checkMicrocontroller(): Promise<void> {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(`avrdude -p m328p -C ~/avrdude_gpio.conf -c peapod -v`, [1]).catch(err => {
      writeFileSync('logs/checkMicrocontroller.log', err);
      rej(new Error(`Failed to communicate with the microcontroller. See logs/checkMicrocontroller.log`));
    }).then(log1 => {
      if (log1) writeFileSync('logs/checkMicrocontroller.log', log1);
      res();
    });
  });
}

/**
 * Compiles the microcontroller software, and flashes the binary to the chip.
 */
export function updateMicrocontroller(): Promise<void> {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(`${ process.env.HOME }/.platformio/penv/bin/platformio run -d PeaPodOS-Arduino/ -e peapod -t upload`, [1]).catch(err => {
      writeFileSync('logs/updateMicrocontroller.log', err);
      rej(new Error(`Failed to update the microcontroller software. See logs/updateMicrocontroller.log`));
    }).then(log1 => {
      if (log1) writeFileSync('logs/updateMicrocontroller.log', log1);
      res();
    });
  });
}

/**
 * General purpose command execution and logging. No `sudo` support.
 */
export function execute(command: string, failureCodes: number[] = []): Promise<string> {
  return new Promise<string>((res, rej) => {
    const args = command.split(' ');
    const eprocess = spawn(args[0], args.slice(1));
    let log = "> "+command+"\n";
    eprocess.stdout?.on('data', out => {
      log += out;
    });
    eprocess.stderr?.on('data', out => {
      log += out;
    });
    eprocess.on('error', error => {
      log += error.message;
      rej(log);
      eprocess.kill();
    });
    eprocess.on('close', code => {
      if (code) {
        log += "> Process exited with code "+code;
        if (failureCodes.includes(code)) {
          rej(log);
        }
        return;
      }
      // If no options, no codes, OR non-failure:
      res(log);
    });
  });
}

/**
 * Exports a pin (if it is not already)
 * @param pin The GPIO pin to export.
 */
export function gpioExport(pin: number): void {
  if (!existsSync(`/sys/class/gpio/gpio${ pin }/`)) {
    try{
      execute(`echo ${ pin } > /sys/class/gpio/export`);
    } catch (err) {
      throw new GPIOError('export', pin);
    }
  }
}

/**
 * Unexports a pin (if it has been previously exported)
 * @param pin The GPIO pin to unexport.
 */
export function gpioUnexport(pin: number): void {
  if (existsSync(`/sys/class/gpio/gpio${ pin }/`)) {
    try {
      execute(`echo ${ pin } > /sys/class/gpio/unexport`);
    } catch (err) {
      throw new GPIOError('unexport', pin);
    }
  }
}

/**
 * Exports and sets the direction (output) of a pin, and writes either 1 or 0 to it.
 * Does NOT unexport the pin.
 * @param pin The GPIO pin to write to.
 * @param value 1 or 0.
 */
export function gpioWrite(pin: number, value: 0 | 1): void {
  gpioExport(pin);
  try {
    writeFileSync(`/sys/class/gpio/gpio${ pin }/direction`, 'out');
  } catch (err) {
    throw new GPIOError('direction', pin, 'out');
  }
  try {
    writeFileSync(`/sys/class/gpio/gpio${ pin }/value`, String(value));
  } catch (err) {
    throw new GPIOError('write', pin, value);
  }
}

/**
 * Exports and sets the direction (input) of a pin, reads from it, and returns the value.
 * Does NOT unexport the pin.
 * @param pin The GPIO pin to read from.
 * @param value 
 */
export function gpioRead(pin: number): number {
  gpioExport(pin);
  try {
    writeFileSync(`/sys/class/gpio/gpio${ pin }/direction`, 'in');
  } catch (err) {
    throw new GPIOError('direction', pin, 'in');
  }
  let raw: string;
  try {
    raw = readFileSync(`/sys/class/gpio/gpio${ pin }/value`).toString();
  } catch (err) {
    throw new GPIOError('read', pin);
  }
  const value: number = Number(raw);
  if (isNaN(value)) throw new GPIOError('read', pin);
  return value;
}