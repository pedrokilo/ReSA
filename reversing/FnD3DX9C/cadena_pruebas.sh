#!/bin/bash
# FnD3DX9C: cadena de ejecuciones del standalone (cada una toma D:\Original\gta_lock dentro de correr_d3dx.py). Uso: bash cadena_pruebas.sh
cd /d/Original/wv-d3dx/reversing/FnD3DX9C || exit 1
EXE='D:\Original\wv-d3dx\reversing\FnFase8\salida\gta_sa_standalone.exe'
CARP='D:\Original\_d3dx_run'
run() { python -I correr_d3dx.py "$@" ; }
pwsh -NoProfile -File crear_carpeta_d3dx.ps1 > out/crear_sin_dll.txt 2>&1
run propio2 "$CARP" "$EXE" 130 20,60,120
run dll_log "$CARP" "$EXE" 130 20,60,120 -- --d3dx9-dll --d3dx9-log
run verificar "$CARP" "$EXE" 130 20,60,120 -- --d3dx9-verificar --d3dx9-log
run bloqueo "$CARP" "$EXE" 130 20,60,120 '--bloquear=d3dx9_;d3dcompiler_4' -- --d3dx9-log
pwsh -NoProfile -File crear_carpeta_d3dx.ps1 -ConDll > out/crear_con_dll.txt 2>&1
run fallback_carpeta "$CARP" "$EXE" 130 20,60,120 '--bloquear=system32\d3dx9;syswow64\d3dx9;system32\d3dcompiler;syswow64\d3dcompiler' -- --d3dx9-dll --d3dx9-log
run propio_con_dll_en_carpeta "$CARP" "$EXE" 130 20,60,120 -- --d3dx9-log
pwsh -NoProfile -File crear_carpeta_d3dx.ps1 > out/crear_sin_dll2.txt 2>&1
run exe_anterior "$CARP" "D:/Original/wv-int/reversing/FnFase8/salida/gta_sa_standalone.exe" 130 20,60,120
echo FIN > out/cadena_fin.txt
