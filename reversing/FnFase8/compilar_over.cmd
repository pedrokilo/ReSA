@echo off
setlocal EnableDelayedExpansion
rem Compila un .cpp del proyecto con las opciones del ASI (sin PCH) a over\<nombre>.obj (gana a obj\). Uso: compilar_over.cmd <repo> <ruta .cpp relativa al repo, con / o \>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
set FN=%~1\reversing\FnFase8
set P=%~2
set P=!P:/=\!
for %%f in ("!P!") do set N=%%~nf
if not exist "%FN%\over" mkdir "%FN%\over"
cl @"%FN%\cl.rsp" /MP1 /Fo"%FN%\over\!N!.obj" "%~1\!P!"
exit /b %ERRORLEVEL%
