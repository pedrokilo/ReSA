@echo off
cd /d %~dp0
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /c /O2 /EHsc /std:c++20 prelude_test.cpp /Fo:prelude_test.obj
