#!/bin/bash
# chain_neg2.sh: repite SOLO la ronda 2 de la prueba negativa (mutacion de FindCorrectRasterFormat reforzada) en una sesion del candado de juego
cd /d/Original/wv-6/reversing
B=Fn225A
OUT=$B/chain_neg2.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-6 chain225n2 $1 $(date +%T)" > "$CLK/owner_wv6"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python $B/build_probe.py > /dev/null 2>&1
  rc=$?
  rm -f "$CLK/owner_wv6"; rmdir "$CLK" 2>/dev/null
  return $rc
}
: > $OUT
python -I $B/mutate.py apply 2 >> $OUT 2>&1 || { log "apply 2 FALLO"; exit 1; }
build mut2 || log "BUILD FAIL mut2"
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
FN225A_CASES=2048 FN225A_TIMEOUT=9400 timeout 9600 python $B/run_local.py 2>&1 | tail -3 > $B/run_neg2b.txt
log "run neg2b: $(head -c 400 $B/run_neg2b.txt | tr '\n' ' ')"
python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore 2 FALLO"
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
