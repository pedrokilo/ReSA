@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
cl /nologo /O1 /MT /EHsc pico_audio.cpp /Fe:pico_audio.exe ole32.lib uuid.lib
