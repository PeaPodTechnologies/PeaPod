import { promiseToReject } from './utils';
import { execute, checkArduino } from '../src/utils';
import { mkdirSync, writeFileSync, existsSync } from 'fs';

/**
 * Run Arduino unit tests, powered by PlatformIO. Log results to file.
 * @returns Resolves with void, rejects with error.
 */
const arduinoUnitTests = () => {
  return new Promise<void>((res, rej) => {
    // Create log folder
    if (!existsSync('logs/')) {
      mkdirSync('logs/', { recursive: true });
    }
    execute(`pio test -d PeaPodOS-Arduino`, [1]).catch(err => {
      writeFileSync('logs/arduinoUnitTests.log', err);
      rej(new Error(`One or more unit tests failed. See logs/arduinoUnitTests.log`));
    }).then(log1 => {
      log1 ? writeFileSync('logs/arduinoUnitTests.log', log1) : null;
      res();
    });
  });
}

jest.setTimeout(60000);

test('avrdude can talk to the Arduino', () => {
  expect.assertions(1);
  return expect(promiseToReject(checkArduino())).resolves.toBe(false);
});

test('Arduino unit tests', () => {
  expect.assertions(1);
  return expect(promiseToReject(arduinoUnitTests())).resolves.toBe(false);
});