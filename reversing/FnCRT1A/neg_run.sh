#!/bin/bash
# Prueba negativa del banco CRT1A: cada mutacion cambia UNA linea de las fuentes propias (copia en un directorio temporal) y el banco debe detectarla (rc != 0).
# Uso: bash neg_run.sh <directorio de trabajo> <ruta de gta_sa.exe>      (escribe neg_run.out en el directorio de trabajo)
W="$1"; EXE="$2"; HERE="$(cd "$(dirname "$0")" && pwd)"; SRC="$HERE/../../source/game_sa/Crt"
OUT="$W/neg_run.out"; [ -z "$3" ] && : > "$OUT"   # tercer argumento opcional: expresion regular con los nombres de las mutaciones a repetir (se añade al final de neg_run.out)
# nombre | archivo | sed | seccion del banco
MUTS=(
"qsort-corte-7|CrtQsort.cpp|s/CUTOFF = 8;/CUTOFF = 7;/|qsort"
"qsort-primer-bucle-estricto|CrtQsort.cpp|s/compare(loguy, mid) <= 0);\$/compare(loguy, mid) < 0);/|qsort"
"qsort-shortsort-igual-cuenta|CrtQsort.cpp|s/if (compare(p, max) > 0) {/if (compare(p, max) >= 0) {/|qsort"
"qsort-mediana-sin-tercera|CrtQsort.cpp|s/if (compare(mid, hi) > 0) {/if (false) {/|qsort"
"qsort-ordena-menor-primero|CrtQsort.cpp|s/if (higuy - lo >= hi - loguy) {/if (higuy - lo > hi - loguy) {/|qsort"
"strncpy-sin-relleno|CrtString.cpp|s/    while (count) { \/\/ relleno/    while (false) { \/\/ relleno/|cadenas"
"strncat-sin-cero-final|CrtString.cpp|s/if (count == static_cast<unsigned int>(-1)) {/if (false) {/|cadenas"
"strcmp-devuelve-diferencia|CrtString.cpp|s/return \*pa < \*pb ? -1 : 1;\$/return (int)*pa - (int)*pb;/|cadenas"
"strstr-aguja-vacia-nula|CrtString.cpp|s/if (\*needle == 0) {/if (false) {/|cadenas"
"calloc-sin-ceros|CrtHeap.cpp|s/HeapAlloc(CrtHeap(), HEAP_ZERO_MEMORY, total)/HeapAlloc(CrtHeap(), 0, total)/|monton"
"malloc-cero-sin-uno|CrtHeap.cpp|s/size ? size : 1u/size/|monton"
"realloc-cero-no-libera|CrtHeap.cpp|s/    if (size == 0) {/    if (size == 1234567) {/|monton"
"free-ignora-nulo-invertido|CrtHeap.cpp|s/    if (block) {\$/    if (!block) {/|monton"
)
rc_all=0
for m in "${MUTS[@]}"; do
  IFS='|' read -r NAME FILE SED SECT <<< "$m"
  [ -n "$3" ] && [[ ! "$NAME" =~ $3 ]] && continue
  D="$W/mut_$NAME"; rm -rf "$D"; mkdir -p "$D"; cp "$SRC"/*.cpp "$SRC"/*.h "$D/"
  sed -i "$SED" "$D/$FILE"
  if cmp -s "$D/$FILE" "$SRC/$FILE"; then echo "$NAME: LA MUTACION NO CAMBIO EL ARCHIVO" >> "$OUT"; rc_all=1; continue; fi
  cmd.exe //c "$(cygpath -w "$HERE/build_bench.cmd") $(cygpath -w "$D/bin") $(cygpath -w "$D")" > "$D/build.log" 2>&1
  [ -x "$D/bin/bench_crt.exe" ] || { echo "$NAME: NO SE PUDO COMPILAR (ver $D/build.log)" >> "$OUT"; rc_all=1; continue; }
  "$D/bin/bench_crt.exe" "$EXE" --casos 20000 --solo "$SECT" > "$D/run.log" 2>&1
  R=$?
  echo "$NAME ($SECT): rc=$R $(grep -c DIFERENCIA "$D/run.log") lineas DIFERENCIA" >> "$OUT"
  [ $R -eq 0 ] && rc_all=1
done
echo "FIN rc_all=$rc_all (0 = todas las mutaciones detectadas)" >> "$OUT"
