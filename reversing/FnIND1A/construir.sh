#!/bin/bash
# IND1A: re-enlaza gta_reversed_exe.exe (solo cargador + .obj ya compilados) bajo D:\Original\compile_lock
cd "$(dirname "$0")/../.." || exit 1
until mkdir /d/Original/compile_lock 2>/dev/null; do sleep 20; done
echo IND1A > /d/Original/compile_lock/owner_ind1a
cmd //c "reversing\FnIND0A\prototipo\enlazar_exe_prueba.cmd D:\Original\wv-6 D:\Original\wv-6\reversing\FnIND1A\salida" 2>&1 | grep -v vswhere | tail -5
rm -rf /d/Original/compile_lock
