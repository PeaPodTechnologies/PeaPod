#!/bin/bash -e

install -m 0644 \
    files/peapod-update.service \
    "${ROOTFS_DIR}/etc/systemd/system/peapod-update.service"

install -m 0755 \
    files/peapod.sh \
    "${ROOTFS_DIR}/usr/local/bin/peapod.sh"

install -m 0644 \
    files/peapod.service \
    "${ROOTFS_DIR}/etc/systemd/system/peapod.service"

install -m 0755 \
    files/peapod-update.sh \
    "${ROOTFS_DIR}/usr/local/bin/peapod-update.sh"