import { execute } from '../src/utils'

test('code execution works', () => {
  expect.assertions(1);
  return expect(execute('echo hello world!')).resolves.toEqual('> echo hello world!\nhello world!\n');
});