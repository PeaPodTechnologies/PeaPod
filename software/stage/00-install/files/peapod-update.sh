#!/bin/bash
set -euo pipefail

echo "PeaPod Update" | systemd-cat -t peapod

npm install -g @peapodtech/peapodos

/usr/local/bin/peapodos