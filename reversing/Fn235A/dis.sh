#!/bin/sh
# dis.sh inicio fin (hex con 0x): desensamblado del EXE canonico
D="/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86"
"$D/dumpbin.exe" //disasm //range:$1,$2 "D:\Grand Theft Auto San Andreas\gta-sa.exe" 2>&1 | sed -n '9,5000p' | grep -E "^  [0-9A-F]{8}:" | sed -E 's/^  ([0-9A-F]{8}): ([0-9A-F]{2} )+ *//; s/^([0-9A-F]{8}):/\1:/' 
