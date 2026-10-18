@echo off
rem Arnes sin juego de Dependencies25B: bytes del EXE (hijo con la imagen mapeada) frente al C++ de rwcore.cpp/rpanisot.cpp/rpworld.cpp (cand.inc). Opciones del proyecto en Debug: /Od /RTC1.
rem Uso: bld.bat [raiz rw] ; obj\g25b.exe [casos] [salida.jsonl] ; python ..\run_local.py --analyze salida.jsonl
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
python -B extract.py %1 || exit /b 1
if not exist obj mkdir obj
if not defined NLOH set NLOH=%~dp0..\..\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /I"%NLOH%" main.cpp /Fe:obj/g25b.exe /Fdobj/t.pdb /Foobj/ /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
