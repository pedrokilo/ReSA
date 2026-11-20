@echo off
rem Compila libjpeg 6b (libs/libjpeg-6b) a .obj + .asm (/FAs) para mapear funciones contra el EXE. Uso: compilar_objs.cmd <dir salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set J=%~dp0..\..\libs\libjpeg-6b
mkdir %OUT% 2>nul
set CF=/nologo /O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /W0 /MT /Zp8 /FAs /FIexe_prelude.h /D_CRT_SECURE_NO_WARNINGS /I%J%
for %%f in (%J%\*.c) do cl %CF% /c /Fo%OUT%\%%~nf.obj /Fa%OUT%\%%~nf.asm %%f >nul || echo FALLO %%f
