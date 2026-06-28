import { promiseToReject } from './utils';
import { execute } from '../src/utils';
import { mkdirSync, writeFileSync, existsSync } from 'fs';

/**
 * Run Arduino unit tests, powered by PlatformIO. Log results to file.
 * @returns Resolves with void, rejects with error.
 */
function arduinoUnitTests() {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(`pio test -d microcontroller`, [1]).catch(err => {
      writeFileSync('logs/arduinoUnitTests.log', err);
      rej(new Error(`One or more unit tests failed. See logs/arduinoUnitTests.log`));
    }).then(log1 => {
      log1 ? writeFileSync('logs/arduinoUnitTests.log', log1) : null;
      res();
    });
  });
}

jest.setTimeout(60000);

test('Arduino unit tests', () => {
  expect.assertions(1);
  return expect(promiseToReject(arduinoUnitTests())).resolves.toBe(false);
});