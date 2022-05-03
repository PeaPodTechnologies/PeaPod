import { execute, gpioExport, gpioUnexport, gpioWrite } from '../src/utils'
import { existsSync, readFileSync } from 'fs';

const TEST_PIN = 24;

test('code execution works', () => {
  expect.assertions(1);
  return expect(execute('echo hello world!')).resolves.toEqual('> echo hello world!\nhello world!\n');
});

test('gpio pins can be exported', () => {
  expect.assertions(1);
  gpioExport(TEST_PIN);
  return expect(existsSync(`/sys/class/gpio/gpio${TEST_PIN}/`)).toBe(true);
});

test('gpio pins are unexported', () => {
  expect.assertions(1);
  gpioUnexport(TEST_PIN);
  return expect(existsSync(`/sys/class/gpio/gpio${TEST_PIN}/`)).toBe(false);
});

test('gpio pins can be written to', () => {
  expect.assertions(1);
  gpioExport(TEST_PIN);
  gpioWrite(TEST_PIN, 1);
  return expect(readFileSync(`/sys/class/gpio/gpio${TEST_PIN}/value`).toString()).toMatch(/^1$/);
});