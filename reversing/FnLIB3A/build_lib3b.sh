#!/bin/bash
# LIB3B: compila el proyecto (Debug) con GTASA_WITH_LIBJPEG apagada y despues encendida, FORZANDO la opcion en cada pasada (la cache de CMake conserva el valor anterior),
# bajo el candado de compilacion. Uso: bash build_lib3b.sh   (salida en build_lib3b.out; escribe 'done' al final)
cd /d/Original/wv-15
OUT=reversing/FnLIB3A/build_lib3b.out
CLK=/d/Original/compile_lock
: > $OUT
while ! mkdir "$CLK" 2>/dev/null; do sleep 20; done
echo "wv-15 LIB3B $(date +%T)" > "$CLK/owner_wv15"
trap 'rm -f "$CLK/owner_wv15"; rmdir "$CLK" 2>/dev/null' EXIT
export CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1
CM=$(ls reversing/CVehicle/conan-cache/p/cmake*/p/bin/cmake.exe | head -1)
for MODE in OFF ON; do
  echo "== $(date +%T) $MODE: cmake -DGTASA_WITH_LIBJPEG=$MODE" >> $OUT
  "$CM" -S . -B build -DGTASA_WITH_LIBJPEG=$MODE > reversing/FnLIB3A/cfg_$MODE.log 2>&1; echo "cfg $MODE rc=$?" >> $OUT
  timeout 7000 "$CM" --build build --config Debug > reversing/FnLIB3A/build_3b_$MODE.log 2>&1; rc=$?
  echo "build $MODE rc=$rc $(date +%T)" >> $OUT
  grep -E " error |error LNK|fatal error" reversing/FnLIB3A/build_3b_$MODE.log | sort -u | head -10 >> $OUT
  [ -f bin/Debug/gta_reversed.asi ] && sha256sum bin/Debug/gta_reversed.asi >> $OUT && ls -l bin/Debug/gta_reversed.asi >> $OUT
  [ $rc -ne 0 ] && break
done
echo done >> $OUT
