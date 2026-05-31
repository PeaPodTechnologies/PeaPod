import { SerialPort } from 'serialport';
import select from '@inquirer/select';

const BAUDRATE = 115200;

function forceExit(reason: string) {
  console.error(`\nForce exit: ${reason}`);
  try {
    if (process.stdin.isTTY) {
      process.stdin.setRawMode(false);
    }
  } catch {}
  process.exit(1);
}

process.on('SIGINT', () => forceExit('SIGINT / Ctrl-C'));
process.on('SIGTERM', () => forceExit('SIGTERM'));
process.on('uncaughtException', (err) => {
  console.error('uncaughtException:', err);
  forceExit('uncaughtException');
});
process.on('unhandledRejection', (err) => {
  console.error('unhandledRejection:', err);
  forceExit('unhandledRejection');
});

// List available serial ports
SerialPort.list().then(ports => {
  console.log('Available Serial Ports:');
  ports.forEach(port => {
    console.log(`- ${port.path} (${port.manufacturer || 'Unknown Manufacturer'})`);
  });

  select({
    message: 'Select Serial Port:',
    choices: ports.map((ser, i) => ({
      value: ser.path,
      name: `${i}: ${ser.path}`
    })),
  }).then(serial => {
    console.log(`Selected Serial Port: ${serial}`);

    const serialport = new SerialPort({
      path: serial,
      baudRate: BAUDRATE,
      autoOpen: false,
    });

    serialport.on('open', () => {
      console.log(`OPEN EVENT: ${serial}`);
    });

    serialport.on('close', () => {
      console.log(`CLOSE EVENT: ${serial}`);
    });

    serialport.on('error', (err) => {
      console.error(`ERROR EVENT: ${err.name} - ${err.message}`);
    });

    serialport.on('data', (data) => {
      console.log(`DATA: ${data.toString()}`);
    });

    serialport.on('data', data => {
      console.log(`Data received from ${serial}:`, data.toString());
    });

    const openTimeout = setTimeout(() => {
      console.error('serialport.open() callback did not fire after 5s');

      try {
        console.error('Port state:', {
          isOpen: serialport.isOpen,
          path: serial,
        });
      } catch {}

      forceExit('open timeout');
    }, 5000);


    serialport.open(err => {
      clearTimeout(openTimeout);
      if (err) {
        console.error('Error opening serial port:', err.message);
        forceExit('open failed');
        return;
      }
      console.log(`Serial port ${serial} opened at baud rate ${BAUDRATE}`);
    });
  });
}).catch(err => {
  console.error('Error listing serial ports:', err);
});