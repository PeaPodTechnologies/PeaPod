export type DebugJsonMessageTypes =
  | 'debug'
  | 'info'
  | 'warn'
  | 'error'
  | 'event'
  | 'command'
  | 'config'
  | 'revision'
  | 'tree';

export type DebugJsonMessage = {
  type: DebugJsonMessageTypes; // * Required; Usually first in a stream
  t?: string; // Additional message typing i.e. device IDs, "WARN", "BSOD", etc.
  timestamp?: number; // ** Suggested; Milliseconds since t=revision sent (or program start)
  msg?: string;
  data?: {
    [key: string]: boolean | number | number[] | string; // I.e. {"temperature": 25.0, "button0": true, "nav": "~/mydir/"}
    // We'll handle floating-point precision at deserialization time, create a class DebugJsonNumber
  } | {
    [key: string]: boolean | number | number[] | string;
  }[];
  units?: {
    [key: string]: string; // I.e. "temperature": "°C"
  };
};

export type DebugJsonInstruction = {
  type: 'config' | 'command',
  data: { 
    fqa?: number, 
    g?: boolean, 
    a?: boolean | number | string, 
    s?: boolean | number | string, 
    b?: boolean | number | string,
    [key: string]: boolean | number | number[] | string | undefined }
};

export function parseType(type: DebugJsonMessageTypes): string {
  switch (type) {
  case 'event':
    return 'Telemetry'; // 'event' is used bi-directionally; Controller-to-Host is telemetry (i.e. component states, program flow), Host-to-Controller is for state changes (non-component/ not command, setting component-state e.g. on/off; targeted/ not config, e.g. setting up the camera lighting component-state as part of the Controller API; instead: specific and programmatic (e.g. enabling/disabling specific component-states mid-program, tuning non-component-state control parameters)
  default:
    return type.toUpperCase().charAt(0) + type.slice(1);
  }
}

// JSON-friendly always
export function parseDatum(value: unknown): string {
  if (typeof value === 'string') {
    return value;
  } else if (Array.isArray(value)) {
    return '[' + value.map(parseDatum).join() + ']'; // Recurse
  } else if (typeof value === 'number') {
    return '' + value; // Let JS handle it
  } else if (typeof value === 'boolean') {
    return value === false ? 'false' : 'true';
  } else if (value === null) {
    return 'null';
  } else {
    return '';
  }
}

export function parseTimestamp(ms: number): string {
  // ms duration since program start
  const seconds = Math.floor(ms / 1000);
  const minutes = Math.floor(seconds / 60);
  const hours = Math.floor(minutes / 60);

  const msString = (ms % 1000).toString().padStart(3, '0');
  const secondsString = (seconds % 60).toString().padStart(2, '0');
  const minutesString = (minutes % 60).toString().padStart(2, '0');
  const hoursString = hours.toString().padStart(2, '0');

  return `${hoursString}:${minutesString}:${secondsString}.${msString}`;
};

export type SchedulerEntry = {
  id: string;
  date: Date;
  title: string;
  description?: string;
  instruction: DebugJsonInstruction;
} & (
  | { entry: 'event' }
  | {
      entry: 'interval';
      endDate: Date;
      interval: number;
    }
);