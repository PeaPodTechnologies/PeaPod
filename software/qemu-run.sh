#!/usr/bin/env bash

docker build --platform linux/amd64 -f Dockerfile.qemu -t peapodos-qemu .

IMAGE="${1:?Usage: $0 <image>}"

DOCKER_DNS_ARGS=()
if [[ -n "${QEMU_DOCKER_DNS:-}" ]]; then
  DOCKER_DNS_ARGS=(--dns "$QEMU_DOCKER_DNS")
fi

docker run --platform linux/amd64 --rm -it "${DOCKER_DNS_ARGS[@]}" \
  -p 2222:2222 \
  --name peapod-qemu \
  --privileged \
  -v "$PWD/deploy:/deploy" \
  peapodos-qemu \
  "${IMAGE}"