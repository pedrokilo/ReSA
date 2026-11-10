#!/bin/bash
# bench_here.sh <Bench> : compila la sonda y corre el banco EN ESTE worktree, con candado global para el juego (una sola ejecucion de GTA a la vez).
S="$(cd "$(dirname "$0")" && pwd)"
R="$(cd "$S/.." && pwd)"
cd "$R"
B=$1
CL=/d/Original/compile_lock
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "bench_here $B $(date +%T)" > "$CL/owner_bench_here"
CMAKE_BUILD_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL:-1} _CL_=${_CL_:--MP1} timeout 3000 python $B/build_probe.py $BENCH_ARGS > /dev/null 2>&1
rcb=$?
rm -f "$CL/owner_bench_here"; rmdir "$CL" 2>/dev/null
if [ $rcb -ne 0 ]; then
  echo "BUILD FAIL $B"; grep -n " error \|warning C4335" $B/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-240 | sort -u -t: -k3 | head -6; exit 1
fi
LOCK=/d/Original/gta_lock
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done   # otro agente puede estar cerrando su GTA
out=$(timeout 3400 python $B/run_local.py $BENCH_ARGS 2>&1 | tail -3)
echo "$out" | head -2
if echo "$out" | grep -q FALLO; then
  d=$(echo "$out" | grep -o "Informe: .*" | sed 's/Informe: //' | tr '\' '/')
  python "$S/showdiff.py" "$d" ${2:-2} 2>&1 | grep -v "waiting_for_player\|'event': 'error'\|case_start" | cut -c1-600 | head -30
fi
rm -f "$B"/artifacts/*.pdb
