#!/bin/sh
# Prueba negativa CRT2A: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
cd /d/Original/wv-22/reversing/FnCRT2A || exit 1
EXE="E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"
SRCDIR=/d/Original/wv-22/source/game_sa/Crt
n=0
run() { # archivo, original, reemplazo, filtro
  n=$((n+1)); T=/d/Original/_crt2a_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $T/src/
  python - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-22\reversing\FnCRT2A\build_bench.cmd D:\Original\_crt2a_neg$n\out D:\Original\_crt2a_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  $T/out/bench_crt2a.exe "$EXE" --casos 60000 --solo $4 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -3
  rm -rf $T
}
run CrtMath.cpp "result = (x < t) ? SubSd(t, 1.0) : t;" "result = (x <= t) ? SubSd(t, 1.0) : t;" mate
run CrtMath.cpp "result = !(x <= t) ? AddSd(t, 1.0) : t;" "result = (x > t + 0.5) ? AddSd(t, 1.0) : t;" mate
run CrtMath.cpp "if (static_cast<int32_t>(e12) < 0xBFF) {  " "if (static_cast<int32_t>(e12) < 0xBFE) {  " mate
run CrtState.cpp "ptd->holdrand * 0x343FDu + 0x269EC3u" "ptd->holdrand * 0x343FDu + 0x269EC2u" rand
run CrtState.cpp "(next >> 16) & 0x7FFF" "(next >> 15) & 0x7FFF" rand
run CrtState.cpp "return start == p ? nullptr" "return start == p + 1 ? nullptr" strtok
run CrtState.cpp "ptd->holdrand = 1;" "ptd->holdrand = 2;" ptd
run CrtState.cpp "return table[c] & 0x157;" "return table[c] & 0x156;" isprint
