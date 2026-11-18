#!/bin/bash
# build239.sh <etiqueta>: compila la sonda de Fn239A con el candado de compilacion (mkdir atomico; solo se retira el propio)
cd /d/Original/wv-6 || exit 1
B=reversing/Fn239A
CL=/d/Original/compile_lock
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "agente 239A $1 $(date +%T)" > "$CL/owner_239A"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3300 python $B/build_probe.py > $B/build_$1.out 2>&1
rc=$?
rm -f "$CL/owner_239A"; rmdir "$CL" 2>/dev/null
echo "rc=$rc" >> $B/build_$1.out
exit $rc
