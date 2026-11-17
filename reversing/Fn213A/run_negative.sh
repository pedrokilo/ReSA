#!/bin/bash
# run_negative.sh <ronda>: aplica la ronda de mutaciones, corre el banco con 2048 casos por funcion, resume y restaura las fuentes
cd /d/Original/wv-5
N=$1
python -I reversing/Fn213A/mutate.py apply $N || exit 1
export FN213A_CASES=2048
out=$(CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn213A 2>&1)
echo "$out" | head -5
python -I reversing/Fn213A/mutate.py restore
d=$(echo "$out" | grep -o "Informe: .*" | sed 's/Informe: //' | tr '\' '/' | head -1)
echo "informe: $d"
if [ -n "$d" ]; then python -I reversing/Fn213A/neg_summary.py "$d" "reversing/Fn213A/negative-runs/$(basename "$d")"; fi
