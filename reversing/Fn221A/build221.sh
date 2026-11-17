#!/bin/bash
# build221.sh: compila la sonda del banco Fn221A con el candado de compilacion (mkdir atomico; solo se quita el propio)
cd /d/Original/wv-6/reversing
CLK=/d/Original/compile_lock
while ! mkdir "$CLK" 2>/dev/null; do sleep 15; done
echo "wv-6 build221 $(date +%T)" > "$CLK/owner_wv6"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 3000 python Fn221A/build_probe.py > Fn221A/build_out.txt 2>&1
rc=$?
rm -f "$CLK/owner_wv6"; rmdir "$CLK" 2>/dev/null
echo "rc=$rc" >> Fn221A/build_out.txt
grep -n " error \|warning C4335" Fn221A/artifacts/probe-build.log | sed 's/\[D:.*//' | cut -c1-260 | sort -u | head -20 > Fn221A/build_errors.txt
exit $rc
