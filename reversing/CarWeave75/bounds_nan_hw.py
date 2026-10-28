"""Compuerta previa de CarWeave75: valida contra el HARDWARE x87 la aritmetica y la emulacion de NaN de Weave75BoundsCpp y Weave75FinalCpp (bounds_nan_hw.cpp).

Corta de source/game_sa/X87Math.cpp: la cabecera de declaraciones (externs de sin/cos/sqrt/tan), «// --- Lote 73 (HeatFire73)» hasta «// --- Lote 74 (PlaneFiring74)», la ayuda CarLink74Pos
(bloque «// --- Lote 74 (CarLink74)» hasta CarLink74ClipCpp) y el bloque «// --- Lote 75 (CarWeave75)» (hasta el siguiente «// --- Lote 75 (» o el final), mas la parte «Lote 75 (CarWeave75)» de
X87Math.h, en weave_slice.inc (produccion TAL CUAL), compila el microtest de 32 bits con las opciones de X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast) bajo vcvars32 y lo ejecuta. Exige 0 discrepancias.
Uso:  python -B bounds_nan_hw.py [ruta/a/X87Math.cpp]      (devuelve 0 si todo coincide; imprime el recuento de combinaciones)
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
BUILD = HERE / 'artifacts' / 'bounds_nan_hw'


def cut(text, start, end=None):
    i = text.index(start)
    j = text.index(end, i + 1) if end else len(text)
    return text[i:j]


def run(x87=None, timeout=3000):
    src = Path(x87) if x87 else ROOT / 'source/game_sa/X87Math.cpp'
    text = src.read_bytes().decode('utf-8')
    header = text[:text.index('#include "X87Math.h"')]
    parts = [header,
             '#include <cstdint>\n#include <cmath>\n#include <bit>\n#include <cstring>\n',
             cut(text, '// --- Lote 73 (HeatFire73)', '// --- Lote 74 (PlaneFiring74)'),
             cut(text, '// --- Lote 74 (CarLink74)', '__declspec(noinline) void __cdecl CarLink74ClipCpp'),
             '#pragma float_control(pop)\n']
    w = text.index('// --- Lote 75 (CarWeave75)')
    nxt = text.find('// --- Lote 75 (', w + 10)
    parts.append(text[w:nxt if nxt >= 0 else len(text)])
    htext = src.with_suffix('.h').read_bytes().decode('utf-8')
    hw = htext.index('// --- Lote 75 (CarWeave75)')
    hn = htext.find('// --- Lote 75 (', hw + 10)
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / 'weave_slice.inc').write_bytes((parts[0] + parts[1] + htext[hw:hn if hn >= 0 else len(htext)] + ''.join(parts[2:])).encode('utf-8'))
    exe = BUILD / 'bounds_nan_hw.exe'
    cmd = f'"{VCVARS}" >nul && cl /nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / "bounds_nan_hw.obj"}" /Fe"{exe}" "{HERE / "bounds_nan_hw.cpp"}"'
    r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, errors='replace')
    if r.returncode != 0 or not exe.exists():
        raise RuntimeError('No compila el microtest:\n' + r.stdout[-3000:] + r.stderr[-1000:])
    p = subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)
    out = p.stdout
    m = re.search(r'combinaciones=(\d+) discrepancias=(\d+)', out)
    if not m:
        raise RuntimeError('Salida inesperada del microtest:\n' + out[-3000:])
    res = dict(combos=int(m.group(1)), mismatch=int(m.group(2)), output=out)
    res['ok'] = p.returncode == 0 and not res['mismatch']
    return res


if __name__ == '__main__':
    res = run(sys.argv[1] if len(sys.argv) > 1 else None)
    print(res['output'])
    print('combinaciones totales:', res['combos'], 'OK' if res['ok'] else 'FALLO')
    sys.exit(0 if res['ok'] else 1)
