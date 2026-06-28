#!/bin/bash

set -euo pipefail

IMAGE_OWNER="${1:?Usage: ./scripts/publish.sh <dockerhub-owner> [tag]}"
IMAGE_TAG="${2:-latest}"

docker login --username "$IMAGE_OWNER"

docker push "$IMAGE_OWNER/peapodos:$IMAGE_TAG"