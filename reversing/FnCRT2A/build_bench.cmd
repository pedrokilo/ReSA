@echo off
rem Compila el banco CRT2A (bench_crt2a.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original). Reutiliza reserva_imagen.asm de FnCRT1A.
rem Uso: build_bench.cmd <directorio de salida> [directorio con las fuentes mutadas (prueba negativa)]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0....sourcegame_saCrt
mkdir %OUT% 2>nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0..\FnCRT1A\reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I"%SRC%" /Fo%OUT%\bench_crt2a.obj /c %~dp0bench_crt2a.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_crt2a.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_crt2a.obj libcmt.lib kernel32.lib user32.lib
