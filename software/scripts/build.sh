#!/bin/bash

set -euo pipefail

IMAGE_OWNER="${1:-}"
IMAGE_TAG="${2:-latest}"

docker buildx create --use --name peapodos 2>/dev/null || true

if [[ -n "$IMAGE_OWNER" ]]; then
  IMAGE_NAME="$IMAGE_OWNER/peapodos:$IMAGE_TAG"
  docker buildx build \
    --platform linux/arm64 \
    --target peapodos \
    --tag "$IMAGE_NAME" \
    --load \
    .
  echo "Built image: $IMAGE_NAME"
else
  rm out.tar.gz && docker buildx build \
    --platform linux/arm64 \
    --target export \
    --output type=local,dest=. \
    .
  echo "Exported runtime bundle to ./out.tar.gz"
fi