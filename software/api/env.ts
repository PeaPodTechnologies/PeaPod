import dotenv from 'dotenv';
import { existsSync } from 'fs';
import { FirebaseOptions } from 'firebase/app';
import { EnvFieldError } from './errors';
import { DeviceFlowUIOptions } from '@peapodtech/firebasedeviceflow';

/**
 * Env fields required for ALL modes.
 */
const ENV_FIELDS_PEAPOD = ['SERIALPORT'];

/**
 * Env fields required for Firebase functionality.
 */
const ENV_FIELDS_FIREBASE = [
  'FIREBASE_APIKEY',
  'FIREBASE_AUTHDOMAIN',
  'FIREBASE_PROJECTID',
  'FIREBASE_STORAGEBUCKET',
  'FIREBASE_MESSAGINGSENDERID',
  'FIREBASE_APPID',
  'FIREBASE_MEASUREMENTID'
];

/**
 * Env fields required for GCP IoT Core functionality.
 */
const ENV_FIELDS_IOT = [
  'IOT_CLOUDREGION',
  'FIREBASE_PROJECTID',
  'IOT_REGISTRY',
  'IOT_JWT_EXPIRYMINS'
];

/**
 * Env fields required for FirebaseDeviceFlow Auth functionality.
 */
const ENV_FIELDS_AUTH = [
  'GOOGLE_SCOPES',
  'GOOGLE_CLIENTID',
  'GOOGLE_CLIENTSECRET',
  'GITHUB_SCOPES',
  'GITHUB_CLIENTID',
  'GITHUB_CLIENTSECRET'
];

export type PeaPodEnv = {
  serialport: string;
};

/**
 * Load a `.env`-style file to `process.env`.
 *
 * Throws an error if the file does not exist.
 */
function loadDotEnv(path: string = '.env'): void {
  // Check for file
  if (existsSync(path)) {
    const config = dotenv.config({ path });
    if (config.error) {
      throw config.error;
    }
  } else {
    throw new Error('Environment variable file not found.');
  }
}

/**
 * Check which of the given fields are missing (undefined) in `process.env`
 * @param fields List of fields to check
 * @returns List of missing fields
 */
function getMissingEnvFields(fields: string[]): string[] {
  return fields.filter(field => !Object.keys(process.env).includes(field));
}

/**
 * Load config for the Firebase app and check for all fields.
 * @returns Firebase app config object
 */
export function loadFirebaseEnv(): FirebaseOptions {
  const missingFields = getMissingEnvFields(ENV_FIELDS_FIREBASE);
  if (missingFields.length) {
    throw new EnvFieldError('Firebase', missingFields);
  } else {
    return {
      apiKey: process.env.FIREBASE_APIKEY,
      authDomain: process.env.FIREBASE_AUTHDOMAIN,
      databaseURL: process.env.FIREBASE_DATABASEURL,
      projectId: process.env.FIREBASE_PROJECTID,
      storageBucket: process.env.FIREBASE_STORAGEBUCKET,
      messagingSenderId: process.env.FIREBASE_MESSAGINGSENDERID,
      appId: process.env.FIREBASE_APPID,
      measurementId: process.env.FIREBASE_MEASUREMENTID
    };
  }
}

/**
 * Load config for GCP IoT Core and check for all fields.
 * @returns GCP IoT Core config object
 */
export function loadIoTEnv(): IoTConfig {
  const missingFields = getMissingEnvFields(ENV_FIELDS_IOT);
  if (missingFields.length) {
    throw new EnvFieldError('GCP IoT Core', missingFields);
  } else {
    return {
      cloudregion: process.env.IOT_CLOUDREGION!,
      projectid: process.env.FIREBASE_PROJECTID!,
      registryid: process.env.IOT_REGISTRY!,
      jwtexpiryminutes: Number(process.env.IOT_JWT_EXPIRYMINS ?? '1440')
    };
  }
}

/**
 * Load config for the Firebase app and check for all fields.
 * @returns Firebase app config object
 */
export function loadAuthEnv(): DeviceFlowUIOptions {
  const missingFields = getMissingEnvFields(ENV_FIELDS_AUTH);
  if (missingFields.length) {
    throw new EnvFieldError('Firebase Device Flow Auth', missingFields);
  } else {
    return {
      Google: {
        scopes: process.env.GOOGLE_SCOPES?.split(' '),
        clientid: process.env.GOOGLE_CLIENTID,
        clientsecret: process.env.GOOGLE_CLIENTSECRET
      },
      GitHub: {
        scopes: process.env.GITHUB_SCOPES?.split(' '),
        clientid: process.env.GITHUB_CLIENTID,
        clientsecret: process.env.GITHUB_CLIENTSECRET
      }
    };
  }
}

export function loadPeaPodEnv(): PeaPodEnv {
  const missingFields = getMissingEnvFields(ENV_FIELDS_PEAPOD);
  if (missingFields.length) {
    throw new EnvFieldError('PeaPod', missingFields);
  } else {
    return {
      serialport: process.env.SERIALPORT!
    };
  }
}

// MAIN
loadDotEnv();
