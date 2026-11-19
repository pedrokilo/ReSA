#!/bin/bash
# Compila el proyecto (Debug) con GTASA_WITH_ZLIB_LIBPNG encendida y luego apagada, bajo el candado de compilacion.
# Uso: bash build_lib1a.sh   (salida en build_lib1a.out; escribe 'done' al final)
cd /d/Original/wv-5
OUT=reversing/FnLIB1A/build_lib1a.out
CM=$(ls reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe)
CLK=/d/Original/compile_lock
: > $OUT
while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
echo "wv-5 LIB1A $(date +%T)" > "$CLK/owner_wv5"
trap 'rm -f "$CLK/owner_wv5"; rmdir "$CLK" 2>/dev/null' EXIT
export CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1
for st in ON OFF; do
  echo "== $(date +%T) configurar GTASA_WITH_ZLIB_LIBPNG=$st" >> $OUT
  "$CM" -S . -B build -DGTASA_WITH_ZLIB_LIBPNG=$st >> reversing/FnLIB1A/cfg_$st.log 2>&1; echo "cfg rc=$?" >> $OUT
  echo "== $(date +%T) compilar $st" >> $OUT
  timeout 7000 "$CM" --build build --config Debug > reversing/FnLIB1A/build_$st.log 2>&1; rc=$?
  echo "build $st rc=$rc" >> $OUT
  grep -E " error |error LNK|fatal error" reversing/FnLIB1A/build_$st.log | sort -u | head -10 >> $OUT
  [ -f bin/Debug/gta_reversed.asi ] && sha256sum bin/Debug/gta_reversed.asi >> $OUT
  ls -l bin/Debug/gta_reversed.asi >> $OUT 2>&1
  [ $rc -ne 0 ] && break
done
echo done >> $OUT
