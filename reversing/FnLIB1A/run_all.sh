#!/bin/bash
# Corrida completa del banco LIB1A: casos generados (gen_cases.py) -> bench.exe sin mutar -> dos rondas negativas.
# Uso: bash run_all.sh <directorio de trabajo con bench/ benchm1/ benchm2/ full/>   (escribe run_all.out; 'done' al final)
S="$1"
cd /d/Original/wv-5/reversing/FnLIB1A
"$S/bench/bench.exe" "$S/full" "E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe" > "$S/bench/full_run.txt" 2>&1
echo "principal rc=$?" > "$S/run_all.out"
bash neg_chain.sh "$S"
echo done >> "$S/run_all.out"
