#!/bin/bash

set -euo pipefail

IMAGE_OWNER="${1:-}"
IMAGE_TAG="${2:-latest}"

docker buildx create --use --name peapodos 2>/dev/null || true

docker build -f Dockerfile.pack -t peapodos-pack:latest --target export --output type=local,dest=./dist .