#!/bin/bash

rm -rf out out.tar.gz
docker buildx create --use --name pi-builder 2>/dev/null || true
docker buildx build \
  -f Dockerfile.buildpi \
  --platform linux/arm64 \
  --target export \
  --output type=local,dest=. \
  .