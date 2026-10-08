#!/bin/sh
# dis.sh start end
D="/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86"
"$D/dumpbin.exe" //disasm //range:$1,$2 "D:\Grand Theft Auto San Andreas\gta-sa.exe" 2>&1 | sed -n '9,$p' | sed -E 's/^  ([0-9A-F]{8}): ([0-9A-F]{2} )+ +/\1: /; s/  +/ /g' | grep -v "^\s*$\|^ [0-9A-F][0-9A-F] \|Summary\|\.text\|^ *[0-9A-F]* \.\|nop$\|int 3"
