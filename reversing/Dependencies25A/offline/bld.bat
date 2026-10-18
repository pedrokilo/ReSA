@echo off
rem Arnes sin juego de Dependencies25A: bytes del EXE (hijo con la imagen mapeada) frente al C++ del arbol de fuentes (inc/*.inc, extraidos sin cambios).
rem Uso: bld.bat [raiz_de_source] ; obj\bank25a.exe [casos] [salida.jsonl] ; python ..\run_local.py --analyze salida.jsonl
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
python -B extract.py %1 || exit /b 1
if not exist obj mkdir obj
if "%NLOH%"=="" set NLOH=%~dp0..\..\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
rem Sin /RTC1: CTask::Destructor se optimiza por pragma, que solo vale sin comprobaciones de ejecucion (como en el proyecto, ver CMakeLists.txt).
cl /nologo /std:c++latest /EHsc /utf-8 /Od /Zi /MT /DNOMINMAX /I"%NLOH%" main.cpp /Fe:obj/bank25a.exe /Fdobj/t.pdb /Foobj/ /link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO /INCREMENTAL:NO user32.lib || exit /b 1
