import { SerialPort, ReadlineParser } from 'serialport';
import select from '@inquirer/select';

const BAUDRATE = 115200;
const TIMEOUT_MS = 5000;

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

  // Do not process.exit immediately if you want to keep observing data.
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