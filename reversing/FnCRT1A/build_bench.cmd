@echo off
rem Compila el banco CRT1A (bench_crt.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original).
rem Uso: build_bench.cmd <directorio de salida> [directorio con CrtQsort.cpp y CrtString.cpp mutados (prueba negativa)]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0..\..\source\game_sa\Crt
mkdir %OUT% 2>nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I"%SRC%" /Fo%OUT%\bench_crt.obj /c %~dp0bench_crt.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_crt.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_crt.obj libcmt.lib kernel32.lib user32.lib
