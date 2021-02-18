#!/bin/bash
[ $# -eq 1 ] || exit 1
dd bs=$((16#$(hexdump -C $1 | grep -m 1 "00 00 00 01 27 64 00 28  ac 2b 40 5a 1e d0 0f 12" | awk '{print $1}'))) skip=1 if=$1 of=s$1
