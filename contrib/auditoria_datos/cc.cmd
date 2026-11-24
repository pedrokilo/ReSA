@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cd /d D:\Original\sim_out\aud2
cl @cl_aud.rsp /MP1 /FA /Fa%~n1.asm /Fo%~n1.obj /I D:\Original\wv-audit\source %1
