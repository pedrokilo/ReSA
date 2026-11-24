@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cd /d D:\Original\sim_out\aud2
cl @cl_aud.rsp /DAUDIT_PROBE /FAs /Faasm\%2.asm /Foasm\%2.obj /Fdasm\%2.pdb /I D:\Original\wv-audit\source D:\Original\wv-audit\source\%1 > asm\%2.log 2>&1
