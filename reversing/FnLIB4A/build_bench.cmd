@echo off
rem Compila los bancos LIB4A (32 bits, base 0x400000 con reserva del espacio del EXE) contra zlib 1.1.3 y libpng 1.0.12 de libs/ (/arch:IA32), con -Dlocal= en deflate.c y trees.c.
rem Uso: build_bench.cmd <directorio de salida> <banco: deflate|png>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set OUT=%1
set W=%~2
set P=%~dp0..\..\libs\libpng-1.0.12
set Z=%~dp0..\..\libs\zlib-1.1.3
mkdir %OUT% 2>nul
set CF=/nologo /O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /W0 /MT /Zp8 /Zi /D_CRT_SECURE_NO_WARNINGS /DPNG_USER_MEM_SUPPORTED /DPNG_NO_PROGRESSIVE_READ /DPNG_NO_STDIO /DPNG_NO_READ_iTXt /DPNG_NO_WRITE_iTXt /DPNG_NO_WRITE_tIME /I%Z% /I%P%
for %%f in (adler32 compress crc32 infblock infcodes inffast inflate inftrees infutil zutil) do cl %CF% /c /Fo%OUT%\z_%%f.obj %Z%\%%f.c || exit /b 1
for %%f in (deflate trees) do cl %CF% /Dlocal= /c /Fo%OUT%\z_%%f.obj %Z%\%%f.c || exit /b 1
if "%W%"=="png" for %%f in (png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran) do cl %CF% /c /Fo%OUT%\p_%%f.obj %P%\%%f.c || exit /b 1
if "%W%"=="png" cl %CF% /FI%P%\exe_funciones_estaticas.h /c /Fo%OUT%\p_pngwutil.obj %P%\pngwutil.c || exit /b 1
ml /nologo /c /Fo%OUT%\reserva.obj %~dp0reserva_imagen.asm || exit /b 1
cl /nologo /O2 /GS- /arch:IA32 /MT /EHsc /Zp8 /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /DPNG_USER_MEM_SUPPORTED /DPNG_NO_PROGRESSIVE_READ /DPNG_NO_STDIO /DPNG_NO_READ_iTXt /DPNG_NO_WRITE_iTXt /DPNG_NO_WRITE_tIME /I%Z% /I%P% /I%~dp0 /c /Fo%OUT%\bench_%W%.obj %~dp0bench_%W%.cpp || exit /b 1
set PO=
if "%W%"=="png" set PO=%OUT%\p_*.obj
link /nologo /DEBUG /MAP:%OUT%\bench_%W%.map /OUT:%OUT%\bench_%W%.exe /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO %OUT%\reserva.obj %OUT%\bench_%W%.obj %OUT%\z_*.obj %PO% libcmt.lib kernel32.lib user32.lib
