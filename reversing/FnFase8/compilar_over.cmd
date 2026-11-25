@echo off
setlocal EnableDelayedExpansion
rem Compila un .cpp del proyecto con las opciones del ASI (sin PCH) a over\<nombre>.obj (gana a obj\). Uso: compilar_over.cmd <repo> <ruta .cpp relativa al repo, con / o \>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
set FN=%~1\reversing\FnFase8
set P=%~2
set P=!P:/=\!
for %%f in ("!P!") do set N=%%~nf
if not exist "%FN%\over" mkdir "%FN%\over"
powershell -NoProfile -ExecutionPolicy Bypass -File "%FN%\expandir_rsp.ps1" "%~1" "%FN%\cl.rsp" "%FN%\over\cl.efectivo.rsp" || exit /b 8
rem Archivos con opciones propias en source/CMakeLists.txt (/O2 /Oi /arch:IA32 /fp:fast): X87Math.cpp tiene una guarda (ResidueHelpersAreTrivial) que falla con /Od
if /I "!N!"=="X87Math" powershell -NoProfile -Command "(Get-Content -LiteralPath '%FN%\over\cl.efectivo.rsp') -replace '^/Od$','/O2 /Oi /arch:IA32 /fp:fast' | Set-Content -LiteralPath '%FN%\over\cl.efectivo.rsp'"
if /I "!N!"=="Vector" powershell -NoProfile -Command "(Get-Content -LiteralPath '%FN%\over\cl.efectivo.rsp') -replace '^/Od$','/O2 /Oi /arch:IA32 /fp:fast' | Set-Content -LiteralPath '%FN%\over\cl.efectivo.rsp'"
cl @"%FN%\over\cl.efectivo.rsp" /MP1 /Fo"%FN%\over\!N!.obj" "%~1\!P!"
exit /b %ERRORLEVEL%
