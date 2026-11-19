@echo off
rem Compila las fuentes de zlib 1.1.3 y libpng 1.0.12 a .obj sueltos (con /Gy y listados .asm) solo para el analisis del mapa EXE <-> fuente.
rem Uso: compilar_objs.cmd <directorio de salida> [/O2|/Od]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
set OUT=%1
set OPT=%2
if "%OPT%"=="" set OPT=/O2
mkdir %OUT% 2>nul
set CF=%OPT% /Ob2 /Oy- /GS- /Gy /Gd /arch:IA32 /W0 /MT /Zp8 /D_CRT_SECURE_NO_WARNINGS /FAs /Fa%OUT%\ /Fo%OUT%\ /c
cl /nologo %CF% /I..\..\libs\zlib-1.1.3 ..\..\libs\zlib-1.1.3\*.c
cl /nologo %CF% /I..\..\libs\zlib-1.1.3 /I..\..\libs\libpng-1.0.12 ..\..\libs\libpng-1.0.12\*.c
