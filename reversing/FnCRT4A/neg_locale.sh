#!/bin/sh
# Prueba negativa CRT4A-locale: una mutacion de una linea por corrida, recompila el banco y comprueba que detecta la diferencia.
ROOT=/d/Original/wv-32
EXE="D:\Original\_crt1a_work\gta_sa.exe"
SRCDIR=$ROOT/source/game_sa/Crt
n=0
run() { # archivo, original, reemplazo, grupo
  n=$((n+1)); T=/d/Original/_crt4l_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
  python - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8',newline='').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-32\reversing\FnCRT4A\build_bench_locale.cmd D:\Original\_crt4l_neg$n\out D:\Original\_crt4l_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  $T/out/bench_locale.exe "$EXE" --casos 100000 --solo $4 2>&1 | grep -E "RESULTADO" | head -1
  rm -rf $T
}
run CrtLocale.cpp "if (old && old->refCount == 0 &&" "if (old && old->refCount <= 0 && old->refCount != 0 &&" refcount
run CrtLocale.cpp "for (int i = 3; i < 10; i++) {" "for (int i = 3; i < 9; i++) {" refcount
run CrtLocale.cpp "if (wc <= 0xFF) {" "if (wc < 0xFF) {" api
run CrtLocale.cpp "MultiByteToWideChar(li->codePage, 9, src, cur," "MultiByteToWideChar(li->codePage, 1, src, cur," api
run CrtLocale.cpp "return typeAndMask[0] & (static_cast<unsigned int>(typeAndMask[1])" "return typeAndMask[0] & (static_cast<unsigned int>(mask)" ctype
run CrtLocale.cpp "return (c >= 0x61 && c <= 0x7A) ? c - 0x20 : c;" "return (c >= 0x61 && c < 0x7A) ? c - 0x20 : c;" ctype
run CrtCtype.cpp "return static_cast<int>(sign == '-' ? 0u - total : total);" "return static_cast<int>(sign == '+' ? 0u - total : total);" str
run CrtCtype.cpp "return l1 < l2 ? -1 : 1;" "return l1 > l2 ? -1 : 1;" str
