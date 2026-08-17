#!/bin/bash
set -euo pipefail
echo "PeaPod: Booting PeaPodOS"

/usr/local/bin/peapodos --host 0.0.0.0 --port 80

echo "PeaPod: Exited with code $?"