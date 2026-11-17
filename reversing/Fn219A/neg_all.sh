#!/bin/bash
# neg_all.sh: dos rondas de prueba negativa en UNA sola sesion del candado de juego, y recompilacion final de la sonda buena (manifiesto con los hashes de las fuentes buenas)
cd /d/Original/wv-5
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
build() {
  CL=/d/Original/compile_lock
  while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
  echo "wv-5 Fn219A neg $(date +%T)" > "$CL/owner_fn219a"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 python reversing/Fn219A/build_probe.py > /dev/null 2>&1
  local rc=$?
  rm -f "$CL/owner_fn219a"; rmdir "$CL" 2>/dev/null
  return $rc
}
for N in 1 2; do
  python -I reversing/Fn219A/mutate.py apply $N || exit 1
  if ! build; then echo "BUILD FAIL ronda $N"; python -I reversing/Fn219A/mutate.py restore; exit 1; fi
  for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  out=$(FN219A_CASES=2048 timeout 9500 python reversing/Fn219A/run_local.py 2>&1 | tail -3)
  echo "RONDA $N: $(echo "$out" | head -3)"
  python -I reversing/Fn219A/mutate.py restore
  d=$(echo "$out" | grep -o "Informe: .*" | sed 's/Informe: //' | tr '\' '/' | head -1)
  echo "informe: $d"
  if [ -n "$d" ]; then python -I reversing/Fn219A/neg_summary.py "$d" "reversing/Fn219A/negative-runs/$(basename "$d")"; fi
done
if build; then echo "SONDA BUENA RECOMPILADA"; else echo "BUILD FAIL final"; fi
