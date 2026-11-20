#!/bin/bash
# Compila el proyecto (Debug) con GTASA_WITH_LIBJPEG apagada (reversing/CVehicle/build-project.ps1, que configura con Conan) y despues encendida (cmake sobre el mismo build/),
# bajo el candado de compilacion. Uso: bash build_lib3a.sh   (salida en build_lib3a.out; escribe 'done' al final)
cd /d/Original/wv-15
OUT=reversing/FnLIB3A/build_lib3a.out
CLK=/d/Original/compile_lock
: > $OUT
while ! mkdir "$CLK" 2>/dev/null; do sleep 20; done
echo "wv-15 LIB3A $(date +%T)" > "$CLK/owner_wv15"
trap 'rm -f "$CLK/owner_wv15"; rmdir "$CLK" 2>/dev/null' EXIT
export CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1
echo "== $(date +%T) OFF (por defecto): build-project.ps1" >> $OUT
powershell.exe -NoProfile -ExecutionPolicy Bypass -File reversing/CVehicle/build-project.ps1 > reversing/FnLIB3A/build_OFF.log 2>&1; rc=$?
echo "build OFF rc=$rc $(date +%T)" >> $OUT
grep -E " error |error LNK|fatal error" reversing/FnLIB3A/build_OFF.log | sort -u | head -10 >> $OUT
[ -f bin/Debug/gta_reversed.asi ] && sha256sum bin/Debug/gta_reversed.asi >> $OUT && ls -l bin/Debug/gta_reversed.asi >> $OUT
if [ $rc -eq 0 ]; then
  CM=$(ls reversing/CVehicle/conan-cache/p/cmake*/p/bin/cmake.exe | head -1)
  echo "== $(date +%T) ON: cmake -DGTASA_WITH_LIBJPEG=ON" >> $OUT
  "$CM" -S . -B build -DGTASA_WITH_LIBJPEG=ON > reversing/FnLIB3A/cfg_ON.log 2>&1; echo "cfg ON rc=$?" >> $OUT
  timeout 7000 "$CM" --build build --config Debug > reversing/FnLIB3A/build_ON.log 2>&1; rc=$?
  echo "build ON rc=$rc $(date +%T)" >> $OUT
  grep -E " error |error LNK|fatal error" reversing/FnLIB3A/build_ON.log | sort -u | head -10 >> $OUT
  [ -f bin/Debug/gta_reversed.asi ] && sha256sum bin/Debug/gta_reversed.asi >> $OUT && ls -l bin/Debug/gta_reversed.asi >> $OUT
fi
echo done >> $OUT
