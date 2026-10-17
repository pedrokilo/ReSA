@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if not exist obj2 mkdir obj2
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /I../annex_x87 ../annex_x87/X87Math.cpp /Foobj2/X87Math.obj /Fdobj2/x.pdb || exit /b 1
cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /DFLC_ANNEX /I../annex_x87 main.cpp obj2/X87Math.obj /Fe:obj2/fch.exe /Fdobj2/t.pdb /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
