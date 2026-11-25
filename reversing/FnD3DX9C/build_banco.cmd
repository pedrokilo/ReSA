@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cd /d %~dp0
if not exist out mkdir out
cl /nologo /O2 /EHsc /std:c++20 /arch:SSE2 banco_matematica.cpp ..\..\source\game_sa\RenderWare\rw\D3DX9NativeMath.cpp /Fe:out\banco_matematica.exe /Fo:out\ /link /MACHINE:X86 2>&1
