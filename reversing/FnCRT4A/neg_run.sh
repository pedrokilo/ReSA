#!/bin/sh
# Prueba negativa CRT4A: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
cd /d/Original/wv-25/reversing/FnCRT4A || exit 1
EXE="E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"
SRCDIR=/d/Original/wv-25/source/game_sa/Crt
n=0
run() { # archivo, original, reemplazo, grupo
  n=$((n+1)); T=/d/Original/_crt4a_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
  python - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8',newline='').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-25\reversing\FnCRT4A\build_bench.cmd D:\Original\_crt4a_neg$n\out D:\Original\_crt4a_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  $T/out/bench_crt4a.exe "$EXE" --casos 300000 --solo $4 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -2
  rm -rf $T
}
run CrtFloat.cpp "if (W(&x)[5] >= 0x3FFF) {" "if (W(&x)[5] > 0x3FFF) {" i10
run CrtFloat.cpp "if (roundDigit >= '5') {" "if (roundDigit > '5') {" i10
run CrtFloat.cpp "if (acc[0] > 0x8000 || low == 0x18000u) {" "if (acc[0] >= 0x8000 || low == 0x18000u) {" ld12
run CrtFloat.cpp "if (*p == dp) {" "if (*p == dp + 1) {" cvt
run CrtFormat.cpp "hexAdd = (type == 'x') ? 0x27 : 7;" "hexAdd = (type == 'x') ? 0x26 : 7;" output
run CrtFormat.cpp "flags &= ~FL_LEADZERO;" ";" output
run CrtInput.cpp "if (width < 1) {" "if (width < 0) {" scanf
run CrtInput.cpp "bx = (bx & ~0x20) - 7;" "bx = (bx & ~0x20) - 8;" scanf
run CrtFloatIn.cpp "if (!Ld12TailZero(w, bit)) {" "if (Ld12TailZero(w, bit)) {" floatin
run CrtFloatIn.cpp "if (ndig > 0x18) {" "if (ndig > 0x19) {" floatin
run CrtFloatIn.cpp "if (digits[0x17] >= 5) {" "if (digits[0x17] > 5) {" floatin
