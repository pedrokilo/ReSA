@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cl /nologo /O1 /GS- /LD /MT "%~dp0espia_archivos.c" /Fe"%~dp0espia_archivos.dll" /Fo"%~dp0espia_archivos.obj" /link kernel32.lib user32.lib 2>&1
