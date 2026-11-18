#!/bin/bash
# run_quick.sh [casos]: corrida corta del banco Fn228A con el candado de juego (la sonda ya compilada)
cd /d/Original/wv-6/reversing
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
FN228A_CASES=${1:-64} FN228A_TIMEOUT=1500 timeout 1600 python Fn228A/run_local.py 2>&1 | tail -5 > Fn228A/run_quick.txt
