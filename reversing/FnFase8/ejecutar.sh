#!/bin/bash
# FASE 8: ejecuta gta_sa_standalone.exe en la carpeta FASE8 bajo D:\Original\gta_lock. Uso: ejecutar.sh <etiqueta> <limite_s> [args extra del exe...]
# Espera (sin matar nada ajeno) a que no haya gta_sa.exe ni otros ejecutables de prueba vivos y a que el candado este libre.
ET=$1; LIM=$2; shift 2
AQUI="$(cd "$(dirname "$0")" && pwd)"
DEST='E:\JUEGOS\GTA San Andreas - CVehicle Test - FASE8'
DESTU='/e/JUEGOS/GTA San Andreas - CVehicle Test - FASE8'
ORIG='E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
SAL="$AQUI/ejecuciones/$ET-$(date +%Y%m%d-%H%M%S)"
until mkdir /d/Original/gta_lock 2>/dev/null; do sleep 2; done
echo "FASE8 $ET" > /d/Original/gta_lock/owner_fase8
while tasklist | grep -qi "gta_sa.exe\|gta_reversed_exe.exe\|gta_sa_standalone.exe"; do sleep 15; done
mkdir -p "$SAL"
cp "$AQUI/salida/gta_sa_standalone.exe" "$DESTU/gta_sa_standalone.exe"
SALW=$(cygpath -w "$SAL")
FASE8_ARGS="--ruta-original \"$ORIG\" --directorio-juego \"$DEST\" --salida \"$SALW\" --limite $LIM $*" \
  pwsh -NoProfile -File "$AQUI/lanzar.ps1" -Exe "$DEST\gta_sa_standalone.exe" -Dir "$DEST" -EsperaS $((LIM+30)) > "$SAL/lanzador.txt" 2>&1
cp "$DESTU/gta-reversed.ini" "$SAL/" 2>/dev/null
cp "$DESTU"/logs/* "$SAL/" 2>/dev/null
rm -rf /d/Original/gta_lock
echo "$SAL"; cat "$SAL/lanzador.txt"
