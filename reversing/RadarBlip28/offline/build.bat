@echo off
rem Ensayo SIN juego del banco RadarBlip28 (CRadar::DrawEntityBlip, 0x587000). No sustituye a la corrida dentro del juego, que NO se ha ejecutado.
rem Ejecuta el Run() REAL de ..\Core.hpp (mismas fixtures, mismos falsos, mismas compuertas) con los bytes REALES del EXE (imagen mapeada en 0x400000 de un proceso hijo suspendido) frente a
rem CRadar::DrawEntityBlip de Radar.cpp compilada con las opciones del proyecto y a RadarAirstripRotateCpp de X87Math.cpp (/arch:IA32). El registro resultante se analiza con run_local.py --analyze.
rem No toca el directorio build\ (solo lee build\source\gta_reversed.vcxproj).
rem Uso: build.bat [raiz del repositorio] [ruta del gta-sa.exe] [casos (todos por defecto)] [registro .jsonl]
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set ROOT=%~1
if "%ROOT%"=="" set ROOT=%~dp0..\..\..
set EXE=%~2
if "%EXE%"=="" set EXE=D:\Grand Theft Auto San Andreas\gta-sa.exe
set CASES=%~3
set X87SRC=%ROOT%\source\game_sa\X87Math.cpp
if not exist obj mkdir obj
python mkrsp.py "%ROOT%" || exit /b 1
python prepare.py "%ROOT%" || exit /b 1
cl @proj.rsp /Fo"obj\Radar_h.obj" obj\Radar_h.cpp > obj\radar.log 2>&1 || (type obj\radar.log & exit /b 1)
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MTd /DNOMINMAX /I"%ROOT%\source\game_sa" "%X87SRC%" /Foobj\X87Math.obj /Fdobj\x.pdb > obj\x87.log 2>&1 || (type obj\x87.log & exit /b 1)
cl @proj.rsp /Fo"obj\hmain.obj" main.cpp > obj\hmain.log 2>&1 || (type obj\hmain.log & exit /b 1)
rem /FORCE:UNRESOLVED: los simbolos sin resolver deben ser metodos exportados que nada llama (comprobar obj\hlink.log)
link /nologo /OUT:obj\h.exe obj\hmain.obj obj\Radar_h.obj obj\X87Math.obj /OPT:REF /FORCE:UNRESOLVED /DEBUG /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO user32.lib kernel32.lib > obj\hlink.log 2>&1
if not exist obj\h.exe (type obj\hlink.log & exit /b 1)
obj\h.exe "%EXE%" %CASES% %~4
endlocal
