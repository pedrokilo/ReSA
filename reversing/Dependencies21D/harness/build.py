"""Arnes local de GangDriveBy (fuera del juego). Uso: python build.py [casos] [funcion -1..3] [semilla]
Extrae el codigo de TaskSimpleGangDriveBy.cpp, copia X87Math.cpp/.h del repositorio, compila con las opciones del proyecto y ejecuta los bytes del EXE
(D:/Grand Theft Auto San Andreas/gta-sa.exe) frente a la DLL."""
import subprocess, sys, shutil
from pathlib import Path
H = Path(__file__).resolve().parent
ROOT = H.parents[2]
(H / 'x87src').mkdir(exist_ok=True)
for n in ('X87Math.cpp', 'X87Math.h'):
    shutil.copyfile(ROOT / 'source/game_sa' / n, H / 'x87src' / n)
subprocess.run([sys.executable, str(H / 'extract_glue.py'), str(ROOT / 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp'), str(H)], check=True)
VC = 'C:/Program Files/Microsoft Visual Studio/18/Community/VC/Auxiliary/Build/vcvars32.bat'
lines = [
    '@echo off',
    'call "%s" >nul' % VC,
    'cd /d "%s"' % H,
    'if not exist obj mkdir obj',
    'cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MT /DNOMINMAX /Ix87src x87src/X87Math.cpp /Foobj/X87Math.obj /Fdobj/x.pdb || exit /b 1',
    'cl /nologo /c /std:c++latest /EHsc /utf-8 /Od /RTC1 /Zi /MT /DNOMINMAX /Ix87src /Ih h/main.cpp /Foobj/main.obj /Fdobj/m.pdb || exit /b 1',
    'link /nologo /OUT:obj/gdtest.exe obj/main.obj obj/X87Math.obj /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /DEBUG /LARGEADDRESSAWARE:NO || exit /b 1',
]
(H / '_b.bat').write_text((chr(13) + chr(10)).join(lines) + chr(13) + chr(10))
if subprocess.run(['cmd', '/c', str(H / '_b.bat')]).returncode:
    sys.exit(1)
sys.exit(subprocess.run([str(H / 'obj/gdtest.exe')] + (sys.argv[1:] or ['20000', '-1', '1'])).returncode)
