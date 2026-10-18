@echo off
rem Arnes sin juego del banco Dependencies25D: bytes del EXE (hijo con la imagen mapeada) frente al C++ de rwcore.cpp / rwplcore.cpp (rw_cpp.inc). Opciones del proyecto en Debug: /Od /RTC1.
rem Uso: bld.bat [rwcore.cpp] [rwplcore.cpp] ; obj\rw25d.exe [casos] [salida.jsonl] [indice de funcion] ; python ..\run_local.py --analyze salida.jsonl
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
python -B extract.py %1 %2 || exit /b 1
if not exist obj mkdir obj
set NLOH=%~dp0..\..\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
cl /nologo /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /I"%NLOH%" main.cpp /Fe:obj/rw25d.exe /Fdobj/t.pdb /Foobj/ /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO || exit /b 1
