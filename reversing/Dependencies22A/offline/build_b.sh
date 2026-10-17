#!/bin/sh
. ./env.sh
set -e
./gen_inc.sh
"$CL" /nologo /c /std:c++20 /O2 /Oi /arch:IA32 /Zi /fp:fast /FAs /Fa"x87.asm" /Fo"x87.obj" /I inc X87Math_plus.cpp
"$CL" /nologo /c /std:c++20 /Od /Zi /I shim /I inc /I .. /I D:/Original/gta-reversed-master/source/game_sa harness_b.cpp /Fo harness_b.obj
"$LINK" /nologo /OUT:harness_b.exe /BASE:0x20000000 /FIXED /DEBUG /INCREMENTAL:NO harness_b.obj x87.obj
