#!/bin/bash
set -euo pipefail

if [ ! -d "${ROOTFS_DIR}" ]; then
    copy_previous
fi