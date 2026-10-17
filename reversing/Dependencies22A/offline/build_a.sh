#!/bin/sh
. ./env.sh
set -e
./gen_inc.sh
"$CL" /nologo /c /std:c++20 /O2 /Oi /arch:IA32 /Zi /fp:fast /FAs /Fa"x87.asm" /Fo"x87.obj" /I inc X87Math_plus.cpp
"$CL" /nologo /c /std:c++20 /Od /Zi /I shim /I inc /I D:/Original/gta-reversed-master/source/game_sa /FAs /Fa"harness_a.asm" /Fo"harness_a.obj" harness_a.cpp
"$LINK" /nologo /OUT:harness_a.exe /BASE:0x20000000 /FIXED /DEBUG /INCREMENTAL:NO harness_a.obj x87.obj
