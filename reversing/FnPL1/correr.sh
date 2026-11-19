#!/bin/bash
# PL1: espera a que no haya gta_sa.exe ni candado, lanza una variante en modo automatico y escribe <salida>.done
# uso: correr.sh <salida> <argumentos de jugar.ps1...>
out="$1"; shift
here="$(cd "$(dirname "$0")" && pwd -W)"
while :; do
  if tasklist | grep -qi gta_sa.exe; then sleep 20; continue; fi
  pwsh -NoProfile -File "$here\jugar.ps1" "$@" > "$out.tmp" 2>&1
  if grep -q "esta tomado por otra prueba\|ya esta en marcha" "$out.tmp"; then sleep 20; continue; fi
  break
done
mv "$out.tmp" "$out"; echo done > "$out.done"
