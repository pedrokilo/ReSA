#!/bin/bash
# Prueba negativa de 2 rondas del banco Fn210A: aplica las mutaciones, compila (candado de compilacion), corre 2048 casos por funcion (candado del juego) y restaura las fuentes.
# Uso: bash reversing/Fn210A/neg.sh [ruta_del_log]
cd /d/Original/wv-6
LOG=${1:-reversing/Fn210A/neg.log}
CL=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
: > "$LOG"
build() {
  while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
  echo "wv6 210A neg $(date +%T)" > "$CL/owner_wv6_210A"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 python reversing/Fn210A/build_probe.py > /dev/null 2>&1
  local rc=$?
  rm -f "$CL/owner_wv6_210A"; rmdir "$CL" 2>/dev/null
  return $rc
}
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
echo "wv6 210A neg" > "$LOCK/owner_wv6_210A"
trap 'rm -f "$LOCK/owner_wv6_210A"; rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
for r in 1 2; do
  python -I reversing/Fn210A/mutate.py apply $r >> "$LOG" 2>&1 || { echo "APPLY FAIL $r" >> "$LOG"; break; }
  if build; then
    FN210A_CASES=2048 python reversing/Fn210A/run_local.py > reversing/Fn210A/round$r.out 2>&1
    echo "ronda $r: $(tail -3 reversing/Fn210A/round$r.out | tr '\n' ' ')" >> "$LOG"
  else
    echo "BUILD FAIL ronda $r" >> "$LOG"; grep -n " error " reversing/Fn210A/artifacts/probe-build.log | head -5 >> "$LOG"
  fi
  python -I reversing/Fn210A/mutate.py restore >> "$LOG" 2>&1
done
build && echo "sonda buena recompilada" >> "$LOG" || echo "BUILD FAIL final" >> "$LOG"
rm -f reversing/Fn210A/artifacts/*.pdb
echo "rc=0" >> "$LOG"
