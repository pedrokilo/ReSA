#!/bin/bash
# Compila (over\) los .cpp indicados y enlaza la ASI a mano en reversing\FnDatos\asi\gta_reversed.asi (objs de wv-int + over\), bajo D:\Original\compile_lock. Uso: build_asi.sh <cpp relativo> ...
RAIZ='D:\Original\wv-datos3'; FD=/d/Original/wv-datos3/reversing/FnDatos
until mkdir /d/Original/compile_lock 2>/dev/null; do sleep 5; done
echo "datos-g5b build_asi $*" > /d/Original/compile_lock/owner_datos_g5b
trap 'rm -rf /d/Original/compile_lock' EXIT
mkdir -p $FD/asi
for f in "$@"; do
  n=$(basename "$f" .cpp); echo "== compilar $f $(date +%T)"
  cmd.exe //c "D:\Original\wv-datos3\reversing\FnFase8\compilar_over.cmd $RAIZ $f" > $FD/cc_$n.log 2>&1
  grep -E "error|fatal" $FD/cc_$n.log | head -5
done
echo "== enlazar standalone $(date +%T)"
FN=/d/Original/wv-datos3/reversing/FnFase8
mkdir -p $FN/lib $FN/salida/imagen
cp -n /d/Original/wv-int/reversing/FnFase8/lib/*.lib $FN/lib/ 2>/dev/null
cp -n /d/Original/wv-int/reversing/FnFase8/salida/imagen/*.obj $FN/salida/imagen/ 2>/dev/null
cmd.exe //c "D:\Original\wv-datos3\reversing\FnFase8\enlazar.cmd $RAIZ D:\Original\wv-datos3\reversing\FnFase8\salida D:\Original\wv-int\build\source\gta_reversed.dir\Debug" 2>&1 | tail -3
python -I $FD/link_asi.py 'D:\Original\wv-datos3\reversing\FnDatos\asi\link.rsp'
echo "== enlazar ASI $(date +%T)"
cat > $FD/asi/l.cmd <<'X'
@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
link /nologo /DLL /DEBUG /INCREMENTAL:NO /MACHINE:X86 /SUBSYSTEM:WINDOWS /SAFESEH:NO /OUT:"D:\Original\wv-datos3\reversing\FnDatos\asi\gta_reversed.asi" /PDB:"D:\Original\wv-datos3\reversing\FnDatos\asi\gta_reversed.pdb" /MAP:"D:Originalwv-datos3eversingFnDatossigta_reversed.map" /IMPLIB:"D:\Original\wv-datos3\reversing\FnDatos\asi\gta_reversed.lib" @"D:\Original\wv-datos3\reversing\FnDatos\asi\link.rsp" > "D:\Original\wv-datos3\reversing\FnDatos\asi\link.log" 2>&1
echo link exit=%ERRORLEVEL%
X
cmd.exe //c "D:\Original\wv-datos3\reversing\FnDatos\asi\l.cmd"
tail -5 $FD/asi/link.log
echo "fin $(date +%T)"; ls -la $FD/asi/gta_reversed.asi
