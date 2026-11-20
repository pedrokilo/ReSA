@echo off
rem Compila el banco LIB2A (bench.exe de 32 bits, base 0x400000 con reserva del espacio del EXE original) contra libpng 1.0.12 y zlib 1.1.3 de libs/ (compiladas con /arch:IA32).
rem Uso: build_bench.cmd <directorio de salida> [directorio libpng] [directorio zlib]   (los directorios opcionales son copias mutadas para la prueba negativa)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set P=%2
set Z=%3
if "%P%"=="" set P=%~dp0..\..\libs\libpng-1.0.12
if "%Z%"=="" set Z=%~dp0..\..\libs\zlib-1.1.3
mkdir %OUT% 2>nul
set CF=%LIBX% /nologo /O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /W0 /MT /Zp8 /Zi /D_CRT_SECURE_NO_WARNINGS /DPNG_USER_MEM_SUPPORTED /I%Z% /I%P%
for %%f in (adler32 crc32 deflate infblock infcodes inffast inflate inftrees infutil trees zutil) do cl %CF% /c /Fo%OUT%\z_%%f.obj %Z%\%%f.c || exit /b 1
for %%f in (png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil) do cl %CF% /c /Fo%OUT%\p_%%f.obj %P%\%%f.c || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /arch:IA32 /MT /EHsc /Zp8 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /DPNG_USER_MEM_SUPPORTED /I%Z% /I%P% /I%~dp0 /c /Fo%OUT%\bench.obj %~dp0bench.cpp || exit /b 1
link /nologo /DEBUG /OUT:%OUT%\bench.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench.obj %OUT%\z_*.obj %OUT%\p_*.obj libcmt.lib kernel32.lib user32.lib
