@echo off
rem Compila el ensayo sin juego. Requiere glue.txt (extract_glue.py) y el include de nlohmann/json (ruta en NLOHMANN).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if "%NLOHMANN%"=="" set NLOHMANN=D:\Original\gta-reversed-master\reversing\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
if "%X87SRC%"=="" set X87SRC=%~dp0..\work\tree\source\game_sa
if "%FIXDIR%"=="" set FIXDIR=%~dp0..\..\g23a-bank
if not exist obj mkdir obj
if not "%SKIPX87%"=="1" cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /fp:fast /Zi /MT /DNOMINMAX %XDEF% /I"%X87SRC%" "%X87SRC%\X87Math.cpp" /Foobj\X87Math.obj /Fdobj\x.pdb /FS /FAs /Faobj\X87Math.cod || exit /b 1
cl /nologo /c /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /I"%X87SRC%" /I. /I"%FIXDIR%" /I"%NLOHMANN%" rehearsal.cpp /Foobj\rehearsal.obj /Fdobj\r.pdb /FS /FAs /Faobj\rehearsal.cod || exit /b 1
link /nologo /OUT:obj\rehearsal.exe obj\rehearsal.obj obj\X87Math.obj /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /DEBUG /INCREMENTAL:NO /LARGEADDRESSAWARE:NO || exit /b 1
echo Uso: obj\rehearsal.exe [casos=4096] [primer caso=0] [salida.jsonl o -] [1 = solo estado vivo]
