#!/bin/bash
set -euo pipefail

install -m 0755 \
    files/peapod-update.sh \
    "${ROOTFS_DIR}/usr/local/bin/peapod-update.sh"

install -m 0644 \
    files/peapod-update.service \
    "${ROOTFS_DIR}/etc/systemd/system/peapod-update.service"
