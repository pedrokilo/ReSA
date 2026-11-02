@echo off
cd /d %~dp0
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cl /nologo /O2 /EHsc /std:c++20 dump_cases.cpp /Fe:dump_cases.exe /Fo:dump_cases.obj
