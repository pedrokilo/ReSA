#!/bin/bash
# build_run.sh: compila la sonda (candado de compilacion) y corre el banco (candado de juego)
cd /d/Original/wv-5
CL=/d/Original/compile_lock
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "wv-5 Fn215A $(date +%T)" > "$CL/owner_fn215a"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 python reversing/Fn215A/build_probe.py > /dev/null 2>&1
rcb=$?
rm -f "$CL/owner_fn215a"; rmdir "$CL" 2>/dev/null
if [ $rcb -ne 0 ]; then echo "BUILD FAIL"; grep -n " error \|error C" reversing/Fn215A/artifacts/probe-build.log | cut -c1-300 | head -8; exit 1; fi
bash reversing/Fn215A/run_main.sh
