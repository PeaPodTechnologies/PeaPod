import { hostname } from 'os';
import { lookup } from 'dns';

import { createServer } from 'http';
import { Server } from 'socket.io';

import next from 'next';
import nextConfig from './next.config';

import { findSerialPort, MicroController, CONTROLLER_REVISION, Controller, SimulatedController, SimulatorConfig } from './api/controller';
// import { pushDebugMessage, pushDebugMessages } from './api/firebase';
import ui, { _logRedirect, _errRedirect } from './api/ui';
import {DebugJsonSerialportError} from './api/errors';
import { appendFileSync, readFileSync } from 'fs';
import yargs from 'yargs';
import { hideBin } from 'yargs/helpers';
import { initializeApp } from 'firebase/app';
import { getAuth } from 'firebase/auth';
import { DeviceFlowUI } from '@peapodtech/firebasedeviceflow';
import { DebugJsonInstruction } from './api/types';
import checkbox from '@inquirer/checkbox';
import { cameraCapture, ipv4Lookup, updateMicrocontroller } from './api/utils';
import { pushDebugMessages } from './api/firebase';

enum PublishingMode {
	FIREBASE = 'Firebase (CloudPonics)',
	LOCAL = 'Local (Filesystem)',
  FRONTEND = 'Dashboard (Frontend)'
}

// Handle Command-Line Arguments
const argv = await yargs(hideBin(process.argv))
  .option('simulator', {
    alias: 's',
    type: 'boolean',
    describe: 'Run against the simulated controller'
  })
  .option('port', {
    alias: 'p',
    type: 'number',
    default: 3001,
    describe: 'HTTP port'
  })
  .option('host', {
    alias: 'h',
    type: 'string',
    default: 'localhost',
    describe: 'Hostname to bind'
  })
  .option('serialport', {
    alias: 'd',
    type: 'string',
    describe: 'SerialPort device path (overrides auto-detection)',
    default: process.env.SERIALPORT || undefined
  })
  .help().parse();

type Linker = {
  instruction: DebugJsonInstruction, 
  key: string,
  cast: 'string' | 'number' | 'boolean', 
  eval: string
};

// Redirect console.log/.error calls to ui.log/.err
console.log = _logRedirect;
console.error = _errRedirect;

// Options: SerialPort Message Logging
// const _msg_print_delta = 1;
let _msg_print_count = 0;

const schedule: { [key: string]: NodeJS.Timeout } = {}; // { [key: instruction string]: NodeJS.Timeout} - Scheduled tasks
const linker: {
  [key: string]: Linker[]
} = {};
// var linker = {encoder: [{key: 's', cast: 'number', eval: 'value * (-15)', instruction: {type: 'command', data: {fqa: 8183, b: 3}}}]}; // { [key: `telemetry key`]: { `instruction`: object, `key`: string, `cast`: 'string' | 'number' | 'boolean' }[] } - On `telemetry key`, send {...`instruction`, [`key`]: telemetry value as `cast`}

const linkerTyper = (value, cast)  => {
  if(cast === 'string') return String(value);
  if(cast === 'number') return Number(value);
  if(cast === 'boolean') return Boolean(value);
  return undefined;
};

const linkerChecker = (value, cast) => {
  if(cast === 'string') return typeof value === 'string';
  if(cast === 'number') return typeof value === 'number' && !isNaN(value);
  if(cast === 'boolean') return typeof value === 'boolean';
  return false;
};

const linkerEvaluator = (value, evalStr) => {
  if(!evalStr || typeof evalStr !== 'string') return value;
  try {
    return eval(evalStr.replace(/value/g, value));
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  } catch (err) {
    return undefined;
  }
};

const SIMULATOR_CONFIG: SimulatorConfig = {
  'temperature': { min: 20, max: 30, interval: 1000 },
  'humidity': { min: 30, max: 70, interval: 1500 },
};

const DEFAULT_SERIALPORT_STEM = '/dev/ttyACM'; // Linux default
// const DEFAULT_SERIALPORT_STEM = '/dev/ttyS';
// const DEFAULT_SERIALPORT_STEM = 'usbserial';

const findController = (simulator?: boolean): Promise<Controller> => {
  if(simulator) return Promise.resolve(new SimulatedController(SIMULATOR_CONFIG));
  ui.start('SerialPort: Scanning...');
  return new Promise((res, rej) => {
    findSerialPort(process.env.SERIALPORT ?? DEFAULT_SERIALPORT_STEM).then((ports) => {
      if(ports.length === 0) { rej(new DebugJsonSerialportError('No SerialPorts Found!')); }

      ui.succeed(`SerialPorts[${ports.length}]`);
      let resolved = false;
      ports.forEach((ser, i) => {
        console.info(`SerialPort[${i}]: ${ser}`);
        if(process.env.SERIALPORT && resolved === false) { res(new MicroController(ser)); resolved = true; }
        else if(!process.env.SERIALPORT && i === 0) res(new MicroController(ser)); // First one if none specified
      });
    });
  });
};

let io = undefined;

// ===== MAIN =====
(async () => {
  // 4. Select & Prepare Publishing Modes
  const pms = await checkbox({
    message: 'Select publishing modes:',
    choices: Object.entries(PublishingMode).map(pm => ({
      // Object.entries casts `PublishingMode` values to `string`. This recasts them to PublishingMode enum values (TypeScript magic)
      value: PublishingMode[pm[0] as keyof typeof PublishingMode],
      name: pm[1]
    })),
  });
  
  // 4A. Firebase w/ Auth
  if(pms.includes(PublishingMode.FIREBASE)) {
    const firebaseApp = initializeApp({
      apiKey: process.env.FIREBASE_APIKEY,
      authDomain: process.env.FIREBASE_AUTHDOMAIN,
      projectId: process.env.FIREBASE_PROJECTID,
      storageBucket: process.env.FIREBASE_STORAGEBUCKET,
      messagingSenderId: process.env.FIREBASE_MESSAGINGSENDERID,
      appId: process.env.FIREBASE_APPID,
      measurementId: process.env.FIREBASE_MEASUREMENTID
    });

    getAuth(firebaseApp);

    const deviceFlowUI = new DeviceFlowUI(firebaseApp, {
      Google : {
        clientid: process.env.GOOGLE_CLIENTID,
        clientsecret: process.env.GOOGLE_CLIENTSECRET,
        scopes: process.env.GOOGLE_SCOPES?.split(' ')
      },
      GitHub : {
        clientid: process.env.GITHUB_CLIENTID,
        clientsecret: process.env.GITHUB_CLIENTSECRET,
        scopes: process.env.GITHUB_SCOPES?.split(' ')
      }
    });

    await deviceFlowUI.signIn().then((user) => {
      ui.succeed(`Firebase Auth: Signed in as ${user.email}!`);
    });
  }

  if(pms.includes(PublishingMode.FRONTEND)) {
    // 1. IPv4 Lookup
    ui.start('IPv4: Lookup...');
    const host = await ipv4Lookup();
    ui.succeed(`IPv4: ${host}`);

    const hostname =  host || argv.host || 'localhost';
    const port = argv.port || 3001;

    // 2. Next.JS App and HTTP Server
    ui.start('Next.JS: Preparing...');
    const app = next({ dev: (process.env.NODE_ENV !== 'production'), hostname, port, conf: nextConfig });
    const handler = app.getRequestHandler();
    const server = createServer((req, res) => {
      handler(req, res);
    });
    await app.prepare();
    ui.succeed('Next.JS: Ready!');

    await new Promise<void>(r => server.listen(port, host, () => {
      ui.info(`HTTP Server Listening: http://${host}:${port}/`);
      r();
    }));

    // 3. WebSockets
    io = new Server(server, {
      cors: {
        origin: '*',
      },
    });
  }

  // 5. SerialPort DebugJson Controller
  const controller = await findController(argv.simulator || false);
  controller.start((messages) =>  {
    
    // Message Handling
    messages.forEach((msg) => {

      // 1. Emit to WebSocket Clients
      if(pms.includes(PublishingMode.FRONTEND) && !!io) io.emit('microcontroller', msg);

      // 2. (aka 4B.) Local Filesystem Logging
      if(pms.includes(PublishingMode.LOCAL)) appendFileSync(`logs/${argv.simulator ? 'simulator_' : ''}${(new Date()).toISOString().split('T')[0]}.txt`, JSON.stringify(msg) + '\n');

      // LINKER HANDLER
      if(msg.data) {
        Object.keys(linker).forEach((key) => {
          linker[key].forEach((item) => {
            if(msg.data[key] !== undefined) {
              const evalStr = item.eval;
              const cast = item.cast;
              const value = linkerTyper(linkerEvaluator(msg.data[key], evalStr), cast);
              if(!linkerChecker(value, cast)) {
                ui.fail(`LINKER ERROR: Invalid Type ${cast} for ${key} -> ${value} (${typeof value})`);
                return; // Next key
              }
              const instruction = { ...item.instruction, data: { ...item.instruction.data, [item.key]: value } };
              ui.info(`LINKER: ${key}: ${msg.data[key]} -> '${evalStr}' as ${cast} = ${value} -> ${JSON.stringify(instruction)}`);
              try {
                controller.write(instruction);
              } catch (err) {
                ui.fail(`LINKER ERROR: ${err}`);
                if(pms.includes(PublishingMode.FRONTEND) && !!io) io.emit('server', {type: 'error', msg: `Linker TX Error: ${err}`});
                return; // Next key
              }
            }
          });
        });
      }
    });
              
    // 3. (aka 4Aii.) Push to Firebase Realtime Database
    if(pms.includes(PublishingMode.FIREBASE)) pushDebugMessages(messages, 'microcontroller');
    
    // if (_msg_print_count % _msg_print_delta === 0) {
    ui.info(`CONTROLLER JSON[${_msg_print_count}]: ${JSON.stringify(messages[0])}`);
    // }
    _msg_print_count++;

    // const timestamp = messages.reduce((max, msg) => ((msg['timestamp'] ? (msg['timestamp'] > max ? msg['timestamp'] : max) : max)), 0);
    // if(timestamp) microcontroller.write({
    //   type: 'command',
    //   data: {
    //     'fqa': 8183,
    //     's': Math.floor(timestamp / 1000),
    //     'b': 2
    //   }
    // });

  }).catch((err) => {
    if(!!io) io.emit('server', {type: 'error', msg: `Lost Controller: ${err}`});
    ui.fail(err);
    // io.off('connection', handleSocketConnection);
    // microcontroller.reset();
  }).then(() => {
    ui.succeed('PeaPodOS: Ready!');

    if(pms.includes(PublishingMode.FRONTEND) && !!io) {
      // 6. Socket.IO Connection Handler
      io.on('connection', (socket) => {
        ui.log('Socket.IO ++');

        setTimeout(() => {
          ui.info(`Socket.IO: ${socket.id}`);
          socket.emit('json', { type: 'info', msg: 'Server Start', _socket: 'server' }); // Open subsocket 'server'
          socket.emit('json', { type: 'revision', msg: 'Microcontroller Revision Match', data: CONTROLLER_REVISION, _socket: 'microcontroller' }); // Open subsocket 'microcontroller'
        }, 1000);

        socket.on('disconnect', () => {
          ui.log('Socket.IO --');
        });

        // SERIAL INPUT SOCKET HANDLERS

        socket.on('serialinput', (data: DebugJsonInstruction, callback: (error?: {error: string}) => void) => {
          ui.info(`CONTROLLER INPUT: ${JSON.stringify(data)}`);
          if(!data || !data.type || !data.data) {
            ui.fail('CONTROLLER INPUT ERROR: Invalid Data');
            socket.emit('server', {type: 'error', msg: 'Controller Input Error: Invalid Data'});
            callback({error: 'Invalid Serial Data'});
            return;
          }
          try {
            controller.write({type: data.type, data: data.data});
          } catch (err) {
            ui.fail(`CONTROLLER INPUT ERROR: ${err}`);

            socket.emit('server', {type: 'error', msg: `Controller TX Error: ${err}`});
            callback({error: `Controller TX Error: ${err}`});
            return;
          }
          callback();
        });

        // SCHEDULER SOCKET HANDLERS

        socket.on('scheduler-post', (data: { interval: number, instruction: DebugJsonInstruction }, callback: (error?: {error: string}) => void) => {
          ui.info(`SCHEDULER POST: ${JSON.stringify(data)}`);
          if(!data || !data.interval || !data.instruction || !data.instruction.type || !data.instruction.data) {
            ui.fail('SCHEDULER POST ERROR: Invalid Data');
            socket.emit('server', {type: 'error', msg: 'Scheduler Post Error: Invalid Data'});
            callback({error: 'Invalid Scheduler Data'});
            return;
          }
          const schedulerLabel = JSON.stringify(data.instruction);
          if(schedule[schedulerLabel]) {
            ui.info(`CONTROLLER INPUT SCHEDULE CLEARED: ${schedulerLabel}`);
            clearInterval(schedule[schedulerLabel]);
            delete schedule[schedulerLabel];
          }
          if(typeof data.interval === 'number' && data.interval >= 100) {
            ui.info(`CONTROLLER INPUT SCHEDULE: ${schedulerLabel} @${data.interval}ms`);
            schedule[schedulerLabel] = setInterval(() => {
              ui.info(`CONTROLLER INPUT SCHEDULE: ${schedulerLabel} @${data.interval}ms`);
              try {
                controller.write({type: data.instruction.type, data: data.instruction.data});
              } catch (err) {
                ui.fail(`CONTROLLER INPUT ERROR: ${err}`);
                socket.emit('server', {type: 'error', msg: `Controller TX Error: ${err}`});
                clearInterval(schedule[schedulerLabel]);
                delete schedule[schedulerLabel];

                callback({error: `Controller TX Error: ${err}`});
                return;
              }
            }, data.interval);
            callback();
          }
        });

        socket.on('scheduler-get', (data: unknown, callback: (keys: string[]) => void) => {
          ui.info('SCHEDULER GET');
          callback(Object.keys(schedule));
        });

        socket.on('scheduler-clear', (key: string, callback: (error?: {error: string}) => void) => {
          ui.info(`SCHEDULER CLEAR: ${key}`);
          if(!schedule[key]) {
            ui.fail(`SCHEDULER CLEAR ERROR: No such key "${key}"`);
            socket.emit('server', {type: 'error', msg: `Scheduler Clear Error: No such key "${key}"`});
            callback({error: `No such key "${key}"`});
            return;
          }
          clearInterval(schedule[key]);
          delete schedule[key];
          callback();
        });

        // LINKER SOCKET HANDLERS

        socket.on('linker-post', (data: Linker & {label: string}, callback: (error?: {error: string}) => void) => {
          ui.info(`LINKER POST: ${JSON.stringify(data)}`);
          if(!data || !data.label || !data.key || !data.cast || !data.instruction || !data.eval || !data.instruction.type || !data.instruction.data) {
            ui.fail('LINKER POST ERROR: Invalid Data');
            socket.emit('server', {type: 'error', msg: 'Linker Post Error: Invalid Data'});
            callback({error: 'Invalid Linker Data'});
            return;
          }
          if(!linker[data.label]) linker[data.label] = [];
          linker[data.label].push({
            instruction: {
              type: data.instruction.type,
              data: {
                ...data.instruction.data,
                [data.key]: undefined
              } 
            }, 
            key: data.key, 
            eval: data.eval, 
            cast: data.cast
          });
          callback();
        });

        socket.on('linker-get', (data: unknown, callback: (linker: {[key: string]: Linker[]}) => void) => {
          ui.info('LINKER GET');
          callback(linker);
        });

        socket.on('linker-clear', (label: string, instruction: DebugJsonInstruction, callback: (error?: {error: string}) => void) => {
          ui.info(`LINKER CLEAR: ${label} ${JSON.stringify(instruction)}`);
          if(linker[label]) {
            linker[label] = linker[label].filter((item) => {
              return !(item.instruction.type === instruction.type && JSON.stringify(item.instruction.data) === JSON.stringify(instruction.data));
            });
          }
          callback();
        });

        socket.on('firmware', (_: unknown, callback: (error?: {error: string}) => void) => {
          ui.start('FIRMWARE FLASH');
          if(argv.simulator) {callback({error: 'Simulator!'}); return;}
          controller.stop();
          updateMicrocontroller().then(() => {
            ui.succeed('FIRMWARE FLASH SUCCESSFUL');
            callback();
          }).catch((err) => {
            ui.fail(`FIRMWARE FLASH ERROR: ${err}`);
            socket.emit('server', {type: 'error', msg: `Firmware Flash Error: ${err}`});
            callback({error: `Firmware Flash Error: ${err}`});
          });
        });

        socket.on('camera', async (_: unknown, callback: (response: {error?: string, mime?: string, blob?: Buffer}) => void) => {
          try {
            ui.start('CAMERA CAPTURE');
            if(argv.simulator) {callback({mime: 'image/jpeg', blob: readFileSync('sample.jpg')}); return;}
            controller.write({type: 'config', data: {enable_camera: true}});
            const path = await cameraCapture();
            const buf = readFileSync(path);
            ui.succeed('CAMERA CAPTURE SUCCESSFUL: ' + path);
            controller.write({type: 'config', data: {enable_camera: false}});
            callback({mime: 'image/jpeg', blob: buf});
          } catch (err) {
            callback({error: `Camera Capture Error: ${err}`});
          }
        });
      });
    }
  });
})();
