"""Compuerta previa de CarNodes75: valida contra el HARDWARE x87 la aritmetica de Nodes75RegionCpp, Nodes75InRangeCpp, Nodes75Z3Cpp y Nodes75ScoreCpp (nodes_nan_hw.cpp).

Corta de source/game_sa/X87Math.cpp la cabecera de declaraciones (externs de sin/cos/sqrt/tan) y el bloque «// --- Lote 75 (CarNodes75)» (hasta el siguiente «// --- Lote 75 (» o el final), mas la parte
«Lote 75 (CarNodes75)» de X87Math.h, en nodes_slice.inc (produccion TAL CUAL), compila el microtest de 32 bits con las opciones de X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast) bajo vcvars32 y lo ejecuta.
Exige 0 discrepancias.
Uso:  python -B nodes_nan_hw.py [ruta/a/X87Math.cpp]      (devuelve 0 si todo coincide; imprime el recuento de combinaciones)
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
BUILD = HERE / 'artifacts' / 'nodes_nan_hw'


def cut(text, start, end=None):
    i = text.index(start)
    j = text.index(end, i + 1) if end else len(text)
    return text[i:j]


def run(x87=None, timeout=3000):
    src = Path(x87) if x87 else ROOT / 'source/game_sa/X87Math.cpp'
    text = src.read_bytes().decode('utf-8')
    header = text[:text.index('#include "X87Math.h"')]
    w = text.index('// --- Lote 75 (CarNodes75)')
    nxt = text.find('// --- Lote 75 (', w + 10)
    body = text[w:nxt if nxt >= 0 else len(text)]
    htext = src.with_suffix('.h').read_bytes().decode('utf-8')
    hw = htext.index('// --- Lote 75 (CarNodes75)')
    hn = htext.find('// --- Lote 75 (', hw + 10)
    BUILD.mkdir(parents=True, exist_ok=True)
    inc = '#include <cstdint>' + chr(10) + '#include <cmath>' + chr(10) + '#include <bit>' + chr(10) + '#include <cstring>' + chr(10)
    (BUILD / 'nodes_slice.inc').write_bytes((header + inc + htext[hw:hn if hn >= 0 else len(htext)] + body).encode('utf-8'))
    exe = BUILD / 'nodes_nan_hw.exe'
    cmd = f'"{VCVARS}" >nul && cl /nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / "nodes_nan_hw.obj"}" /Fe"{exe}" "{HERE / "nodes_nan_hw.cpp"}"'
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
