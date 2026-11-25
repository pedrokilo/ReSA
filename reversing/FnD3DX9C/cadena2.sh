#!/bin/bash
# FnD3DX9C: segunda cadena: comportamiento anterior (--d3dx9-dll) con la DLL ausente (degradacion) y verificacion larga.
cd /d/Original/wv-d3dx/reversing/FnD3DX9C || exit 1
EXE='D:/Original/wv-d3dx/reversing/FnFase8/salida/gta_sa_standalone.exe'
CARP='D:\Original\_d3dx_run'
while [ ! -f out/cadena_fin.txt ]; do sleep 10; done
pwsh -NoProfile -File crear_carpeta_d3dx.ps1 > out/crear_sin_dll3.txt 2>&1
python -I correr_d3dx.py dll_ausente "$CARP" "$EXE" 130 20,60,120 '--bloquear=d3dx9_;d3dcompiler_4' -- --d3dx9-dll --d3dx9-log
echo FIN > out/cadena2_fin.txt
