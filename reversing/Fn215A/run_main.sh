#!/bin/bash
# run_main.sh: corre el banco ya compilado (sin recompilar) con el candado de juego
cd /d/Original/wv-5
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
out=$(timeout 9500 python reversing/Fn215A/run_local.py 2>&1 | tail -3)
echo "$out" | head -3
if echo "$out" | grep -q FALLO; then
  d=$(echo "$out" | grep -o "Informe: .*" | sed 's/Informe: //' | tr '\' '/')
  python reversing/_handoff/showdiff.py "$d" 3 2>&1 | grep -v "waiting_for_player\|'event': 'error'\|case_start" | cut -c1-700 | head -40
fi
