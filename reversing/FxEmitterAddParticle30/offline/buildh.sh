#!/bin/sh
# buildh.sh <arnes.cpp> [opciones]: compila X87Math.cpp (fuente de produccion, /O2 /Oi /arch:IA32 /fp:fast) y el arnes (por defecto /O1 /EHsc) y los enlaza.
D="C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231"
K="C:/Program Files (x86)/Windows Kits/10"; V=10.0.26100.0
export INCLUDE="$D/include;$K/Include/$V/ucrt;$K/Include/$V/um;$K/Include/$V/shared;$EXTRA_INC"
export LIB="$D/lib/x86;$K/Lib/$V/ucrt/x86;$K/Lib/$V/um/x86"
B="$D/bin/Hostx64/x86"
HERE="$(cd "$(dirname "$0")" && pwd -W)"
SRC="$HERE/../../../source/game_sa"
src=$1; shift
OPT=${OPT:-/O1}
MSYS_NO_PATHCONV=1 "$B/cl.exe" /nologo /std:c++20 /c /O2 /Oi /arch:IA32 /Zi /fp:fast /I"$SRC" /Fo"$HERE/X87Math_prod.obj" /Fd"$HERE/x87.pdb" "$SRC/X87Math.cpp" 2>&1 | grep -v "^X87Math.cpp$"
MSYS_NO_PATHCONV=1 "$B/cl.exe" /nologo /std:c++20 $OPT /EHsc /Fe"${src%.cpp}.exe" /Fo"${src%.cpp}_o.obj" /Fd"${src%.cpp}.pdb" "$src" "$@" "$HERE/X87Math_prod.obj" /link /BASE:0x10000000 /DYNAMICBASE:NO 2>&1 | tail -30
