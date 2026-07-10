import { main } from './server';

import { list } from 'inquirer';

enum Launch {
  FLASH = 'Flash Firmware',
  SERIAL = 'Test Serial',
  SERVER = 'Launch Server',
};
  

const select = await list({
    message: 'PeaPod OS:',
    choices: Object.entries(Launch).map(pm => ({
      // Object.entries casts `PublishingMode` values to `string`. This recasts them to PublishingMode enum values (TypeScript magic)
      value: Launch[pm[0] as keyof typeof Launch],
      name: pm[1]
    })),
  });