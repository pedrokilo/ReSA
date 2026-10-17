import subprocess, sys
from pathlib import Path
H = Path(__file__).resolve().parent
x = (H.parent.parent / 'ge-x87.cpp.txt').read_text(encoding='utf-8')
pre = r'''extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
extern "C" double __cdecl sqrt(double);
extern "C" double __cdecl atan2(double, double);
extern "C" double __cdecl log10(double);
extern "C" double __cdecl pow(double, double);
extern "C" double __cdecl log(double);
extern "C" double __cdecl tan(double);
#pragma intrinsic(sin, cos, sqrt, atan2, log10, pow, log, tan)
#include <float.h>
#include <string.h>
#include <assert.h>
#include <bit>
'''
(H / 'helpers_tu.cpp').write_text(pre + x, encoding='utf-8')
VC = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
extra = sys.argv[1:] or []
srcs = 'main.cpp enter.cpp'
bat = ('@echo off\r\n'
       'set PATH=%PATH%;C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\r\n'
       'call "' + VC + '" >nul\r\n'
       'cd /d "' + str(H) + '"\r\n'
       'cl /nologo /c /std:c++latest /O2 /Oi /arch:IA32 /Zi /fp:fast /EHsc /utf-8 /MTd /FAs /Fahelpers.asm /Fohelpers.obj /Fdhelpers.pdb helpers_tu.cpp || exit /b 1\r\n'
       'cl /nologo /std:c++latest /Od /Zi /EHsc /utf-8 /MTd /LD /Fe:harness.dll dllmain.cpp ' + srcs + ' helpers.obj /link /DEBUG /BASE:0x10000000 || exit /b 1\r\n'
       'cl /nologo /O1 /GS- /Gs1000000 /Zl /c stub.cpp || exit /b 1\r\n'
       'link /nologo /OUT:harness.exe /NODEFAULTLIB /ENTRY:Start /SUBSYSTEM:CONSOLE /BASE:0x400000 /FIXED /DYNAMICBASE:NO stub.obj kernel32.lib || exit /b 1\r\n')
(H / 'mk.bat').write_text(bat)
r = subprocess.run(['cmd', '/c', str(H / 'mk.bat')], capture_output=True, text=True, encoding='utf-8', errors='replace')
out = r.stdout + r.stderr
print('\n'.join(l for l in out.splitlines() if 'error' in l.lower() or 'warning' in l.lower() or 'LNK' in l)[-4000:] or 'built OK', r.returncode)
sys.exit(r.returncode)
