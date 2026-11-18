#!/bin/bash
# chain228.sh: corrida principal + 2 rondas de prueba negativa del banco Fn228A en UNA sola sesion del candado de juego.
cd /d/Original/wv-6/reversing
B=Fn228A
OUT=$B/chain.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-6 chain228 $1 $(date +%T)" > "$CLK/owner_wv6"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python $B/build_probe.py > /dev/null 2>&1
  rc=$?
  rm -f "$CLK/owner_wv6"; rmdir "$CLK" 2>/dev/null
  return $rc
}
run() { # $1 = etiqueta
  for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  FN228A_TIMEOUT=9400 timeout 9600 python $B/run_local.py 2>&1 | tail -3 > $B/run_$1.txt
  log "run $1: $(head -c 300 $B/run_$1.txt | tr '\n' ' ')"
}
: > $OUT
build good-main || { log "BUILD FAIL good-main"; exit 1; }
log "build ok main"
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
run main
if ! grep -q '^APROBADO' $B/run_main.txt; then log "main sin aprobar: se omiten las rondas negativas"; rm -f $B/artifacts/*.pdb; exit 2; fi
for r in 1 2; do
  python -I $B/mutate.py apply $r >> $OUT 2>&1 || { log "apply $r FALLO"; break; }
  build mut$r || log "BUILD FAIL mut$r"
  FN228A_CASES=2048 run neg$r
  python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore $r FALLO"
done
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
