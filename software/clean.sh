#!/bin/bash

set -euo pipefail

# Typescript build artifacts
rm -f api/*.mjs index.mjs server.mjs serialtest.mjs flash.mjs

# Next.js build artifacts
rm -rf .next

# Node modules
rm -rf node_modules

# Pack artifacts
rm -rf dist