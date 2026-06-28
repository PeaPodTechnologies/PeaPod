import type { NextConfig } from 'next';

console.log('Using next.config.ts with Node Version:', process.version);

const nextConfig: NextConfig = {
  /* config options here */
  typescript: {
    tsconfigPath: './next.tsconfig.json',
  },
  eslint: {
    ignoreDuringBuilds: true,
  },
  output: 'standalone',
};

export default nextConfig;
