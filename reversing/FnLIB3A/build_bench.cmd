@echo off
rem Compila el banco LIB3A (bench.exe de 32 bits, base 0x400000 con reserva del espacio del EXE original) contra libjpeg 6b de libs/libjpeg-6b.
rem Uso: build_bench.cmd <directorio de salida> [directorio libjpeg]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set J=%2
if "%J%"=="" set J=%~dp0..\..\libs\libjpeg-6b
mkdir %OUT% 2>nul
set CF=/nologo /O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /W0 /MT /Zp8 /Zi /D_CRT_SECURE_NO_WARNINGS /I%J%
set OBJS=
for %%f in (%J%\wrap\w_*.c) do (if /I "%%~nf"=="w_jmemnobs" (cl %CF% /Dmalloc=lib3b_malloc /Dfree=lib3b_free /c /Fo%OUT%\%%~nf.obj %%f || exit /b 1) else (cl %CF% /c /Fo%OUT%\%%~nf.obj %%f || exit /b 1))
cl %CF% /c /Fo%OUT%\jpeg6b_exports.obj %J%\jpeg6b_exports.c || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I%J% /c /Fo%OUT%\bench.obj %~dp0bench.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench.obj %OUT%\jpeg6b_exports.obj %OUT%\w_*.obj libcmt.lib kernel32.lib user32.lib
