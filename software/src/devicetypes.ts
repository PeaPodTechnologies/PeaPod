export type DeviceID =
  | 'SHT45'
  | '24LC32'
  | 'SEESAW'
  | 'MCP23017'
  | 'JHD1313'
  | 'PCA9685'
  | 'HT16K33';

export const DEVICE_ARG_TYPES_GET: {
  [key in DeviceID]: (a?: string) => boolean;
} = {
  SHT45: () => true, // No args required
  '24LC32': () => false, // Disabled for now
  SEESAW: () => true, // No args required
  MCP23017: (a) => {
    const _a = parseInt(a);
    if (isNaN(_a)) return false;
    return _a >= 0 && _a <= 0xffff;
  },
  JHD1313: () => false, // No get operation for JHD1313
  PCA9685: () => false, // No get operation for PCA9685
  HT16K33: () => false, // No get operation for HT16K33
};

export const DEVICE_ARG_TYPES_SET: {
  [key in DeviceID]: (s?: string, b?: string) => boolean;
} = {
  SHT45: () => false,
  '24LC32': () => false, // Disabled for now
  SEESAW: () => false, // No set operation for SEESAW
  MCP23017: (s, b) => {
    const _s = parseInt(s);
    const _b = parseInt(b);
    if (isNaN(_s) || isNaN(_b)) return false;
    return _s >= 0 && _b >= 0 && _s <= 0xffff && _b <= 0xffff;
  },
  JHD1313: (s, b) => {
    const _b = parseInt(b);
    if (isNaN(_b)) return false;
    return _b >= 0 && _b <= 0xffffff;
  },
  PCA9685: (s, b) => {
    const _s = parseInt(s);
    const _b = parseInt(b);
    if (isNaN(_s) || isNaN(_b)) return false;
    return (
      _s >= 0 &&
      _b >= 0 &&
      _s <= 4096 &&
      _b <= 4096
    );
  },
  HT16K33: (s, b) => {
    const _b = parseInt(b);
    if (isNaN(_b)) return false;
    return (
      _b >= 0 &&
      _b <= 7
    );
  }
};

export const DEVICE_ARG_PARSE_A: {
  [key in DeviceID]: (a: string) => number | string  | boolean;
} = {
  SHT45: (a) => a, // No parsing required
  '24LC32': (a) => a, // Disabled for now
  SEESAW: (a) => a, // No parsing required
  MCP23017: (a) => {
    return parseInt(a);
  },
  JHD1313: (a) => a, // No parsing required
  PCA9685: (a) => a, // No parsing required
  HT16K33: (a) => a, // No parsing required
};

export const DEVICE_ARG_PARSE_S: {
  [key in DeviceID]: (s: string) => number | string  | boolean;
} = {
  SHT45: (s) => s, // No parsing required
  '24LC32': (s) => s, // Disabled for now
  SEESAW: (s) => s, // No parsing required
  MCP23017: (s) => {
    return parseInt(s);
  },
  JHD1313: (s) => s, // No parsing required
  PCA9685: (s) => {
    return parseInt(s);
  },
  HT16K33: (s) => s, // No parsing required
};

export const DEVICE_ARG_PARSE_B: {
  [key in DeviceID]: (b: string) => number | string  | boolean
} = {
  SHT45: (b) => b, // No parsing required
  '24LC32': (b) => b, // Disabled for now
  SEESAW: (b) => b, // No parsing required
  MCP23017: (b) => {
    return parseInt(b);
  },
  JHD1313: (b) => {
    return parseInt(b);
  },
  PCA9685: (b) => {
    return parseInt(b);
  },
  HT16K33: (b) => {
    // return parseInt(b);
    return 4; // Hardcoded string mode
  },
};

export const DEVICE_ARG_HAS: {
  [key in DeviceID]: {
    g: boolean;
    a: boolean;
    s: boolean;
    b: boolean;
  };
} = {
  SHT45: { g: true, a: false, s: false, b: false },
  '24LC32': { g: true, a: false, s: false, b: false },
  SEESAW: { g: true, a: false, s: false, b: false },
  MCP23017: { g: true, a: true, s: true, b: true },
  JHD1313: { g: false, a: false, s: true, b: true },
  PCA9685: { g: false, a: false, s: true, b: true },
  HT16K33: { g: false, a: false, s: true, b: true },
};