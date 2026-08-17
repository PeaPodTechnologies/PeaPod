#!/usr/bin/env bash

set -euo pipefail

brew install gnu-sed

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

DIR_STAGE="$SCRIPT_DIR/stage"
DIR_PIGEN="$SCRIPT_DIR/pi-gen"
DIR_CONFIG="$SCRIPT_DIR/config"

chmod +x "$DIR_STAGE/prerun.sh"
chmod +x "$DIR_STAGE/00-install/00-run.sh"
chmod +x "$DIR_STAGE/00-install/01-run-chroot.sh"
chmod +x "$DIR_STAGE/00-install/files/peapod-update.sh"
chmod +x "$DIR_STAGE/01-peapod/00-run.sh"
chmod +x "$DIR_STAGE/01-peapod/files/peapod.sh"
chmod +x "$DIR_STAGE/01-peapod/files/00-peapod-launch.sh"

GNU_SED_PREFIX="$(brew --prefix gnu-sed)"
export PATH="$GNU_SED_PREFIX/libexec/gnubin:$PATH"

export PIGEN_DOCKER_OPTS="--mount type=bind,source=${DIR_STAGE},target=/stage-peapod,readonly"

"$DIR_PIGEN/build-docker.sh" -c "$DIR_CONFIG/pi-gen-qemu.txt"