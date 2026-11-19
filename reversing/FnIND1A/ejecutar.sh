#!/bin/bash
# IND1A: ejecuta gta_reversed_exe.exe en la carpeta IND1A bajo D:\Original\gta_lock. Uso: ejecutar.sh <etiqueta> <limite_s> [args extra del exe...]
# Espera (sin matar nada ajeno) a que no haya gta_sa.exe ni gta_reversed_exe.exe vivos y a que el candado este libre.
ET=$1; LIM=$2; shift 2
AQUI="$(cd "$(dirname "$0")" && pwd)"
DEST='E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A'
ORIG='E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
SAL="$AQUI/ejecuciones/$ET-$(date +%Y%m%d-%H%M%S)"
until mkdir /d/Original/gta_lock 2>/dev/null; do sleep 0.2; done
echo "IND1A $ET" > /d/Original/gta_lock/owner_ind1a
while tasklist | grep -qi "gta_sa.exe\|gta_reversed_exe.exe"; do sleep 15; done
mkdir -p "$SAL"
cp "$AQUI/salida/gta_reversed_exe.exe" "/e/JUEGOS/GTA San Andreas - CVehicle Test - IND1A/gta_reversed_exe.exe"
SALW=$(cygpath -w "$SAL")
IND_ARGS="--ruta-original \"$ORIG\" --directorio-juego \"$DEST\" --salida \"$SALW\" --ejecutar --limite $LIM $*" \
  pwsh -NoProfile -File "$AQUI/lanzar_carga.ps1" -Exe "$DEST\gta_reversed_exe.exe" -Dir "$DEST" -EsperaS $((LIM+30)) > "$SAL/lanzador.txt" 2>&1
cp "/e/JUEGOS/GTA San Andreas - CVehicle Test - IND1A/gta-reversed.ini" "$SAL/" 2>/dev/null
cp /e/JUEGOS/GTA\ San\ Andreas\ -\ CVehicle\ Test\ -\ IND1A/logs/* "$SAL/" 2>/dev/null
rm -rf /d/Original/gta_lock
echo "$SAL"; cat "$SAL/lanzador.txt"
