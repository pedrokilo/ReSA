#!/bin/bash
# chain230.sh: UNA sola sesion del candado de juego: compila, corrida principal (16384 casos por funcion) y, si aprueba, 2 rondas de prueba negativa (2048 casos por funcion) y la compilacion final con las fuentes buenas.
cd /d/Original/wv-5/reversing
B=Fn230A
OUT=$B/chain.out
CLK=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
log() { echo "$(date +%T) $*" >> $OUT; }
build() {
  while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
  echo "wv-5 chain230 $1 $(date +%T)" > "$CLK/owner_wv5"
  CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 python $B/build_probe.py > $B/build.out 2>&1
  rc=$?
  rm -f "$CLK/owner_wv5"; rmdir "$CLK" 2>/dev/null
  if [ $rc -ne 0 ]; then grep -n " error \|warning C4335" $B/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-260 | sort -u -t: -k3 | head -12 >> $OUT; fi
  return $rc
}
run() { # $1 = etiqueta, $2 = casos
  for _ in $(seq 1 360); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
  FN230A_CASES=$2 timeout 9600 python $B/run_local.py 2>&1 | tail -4 > $B/run_$1.txt
  log "run $1: $(head -c 700 $B/run_$1.txt | tr '\n' ' ')"
}
report_dir() { grep -o "Informe: .*" $B/run_$1.txt | sed 's/Informe: //' | tr '\' '/' | head -1; }
[ -n "$KEEP_LOG" ] || : > $OUT
log "inicio"
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
trap 'rmdir "$LOCK" 2>/dev/null' EXIT
log "gta_lock tomado"
build good-main || { log "BUILD FAIL good-main"; exit 1; }
log "build ok main"
run main 16384
if ! grep -q APROBADO $B/run_main.txt; then log "corrida principal sin aprobar: se aborta (sin rondas negativas)"; exit 1; fi
for r in 1 2; do
  python -I $B/mutate.py apply $r >> $OUT 2>&1 || { log "apply $r FALLO"; break; }
  build mut$r || log "BUILD FAIL mut$r"
  run neg$r 2048
  python -I $B/mutate.py restore >> $OUT 2>&1 || log "restore $r FALLO"
  d=$(report_dir neg$r)
  log "informe neg$r: $d"
  if [ -n "$d" ]; then python -I $B/neg_summary.py "$d" "$B/negative-runs/$(basename "$d")" >> $OUT 2>&1; fi
done
build good-final || log "BUILD FAIL good-final"
log "fin"
rm -f $B/artifacts/*.pdb
