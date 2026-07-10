#!/bin/bash
set -euo pipefail

echo "PeaPod Boot" | systemd-cat -t peapod

/usr/local/bin/peapodos