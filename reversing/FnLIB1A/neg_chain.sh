#!/bin/bash
# Dos rondas de prueba negativa (mutate.py + neg_run.py). Uso: bash neg_chain.sh <scratch dir con casos 'full' y benchm1/benchm2 ya compilados>
S="$1"
cd /d/Original/wv-5/reversing/FnLIB1A
OUT="$S/neg_chain.out"; : > "$OUT"
for r in 1 2; do
  python -I neg_run.py $r "$S/benchm$r/bench.exe" "$S/full" "E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe" >> "$OUT" 2>&1
done
echo done >> "$OUT"
