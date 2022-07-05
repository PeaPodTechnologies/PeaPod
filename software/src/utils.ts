import * as dns from 'dns';
import { existsSync, mkdirSync, writeFileSync, readFileSync } from 'fs';
import axios from 'axios';
import { spawn } from 'child_process';

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
 * Compiles the microcontroller software, and flashes the binary to the chip.
 */
export function updateMicrocontroller(): Promise<void> {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(`${ process.env.HOME }/.platformio/penv/bin/platformio run -d microcontroller/ -e peapod -t upload`, [1]).catch(err => {
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