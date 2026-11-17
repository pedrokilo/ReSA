#!/bin/bash
# Corrida principal del banco Fn210A (16384 casos por funcion): compila con el candado de compilacion, toma el candado del juego y corre sin limite de 3400 s.
# Uso: bash reversing/Fn210A/main.sh [ruta_del_log]
cd /d/Original/wv-6
LOG=${1:-reversing/Fn210A/main.log}
CL=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
: > "$LOG"
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "wv6 210A main $(date +%T)" > "$CL/owner_wv6_210A"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 python reversing/Fn210A/build_probe.py > /dev/null 2>&1
rcb=$?
rm -f "$CL/owner_wv6_210A"; rmdir "$CL" 2>/dev/null
if [ $rcb -ne 0 ]; then echo "BUILD FAIL" >> "$LOG"; grep -n " error " reversing/Fn210A/artifacts/probe-build.log | head -5 >> "$LOG"; echo "rc=1" >> "$LOG"; exit 1; fi
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
echo "wv6 210A main" > "$LOCK/owner_wv6_210A"
trap 'rm -f "$LOCK/owner_wv6_210A"; rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
python reversing/Fn210A/run_local.py >> "$LOG" 2>&1
echo "rc=$?" >> "$LOG"
rm -f reversing/Fn210A/artifacts/*.pdb
