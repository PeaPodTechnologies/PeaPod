#!/bin/bash
set -euo pipefail

echo "PeaPod Update: waiting for network and DNS"

DNS_HOST="${PEAPOD_UPDATE_DNS_HOST:-registry.npmjs.org}"
MAX_ATTEMPTS="${PEAPOD_UPDATE_MAX_ATTEMPTS:-90}"
RETRY_DELAY_SEC="${PEAPOD_UPDATE_RETRY_DELAY_SEC:-2}"
REPAIR_DELAY_SEC="${PEAPOD_UPDATE_REPAIR_DELAY_SEC:-1}"

has_default_route() {
	ip route show default 2>/dev/null | grep -q '^default '
}

has_nameserver() {
	grep -Eq '^nameserver[[:space:]]+[0-9a-fA-F:.]+' /etc/resolv.conf 2>/dev/null
}

find_candidate_iface() {
	for n in eth0 usb0 enx0 wlan0; do
		if ip link show "$n" >/dev/null 2>&1; then
			echo "$n"
			return
		fi
	done

	ip -o link show | awk -F': ' '$2 != "lo" { print $2; exit }'
}

request_dhcp_lease() {
	local iface
	iface="$1"

	if command -v dhcpcd >/dev/null 2>&1; then
		dhcpcd -n "${iface}" >/dev/null 2>&1 || true
		return
	fi

	if command -v dhclient >/dev/null 2>&1; then
		dhclient -1 -v "${iface}" >/dev/null 2>&1 || true
		return
	fi

	if command -v udhcpc >/dev/null 2>&1; then
		udhcpc -i "${iface}" -q -n >/dev/null 2>&1 || true
		return
	fi

	if command -v networkctl >/dev/null 2>&1; then
		networkctl renew "${iface}" >/dev/null 2>&1 || true
	fi
}

repair_network() {
	local iface
	iface="$(find_candidate_iface)"

	if [[ -z "${iface}" ]]; then
		return
	fi

	ip link set dev "${iface}" up >/dev/null 2>&1 || true
	request_dhcp_lease "${iface}"
	sleep "${REPAIR_DELAY_SEC}"
}

for i in $(seq 1 "${MAX_ATTEMPTS}"); do
	repair_network

	if has_default_route && has_nameserver && getent hosts "${DNS_HOST}" >/dev/null 2>&1; then
		echo "PeaPod Update: network and DNS ready"
		break
	fi

	route_state="missing"
	resolver_state="missing"
	if has_default_route; then
		route_state="ok"
	fi
	if has_nameserver; then
		resolver_state="ok"
	fi

	echo "PeaPod Update: DNS not ready (attempt ${i}/${MAX_ATTEMPTS}, route=${route_state}, resolv.conf=${resolver_state})"
	sleep "${RETRY_DELAY_SEC}"
done

if ! has_default_route || ! has_nameserver || ! getent hosts "${DNS_HOST}" >/dev/null 2>&1; then
	echo "PeaPod Update: DNS unavailable after wait, exiting"
	echo "PeaPod Update: ip addr"
	ip addr || true
	echo "PeaPod Update: ip route"
	ip route || true
	echo "PeaPod Update: resolv.conf"
	cat /etc/resolv.conf || true
	exit 1
fi

echo "PeaPod Update: running npm install"

NODE_OPTIONS="--max-old-space-size=1024" \
npm install -g @peapodtech/peapodos \
	--verbose \
  --foreground-scripts \
  --timing \
	--progress=false \
	--fetch-retries=5 \
	--fetch-retry-factor=2 \
	--fetch-retry-mintimeout=10000 \
	--fetch-retry-maxtimeout=120000

echo "PeaPod Update: npm install complete"