@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if not exist out mkdir out
python -I extraer_generador.py
cl /nologo /O2 /EHsc /std:c++20 /arch:SSE2 banco_ensamblador.cpp ..\..\source\game_sa\RenderWare\rw\D3DX9NativeAssembler.cpp /Fe:out\banco_ensamblador.exe /Fo:out\ /link /MACHINE:X86
