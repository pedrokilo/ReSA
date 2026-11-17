#!/bin/bash
# build223.sh: compila la sonda del banco Fn223A con el candado de compilacion (mkdir atomico; solo se quita el propio)
cd /d/Original/wv-6/reversing
CLK=/d/Original/compile_lock
while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
echo "wv-6 build223 $(date +%T)" > "$CLK/owner_wv6"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python Fn223A/build_probe.py > Fn223A/build_out.txt 2>&1
rc=$?
rm -f "$CLK/owner_wv6"; rmdir "$CLK" 2>/dev/null
echo "rc=$rc" >> Fn223A/build_out.txt
grep -n " error \|warning C4335" Fn223A/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-300 | sort -u | head -30 > Fn223A/build_errors.txt
exit $rc
