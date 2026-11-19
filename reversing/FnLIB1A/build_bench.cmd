@echo off
rem Compila el banco LIB1A (bench.exe de 32 bits, base 0x400000 con reserva del espacio del EXE original) contra zlib 1.1.3 de libs/zlib-1.1.3
rem (o contra una copia mutada: segundo argumento = directorio de fuentes de zlib).
rem Uso: build_bench.cmd <directorio de salida> [directorio zlib]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set Z=%2
if "%Z%"=="" set Z=%~dp0..\..\libs\zlib-1.1.3
mkdir %OUT% 2>nul
set CF=/nologo /O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /W0 /MT /Zp8 /Zi /D_CRT_SECURE_NO_WARNINGS /I%Z%
for %%f in (adler32 zutil infblock infcodes inffast infutil) do cl %CF% /c /Fo%OUT%\%%f.obj %Z%\%%f.c || exit /b 1
cl %CF% /Dlocal= /c /Fo%OUT%\inftrees.obj %Z%\inftrees.c || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I%Z% /c /Fo%OUT%\bench.obj %~dp0bench.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench.obj %OUT%\adler32.obj %OUT%\zutil.obj %OUT%\infblock.obj %OUT%\infcodes.obj %OUT%\inffast.obj %OUT%\infutil.obj %OUT%\inftrees.obj libcmt.lib kernel32.lib user32.lib
