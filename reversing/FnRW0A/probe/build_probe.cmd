@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
cl /nologo /D_CRT_SECURE_NO_WARNINGS /O1 /MT /LD /Zi /W3 rwprobe.c /Fe:zz_rwprobe.asi /link dbghelp.lib user32.lib
