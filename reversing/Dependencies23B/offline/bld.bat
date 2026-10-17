@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if not exist obj mkdir obj
if not exist obj\x87ok (
 cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /I../micro/../x87src ../x87src/X87Math.cpp /Foobj/X87Math.obj /Fdobj/x.pdb || exit /b 1
 echo ok> obj\x87ok
)
cl /nologo /c /std:c++latest /EHsc /O1 /Zi /MT /DNOMINMAX ../soft/flc_soft.cpp /Foobj/soft.obj /Fdobj/s.pdb || exit /b 1
cl /nologo /c /std:c++latest /EHsc /O1 /Zi /MT /DNOMINMAX ../soft/flc_macros.cpp /Foobj/macros.obj /Fdobj/m.pdb || exit /b 1
cl /nologo /c /std:c++latest /EHsc /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX ../soft/flc_commit.cpp /Foobj/commit.obj /Fdobj/c.pdb || exit /b 1
cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX main.cpp obj/soft.obj obj/macros.obj obj/commit.obj obj/X87Math.obj /Fe:obj/fch.exe /Fdobj/t.pdb /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
