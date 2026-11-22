@echo off
rem Compila el banco D3DSTD-INST (bench_inst.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original).
rem Uso: build_bench_inst.cmd <directorio de salida> [rpworld_p6b.cpp alternativo (prueba negativa)]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0..\..\source\game_sa\RenderWare\rw\rpworld_p6b.cpp
mkdir %OUT%\src 2>nul
python %~dp0extract_inst.py %SRC% %OUT%\src\inst_func.inc || exit /b 1
copy /y %~dp0shim\StdInc.h %OUT%\src >nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0..\FnCRT1A\reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /Fd"%OUT%/vc.pdb" /FS /W3 /D_CRT_SECURE_NO_WARNINGS /I"%OUT%\src" /Fo%OUT%\bench_inst.obj /c %~dp0bench_inst.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_inst.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_inst.obj libcmt.lib kernel32.lib user32.lib
