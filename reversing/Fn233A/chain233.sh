#!/bin/bash
# chain231.sh: una sola sesion del candado del juego con la corrida principal de Fn233A (16384 casos por funcion) y las 2 rondas de la prueba negativa (2048 casos por funcion).
# Compila con el candado de compilacion (D:/Original/compile_lock, mkdir atomico; solo se retira el propio). Resultados en chain.out. Reanudable a mano: cada fase deja una linea FASE ... en chain.out.
cd /d/Original/wv-6 || exit 1
B=reversing/Fn233A
OUT=/d/Original/wv-6/$B/chain.out
CL=/d/Original/compile_lock
LOCK=/d/Original/gta_lock
say() { echo "$(date +%T) $*" >> "$OUT"; }
build() {   # build <etiqueta>
    while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
    echo "agente 233A $1 $(date +%T)" > "$CL/owner_233A"
    CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python $B/build_probe.py > /d/Original/wv-6/$B/build_$1.out 2>&1
    local rc=$?
    rm -f "$CL/owner_233A"; rmdir "$CL" 2>/dev/null
    return $rc
}
: > "$OUT"
say "FASE inicio: compilar la sonda buena"
build main || { say "BUILD FAIL main"; exit 1; }
cp $B/artifacts/build-manifest.json $B/artifacts/build-manifest.main.json
while ! mkdir "$LOCK" 2>/dev/null; do sleep 10; done
echo "agente 233A cadena $(date +%T)" > "$LOCK/owner_233A"
trap 'rm -f "$LOCK/owner_233A"; rmdir "$LOCK" 2>/dev/null' EXIT
say "FASE candado del juego obtenido"
for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
say "FASE corrida principal"
FN233A_CASES=16384 FN233A_TIMEOUT=9000 timeout 9500 python $B/run_local.py > $B/run_main.txt 2>&1
say "FASE corrida principal terminada rc=$? ($(head -c 300 $B/run_main.txt | tr '\n' ' '))"
if ! head -1 "$B/run_main.txt" | grep -q "^APROBADO"; then
    say "FASE la corrida principal NO aprobo: no se hacen las rondas negativas"
    exit 2
fi
for R in 1 2; do
    say "FASE ronda negativa $R: aplicar mutaciones"
    python -I $B/mutate.py apply $R >> "$OUT" 2>&1 || { say "mutate apply fallo"; python -I $B/mutate.py restore >> "$OUT" 2>&1; exit 3; }
    build neg$R || { say "BUILD FAIL ronda $R"; python -I $B/mutate.py restore >> "$OUT" 2>&1; exit 4; }
    for _ in $(seq 1 120); do tasklist 2>/dev/null | grep -qi "^gta_sa.exe" || break; sleep 10; done
    FN233A_CASES=2048 FN233A_TIMEOUT=3000 timeout 3400 python $B/run_local.py > $B/run_neg$R.txt 2>&1
    say "FASE ronda negativa $R terminada rc=$? ($(head -c 300 $B/run_neg$R.txt | tr '\n' ' '))"
    python -I $B/mutate.py restore >> "$OUT" 2>&1
done
cp $B/artifacts/build-manifest.main.json $B/artifacts/build-manifest.json
rm -f $B/artifacts/*.pdb
say "FASE fin"
