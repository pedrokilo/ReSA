#!/bin/bash
# ronda 4: enlaza gta_sa_standalone.exe de wv-r4 con los .obj de wv-int. Log: /d/Original/_build_R4.log
LOG=/d/Original/_build_R4.log
echo "inicio $(date)" > "$LOG"
cd /d/Original/wv-r4 || exit 1
cmd //c "reversing\FnFase8\enlazar.cmd D:\Original\wv-r4 D:\Original\wv-r4\reversing\FnFase8\salida D:\Original\wv-int\build\source\gta_reversed.dir\Debug" >> "$LOG" 2>&1
echo "EXIT=$?" >> "$LOG"
echo "fin $(date)" >> "$LOG"
