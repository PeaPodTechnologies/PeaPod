# Build

#!/bin/bash

set -euo pipefail

IMAGE_OWNER="${1:-}"
IMAGE_TAG="${2:-latest}"

docker buildx create --use --name peapodos-export 2>/dev/null || true

rm out.tar.gz && docker buildx build \
  --platform linux/arm64 \
  --target export \
  --output type=local,dest=. \
  .
echo "Exported runtime bundle to ./out.tar.gz"