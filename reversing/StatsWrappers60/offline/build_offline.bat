@echo off
rem Compila el arnes offline de StatsWrappers60 (x86). Uso: build_offline.bat <raiz del repo> <carpeta de trabajo>
rem Requiere cl (vcvars32) y nlohmann/json (ruta en NLOHMANN). Produce harness.dll (las funciones y los falsos) y harness.exe (host sin CRT enlazado en 0x400000).
setlocal
set ROOT=%~1
set WORK=%~2
set SRC=%ROOT%\reversing\StatsWrappers60\offline
if "%NLOHMANN%"=="" set NLOHMANN=D:\Original\gta-reversed-master\reversing\CVehicle\conan-cache\p\nlohmd014ef7748f4b\p\include
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
python "%SRC%\gen_port.py" "%ROOT%" "%WORK%\gen" || exit /b 1
cd /d "%WORK%"
cl /nologo /c /std:c++20 /O2 /Oi /arch:IA32 /fp:fast /Zi /Fo:x87.obj /Fd:x87.pdb /I"%WORK%\gen" "%SRC%\x87.cpp" || exit /b 1
cl /nologo /c /EHsc /std:c++20 /Od /Zi /Fo:harness.obj /Fd:harness.pdb /I"%NLOHMANN%" /I"%WORK%\gen" "%SRC%\harness.cpp" || exit /b 1
link /nologo /DLL /OUT:harness.dll /BASE:0x20000000 /DEBUG harness.obj x87.obj || exit /b 1
cl /nologo /c /GS- /Zl /Fo:host.obj "%SRC%\host.cpp" || exit /b 1
link /nologo /OUT:harness.exe /NODEFAULTLIB /ENTRY:Start /SUBSYSTEM:CONSOLE /BASE:0x400000 /FIXED /DYNAMICBASE:NO kernel32.lib host.obj || exit /b 1
echo built
