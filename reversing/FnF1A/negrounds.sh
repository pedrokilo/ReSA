#!/bin/bash
# Prueba negativa de FnF1A: dos rondas de mutaciones con UNA sola sesion del candado del juego (gta_lock). Uso: bash reversing/FnF1A/negrounds.sh
# Cada ronda: mutate apply N -> compilar (compile_lock solo durante la compilacion) -> run_local con FNF1A_CASES=2048 -> mutate restore -> neg_summary. Las fuentes se restauran siempre.
cd "$(cd "$(dirname "$0")/../.." && pwd)"
CL=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
OUT=reversing/FnF1A/negative-runs
mkdir -p $OUT
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'python -I reversing/FnF1A/mutate.py restore >/dev/null 2>&1; rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
cp reversing/FnF1A/artifacts/build-manifest.json reversing/FnF1A/artifacts/build-manifest-main.json
for N in ${ROUNDS:-1 2}; do
  python -I reversing/FnF1A/mutate.py apply $N || exit 1
  while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
  echo "FnF1A neg $N $(date +%T)" > "$CL/owner_negrounds"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 9000 python reversing/FnF1A/build_probe.py > /dev/null 2>&1
  rcb=$?
  rm -f "$CL/owner_negrounds"; rmdir "$CL" 2>/dev/null
  if [ $rcb -ne 0 ]; then echo "BUILD FAIL ronda $N"; python -I reversing/FnF1A/mutate.py restore; exit 1; fi
  FNF1A_CASES=2048 timeout 9500 python reversing/FnF1A/run_local.py 2>&1 | tail -3 > reversing/FnF1A/neg$N.txt
  cat reversing/FnF1A/neg$N.txt
  d=$(grep -o "Informe: .*" reversing/FnF1A/neg$N.txt | sed 's/Informe: //' | tr '\\' '/')
  python -I reversing/FnF1A/mutate.py restore
  python -I reversing/FnF1A/neg_summary.py "$d" "$OUT/ronda$N-$(basename "$d")"
  rm -f reversing/FnF1A/artifacts/*.pdb
done
cp reversing/FnF1A/artifacts/build-manifest-main.json reversing/FnF1A/artifacts/build-manifest.json
