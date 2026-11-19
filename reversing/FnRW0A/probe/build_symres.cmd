@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d %~dp0
cl /nologo /D_CRT_SECURE_NO_WARNINGS /O1 symres.c /Fe:symres.exe /link dbghelp.lib
