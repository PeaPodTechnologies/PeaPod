import esbuild from 'esbuild';
import {
  esbuildPluginFilePathExtensions
} from 'esbuild-plugin-file-path-extensions';

console.log('Building API...');

const esbuildConfig = {
  format: 'esm',
  platform: 'node',
  target: 'esnext',
  packages: 'external',
  bundle: true,
  allowOverwrite: true,
  keepNames: true,
  outExtension: { '.js': '.mjs' },
  tsconfig: './api.tsconfig.json',
  external: ['debug', 'ora', 'chalk', 'socket.io', 'blessed'],
  plugins: [esbuildPluginFilePathExtensions({
    esm: true,
    esmExtension: 'mjs',
    filter: /^\..*\.(js|ts)$/ // '.*.js' or '.*.ts'
  })]
};

// Build API
const build_api = esbuild.build({
  ...esbuildConfig,
  entryPoints: ['./api/*.ts'],
  outdir: './api/'
});

// Build server
const build_server = esbuild.build({
  ...esbuildConfig,
  entryPoints: ['./server.ts'],
  outdir: './'
});

await Promise.all([build_api, build_server]).catch((err) => { console.error(err); process.exit(1); } );