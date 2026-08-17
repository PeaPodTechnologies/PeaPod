#!/usr/bin/env bash

# NOTE: This script is not intended to be run directly on the host system. It is meant to be executed inside a Docker container that has QEMU installed and configured.

set -euo pipefail

IMAGE="${1:?Usage: $0 <image>}"
BOOT_DIR="/tmp/qemu-boot"

if [[ ! -f "$IMAGE" ]]; then
  echo "Image not found: $IMAGE" >&2
  exit 1
fi

echo "Resizing image to 16GB..."

qemu-img resize -f raw "$IMAGE" 16G

mkdir -p "$BOOT_DIR"

echo "Inspecting partition table..."

# pi-gen's first partition is the FAT boot partition.
# parted outputs its start offset in bytes.
BOOT_OFFSET="$(
  parted -sm "$IMAGE" unit B print |
    awk -F: '
      $1 == "1" {
        gsub(/B$/, "", $2)
        print $2
        exit
      }
    '
)"

if [[ -z "$BOOT_OFFSET" ]]; then
  echo "Could not determine the boot partition offset." >&2
  exit 1
fi

echo "Boot partition starts at byte: $BOOT_OFFSET"

echo "Extracting kernel and device tree..."

mcopy \
  -i "${IMAGE}@@${BOOT_OFFSET}" \
  "::kernel8.img" \
  "$BOOT_DIR/kernel8.img"

mcopy \
  -i "${IMAGE}@@${BOOT_OFFSET}" \
  "::bcm2710-rpi-3-b.dtb" \
  "$BOOT_DIR/bcm2710-rpi-3-b.dtb"

echo "Starting QEMU..."

qemu-system-aarch64 \
  -machine raspi3b \
  -cpu cortex-a53 \
  -smp 4 \
  -m 1G \
  -kernel "$BOOT_DIR/kernel8.img" \
  -dtb "$BOOT_DIR/bcm2710-rpi-3-b.dtb" \
  -drive "file=$IMAGE,format=raw,if=sd" \
  -append "mem=512M earlycon=pl011,0x3f201000 console=ttyAMA1,115200 loglevel=7 systemd.show_status=1 systemd.mask=systemd-networkd-wait-online.service systemd.watchdog_device=/dev/watchdog-do-not-use systemd.runtime_watchdog_sec=0 systemd.reboot_watchdog_sec=0 systemd.kexec_watchdog_sec=0 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait" \
  -serial stdio \
  -monitor none \
  -display none \
  -netdev "user,id=net0,hostfwd=tcp::2222-:22,ipv6=off" \
  -device usb-net,netdev=net0 \
  -no-reboot \
  -d guest_errors,unimp \
  -D /tmp/qemu.log

STATUS=$?
echo "QEMU exited with status: $STATUS"