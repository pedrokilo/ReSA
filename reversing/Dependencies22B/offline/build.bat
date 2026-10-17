@echo off
rem Compila y ejecuta el ensayo sin juego. Requiere x87src\ (make_x87src.py), glue_head.txt / glue_tail.txt (extract_glue.py) y el include de nlohmann/json (ruta en NLOHMANN).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if "%NLOHMANN%"=="" set NLOHMANN=%~dp0..\..\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
if not exist obj mkdir obj
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /Ix87src x87src\X87Math.cpp /Foobj\X87Math.obj /Fdobj\x.pdb || exit /b 1
cl /nologo /c /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /Ix87src /I. /I.. /I"%NLOHMANN%" rehearsal.cpp /Foobj\rehearsal.obj /Fdobj\r.pdb || exit /b 1
link /nologo /OUT:obj\rehearsal.exe obj\rehearsal.obj obj\X87Math.obj /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /DEBUG /LARGEADDRESSAWARE:NO || exit /b 1
echo Uso: obj\rehearsal.exe [casos=4096] [primer caso=0] [salida.jsonl] [-1 ambas, 0 Head, 1 Tail]
