#!/usr/bin/env node
import chalk from 'chalk';
import * as inquirer from 'inquirer';

import { checkInternet } from './src/utils';
import * as Spinner from './src/ui';
import { PublishingMode } from './src/publisher';
import PeaPod from './src/peapod';

// MAIN

async function main(): Promise<void> {
  Spinner.start(`Checking for ${ chalk.blue('Internet') } connection...'`);
  const internet = await checkInternet();
  if (internet) {
    Spinner.succeed(`Connected to the ${ chalk.blue('Internet') }!`);
  } else {
    Spinner.fail(`Failed to connect to the ${ chalk.blue('Internet') }.`);
  }

  // Get desired publishing mode
  let publishingmode = (await inquirer.prompt<{pm: PublishingMode}>([
    {
      type: 'list',
      name: 'pm',
      message: 'Select publishing mode:',
      choices: Object.entries(PublishingMode).map(pm => {
        return {
          // Object.entries casts `PublishingMode` values to `string`. This recasts them to PublishingMode enum values (TypeScript magic)
          value: PublishingMode[pm[0] as keyof typeof PublishingMode], 
          name: pm[1]
        };
      })
    }
  ])).pm;

  let peapod: PeaPod = new PeaPod(publishingmode);

  peapod.start();
}

main().catch((err)=>{
  // Last resort catch
  Spinner.fail(JSON.stringify(err));
  process.exit(1);
});