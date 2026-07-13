import { SerialPort, ReadlineParser } from 'serialport';
import { updateMicrocontroller } from './api/utils';
import main from './server';
import select from '@inquirer/select';

const BAUDRATE = 115200;
const TIMEOUT_MS = 5000;

enum Launch {
  SERIAL = 'Test Serial',
  FLASH = 'Flash Firmware',
  SERVER = 'Launch Server',
};

// eslint-disable-next-line prefer-const
let serialport: SerialPort | undefined;

function restoreTerminal() {
  try {
    if (process.stdin.isTTY) {
      process.stdin.setRawMode(false);
    }
    process.stdin.pause();
  } catch {}
}

function exitCleanly(code = 0) {
  restoreTerminal();

  if (serialport?.isOpen) {
    serialport.close(() => process.exit(code));
  } else {
    process.exit(code);
  }
}

process.on('SIGINT', () => {
  console.log('\nCaught Ctrl-C');
  exitCleanly(0);
});

process.on('SIGTERM', () => {
  console.log('\nCaught SIGTERM');
  exitCleanly(0);
});

async function selectSerial(): Promise<string> {
  const ports = await SerialPort.list();

  console.log('Available Serial Ports:');
  ports.forEach((port) => {
    console.log(`- ${port.path} (${port.manufacturer || 'Unknown Manufacturer'})`);
  });

  const serial = await select({
    message: 'Select Serial Port:',
    choices: ports.map((ser, i) => ({
      value: ser.path,
      name: `${i}: ${ser.path}`,
    })),
  });

  restoreTerminal();
  
  return serial;
}

async function launchSerial() {
  const serial = await selectSerial();
  console.log(`Selected Serial Port: ${serial}`);

  serialport = new SerialPort({
    path: serial,
    baudRate: BAUDRATE,
    autoOpen: false,
  });
  
  const parser = serialport.pipe(new ReadlineParser({ delimiter: '\n' }));
  
  parser.on('data', (line) => {
    console.log(JSON.stringify(line));
  });
  
  serialport.on('open', () => {
    console.log('OPEN EVENT');
  });
  
  serialport.on('error', (err) => {
    console.error('ERROR EVENT:', err);
  });
  
  serialport.on('close', () => {
    console.log('CLOSE EVENT');
  });
  
  console.log('About to open serial port');
  
  const timer = setTimeout(() => {
    console.error(`serialport.open() callback did not fire after ${TIMEOUT_MS}ms`, {
      isOpen: serialport?.isOpen,
      readable: serialport?.readable,
      writable: serialport?.writable,
    });
  
    exitCleanly(1);
  }, TIMEOUT_MS);
  
  serialport.open((err) => {
    clearTimeout(timer);
  
    if (err) {
      console.error('Error opening serial port:', err.message);
      exitCleanly(1);
      return;
    }
  
    console.log(`Serial port ${serial} opened at baud rate ${BAUDRATE}`);
  });
}

async function launchFlash() {
  const serial = await selectSerial();
  console.log(`Selected Serial Port: ${serial}`);

  serialport = new SerialPort({
    path: serial,
    baudRate: BAUDRATE,
    autoOpen: false,
  });
  
  const parser = serialport.pipe(new ReadlineParser({ delimiter: '\n' }));
  
  parser.on('data', (line) => {
    console.log(JSON.stringify(line));
  });
  
  serialport.on('open', () => {
    console.log('OPEN EVENT');
  });
  
  serialport.on('error', (err) => {
    console.error('ERROR EVENT:', err);
  });
  
  serialport.on('close', () => {
    console.log('CLOSE EVENT');
  });

  const timer = setTimeout(() => {
    console.error(`serialport.open() callback did not fire after ${TIMEOUT_MS}ms`, {
      isOpen: serialport?.isOpen,
      readable: serialport?.readable,
      writable: serialport?.writable,
    });

    exitCleanly(1);
  }, TIMEOUT_MS);

  console.log('About to open serial port');

  serialport.open((err) => {
    clearTimeout(timer);

    if (err) {
      console.error('Error opening serial port:', err.message);
      exitCleanly(1);
      return;
    }

    console.log(`Serial port ${serial} opened at baud rate ${BAUDRATE}; Closing serial port now...`);

    serialport.close((closeErr) => {
      if (closeErr) {
        console.error('Error closing serial port:', closeErr.message);
        exitCleanly(1);
        return;
      }

      console.log(`Serial port ${serial} closed successfully; Flashing firmware now...`);

      updateMicrocontroller().then(() => {
        console.log('Firmware update completed successfully.');
        exitCleanly(0);
      }).catch((updateErr) => {
        console.error('Error updating firmware:', updateErr);
        exitCleanly(1);
      });
    });
  });
}

async function launchServer() {
  console.log('Launching server...');
  await main();
}

async function launch() {
  const s = await select({
    message: 'PeaPod OS:',
    choices: Object.entries(Launch).map(pm => ({
      // Object.entries casts `PublishingMode` values to `string`. This recasts them to PublishingMode enum values (TypeScript magic)
      value: Launch[pm[0] as keyof typeof Launch],
      name: pm[1]
    })),
  });
  switch (s) {
  case Launch.SERIAL:
    await launchSerial();
    break;
  case Launch.FLASH:
    await launchFlash();
    break;
  case Launch.SERVER:
    await launchServer();
    break;
  }
}

launch().catch((err) => {
  console.error('Error during launch:', err);
  exitCleanly(1);
});