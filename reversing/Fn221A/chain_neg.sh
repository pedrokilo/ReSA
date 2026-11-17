#!/bin/bash
# chain_neg.sh: dos rondas de prueba negativa del banco Fn221A (compilar con mutaciones, correr, restaurar) en UNA sola sesion del candado de juego
cd /d/Original/wv-6/reversing
B=Fn221A
OUT=$B/chain_neg.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-6 chain221neg $1 $(date +%T)" > "$CLK/owner_wv6"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python $B/build_probe.py > /dev/null 2>&1
  rc=$?
  rm -f "$CLK/owner_wv6"; rmdir "$CLK" 2>/dev/null
  return $rc
}
: > $OUT
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
for r in 1 2; do
  python -I $B/mutate.py apply $r >> $OUT 2>&1 || { log "apply $r FALLO"; break; }
  build mut$r || { log "BUILD FAIL mut$r"; python -I $B/mutate.py restore >> $OUT 2>&1; break; }
  for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  FN221A_CASES=2048 FN221A_TIMEOUT=9400 timeout 9600 python $B/run_local.py 2>&1 | tail -3 > $B/run_neg$r.txt
  log "run neg$r: $(head -c 400 $B/run_neg$r.txt | tr '\n' ' ')"
  python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore $r FALLO"
done
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
