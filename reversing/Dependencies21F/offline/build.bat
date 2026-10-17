@echo off
rem Compila el arnes nativo con las mismas opciones que source/CMakeLists.txt aplica a D3DResourceSystem.cpp (/O2 /Oi /Oy /arch:IA32 /fp:fast).
rem Uso: build.bat <repo>      (necesita body.inc, exe_tex.bin y exe_ib.bin de prepare.py)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cl /nologo /EHsc /O2 /Oi /Oy /arch:IA32 /fp:fast /Zi /GS- /I%1\source\game_sa\RenderWare harness.cpp /Fe:harness.exe /link /DYNAMICBASE:NO /BASE:0x2000000 /FIXED
