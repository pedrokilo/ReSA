@echo off
rem Compila el banco CRTB8A (bench_crtb8a.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original). reserva_imagen.asm es copia de FnCRT1A.
rem Uso: build_bench.cmd <directorio de salida> [directorio con las fuentes mutadas (prueba negativa)]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0..\..\source\game_sa\Crt
mkdir %OUT% 2>nul
set CLF=/nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /DCRT_BENCH /I"%SRC%" /I"%~dp0." /Fd%OUT%\vc_bench.pdb
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl %CLF% /Fo%OUT%\bench_crtb8a.obj /c %~dp0bench_crtb8a.cpp || exit /b 1
cl %CLF% /Fo%OUT%\bench_startup.obj /c %~dp0bench_startup_wrap.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_crtb8a.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_crtb8a.obj %OUT%\bench_startup.obj libcmt.lib kernel32.lib user32.lib
