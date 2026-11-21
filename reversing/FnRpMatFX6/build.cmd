@echo off
rem Compila el banco FnRpMatFX6 (32 bits, base 0x400000 con reserva del espacio del EXE). Uso: build.cmd <directorio de salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
mkdir %OUT% 2>nul
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0..\FnLIB4A\reserva_imagen.asm || exit /b 1
cl /nologo /O2 /Oy- /GS- /arch:IA32 /MT /EHsc /Zp8 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /I%~dp0shim /I%~dp0..\..\source\app\platform\win /I%~dp0 /c /Fo%OUT%\bench.obj %~dp0bench.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench.obj libcmt.lib kernel32.lib user32.lib advapi32.lib
