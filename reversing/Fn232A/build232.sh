#!/bin/bash
# build232.sh: compila la sonda Fn232A con el candado de compilacion (mkdir atomico; solo se retira el propio).
cd /d/Original/wv-5/reversing
B=Fn232A
CLK=/d/Original/compile_lock
while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
echo "wv-5 build232 $(date +%T)" > "$CLK/owner_wv5"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 python $B/build_probe.py > $B/build.out 2>&1
rc=$?
rm -f "$CLK/owner_wv5"; rmdir "$CLK" 2>/dev/null
echo "rc=$rc" >> $B/build.out
if [ $rc -ne 0 ]; then grep -n " error \|warning C4335" $B/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-300 | sort -u -t: -k3 | head -20 >> $B/build.out; fi
