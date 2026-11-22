#!/bin/sh
# Prueba negativa CRTF8A: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run.sh  (desde cualquier directorio; ajustar WT y EXE si se mueve el arbol)
WT=/d/Original/wv-crtf8
EXE='D:\Original\_crt1a_work\gta_sa.exe'
SRCDIR=$WT/source/game_sa/Crt
PY=/d/Original/gta-reversed-master/reversing/CVehicle/tooling/Scripts/python.exe
n=0
run() { # archivo, original, reemplazo, filtro
  n=$((n+1)); T=/d/Original/_crtf8a_neg$n; rm -rf $T; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
  $PY -I - "$T/src/$1" "$2" "$3" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd.exe //c "D:\Original\wv-crtf8\reversing\FnCRTF8A\build_bench.cmd D:\Original\_crtf8a_neg$n\out D:\Original\_crtf8a_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $1: '$2' -> '$3'"
  cmd.exe //c "D:\Original\_crtf8a_neg$n\out\bench_crtf8a.exe $EXE --casos 1500 --solo $4" 2>&1 | grep -E "DIFERENCIA|RESULTADO|EXCEPCION" | head -2
  rm -rf $T
}
run CrtExceptionDtor.cpp "    exception->m_vtbl = reinterpret_cast<const void*>(0x887EA0);
    exception->Destruct();" "    exception->m_vtbl = reinterpret_cast<const void*>(0x887EA0);" dtor
run CrtExceptionDtor.cpp "reinterpret_cast<const void*>(0x887EC4)" "reinterpret_cast<const void*>(0x887EC0)" dtor
run CrtExceptionDtor.cpp "        Free(const_cast<char*>(exception->m_what));" "        (void)0;" dtor
run CrtExceptionDtor.cpp "DeletingDtor(&DtorBody822EFA, self, flags)" "DeletingDtor(&DtorBody822F94, self, flags)" dtor
run CrtStartup.cpp "numSlash /= 2;" "numSlash /= 1;" args
run CrtStartup.cpp "if (inQuote && p[1] == '\"') {" "if (p[1] == '\"') {" args
run CrtStartup.cpp "    if (c == '\0') {
        --p;" "    if (c == '\0') {
        (void)0;" args
run CrtStartup.cpp "    ArgCount()  = numArgs - 1;" "    ArgCount()  = numArgs;" args
run CrtStartup.cpp "        if (*p != '=') {
            char* const copy" "        if (true) {
            char* const copy" env
run CrtStartup.cpp "    EnvInitialized() = 1;" "    EnvInitialized() = 0;" env
run CrtStartup.cpp "static_cast<int>(BlockEnd(wide) - wide) + 1;" "static_cast<int>(BlockEnd(wide) - wide);" env
run CrtStartup.cpp "        runTable(0x8A5A34, 0x8A5A3C);" "        (void)0;" exit
run CrtStartup.cpp "callerReturned = static_cast<uint8_t>(retcaller);" "callerReturned = 0;" exit
run CrtStartup.cpp "                next        = OnexitEnd() - 4;
                more        = next >= OnexitBegin();" "                next        = next - 4;
                more        = next >= OnexitBegin();" exit
run CrtStartup.cpp "    exitFlag       = 1;" "    exitFlag       = 0;" exit
