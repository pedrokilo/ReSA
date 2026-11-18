#!/bin/bash
# smoke238.sh <casos> : una corrida corta del banco con el candado de juego (para diagnosticar; no acredita nada).
cd /d/Original/wv-5/reversing
B=Fn238A
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
FN238A_CASES=$1 timeout 3000 python $B/run_local.py 2>&1 | tail -6 > $B/smoke_out.txt
echo "fin $(date +%T)" >> $B/smoke_out.txt
