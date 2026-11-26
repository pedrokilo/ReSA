@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cl /nologo /std:c++20 /EHsc /O2 /Fe:%~dp0bench_regex.exe /Fo:%~dp0bench_regex.obj %~dp0bench_regex.cpp
