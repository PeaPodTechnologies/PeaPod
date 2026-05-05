import { createServer } from 'http';
import { Server } from 'socket.io';

import next from 'next';
import nextConfig from './next.config';

import { findSerialPort, MicroController, CONTROLLER_REVISION, Controller, SimulatedController, SimulatorConfig } from './api/controller';
// import { pushDebugMessage, pushDebugMessages } from './api/firebase';
import ui, { _logRedirect, _errRedirect } from './api/ui';
import {DebugJsonSerialportError} from './api/errors';
import { appendFileSync, readFileSync, writeFileSync } from 'fs';
import yargs from 'yargs';
import { hideBin } from 'yargs/helpers';
import { initializeApp } from 'firebase/app';
import { getAuth } from 'firebase/auth';
import { DeviceFlowUI } from '@peapodtech/firebasedeviceflow';
import { DebugJsonInstruction } from './api/types';
import checkbox from '@inquirer/checkbox';
import { cameraCapture, ipv4Lookup, updateMicrocontroller } from './api/utils';
import { pushDebugMessages } from './api/firebase';
import loadDotEnv from './api/env';
import { SchedulerEntry } from './api/types';

loadDotEnv();

const SCHEDULER_INTERVAL = 5000; // ms - How often to check the scheduler for pending tasks

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
    describe: 'HTTP port'
  })
  .option('host', {
    alias: 'h',
    type: 'string',
    describe: 'Hostname to bind'
  })
  .option('serialport', {
    alias: 'd',
    type: 'string',
    describe: 'SerialPort device path (overrides auto-detection)',
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

const intervals: { [key: string]: NodeJS.Timeout } = {}; // { [key: instruction string]: NodeJS.Timeout} - Scheduled tasks
const scheduler : {entry: SchedulerEntry & ({entry: 'event', executed: boolean} | {entry: 'interval', last: number}), interval: NodeJS.Timeout, instruction: DebugJsonInstruction}[] = []; // Scheduler Entries
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

let io: Server | undefined = undefined;
let schedulerInterval: NodeJS.Timeout | undefined = undefined;

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
    // ui.start('IPv4: Lookup...');
    // const host = await ipv4Lookup();
    // ui.succeed(`IPv4: ${host}`);

    // const hostname = argv.host ?? host ?? 'localhost';
    const hostname = argv.host ?? 'localhost';
    const port = argv.port ?? 3005;

    // 2. Next.JS App and HTTP Server
    ui.start(`Next.JS: Preparing${process.env.NODE_ENV === 'production' ? ' (Production)' : ' (Development)'}...`);
    const app = next({ dev: (process.env.NODE_ENV !== 'production'), hostname, port, conf: nextConfig });
    await app.prepare();
    const handler = app.getRequestHandler();
    const server = createServer((req, res) => {
      // console.log('[HTTP]', req.method, req.url);
      handler(req, res);
    });
    ui.succeed('Next.JS: Ready!');

    await new Promise<void>(r => server.listen(port, hostname, () => {
      ui.log(`HTTP Server Listening: http://${hostname}:${port}/`);
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
              ui.log(`LINKER: ${key}: ${msg.data[key]} -> '${evalStr}' as ${cast} = ${value} -> ${JSON.stringify(instruction)}`);
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
    ui.log(`CONTROLLER JSON[${_msg_print_count}]: ${JSON.stringify(messages[0])}`);
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

        socket.emit('scheduler')

        setTimeout(() => {
          ui.log(`Socket.IO: ${socket.id}`);
          socket.emit('json', { type: 'info', msg: 'Server Start', _socket: 'server' }); // Open subsocket 'server'
          socket.emit('json', { type: 'info', msg: 'Scheduler Start', _socket: 'scheduler' }); // Open subsocket 'scheduler'
          socket.emit('json', { type: 'revision', msg: 'Microcontroller Revision Match', data: CONTROLLER_REVISION, _socket: 'microcontroller' }); // Open subsocket 'microcontroller'
        }, 1000);

        socket.on('disconnect', () => {
          ui.log('Socket.IO --');
        });

        // SERIAL INPUT SOCKET HANDLERS

        socket.on('serialinput', (data: DebugJsonInstruction, callback: (error?: {error: string}) => void) => {
          ui.log(`CONTROLLER INPUT: ${JSON.stringify(data)}`);
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

        // INTERVALS SOCKET HANDLERS

        socket.on('intervals-post', (data: { interval: number, instruction: DebugJsonInstruction }, callback: (error?: {error: string}) => void) => {
          ui.log(`INTERVALS POST: ${JSON.stringify(data)}`);
          if(!data || !data.interval || !data.instruction || !data.instruction.type || !data.instruction.data) {
            ui.fail('INTERVALS POST ERROR: Invalid Data; Missing Fields: ' + Object.keys(data).filter(k => !data[k as keyof typeof data]).join(', '));
            socket.emit('server', {type: 'error', msg: 'Intervals Post Error: Invalid Data'});
            callback({error: 'Invalid Intervals Data'});
            return;
          }
          const schedulerLabel = JSON.stringify(data.instruction);
          if(intervals[schedulerLabel]) {
            ui.log(`CONTROLLER INPUT INTERVAL CLEARED: ${schedulerLabel}`);
            clearInterval(intervals[schedulerLabel]);
            delete intervals[schedulerLabel];
          }
          if(typeof data.interval === 'number' && data.interval >= 100) {
            ui.log(`CONTROLLER INPUT INTERVAL: ${schedulerLabel} @${data.interval}ms`);
            intervals[schedulerLabel] = setInterval(() => {
              ui.log(`CONTROLLER INPUT INTERVAL: ${schedulerLabel} @${data.interval}ms`);
              try {
                controller.write({type: data.instruction.type, data: data.instruction.data});
              } catch (err) {
                ui.fail(`CONTROLLER INPUT ERROR: ${err}`);
                socket.emit('server', {type: 'error', msg: `Controller TX Error: ${err}`});
                clearInterval(intervals[schedulerLabel]);
                delete intervals[schedulerLabel];

                callback({error: `Controller TX Error: ${err}`});
                return;
              }
            }, data.interval);
            callback();
          }
        });

        socket.on('intervals-get', (data: unknown, callback: (keys: string[]) => void) => {
          ui.log('INTERVALS GET');
          callback(Object.keys(intervals));
        });

        socket.on('intervals-clear', (key: string, callback: (error?: {error: string}) => void) => {
          ui.log(`INTERVALS CLEAR: ${key}`);
          if(!intervals[key]) {
            ui.fail(`INTERVALS CLEAR ERROR: No such key "${key}"`);
            socket.emit('server', {type: 'error', msg: `Intervals Clear Error: No such key "${key}"`});
            callback({error: `No such key "${key}"`});
            return;
          }
          clearInterval(intervals[key]);
          delete intervals[key];
          callback();
        });

        // LINKER SOCKET HANDLERS

        socket.on('linker-post', (data: Linker & {label: string}, callback: (error?: {error: string}) => void) => {
          ui.log(`LINKER POST: ${JSON.stringify(data)}`);
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
          ui.log('LINKER GET');
          callback(linker);
        });

        socket.on('linker-clear', (label: string, instruction: DebugJsonInstruction, callback: (error?: {error: string}) => void) => {
          ui.log(`LINKER CLEAR: ${label} ${JSON.stringify(instruction)}`);
          if(linker[label]) {
            linker[label] = linker[label].filter((item) => {
              return !(item.instruction.type === instruction.type && JSON.stringify(item.instruction.data) === JSON.stringify(instruction.data));
            });
          }
          callback();
        });

        // FIRMWARE & CAMERA HANDLERS

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
          if(argv.simulator) { callback({mime: 'image/jpeg', blob: readFileSync('sample.jpg')}); return; }
          controller.write({type: 'config', data: { enable_camera: true }});
          try {
            ui.start('CAMERA CAPTURE');
            const path = await cameraCapture();
            const buf = readFileSync(path);
            ui.succeed('CAMERA CAPTURE SUCCESSFUL: ' + path);
            callback({mime: 'image/jpeg', blob: buf});
          } catch (err) {
            ui.fail(`CAMERA CAPTURE ERROR: ${err}`);
            socket.emit('server', {type: 'error', msg: `Camera Capture Error: ${err}`});
            callback({error: `Camera Capture Error: ${err}`});
          } finally {
            controller.write({type: 'config', data: { enable_camera: false }});
          }
        });

        // SCHEDULER

        const deleteScheduledById = (id: string) => {
          const index = scheduler.findIndex((item) => item.entry.id === id);
          if(index !== -1) {
            const item = scheduler[index];
            if(item.interval) clearInterval(item.interval);
            scheduler.splice(index, 1);
          }
        };

        const schedulerEmit = () => {
          scheduler.forEach((item) => {
            socket.emit('scheduler', item.entry);
          });
          console.log(`SCHEDULER EMIT (${scheduler.length} entries)`);
        };

        socket.on('scheduler-save', (callback: (error?: {error: string}) => void) => {
          ui.log(`SCHEDULER SAVE ${JSON.stringify(scheduler.map((item) => item.entry))} => scheduler.json`);
          try {
            const data = JSON.stringify(scheduler.map((item) => item.entry));
            writeFileSync('scheduler.json', data, 'utf-8');
            ui.succeed('SCHEDULER SAVE SUCCESSFUL');
            callback();
          } catch (err) {
            ui.fail(`SCHEDULER SAVE ERROR: ${err}`);
            socket.emit('server', {type: 'error', msg: `Scheduler Save Error: ${err}`});
            callback({error: `Scheduler Save Error: ${err}`});
          }
        });

        const addSchedulerEntry = (data: SchedulerEntry) => {
          scheduler.push({entry: {...data, date: new Date(data.date), endDate: data.entry === 'interval' ? new Date(data.endDate) : undefined, executed: data.entry === 'event' ? false : undefined, last: data.entry === 'interval' ? Date.now() : undefined}, instruction: data.instruction, interval: data.entry === 'interval' ? setTimeout(() => {
            try{
              ui.log(`SCHEDULER INTERVAL START: ${JSON.stringify(data)}`);
              controller.write(data.instruction);
              setInterval(() => {
                try {
                  ui.log(`SCHEDULER INTERVAL TRIGGER: ${JSON.stringify(data)}`);
                  controller.write(data.instruction);
                } catch (err) {
                  ui.fail(`SCHEDULER INTERVAL ERROR: ${err}`);
                  socket.emit('server', {type: 'error', msg: `Scheduler Interval TX Error: ${err}`});
                  deleteScheduledById(data.id);
                }
              }, data.interval);
            } catch (err) {
              ui.fail(`SCHEDULER INTERVAL ERROR: ${err}`);
              socket.emit('server', {type: 'error', msg: `Scheduler Interval TX Error: ${err}`});
              deleteScheduledById(data.id);
            }
          }, new Date(data.date).getTime() - Date.now()) : data.entry === 'event' ? setTimeout(() => {
            try {
              ui.log(`SCHEDULER EVENT TRIGGER: ${JSON.stringify(data)}`);
              controller.write(data.instruction);
            }
            catch (err) {
              ui.fail(`SCHEDULER EVENT ERROR: ${err}`);
              socket.emit('server', {type: 'error', msg: `Scheduler Event TX Error: ${err}`});
            }
            deleteScheduledById(data.id);
          }, new Date(data.date).getTime() - Date.now()) : undefined});
        };

        socket.on('scheduler-load', (callback: (error?: {error: string}) => void) => {
          // Overwrite entire scheduler with loaded data from scheduler.json
          ui.log('SCHEDULER LOAD scheduler.json');
          try {
            const data = readFileSync('scheduler.json', 'utf-8');
            const entries = JSON.parse(data);
            scheduler.length = 0;
            entries.forEach((entry: SchedulerEntry) => {
              addSchedulerEntry(entry);
            });
            ui.succeed('SCHEDULER LOAD SUCCESSFUL');
            callback();
            schedulerEmit();
          } catch (err) {
            ui.fail(`SCHEDULER LOAD ERROR: ${err}`);
            socket.emit('server', {type: 'error', msg: `Scheduler Load Error: ${err}`});
            callback({error: `Scheduler Load Error: ${err}`});
          }
        });

        socket.on('scheduler-post', (data: SchedulerEntry, callback: (error?: {error: string}) => void) => {
          ui.log(`SCHEDULER CREATE ${data.entry === 'interval' ? 'INTERVAL' : 'EVENT'}: ${JSON.stringify(data)}`);
          if(!data || !data.id || !data.entry || !data.instruction || !data.instruction.type || !data.instruction.data) {
            ui.fail(`SCHEDULER CREATE ERROR: Invalid Data ${JSON.stringify(data)}`);
            socket.emit('server', {type: 'error', msg: 'Scheduler Create Error: Invalid Data'});
            callback({error: 'Invalid Scheduler Data'});
            return;
          }

          const idx = scheduler.findIndex((item) => item.entry.id === data.id);
          if(idx !== -1) {
            ui.fail(`SCHEDULER CREATE: ID "${data.id}" already exists; replacing...`);
            deleteScheduledById(data.id);
          }

          addSchedulerEntry(data);
          callback();
          schedulerEmit();
        });

        socket.on('scheduler-delete', (id: string, callback: (error?: {error: string}) => void) => {
          ui.log(`SCHEDULER DELETE: ${id}`);
          const index = scheduler.findIndex((item) => item.entry.id === id);
          if(index === -1) {
            ui.fail(`SCHEDULER DELETE ERROR: No such id "${id}"`);
            socket.emit('server', {type: 'error', msg: `Scheduler Delete Error: No such id "${id}"`});
            callback({error: `No such id "${id}"`});
            return;
          }
          const item = scheduler[index];
          if(item.interval) clearInterval(item.interval);
          scheduler.splice(index, 1);
          callback();
          schedulerEmit();
        });

        schedulerInterval = setInterval(schedulerEmit, SCHEDULER_INTERVAL);
      });
    }
  });
})();
