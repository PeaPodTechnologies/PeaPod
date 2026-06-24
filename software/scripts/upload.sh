#!/bin/bash

HOSTNAME="${1:?Usage: ./scripts/upload.sh <hostname>}"

scp out.tar.gz pi@"$HOSTNAME":~/