import * as dns from 'dns';
import { existsSync, mkdirSync, writeFileSync } from 'fs';
import dotenv from 'dotenv';
import { SerialPort } from 'serialport';
import axios from 'axios';
import { spawn } from 'child_process';

/**
* Checks the internet connectivity.
* @param {number} timeout Timeout in milliseconds.
* @param {dns.Resolver} resolver DNS Resolver object.
*/
export const checkInternet = (timeout : number = 5000, url: string = 'www.google.com') : Promise<boolean> => {
  const resolver = new dns.Resolver({timeout});
  return new Promise<boolean>(ret=>{
    resolver.resolve(url, err=>{
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
* Usage: `await sleep(x)` or `sleep(x).then(...)`
* @param millis Number of milliseconds to sleep.
* @returns 
*/
export const sleep = (millis : number) => {
  return new Promise(resolve => {
    setTimeout(resolve, millis);
  });
};

/**
* Load a `.env`-style file to `process.env`.
* 
* Throws an error if the file does not exist.
*/
export const loadDotenv = (path: string = '.env') => {
  // Check for file
  if(existsSync(path)){
    const config = dotenv.config({path});
    if (config.error) {
      throw config.error
    }
  } else {
    throw new Error('Environment variable file not found.');
  }
};

/**
* Find the path to the serial port of a given device. 
* 
* Returns the first port matching the given parameters.
* @param manufacturer Device manufacturer name (or a substring thereof).
* 
* TODO: More search fields?
* @returns Path to port.
*/
export const findSerialPath = async (manufacturer: string = "arduino") : Promise<string> => {
  // List all ports
  let ports = await SerialPort.list();
  // Search ports and find one that matches our parameters
  for(const port of ports){
    if(port.manufacturer?.toLowerCase().includes(manufacturer)){
      return port.path;
    }
  }
  throw new Error('Arduino not found! Check wiring.');
};

/**
* Converts a set of strings to a tuple. Useful TypeScript nonsense.
* @param data 
* @returns 
*/
export const stringsToTuple = <T extends [string] | string[]>(...data: T): T => {
  return data;
};

export const fetchServerCert = async (): Promise<string> => {
  return (await axios.get('https://pki.goog/roots.pem')).data as string;
}

export const checkArduino = async (processor: string = 'm328p') => {
  // Create log folder
  if (!existsSync('logs/')) {
    mkdirSync('logs/', { recursive: true });
  }
  execute(`avrdude -p ${processor} -C ~/avrdude_gpio.conf -c peapod -v`, [1]).catch(err => {
    writeFileSync('logs/arduinoComms.log', err);
    throw new Error(`Failed to communicate with the Arduino. See logs/arduinoComms.log`);
  }).then(log1 => {
    writeFileSync('logs/arduinoComms.log', log1);
  });
}

export const updateArduino = async (sketch: string = 'PeaPodOS-Arduino', fqbn: string = 'arduino:avr:nano', processor: string = 'm328p'): Promise<void> => {
  // Create log folder
  if (!existsSync('logs/')) {
    mkdirSync('logs/', { recursive: true });
  }
  execute(`arduino-cli compile -b ${fqbn} ${sketch} -e`, [1])
  .catch(err => {
    writeFileSync('logs/arduinoCompile.log', err);
    throw new Error(`Failed to compile '${sketch}'. See logs/arduinoCompile.log`);
  }).then(log1 => {
    writeFileSync('logs/arduinoCompile.log', log1);
    // execute('arduino-cli', ['upload', '-p', serialport, '-b', fqbn, script], { sudo: true, failureCodes: [1] })
    execute(`sudo avrdude -p ${processor} -C ${process.env.HOME}/avrdude_gpio.conf -c peapod -v -U flash:w:${sketch}/build/${fqbn.replace(/:/g, '.')}/${sketch}.ino.hex:i`, [1])
    .catch(err => {
      writeFileSync('logs/arduinoUpload.log', err);
      throw new Error(`Failed to flash the '${sketch}' binary to the Arduino. See logs/arduinoUpload.log`);
    }).then(log2 => {
      writeFileSync('logs/arduinoUpload.log', log2);
    });
  });
}

/**
 * General purpose command execution and logging. No `sudo` support.
 */
export const execute = (command: string, failureCodes: number[] = []): Promise<string> => {
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
      eprocess.kill();
      log += error.message;
      rej(log);
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