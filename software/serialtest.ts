import { SerialPort } from 'serialport';
import select from '@inquirer/select';

const BAUDRATE = 115200;

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

    serialport.on('data', data => {
      console.log(`Data received from ${serial}:`, data.toString());
    });

    serialport.open(err => {
      if (err) {
        return console.error('Error opening serial port:', err.message);
      }
      console.log(`Serial port ${serial} opened at baud rate ${BAUDRATE}`);
    });
  });
}).catch(err => {
  console.error('Error listing serial ports:', err);
});