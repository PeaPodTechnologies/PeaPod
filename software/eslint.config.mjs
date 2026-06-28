import globals from 'globals';

import { FlatCompat } from '@eslint/eslintrc';
import typescriptEslint from '@typescript-eslint/eslint-plugin';


const compat = new FlatCompat({
  // import.meta.dirname is available after Node.js v20.11.0
  baseDirectory: import.meta.dirname,
  recommendedConfig: typescriptEslint.configs['recommended'],
});

console.log('Using eslint.config.mjs');

var eslintConfig = compat.config({
  extends: ['prettier', 'next', 'next/typescript'],
  plugins: ['@typescript-eslint', 'react'],
});

eslintConfig.push({ files: [
  './*.{js,mjs,cjs,ts,jsx,tsx}', 
  './api/*.{js,mjs,cjs,ts,jsx,tsx}', 
  './api/**/*.{js,mjs,cjs,ts,jsx,tsx}', 
  './src/*.{js,mjs,cjs,ts,jsx,tsx}',
  './src/**/*.{js,mjs,cjs,ts,jsx,tsx}'
]})

eslintConfig.push({ignores: ['node_modules', '.next', 'out', 'dist', 'build', 'coverage', 'public', 'lib']});

eslintConfig.push({
  languageOptions: {
    globals: {...globals.browser, ...globals.node }
  },
  // linterOptions: { parserOptions: { projectService: true, tsconfigRootDir: import.meta.dirname}},
  rules: { 
    indent: ['error', 2], 
    'linebreak-style': ['error', 'unix'], 
    quotes: ['error', 'single'], 
    semi: ['error', 'always'], 
    'react/react-in-jsx-scope': 'off'
  }
});

/** @type {import('eslint').Linter.Config[]} */
export default eslintConfig;