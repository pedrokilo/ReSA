"""Compuerta previa de PlaneFiring74: valida contra el HARDWARE x87 la aritmetica y la emulacion de NaN de PlaneFiring74FlareCpp (flare_nan_hw.cpp).

Corta de source/game_sa/X87Math.cpp el bloque «// --- Lote 73» hasta el final (incluye el «Lote 74») en flare_slice.inc (produccion TAL CUAL), compila el microtest de 32 bits con las
opciones de X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast) bajo vcvars32 y lo ejecuta. Exige 0 discrepancias.
Uso:  python -B flare_nan_hw.py [ruta/a/X87Math.cpp]      (devuelve 0 si todo coincide; imprime el recuento de combinaciones)
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
BUILD = HERE / 'artifacts' / 'flare_nan_hw'


def run(x87=None, timeout=3000):
    src = Path(x87) if x87 else ROOT / 'source/game_sa/X87Math.cpp'
    text = src.read_bytes().decode('utf-8')
    i = text.index('// --- Lote 73 (HeatFire73)')
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / 'flare_slice.inc').write_bytes(('#include <cstdint>\n#include <cmath>\n#include <bit>\n' + text[i:]).encode('utf-8'))
    exe = BUILD / 'flare_nan_hw.exe'
    cmd = f'"{VCVARS}" >nul && cl /nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / "flare_nan_hw.obj"}" /Fe"{exe}" "{HERE / "flare_nan_hw.cpp"}"'
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
