@echo off
rem Ensayo SIN juego de Win::__MainWndProc (0x747EB0): bytes reales del EXE (proceso hijo suspendido con la imagen mapeada en 0x400000) frente a __MainWndProc de WndProc.cpp compilado con las
rem opciones del proyecto. Requiere g24a.patch aplicado (y el anexo g24a-x87 en X87Math). No toca el directorio build\ (solo lee build\source\gta_reversed.vcxproj).
rem Uso: build.bat [raiz del repositorio] [ruta del gta-sa.exe] [casos por ruta (1024 por defecto)]
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set ROOT=%~1
if "%ROOT%"=="" set ROOT=%~dp0..\..\..
set EXE=%~2
if "%EXE%"=="" set EXE=D:\Grand Theft Auto San Andreas\gta-sa.exe
set CASES=%~3
if "%CASES%"=="" set CASES=1024
if not exist obj mkdir obj
python mkrsp.py "%ROOT%" || exit /b 1
python prepare.py "%ROOT%" || exit /b 1
cl @proj.rsp /Fo"obj\WndProc_h.obj" obj\WndProc_h.cpp > obj\wph.log 2>&1 || (type obj\wph.log & exit /b 1)
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MTd /DNOMINMAX /I"%ROOT%\source\game_sa" "%ROOT%\source\game_sa\X87Math.cpp" /Foobj\X87Math.obj /Fdobj\x.pdb > obj\x87.log 2>&1 || (type obj\x87.log & exit /b 1)
cl @proj.rsp /Fo"obj\hmain.obj" main.cpp > obj\hmain.log 2>&1 || (type obj\hmain.log & exit /b 1)
rem /FORCE:UNRESOLVED: los simbolos sin resolver son metodos exportados (NOTSA_EXPORT_VTABLE) que nada llama; ninguno de los colaboradores esta entre ellos (comprobado en obj\hlink.log)
link /nologo /OUT:obj\h.exe obj\hmain.obj obj\WndProc_h.obj obj\X87Math.obj /OPT:REF /FORCE:UNRESOLVED /DEBUG /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO user32.lib kernel32.lib strmiids.lib dxguid.lib > obj\hlink.log 2>&1
if not exist obj\h.exe (type obj\hlink.log & exit /b 1)
obj\h.exe "%EXE%" %CASES%
endlocal
