#!/bin/bash
# Compila uno o varios .cpp del arbol con las opciones del ASI (sin PCH) a reversing\FnFase8\over\<n>.obj, bajo D:\Original\compile_lock. Uso: compilar.sh <ruta .cpp relativa> ...
RAIZ='D:\Original\wv-datos3'
until mkdir /d/Original/compile_lock 2>/dev/null; do sleep 5; done
echo "datos-g5b compilar $*" > /d/Original/compile_lock/owner_datos_g5b
trap 'rm -rf /d/Original/compile_lock' EXIT
for f in "$@"; do
  echo "== $f $(date +%T)"
  cmd.exe //c "D:\Original\wv-datos3\reversing\FnFase8\compilar_over.cmd $RAIZ $f" 2>&1 | tail -15
done
echo "fin $(date +%T)"
