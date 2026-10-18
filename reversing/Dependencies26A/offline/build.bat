@echo off
rem Ensayo SIN juego de CAEAudioHardware::PlaySound (0x4D86B0): bytes reales del EXE (proceso hijo suspendido con la imagen mapeada en 0x400000) frente a PlaySound de AEAudioHardware.cpp compilado con las
rem opciones del proyecto y a AudioHwPercentPositionCpp de X87Math.cpp (/arch:IA32). Requiere g26a.patch aplicado y el anexo g26a-x87 en X87Math (o G26A_OVERLAY con una copia de game_sa/ con ambos).
rem No toca el directorio build\ (solo lee build\source\gta_reversed.vcxproj).
rem Uso: build.bat [raiz del repositorio] [ruta del gta-sa.exe] [casos (2048 por defecto)] [registro .jsonl]
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set ROOT=%~1
if "%ROOT%"=="" set ROOT=%~dp0..\..\..
set EXE=%~2
if "%EXE%"=="" set EXE=D:\Grand Theft Auto San Andreas\gta-sa.exe
set CASES=%~3
if "%CASES%"=="" set CASES=2048
set OV=
if defined G26A_OVERLAY set OV=/I"%G26A_OVERLAY%" /I"%G26A_OVERLAY%\game_sa"
set X87SRC=%ROOT%\source\game_sa\X87Math.cpp
if defined G26A_OVERLAY set X87SRC=%G26A_OVERLAY%\game_sa\X87Math.cpp
if not exist obj mkdir obj
python mkrsp.py "%ROOT%" || exit /b 1
python prepare.py "%ROOT%" %G26A_OVERLAY% || exit /b 1
cl %OV% @proj.rsp /Fo"obj\Hw_h.obj" obj\Hw_h.cpp > obj\hw.log 2>&1 || (type obj\hw.log & exit /b 1)
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MTd /DNOMINMAX %OV% /I"%ROOT%\source\game_sa" "%X87SRC%" /Foobj\X87Math.obj /Fdobj\x.pdb > obj\x87.log 2>&1 || (type obj\x87.log & exit /b 1)
cl %OV% @proj.rsp /Fo"obj\hmain.obj" main.cpp > obj\hmain.log 2>&1 || (type obj\hmain.log & exit /b 1)
rem /FORCE:UNRESOLVED: los simbolos sin resolver son metodos exportados (NOTSA_EXPORT_VTABLE) que nada llama; ninguno de los colaboradores esta entre ellos (comprobado en obj\hlink.log)
link /nologo /OUT:obj\h.exe obj\hmain.obj obj\Hw_h.obj obj\X87Math.obj /OPT:REF /FORCE:UNRESOLVED /DEBUG /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO user32.lib kernel32.lib > obj\hlink.log 2>&1
if not exist obj\h.exe (type obj\hlink.log & exit /b 1)
obj\h.exe "%EXE%" %CASES% %~4
endlocal
