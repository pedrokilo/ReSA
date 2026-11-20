@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
cl /nologo /Zs /arch:IA32 /Zp8 /W3 /D_CRT_SECURE_NO_WARNINGS /DPNG_USER_MEM_SUPPORTED /I%~dp0..\..\libs\zlib-1.1.3 /I%~dp0..\..\libs\libpng-1.0.12 /I%~dp0 /EHsc %~dp0bench.cpp
