#!/bin/bash
# chain222.sh: compila la sonda, y en UNA sola sesion del candado de juego corre: humo (2048 casos), corrida principal (16384 casos por funcion) y 2 rondas de prueba negativa (2048 casos por funcion).
cd /d/Original/wv-5/reversing
B=Fn222A
OUT=$B/chain.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-5 chain222 $1 $(date +%T)" > "$CLK/owner_wv5"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python $B/build_probe.py > /dev/null 2>&1
  rc=$?
  rm -f "$CLK/owner_wv5"; rmdir "$CLK" 2>/dev/null
  return $rc
}
run() { # $1 = etiqueta
  for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  timeout 9600 python $B/run_local.py 2>&1 | tail -3 > $B/run_$1.txt
  log "run $1: $(head -c 300 $B/run_$1.txt | tr '\n' ' ')"
}
: > $OUT
build good-main || { log "BUILD FAIL good-main"; exit 1; }
log "build ok main"
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
FN222A_CASES=2048 run smoke
if ! grep -q APROBADO $B/run_smoke.txt; then log "humo FALLO: se aborta la cadena"; exit 1; fi
run main
for r in 1 2; do
  python -I $B/mutate.py apply $r >> $OUT 2>&1 || { log "apply $r FALLO"; break; }
  build mut$r || log "BUILD FAIL mut$r"
  FN222A_CASES=2048 run neg$r
  python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore $r FALLO"
  d=$(grep -o "Informe: .*" $B/run_neg$r.txt | sed 's/Informe: //' | tr '\\' '/' | head -1)
  log "informe neg$r: $d"
  if [ -n "$d" ]; then python -I $B/neg_summary.py "$d" "$B/negative-runs/$(basename "$d")" >> $OUT 2>&1; fi
done
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
