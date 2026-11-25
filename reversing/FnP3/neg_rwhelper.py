"""Prueba negativa del banco de RwHelper: aplica mutaciones de una linea al .inc generado (slice.inc), recompila solo el banco y comprueba que cada mutacion se detecta.
Uso: python neg_rwhelper.py <raiz del repo> <carpeta del banco ya construida (build_rwhelper.cmd)> <gta-sa.exe>"""
import os
import subprocess
import sys
from pathlib import Path

repo, out, exe = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3]
slice_ = out / 'slice.inc'
orig = slice_.read_text(encoding='utf-8')
muts = [
    ('radio: > por <', 'if (candidate > sphere->radius)', 'if (candidate < sphere->radius)'),
    ('radio: dz*dz por dz*dy', 'Veh83DMul(dz, dz)', 'Veh83DMul(dz, dy)'),
    ('centro: z suma y', 'sum->z = Veh83CAddStore(point.z, sum->z);', 'sum->z = Veh83CAddStore(point.y, sum->z);'),
    ('LTM en la rama sin LTM', '&atomic->boundingSphere.center, 1, &frame->modelling);', '&atomic->boundingSphere.center, 1, RwFrameGetLTM(frame));'),
    ('sin resincronizar (rama LTM)', '    if (useLtm) {\n        if (atomic->interpolator.flags & 2) {', '    if (useLtm) {\n        if (false) {'),
    ('1/n con n = 1', 'Hp66ClumpScaleCenterCpp(&sum.x, numAtomics);', 'Hp66ClumpScaleCenterCpp(&sum.x, 1);'),
    ('global siempre 1', 'useLtm = std::bit_cast<uint8>(bUseLTM);', 'useLtm = 1;'),
    ('textura: devuelve la textura', 'RwTextureDestroy(texture);\n    return nullptr;', 'RwTextureDestroy(texture);\n    return texture;'),
]
here = Path(__file__).resolve().parent
vcvars = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
bad = 0
for name, a, b in muts:
    if a not in orig:
        print('NO APLICA', name)
        bad += 1
        continue
    slice_.write_text(orig.replace(a, b, 1), encoding='utf-8', newline='\n')
    o = str(out)
    cl = ('cl /nologo /c /Od /RTC1 /GS- /Zi /std:c++latest /EHsc /MTd /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX '
          f'/I"{o}" /I"{repo}\\source\\game_sa" /Fo"{o}\\neg.obj" /Fd"{o}\\neg.pdb" "{here}\\bench_rwhelper.cpp"')
    ln = ('link /nologo /FORCE:UNRESOLVED /DEBUG /BASE:0x400000 /DYNAMICBASE:NO /FIXED /LARGEADDRESSAWARE:NO /INCREMENTAL:NO '
          f'/OUT:"{o}\\neg.exe" "{o}\\reserva.obj" "{o}\\neg.obj" "{o}\\X87Math.obj" libcmtd.lib kernel32.lib user32.lib')
    exe_new = out / 'neg.exe'
    if exe_new.exists():
        exe_new.unlink()
    # el enlace con /FORCE:UNRESOLVED termina con codigo distinto de cero aunque genere el exe: se comprueba que exista
    bat = out / 'neg_build.cmd'
    lines = ['@echo off', f'call "{vcvars}" >nul 2>nul', f'{cl} >nul', 'if errorlevel 1 exit /b 1', f'{ln} >nul 2>nul']
    bat.write_text(chr(10).join(lines) + chr(10), encoding='utf-8')
    subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True)
    if not exe_new.exists():
        print('NO COMPILA', name)
        bad += 1
        continue
    p = subprocess.run([str(out / 'neg.exe'), exe, '--casos', '20000'], capture_output=True, text=True)
    detected = p.returncode == 1
    print(('DETECTADA ' if detected else 'NO DETECTADA ') + name)
    if not detected:
        bad += 1
slice_.write_text(orig, encoding='utf-8', newline='\n')
print('mutaciones sin detectar o fallidas:', bad)
