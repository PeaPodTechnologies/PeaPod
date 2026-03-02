import { spawn } from 'child_process';
import { existsSync, mkdirSync, writeFileSync } from 'fs';
import { hostname } from 'os';
import { lookup } from 'dns';

const PATHSTEM_MICROCONTROLLER = './microcontroller/';

/**
 * Compiles the microcontroller software, and flashes the binary to the chip.
 */
export function updateMicrocontroller(): Promise<void> {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(
      `${process.env.HOME}/.platformio/penv/bin/platformio run -d ${PATHSTEM_MICROCONTROLLER} -t upload`,
      [1]
    )
      .catch(err => {
        writeFileSync('logs/updateMicrocontroller.log', err);
        rej(
          new Error(
            'Failed to update the microcontroller software. See logs/updateMicrocontroller.log'
          )
        );
      })
      .then(log1 => {
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
    let log = '> ' + command + '\n';
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
        log += '> Process exited with code ' + code;
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

const dateFormat = (d: Date) => (`${d.getFullYear()}-${d.getMonth()+1}-${d.getDate()}_${d.getHours()}-${d.getMinutes()}-${d.getSeconds()}`);

type CameraCaptureOptions = {
  width?: number;
  height?: number;
  // TODO: Add more options
};

const PATHSTEM_IMAGES = './logs/';
const CAMERATIMEOUT = 3000;

// Returns a path to the JPEG image.
export function cameraCapture(options?: CameraCaptureOptions): Promise<string> {
  return new Promise<string>((res, rej) => {
    const p = `${PATHSTEM_IMAGES}-${dateFormat(new Date())}`;
    execute(`rpicam-still -o ${p}.jpg -n --timeout ${CAMERATIMEOUT} --autofocus`)
      .catch(err => {
        rej(err);
      })
      .then(() => {
        res(`${p}.jpg`);
      });
  });
}

// Helper: IPv4 Address Lookup (Fallback to 'localhost')
export const ipv4Lookup = async (): Promise<string> => {
  const h = hostname();
  if(!h) return 'localhost';
  return await new Promise((res) => {
    lookup(h, { family: 4, all: true }, (err, addrs) => {
      res((err || !addrs || !(addrs.length)) ? 'localhost' : addrs.find((a) => (a.address !== '127.0.0.1')).address);
    });
  });
};