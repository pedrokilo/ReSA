"""Compila el arnes (X87Math.cpp del repositorio con las opciones del proyecto) y genera harness.exe/harness.dll.
Uso: python mk.py; harness.exe [casos] [semilla] [verbosidad] [filtro 1/2/3/B]"""
import subprocess, sys
from pathlib import Path
H = Path(__file__).resolve().parent
S = H.parents[2] / 'source' / 'game_sa'
VC = 'C:/Program Files/Microsoft Visual Studio/18/Community/VC/Auxiliary/Build/vcvars32.bat'
lines = [
    '@echo off',
    'call "%s" >nul' % VC,
    'cd /d "%s"' % H,
    'cl /nologo /c /std:c++latest /O2 /Oi /arch:IA32 /Zi /fp:fast /EHsc /utf-8 /MTd /Fohelpers.obj /Fdhelpers.pdb /I"%s" "%s" || exit /b 1' % (S, S / 'X87Math.cpp'),
    'cl /nologo /std:c++latest /Od /Zi /EHsc /utf-8 /MTd /LD /Fe:harness.dll dllmain.cpp main.cpp enter.cpp helpers.obj /link /DEBUG /BASE:0x10000000 || exit /b 1',
    'cl /nologo /O1 /GS- /Gs1000000 /Zl /c stub.cpp || exit /b 1',
    'link /nologo /OUT:harness.exe /NODEFAULTLIB /ENTRY:Start /SUBSYSTEM:CONSOLE /BASE:0x400000 /FIXED /DYNAMICBASE:NO stub.obj kernel32.lib || exit /b 1',
]
(H / 'mk.bat').write_text(chr(13) + chr(10).join(lines) if False else (chr(13) + chr(10)).join(lines) + chr(13) + chr(10))
sys.exit(subprocess.run(['cmd', '/c', str(H / 'mk.bat')]).returncode)
