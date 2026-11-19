#!/bin/bash
# Compila el proyecto completo de este worktree bajo el candado de compilacion.
cd /d/Original/wv-6
CL=/d/Original/compile_lock
while ! mkdir "$CL" 2>/dev/null; do sleep 15; done
echo "wv-6 RW0A $(date +%T)" > "$CL/owner_wv6"
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 timeout 5400 powershell -NoProfile -ExecutionPolicy Bypass -File reversing/CVehicle/build-project.ps1 > reversing/FnRW0A/build.log 2>&1
echo "EXIT=$?" >> reversing/FnRW0A/build.log
rm -f "$CL/owner_wv6"; rmdir "$CL" 2>/dev/null
echo done > reversing/FnRW0A/build.done
