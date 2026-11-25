@echo off
rem uso: cl_ours.cmd <opt: /O2 | /Od> <salida.obj>
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cl @D:\Original\librw-test\oraculo\cl_base.rsp /c /MD %1 /arch:IA32 /W1 /I"D:\Original\librw-test\oraculo" /Fo:%2 D:\Original\wv-librw\contrib\oraculo_librw\nuestro.cpp
