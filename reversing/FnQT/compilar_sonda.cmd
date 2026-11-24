@echo off
rem Compila mf_probe.exe (x86) con CMfPcmReader.cpp. Uso: compilar_sonda.cmd <carpeta de salida>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
set HERE=%~dp0
if not exist "%~1" mkdir "%~1"
cl /nologo /std:c++20 /EHsc /O2 /MT /DMFPCM_STANDALONE_TEST /Fo"%~1\\" /Fe"%~1\mf_probe.exe" "%HERE%mf_probe.cpp" "%HERE%..\..\source\game_sa\Audio\Loaders\MfPcmReader.cpp" ole32.lib propsys.lib
exit /b %ERRORLEVEL%
