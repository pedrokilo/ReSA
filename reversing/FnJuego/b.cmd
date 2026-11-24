@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cl /nologo /O1 /MT /LD xinput_falso.c /Fe:xinput1_4.dll /link /DEF:xinput_falso.def user32.lib

