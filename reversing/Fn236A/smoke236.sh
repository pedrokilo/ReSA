#!/bin/bash
# smoke233.sh <casos>: una corrida corta del banco Fn236A (con el candado del juego); sin compilar.
cd /d/Original/wv-6 || exit 1
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
echo "agente 236A humo $(date +%T)" > "$LOCK/owner_236A"
trap 'rm -f "$LOCK/owner_236A"; rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
FN236A_CASES=${1:-64} FN236A_TIMEOUT=2400 timeout 2600 python reversing/Fn236A/run_local.py 2>&1 | tail -8
