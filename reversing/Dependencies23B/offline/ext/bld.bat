@echo off
rem Prueba de plantado exacto de T-1 (ext_test.cpp) contra el X87Math.cpp REAL del repositorio y los bytes del EXE (D:\Grand Theft Auto San Andreas\gta-sa.exe).
rem Uso: bld.bat ; obj\ext_test.exe [casos] [0 = asin | 1 = acos | 2 = commit]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set SRC=%~dp0..\..\..\..\source\game_sa
if not exist obj mkdir obj
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /I"%SRC%" "%SRC%\X87Math.cpp" /Foobj/X87Math.obj /Fdobj/x.pdb || exit /b 1
cl /nologo /std:c++latest /EHsc /utf-8 /Od /Zi /MT /DNOMINMAX /I"%SRC%" ext_test.cpp obj/X87Math.obj /Fe:obj/ext_test.exe /Fdobj/t.pdb /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
