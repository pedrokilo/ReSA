@echo off
rem Compila el banco EXT1A (32 bits, base 0x400000 con la reserva del espacio del EXE original). Uso: build.cmd <directorio de salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
mkdir %1 2>nul
ml /nologo /c /Fo%1\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /c /Fo%1\bench.obj %~dp0bench.cpp || exit /b 1
link /nologo /DEBUG /OUT:%1\bench.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %1\reserva.obj %1\bench.obj libcmt.lib kernel32.lib user32.lib
