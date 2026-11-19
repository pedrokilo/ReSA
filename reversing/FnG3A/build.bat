@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /O2 /MD argvdump.cpp /Fe:argvdump.exe || exit /b 1
cl /nologo /EHsc /O2 /MT argvdump.cpp /Fe:argvdump_mt.exe || exit /b 1
cl /nologo /EHsc /O2 /MT crtbatch.cpp /Fe:crtbatch.exe || exit /b 1
