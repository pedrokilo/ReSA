@echo off
rem Compila el banco bench_tristrip (32 bits, base 0x400000 con la reserva del espacio del EXE). Uso: build_bench.cmd <directorio de salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
mkdir %OUT% 2>nul
cd /d %~dp0..\..
python reversing\FnS31AFILE\extract_bench.py %~dp0bloque_s31a.inc || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /MT /EHsc /Zp8 /Zi /W3 /std:c++latest /D_CRT_SECURE_NO_WARNINGS /I%~dp0 /c /Fo%OUT%\bench_tristrip.obj %~dp0bench_tristrip.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench_tristrip.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_tristrip.obj libcmt.lib kernel32.lib user32.lib
