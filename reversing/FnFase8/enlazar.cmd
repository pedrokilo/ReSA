@echo off
rem FASE 8: compila StandaloneExe.cpp con GTASA_STANDALONE_EXE y enlaza el ejecutable independiente gta_sa_standalone.exe
rem con los .obj ya compilados del ASI Debug (copiados a reversing\FnFase8\obj). No ejecuta nada del juego.
rem Con la imagen incrustada el exe no necesita gta-sa.exe ni para enlazar (docs/CodigoVivoExe/ejecucion-imagen-datos.md).
rem Uso: enlazar.cmd <raiz del repo> <carpeta de salida> [carpeta de los .obj del ASI Debug; por defecto reversing\FnFase8\obj]
setlocal EnableDelayedExpansion
set REPO=%~1
set OUT=%~2
if "%REPO%"=="" exit /b 2
if "%OUT%"=="" exit /b 2
if not exist "%OUT%" mkdir "%OUT%"
set FN=%REPO%\reversing\FnFase8
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 3
set CONAN=%REPO%\reversing\CVehicle\conan-cache\p\b
rem cl.rsp lleva marcadores @RAIZ@: se expanden con la raiz del repo en uso (el .rsp versionado no depende de rutas de otros arboles)
powershell -NoProfile -ExecutionPolicy Bypass -File "%FN%\expandir_rsp.ps1" "%REPO%" "%FN%\cl.rsp" "%OUT%\cl.rsp" || exit /b 8
ml /nologo /c /Fo"%OUT%\reserva_imagen.obj" "%FN%\reserva_imagen.asm" || exit /b 5
rem codigo propio del exe: se compila con GTASA_STANDALONE_EXE (el ASI lo compila vacio); /GS- /RTC off para el callback TLS (sin CRT)
cl @"%OUT%\cl.rsp" /DGTASA_STANDALONE_EXE /GS- /Fo"%OUT%\StandaloneExe.obj" "%REPO%\source\app\platform\win\StandaloneExe.cpp" || exit /b 4
rem imagen de datos del exe original (arreglos C++ versionados en source\game_sa\DatosExe\Imagen, generados por gen_imagen.py): solo el independiente; el ASI no los compila
if not exist "%OUT%\imagen" mkdir "%OUT%\imagen"
for %%f in ("%REPO%\source\game_sa\DatosExe\Imagen\*.cpp") do if not exist "%OUT%\imagen\%%~nf.obj" cl @"%OUT%\cl.rsp" /DGTASA_STANDALONE_EXE /GS- /Fo"%OUT%\imagen\%%~nf.obj" "%%f" || exit /b 7
rem overlay: archivos de over_src\*.cpp (copias de trabajo en curso de otras ramas, p. ej. vivo-d3dstd) se compilan con las mismas opciones (sin PCH) solo si falta over\<n>.obj
if not exist "%FN%\over" mkdir "%FN%\over"
for %%f in ("%FN%\over_src\*.cpp") do if not exist "%FN%\over\%%~nf.obj" cl @"%OUT%\cl.rsp" /MP1 /I"%FN%\over_src" /Fo"%FN%\over\%%~nf.obj" "%%f" || exit /b 6
rem .obj de reemplazo (archivos del proyecto recompilados con GTASA_STANDALONE_EXE u otras definiciones): carpeta over\ gana a obj\
set OBJDIR=%~3
if "%OBJDIR%"=="" set OBJDIR=%FN%\obj
rem libs propias del ASI (zlib, libpng, libjpeg 6b): si hay build/ se refrescan desde alli
for %%p in (libjpeg-6b\Debug\libjpeg6b libpng-1.0.12\Debug\libpng1012 zlib-1.1.3\Debug\zlib113) do if exist "%REPO%\build\libs\%%p.lib" copy /y "%REPO%\build\libs\%%p.lib" "%FN%\lib\" >nul
dir /b "%OBJDIR%\*.obj" > "%OUT%\objs.txt"
del "%OUT%\link.rsp" 2>nul
echo "%OUT%\reserva_imagen.obj" >> "%OUT%\link.rsp"
echo "%OUT%\StandaloneExe.obj" >> "%OUT%\link.rsp"
for %%f in ("%OUT%\imagen\*.obj") do echo "%%f" >> "%OUT%\link.rsp"
for /f %%f in (%OUT%\objs.txt) do (
  if exist "%FN%\over\%%f" ( echo "%FN%\over\%%f" >> "%OUT%\link.rsp" ) else ( echo "%OBJDIR%\%%f" >> "%OUT%\link.rsp" )
)
for %%l in (imgui\p\lib\imgui jpeg-static turbojpeg-static ogg spdlogd TracyClient vorbis vorbisenc vorbisfile SDL3-static) do echo.>nul
for %%f in ("%FN%\over\*.obj") do if not exist "%OBJDIR%\%%~nxf" echo "%%f" >> "%OUT%\link.rsp"
for %%p in (libjpeg6b libpng1012 zlib113) do echo "%FN%\lib\%%p.lib" >> "%OUT%\link.rsp"
for %%p in (imgui5396f0f7f8179\p\lib\imgui.lib libjpbe5e6c22fbdf6\p\lib\jpeg-static.lib libjpbe5e6c22fbdf6\p\lib\turbojpeg-static.lib ogg21219f4658423\p\lib\ogg.lib sdlc3cf2c66533bd\p\lib\SDL3-static.lib spdlo8029646db848c\p\lib\spdlogd.lib tracy2fefc9668738f\p\lib\TracyClient.lib vorbi9990e9d508d57\p\lib\vorbis.lib vorbi9990e9d508d57\p\lib\vorbisenc.lib vorbi9990e9d508d57\p\lib\vorbisfile.lib) do echo "%CONAN%\%%p" >> "%OUT%\link.rsp"
echo dsound.lib dxguid.lib strmiids.lib d3d9.lib Winmm.lib dbghelp.lib ws2_32.lib opengl32.lib kernel32.lib user32.lib gdi32.lib imm32.lib ole32.lib oleaut32.lib version.lib uuid.lib advapi32.lib setupapi.lib shell32.lib comdlg32.lib winspool.lib >> "%OUT%\link.rsp"
rem sin /FORCE: cualquier simbolo sin resolver debe aparecer como error (lista en link.log)
link /nologo /DEBUG /OUT:"%OUT%\gta_sa_standalone.exe" /PDB:"%OUT%\gta_sa_standalone.pdb" /MACHINE:X86 /SUBSYSTEM:WINDOWS /ENTRY:FaseEntrada /BASE:0x400000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE /INCREMENTAL:NO /SAFESEH:NO /NXCOMPAT:NO /MAP:"%OUT%\gta_sa_standalone.map" @"%OUT%\link.rsp" > "%OUT%\link.log" 2>&1
set LE=%ERRORLEVEL%
echo link exit=%LE%
endlocal & exit /b %LE%
