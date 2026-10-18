@echo off
rem Prueba aislada del ayudante x87 WndProcTimerPeriodCpp (anexo g24a de X87Math) frente a los bytes REALES de _ftol2 (0x821B40, ftol_bytes.h) tras `fild dword [n]; fdivr dword [1000.0f]`:
rem 390585 casos (45 + 30000 valores de frameLimit x 13 FSW iniciales), FNSAVE completo (FCW, FSW, FTW y los diez bytes de los ocho registros, vacios incluidos) y EAX.
rem Uso: build.bat [raiz del repositorio]   (con el anexo ya aplicado a X87Math)
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
set ROOT=%~1
if "%ROOT%"=="" set ROOT=%~dp0..\..\..\..
if not exist obj mkdir obj
cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /I"%ROOT%\source\game_sa" "%ROOT%\source\game_sa\X87Math.cpp" /Foobj\X87Math.obj /Fdobj\x.pdb || exit /b 1
cl /nologo /c /std:c++latest /EHsc /utf-8 /Od /Zi /MT /DNOMINMAX /I"%ROOT%\source\game_sa" ftol2.cpp /Foobj\ftol2.obj /Fdobj\f.pdb || exit /b 1
link /nologo /OUT:obj\ftol2.exe obj\ftol2.obj obj\X87Math.obj /DEBUG /LARGEADDRESSAWARE:NO || exit /b 1
obj\ftol2.exe
endlocal
