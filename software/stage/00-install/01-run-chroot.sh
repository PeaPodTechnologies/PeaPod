#!/bin/bash
set -euo pipefail

echo "PeaPod Preinstall: initializing 1 GB swap file"

if [ ! -f /swapfile ]; then
	fallocate -l 1G /swapfile
	chmod 600 /swapfile
	mkswap /swapfile
fi

# Persist swap for the finished image.
if ! grep -q '^/swapfile ' /etc/fstab; then
	echo '/swapfile none swap sw 0 0' >> /etc/fstab
fi

# Enable swap now so it can be used by npm during the image build.
swapon /swapfile

echo "PeaPod Preinstall: swap status"
swapon --show
free -h

echo "PeaPod Preinstall: installing latest npm"

npm install -g npm@^11
npm --version

echo "PeaPod Preinstall: installing @peapodtech/peapodos during image build"

for i in $(seq 1 10); do
	echo "PeaPod Preinstall: npm install attempt ${i}/10"

	if npm install -g @peapodtech/peapodos \
    --verbose \
    --foreground-scripts \
    --timing \
    --progress=false \
    --fetch-retries=5 \
    --fetch-retry-factor=2 \
    --fetch-retry-mintimeout=10000 \
    --fetch-retry-maxtimeout=120000; then
		echo "PeaPod Preinstall: install complete"
		break
	fi

	if [ "${i}" -lt 10 ]; then
		echo "PeaPod Preinstall: install failed, retrying in 5s"
		sleep 5
	else
		echo "PeaPod Preinstall: failed after 10 attempts"
		exit 1
	fi
done

systemctl enable peapod-update.service