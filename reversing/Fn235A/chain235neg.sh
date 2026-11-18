#!/bin/bash
# chain235neg.sh: SOLO las dos rondas de la prueba negativa (2048 casos por funcion) en una sola sesion del candado de juego, y la compilacion final con las fuentes buenas.
# (La corrida principal se hizo con chain235.sh; las primeras rondas negativas murieron en jpeg_mem_term / alloc_sarray por mutaciones que escribian o dividian con punteros crudos y se rehicieron.)
cd /d/Original/wv-5/reversing
B=Fn235A
OUT=$B/chain_neg.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-5 chain235neg $1 $(date +%T)" > "$CLK/owner_wv5"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 python $B/build_probe.py > $B/build.out 2>&1
  rc=$?
  rm -f "$CLK/owner_wv5"; rmdir "$CLK" 2>/dev/null
  if [ $rc -ne 0 ]; then grep -n " error \|warning C4335" $B/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-260 | sort -u -t: -k3 | head -12 >> $OUT; fi
  return $rc
}
run() { # $1 = etiqueta, $2 = casos, $3 = tiempo maximo
  for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  FN235A_CASES=$2 timeout ${3:-6000} python $B/run_local.py 2>&1 | tail -4 > $B/run_$1.txt
  log "run $1: $(head -c 900 $B/run_$1.txt | tr '\n' ' ')"
}
report_dir() { grep -o "Informe: .*" $B/run_$1.txt | sed 's/Informe: //' | tr '\' '/' | head -1; }
: > $OUT
log "inicio (rondas: ${ROUNDS:-1 2})"
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
for r in ${ROUNDS:-1 2}; do
  python -I $B/mutate.py apply $r >> $OUT 2>&1 || { log "apply $r FALLO"; break; }
  build mut$r || log "BUILD FAIL mut$r"
  run neg$r 2048 6000
  python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore $r FALLO"
  d=$(report_dir neg$r)
  log "informe neg$r: $d"
  if [ -n "$d" ]; then python -I $B/neg_summary.py "$d" "$B/negative-runs/$(basename "$d")" >> $OUT 2>&1; fi
done
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
