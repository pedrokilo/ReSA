@echo off
rem Compila el banco P3-S2 (bench_d3d.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original).
rem Uso: build_bench_d3d.cmd <directorio de salida> [directorio con D3D9RasterDriver.cpp/.h (prueba negativa)]
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set SRC=%2
if "%SRC%"=="" set SRC=%~dp0..\..\source\game_sa\RenderWare
mkdir %OUT%\src\rw 2>nul
copy /y %SRC%\D3D9RasterDriver.cpp %OUT%\src >nul
copy /y %SRC%\D3D9RasterDriver.h %OUT%\src >nul
copy /y %~dp0shim\StdInc.h %OUT%\src >nul
copy /y %~dp0shim\D3DResourceSystem.h %OUT%\src >nul
copy /y %~dp0shim\rw\rwplcore.h %OUT%\src\rw >nul
copy /y %~dp0shim\rw\rwcore.h %OUT%\src\rw >nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0..\FnCRT1A\reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /Oy- /arch:IA32 /MT /EHsc /std:c++17 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I"%OUT%\src" /Fo%OUT%\bench_d3d.obj /c %~dp0bench_d3d.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_d3d.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_d3d.obj libcmt.lib kernel32.lib user32.lib
