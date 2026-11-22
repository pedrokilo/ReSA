@echo off
rem Compila el arnes del lote RW5 (MSVC x86). Genera antes core_*.inc desde las fuentes del proyecto.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
python extraer.py
cl /nologo /std:c++20 /EHsc /O1 /W3 /wd4996 /wd4244 /wd4267 /wd4838 /wd4305 /arch:SSE2 harness.cpp /Fe:harness.exe /link /BASE:0x400000 /FIXED /DYNAMICBASE:NO /MERGE:.text=.aaa /MERGE:.rdata=.aaa /MERGE:.data=.aaa
