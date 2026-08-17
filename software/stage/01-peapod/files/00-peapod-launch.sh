#!/bin/bash

case "$-" in
    *i*) ;;
    *) return 0 ;;
esac

if [ "${PEAPOD_LAUNCHED:-0}" = "1" ]; then
    return 0
fi

if [ -t 0 ] && [ "$(tty 2>/dev/null || true)" = "/dev/tty1" ]; then
    export PEAPOD_LAUNCHED=1
    exec /usr/local/bin/peapod.sh
fi
