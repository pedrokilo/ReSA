#!/bin/sh
# Compila el arnes offline en la carpeta de trabajo dada (por defecto el scratchpad de la sesion).
W="${1:-/c/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/9be5de3f-58af-4ed3-a0f7-2492b320b96d/scratchpad/sw60}"
R="${2:-D:/Original/wc-5}"
cmd //c "$(cygpath -w "$R/reversing/StatsWrappers60/offline/build_offline.bat") $(cygpath -w "$R") $(cygpath -w "$W")" 2>&1
