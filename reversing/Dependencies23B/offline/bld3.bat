@echo off
rem Arnes sin juego contra el X87Math.cpp REAL del repositorio (source/game_sa), con las opciones de source/CMakeLists.txt (/O2 /Oi /arch:IA32 /fp:fast).
rem Uso: bld3.bat ; obj3\fch.exe [casos por ruta] [ruta 0..10] [primer caso]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set SRC=%~dp0..\..\..\source\game_sa
if not exist obj3 mkdir obj3
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /I"%SRC%" "%SRC%\X87Math.cpp" /Foobj3/X87Math.obj /Fdobj3/x.pdb || exit /b 1
cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /DFLC_ANNEX /I"%SRC%" main.cpp obj3/X87Math.obj /Fe:obj3/fch.exe /Fdobj3/t.pdb /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
