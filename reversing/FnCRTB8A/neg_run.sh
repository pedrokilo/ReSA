#!/bin/sh
# Prueba negativa CRTB8A: aplica una mutacion de una linea al C++, recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run.sh   (ajustar WT y EXE si se mueve el arbol)
WT=/d/Original/wv-crtb
EXE='D:\Original\_crt1a_work\gta_sa.exe'
SRCDIR=$WT/source/game_sa/Crt
PY=python
n=0
run() { # archivo, original, reemplazo, seccion
  n=$((n+1)); T=/d/Original/_crtb8a_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
  $PY -I - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-crtb\reversing\FnCRTB8A\build_bench.cmd D:\Original\_crtb8a_neg$n\out D:\Original\_crtb8a_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  cmd.exe //c "D:\Original\_crtb8a_neg$n\out\bench_crtb8a.exe $EXE --casos 1500 --solo $4" 2>&1 | grep -E "DIFERENCIA|RESULTADO|EXCEPCION" | head -2
  rm -rf $T
}
run CrtMbcs.cpp "constexpr unsigned int CP_TABLE_COUNT = 5;" "constexpr unsigned int CP_TABLE_COUNT = 4;" mbcs
run CrtMbcs.cpp "case 0x3B5: return 0x412;" "case 0x3B5: return 0x413;" mbcs
run CrtMbcs.cpp "for (unsigned int c = 1; c < 0xFF; ++c) {" "for (unsigned int c = 1; c < 0xFE; ++c) {" mbcs
run CrtMbcs.cpp "if (!block || block->refCount != 0) {" "if (!block || block->refCount == 0) {" mbcs
run CrtMbcs.cpp "    if (cp == MbCodePage()) {
        return 0;
    }" "    if (false) {
        return 0;
    }" mbcs
run CrtMbcs.cpp "            if (trail == 0) {
                return nullptr;
            }" "            if (false) {
                return nullptr;
            }" mbcs
run CrtMbcs.cpp "return count - left - 1;" "return count - left;" mbcs
run CrtMbcs.cpp "        chars[0] = 0x20;" "        chars[0] = 0;" mbcs
run CrtMbcs.cpp "        ++current->refCount;" "        (void)current;" mbcs
run CrtMbcs.cpp "MbcCaseMap()[i] = static_cast<uint8_t>(i + 0x20);" "MbcCaseMap()[i] = static_cast<uint8_t>(i + 0x21);" mbcs
run CrtMbcs.cpp "    if (result == -1 && block != PtMbcInfo()) {" "    if (false) {" mbcs
run CrtMbcs.cpp "                    for (unsigned int c = low; c <= high; ++c) {
                        MbcType()[c + 1] |= CpGroupFlag()[group];" "                    for (unsigned int c = low; c < high; ++c) {
                        MbcType()[c + 1] |= CpGroupFlag()[group];" mbcs
run CrtStartup.cpp "    for (int i = 0; i < 8; ++i) {
        *reinterpret_cast<volatile uint8_t*>(0x8E37B4 + i) = 0xFF;" "    for (int i = 0; i < 7; ++i) {
        *reinterpret_cast<volatile uint8_t*>(0x8E37B4 + i) = 0xFF;" tablac
run CrtStartup.cpp "        Sse2FlagB() = 1;" "        Sse2FlagB() = 0;" tablac
run CrtStartup.cpp "? 0 : 1;
}" "? 1 : 0;
}" tablac
run CrtStartup.cpp "DEFAULT_SECURITY_COOKIE = 0xBB40E64E;" "DEFAULT_SECURITY_COOKIE = 0xBB40E64F;" tablac
run CrtStartup.cpp "    value ^= GetTickCount();" "    value ^= 0;" tablac
run CrtStartup.cpp "    PrevXcptFilter() = reinterpret_cast<uint32_t>(SetUnhandledExceptionFilter(" "    reinterpret_cast<uint32_t>(SetUnhandledExceptionFilter(" tablac
run CrtStartup.cpp "    table[5] = 0x826E61;" "    table[5] = 0x826E62;" tablac
run CrtStartup.cpp "    _controlfp(0x10000, 0x30000);" "    _controlfp(0x20000, 0x30000);" tablac
run CrtStartup.cpp "            return present(0);" "            return !present(0);" tablac
