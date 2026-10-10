#!/usr/bin/env bash

# NOTE: This script is not intended to be run directly on the host system. It is meant to be executed inside a Docker container that has QEMU installed and configured.

set -euo pipefail

IMAGE="${1:?Usage: $0 <image>}"
BOOT_DIR="/tmp/qemu-boot"
DTB_PATH="$BOOT_DIR/rpi.dtb"

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

# Prefer Pi 400 DTB, then fall back to Pi 4B DTB if needed.
DTB_CANDIDATES=(
  "bcm2711-rpi-400.dtb"
  "bcm2711-rpi-4-b.dtb"
)

DTB_FOUND=""
for dtb in "${DTB_CANDIDATES[@]}"; do
  if mcopy -i "${IMAGE}@@${BOOT_OFFSET}" "::${dtb}" "$DTB_PATH" 2>/dev/null; then
    DTB_FOUND="$dtb"
    break
  fi
done

if [[ -z "$DTB_FOUND" ]]; then
  echo "Could not find a Pi 400/Pi 4 DTB in the boot partition." >&2
  exit 1
fi

echo "Using device tree: $DTB_FOUND"

echo "Starting QEMU..."

# Should be 4GB mem but QEMU only allows 2GB
qemu-system-aarch64 \
  -machine raspi4b \
  -cpu cortex-a72 \
  -smp 4 \
  -m 2G \
  -kernel "$BOOT_DIR/kernel8.img" \
  -dtb "$DTB_PATH" \
  -drive "file=$IMAGE,format=raw,if=sd" \
  -append "mem=2G earlycon=pl011,0xfe201000 console=ttyAMA1,115200 loglevel=7 ignore_loglevel systemd.show_status=1 systemd.log_target=console systemd.log_level=info systemd.journald.forward_to_console=1 systemd.getty_auto=console systemd.mask=regenerate_ssh_host_keys.service systemd.mask=systemd-networkd.service systemd.mask=systemd-networkd.socket systemd.mask=systemd-networkd-wait-online.service systemd.mask=rpi-usb-gadget-ics.service systemd.wants=NetworkManager.service systemd.wants=NetworkManager-wait-online.service modules-load=usbnet,cdc_ether,cdc_ncm,rndis_host,asix,ax88179_178a,r8152,smsc95xx systemd.watchdog_device=/dev/watchdog-do-not-use systemd.runtime_watchdog_sec=0 systemd.reboot_watchdog_sec=0 systemd.kexec_watchdog_sec=0 root=/dev/mmcblk1p2 rootfstype=ext4 rw rootwait" \
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