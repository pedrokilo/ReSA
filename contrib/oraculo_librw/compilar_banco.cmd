@echo off
rem uso: compilar_banco.cmd <Od|O2>   (compila nuestro.cpp y banco.cpp y enlaza con librw.lib)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
set O=D:\Original\librw-test\oraculo
python D:\Original\wv-librw\contrib\oraculo_librw\extraer.py D:\Original\wv-librw %O%\gen || exit /b 1
call %O%\cl_ours.cmd /%1 %O%\nuestro_%1.obj || exit /b 2
cl /nologo /EHsc /MD /%1 /arch:IA32 /DRW_NULL /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4996 /I D:\Original\librw-test\librw /I D:\Original\librw-test\librw\src D:\Original\wv-librw\contrib\oraculo_librw\banco.cpp %O%\nuestro_%1.obj D:\Original\librw-test\build_null\src\librw.lib /Fe:%O%\banco_%1.exe /Fo:%O%\banco_%1.obj /link /FORCE:UNRESOLVED /OPT:REF

