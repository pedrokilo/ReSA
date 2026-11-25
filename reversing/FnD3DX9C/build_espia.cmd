@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0
if not exist out mkdir out
cl /nologo /O1 /GS- /LD /MT espia_bloqueo.c /Fe:out\espia_bloqueo.dll /Fo:out\ /link kernel32.lib user32.lib
