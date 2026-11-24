#!/bin/bash
# Compila (over\) los .cpp indicados y enlaza el standalone en reversing\FnFase8\salida con los .obj del ASI Debug de wv-int, todo bajo D:\Original\compile_lock.
# Uso: build_standalone.sh <cpp relativo> ...
RAIZ='D:\Original\wv-datos3'
FN=/d/Original/wv-datos3/reversing/FnFase8
until mkdir /d/Original/compile_lock 2>/dev/null; do sleep 5; done
echo "datos-g5b build_standalone $*" > /d/Original/compile_lock/owner_datos_g5b
trap 'rm -rf /d/Original/compile_lock' EXIT
mkdir -p $FN/lib $FN/salida/imagen
cp -n /d/Original/wv-int/reversing/FnFase8/lib/*.lib $FN/lib/ 2>/dev/null
cp -n /d/Original/wv-int/reversing/FnFase8/salida/imagen/*.obj $FN/salida/imagen/ 2>/dev/null
for f in "$@"; do
  echo "== compilar $f $(date +%T)"
  n=$(basename "$f" .cpp)
  cmd.exe //c "D:\Original\wv-datos3\reversing\FnFase8\compilar_over.cmd $RAIZ $f" > /d/Original/wv-datos3/reversing/FnDatos/cc_$n.log 2>&1
  grep -E "error|fatal" /d/Original/wv-datos3/reversing/FnDatos/cc_$n.log | head -5
done
echo "== enlazar $(date +%T)"
cmd.exe //c "D:\Original\wv-datos3\reversing\FnFase8\enlazar.cmd $RAIZ D:\Original\wv-datos3\reversing\FnFase8\salida D:\Original\wv-int\build\source\gta_reversed.dir\Debug" 2>&1 | tail -5
echo "fin $(date +%T)"
ls -la $FN/salida/gta_sa_standalone.exe $FN/salida/gta_sa_standalone.map
