#!/bin/bash
# Compila la variante Release de la DLL completa en este worktree, bajo el candado de compilacion.
# Salida: bin/Release/gta_reversed.asi (si el proyecto la admite). Registro: reversing/FnSIM1/build_release.log
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
CL=/d/Original/compile_lock
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "wv-8 SIM1 release $(date +%T)" > "$CL/owner_wv8_sim1"
trap 'rm -f "$CL/owner_wv8_sim1"; rmdir "$CL" 2>/dev/null' EXIT
export CONAN_HOME="$ROOT/reversing/CVehicle/conan-cache"
TS="$ROOT/reversing/CVehicle/tooling/Scripts"
CMK=$(ls -d "$CONAN_HOME"/p/cmake*/p/bin | head -1)
NSM=$(ls -d "$CONAN_HOME"/p/nasm*/p/bin | head -1)
export PATH="$CMK:$NSM:$PATH"
# /WX- : avisos de generacion de codigo no son errores en Release; /FI: ver release_wassert.h (no se toca el fuente)
export CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_="-MP1 /WX- /FI$(cygpath -w "$ROOT/reversing/FnSIM1/release_wassert.h")"
timeout 9000 "$TS/python.exe" setup.py --build --profile "$ROOT/reversing/FnSIM1/windows-msvc-release.txt" > reversing/FnSIM1/build_release.log 2>&1
echo "EXIT=$?" >> reversing/FnSIM1/build_release.log
