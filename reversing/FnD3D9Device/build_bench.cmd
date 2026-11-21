@echo off
rem Compila el banco bench_d3d9sys (32 bits, base 0x400000 con la reserva del espacio del EXE). Uso: build_bench.cmd <directorio de salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
mkdir %OUT% 2>nul
cd /d %~dp0..\..
"C:\Users\pedro\AppData\Local\Programs\Python\Python314\python.exe" reversing\FnD3D9Device\extract_bench.py %~dp0bloque_d3d9sys.inc || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /MT /EHsc /Zp8 /Zi /W3 /std:c++latest /D_CRT_SECURE_NO_WARNINGS /I%~dp0 /c /Fo%OUT%\bench_d3d9sys.obj %~dp0bench_d3d9sys.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_d3d9sys.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_d3d9sys.obj libcmt.lib kernel32.lib user32.lib
