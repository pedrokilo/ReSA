@echo off
rem Compila el banco de RwHelper (bench_rwhelper.exe de 32 bits, base 0x400000 con la reserva del espacio del EXE original).
rem El codigo bajo prueba (RwHelper.cpp / rwtexdict.cpp) se extrae con gen_slice_rwhelper.py y se compila con las opciones de Debug del proyecto (/Od, SSE2, /RTC1);
rem X87Math.cpp se compila con sus opciones por archivo (/O2 /Oi /arch:IA32 /fp:fast). Uso: build_rwhelper.cmd <raiz del repo> <directorio de salida>
rem El enlace usa /FORCE:UNRESOLVED: X87Math.cpp referencia cientos de datos y funciones del proyecto que este banco no ejecuta (ninguno de los ayudantes que usan los callbacks los toca).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
set REPO=%~1
set OUT=%~2
mkdir "%OUT%" 2>nul
python "%~dp0gen_slice_rwhelper.py" "%REPO%" "%OUT%\slice.inc" || exit /b 1
ml /nologo /c /Fo"%OUT%\reserva.obj" "%~dp0..\FnCRT1A\reserva_imagen.asm" || exit /b 1
cl /nologo /c /O2 /Oi /arch:IA32 /fp:fast /GS- /Zi /std:c++latest /EHsc /MTd /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX /I"%REPO%\source" /I"%REPO%\source\game_sa" /Fo"%OUT%\X87Math.obj" /Fd"%OUT%\x87.pdb" "%REPO%\source\game_sa\X87Math.cpp" || exit /b 1
cl /nologo /c /Od /RTC1 /GS- /Zi /std:c++latest /EHsc /MTd /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX /I"%OUT%" /I"%REPO%\source\game_sa" /Fo"%OUT%\bench_rwhelper.obj" /Fd"%OUT%\rwh.pdb" "%~dp0bench_rwhelper.cpp" || exit /b 1
link /nologo /FORCE:UNRESOLVED /DEBUG /OUT:"%OUT%\bench_rwhelper.exe" /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO "%OUT%\reserva.obj" "%OUT%\bench_rwhelper.obj" "%OUT%\X87Math.obj" libcmtd.lib kernel32.lib user32.lib
