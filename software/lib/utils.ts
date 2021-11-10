import * as dns from 'dns';
import fs from 'fs';
import dotenv from 'dotenv';
import SerialPort from 'serialport';
import axios from 'axios';

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
  if(fs.existsSync(path)){
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
  return (await axios.get("https://pki.goog/roots.pem")).data as string;
}