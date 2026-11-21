#!/bin/sh
# Prueba negativa CRT3A: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
cd /d/Original/wv-24/reversing/FnCRT3A || exit 1
EXE="E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"
SRCDIR=/d/Original/wv-24/source/game_sa/Crt
n=0
run() { # archivo, original, reemplazo, filtro
  n=$((n+1)); T=/d/Original/_crt3a_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
  python - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-24\reversing\FnCRT3A\build_bench.cmd D:\Original\_crt3a_neg$n\out D:\Original\_crt3a_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  $T/out/bench_crt3a.exe "$EXE" --casos 60000 --solo $4 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -2
  rm -rf $T
}
run CrtPow.cpp "eax &= 0xFF; eax += 1; eax &= 0x1FE;
    x7 = _mm_mul_sd(x7, LdS(0x8884F0 + eax * 4));" "eax &= 0xFF; eax += 1; eax &= 0x1FC;
    x7 = _mm_mul_sd(x7, LdS(0x8884F0 + eax * 4));" pow
run CrtPow.cpp "x6 = _mm_add_pd(x6, LdP(0x889520 + eax * 4));" "x6 = _mm_add_pd(x6, LdP(0x889530 + eax * 4));" pow
run CrtPow.cpp "    x0 = _mm_add_sd(x0, x3);
    return Ret(x0);" "    return Ret(x0);" pow
run CrtPow.cpp "edx = 0x3EE;" "edx = 0x3EF;" pow
run CrtPow.cpp "    edx = 0x18;
    if (ecx == 0) goto LA634;" "    edx = 0x19;
    if (ecx == 0) goto LA634;" pow
run CrtPow.cpp "edx = 0x7FEF;
    edx -= ecx;" "edx = 0x7FEE;
    edx -= ecx;" pow
run CrtPow.cpp "edx = 0x1B;" "edx = 0x1A;" pow
run CrtPow.cpp "if (eax < 0x3FF0u) goto LA31E;" "if (eax <= 0x3FF0u) goto LA31E;" pow
run CrtPow.cpp "x3 = _mm_sub_sd(x3, LdS(0x88BD70));" "x3 = _mm_sub_sd(x3, LdS(0x88BD80));" pow
run CrtPow.cpp "    if (UseSse2MathPath()) {
        return PowSse2(x, y);" "    if (!UseSse2MathPath()) {
        return PowSse2(x, y);" pow
run CrtException.cpp "ExeCrt::Strlen(o.m_what) + 1" "ExeCrt::Strlen(o.m_what)" exc
run CrtException.cpp "    if (doFree != 0) {
        ExeFree" "    if (doFree != 0 && false) {
        ExeFree" exc
run CrtException.cpp "    m_doFree = 0;
    m_vtbl" "    m_doFree = 1;
    m_vtbl" exc
run CrtException.cpp "ThrowRuntimeError(0x887D7C, 0x887D74, 0x8A1E54)" "ThrowRuntimeError(0x887D7C, 0x887D75, 0x8A1E54)" xran
run CrtException.cpp "ThrowRuntimeError(0x887D94, 0x887D68, 0x893634)" "ThrowRuntimeError(0x887D7C, 0x887D68, 0x893634)" xran
run CrtException.cpp "    TempStringGuard guard{&tmp};" "    (void)0;" xran
run CrtOnexit.cpp "unsigned int grow = size < 0x800 ? size : 0x800;" "unsigned int grow = size < 0x400 ? size : 0x400;" onexit
run CrtOnexit.cpp "Realloc(reinterpret_cast<void*>(TableBegin()), size + 0x10)" "Realloc(reinterpret_cast<void*>(TableBegin()), size + 0x20)" onexit
run CrtOnexit.cpp "if (size < TableEnd() - TableBegin() + 4) {" "if (size < TableEnd() - TableBegin() + 8) {" onexit
