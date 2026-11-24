@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cd /d "%~dp0"
cl /nologo /O2 /LD /MT redir_docs.cpp /Fe:redir_docs.asi /link /DLL
exit /b %ERRORLEVEL%
