@echo off
rem Compila el banco NEWDEL (bench_newdel.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original).
rem Uso: build_bench.cmd <directorio de salida> [directorio con las fuentes mutadas (prueba negativa)] [define extra]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0..\..\source\game_sa\Crt
mkdir %OUT% 2>nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS %3 /I"%SRC%" /Fd%OUT%\vc_bench.pdb /Fo%OUT%\bench_newdel.obj /c %~dp0bench_newdel.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_newdel.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_newdel.obj libcmt.lib kernel32.lib user32.lib
