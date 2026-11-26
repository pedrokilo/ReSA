@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cl /nologo /std:c++20 /EHsc /O2 /arch:IA32 /I..\..\source\game_sa /Fe:%~dp0bench_core.exe /Fo:%~dp0bench_core.obj %~dp0bench_core.cpp
