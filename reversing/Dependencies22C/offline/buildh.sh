#!/bin/sh
D="C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231"
K="C:/Program Files (x86)/Windows Kits/10"; V=10.0.26100.0
export INCLUDE="$D/include;$K/Include/$V/ucrt;$K/Include/$V/um;$K/Include/$V/shared"
export LIB="$D/lib/x86;$K/Lib/$V/ucrt/x86;$K/Lib/$V/um/x86"
B="/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86"
src=$1; shift
MSYS_NO_PATHCONV=1 "$B/cl.exe" /nologo /std:c++20 /O1 /EHsc /Fe"${src%.cpp}.exe" /Fo"${src%.cpp}_o.obj" "$src" "$@" /link /BASE:0x10000000 /DYNAMICBASE:NO 2>&1 | tail -15
