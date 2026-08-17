#!/bin/bash
set -euo pipefail

install -m 0755 \
    files/peapod.sh \
    "${ROOTFS_DIR}/usr/local/bin/peapod.sh"

install -m 0755 \
    files/00-peapod-launch.sh \
    "${ROOTFS_DIR}/etc/profile.d/00-peapod-launch.sh"