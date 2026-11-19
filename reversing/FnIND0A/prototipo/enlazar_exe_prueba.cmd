@echo off
rem Lote IND0A: enlaza el codigo propio ya compilado (los .obj de build\source\gta_reversed.dir\Debug) como EXE con base fija
rem 0x400000 y la reserva dentro de .text que cubre 0x401000-0xCB1000. NO compila el proyecto: solo compila 2 archivos pequenos y enlaza.
rem Uso: enlazar_exe_prueba.cmd <raiz del repo> <carpeta de salida> [carpeta de los .obj]
rem Si hay otros agentes compilando, tomar antes D:\Original\compile_lock (mkdir atomico) y liberarlo despues.
setlocal EnableDelayedExpansion
set REPO=%~1
set OUT=%~2
if "%REPO%"=="" exit /b 2
if "%OUT%"=="" exit /b 2
if not exist "%OUT%" mkdir "%OUT%"
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
set OBJDIR=%~3
if "%OBJDIR%"=="" set OBJDIR=%REPO%\build\source\gta_reversed.dir\Debug
set CONAN=%REPO%\reversing\CVehicle\conan-cache\p\b
ml /nologo /c /Fo"%OUT%\reserva_imagen.obj" "%~dp0reserva_imagen.asm" || exit /b 5
cl /nologo /c /Z7 /MTd /EHsc /Fo"%OUT%\\" "%~dp0cargador_imagen.cpp" || exit /b 4
dir /b "%OBJDIR%\*.obj" > "%OUT%\objs.txt"
del "%OUT%\link.rsp" 2>nul
echo "%OUT%\reserva_imagen.obj" >> "%OUT%\link.rsp"
echo "%OUT%\cargador_imagen.obj" >> "%OUT%\link.rsp"
for /f %%f in (%OUT%\objs.txt) do echo "%OBJDIR%\%%f" >> "%OUT%\link.rsp"
for %%l in (ogg spdlogd TracyClient imgui SDL3-static vorbisenc vorbisfile vorbis jpeg-static turbojpeg-static) do (
  for /f "delims=" %%p in ('dir /s /b "%CONAN%\%%l.lib" 2^>nul') do echo "%%p" >> "%OUT%\link.rsp"
)
echo ddraw.lib dsound.lib dxguid.lib strmiids.lib d3d9.lib Winmm.lib dbghelp.lib ws2_32.lib opengl32.lib kernel32.lib user32.lib gdi32.lib imm32.lib ole32.lib oleaut32.lib version.lib uuid.lib advapi32.lib setupapi.lib shell32.lib comdlg32.lib >> "%OUT%\link.rsp"
rem /FORCE:UNRESOLVED permite medir los simbolos sin resolver sin abortar; si hay alguno, el EXE resultante NO debe ejecutarse.
link /nologo /OUT:"%OUT%\gta_reversed_exe.exe" /MACHINE:X86 /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup /BASE:0x400000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE /INCREMENTAL:NO /SAFESEH:NO /NXCOMPAT:NO /MAP:"%OUT%\gta_reversed_exe.map" /FORCE:UNRESOLVED @"%OUT%\link.rsp" > "%OUT%\link.log" 2>&1
echo link exit=%ERRORLEVEL%
endlocal
