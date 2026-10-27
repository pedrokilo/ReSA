"""Compuerta previa de PlayerCrosshair74 / CrosshairRender74: valida contra el HARDWARE x87 las funciones Cross74*Cpp de produccion (nan_rules_hw.cpp).

Corta de source/game_sa/X87Math.cpp el bloque «// --- Lote 73 (HeatFire73)» hasta el final (incluye el «Lote 74») en nan_rules_slice.inc (produccion TAL CUAL), compila el microtest de 32 bits con las
opciones de X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast) bajo vcvars32 y lo ejecuta. Exige 0 discrepancias de bits (incluidas las cargas de NaN) y la pila x87 equilibrada.
Uso:  python -B nan_rules_hw.py [ruta/a/X87Math.cpp]      (devuelve 0 si todo coincide)
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
BUILD = HERE / 'artifacts' / 'nan_rules_hw'


def run(x87=None, timeout=3000):
    src = Path(x87) if x87 else ROOT / 'source/game_sa/X87Math.cpp'
    text = src.read_bytes().decode('utf-8')
    i = text.index('// --- Lote 73 (HeatFire73)')
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / 'nan_rules_slice.inc').write_bytes(('#include <cstdint>\n#include <cmath>\n#include <bit>\n#include <string.h>\n' + text[:text.index('#include "X87Math.h"')][:0] + text[i:]).encode('utf-8'))
    exe = BUILD / 'nan_rules_hw.exe'
    cmd = f'"{VCVARS}" >nul && cl /nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / "nan_rules_hw.obj"}" /Fe"{exe}" "{HERE / "nan_rules_hw.cpp"}"'
    r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, errors='replace')
    if r.returncode != 0 or not exe.exists():
        raise RuntimeError('No compila el microtest:\n' + r.stdout[-3000:] + r.stderr[-1000:])
    p = subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)
    out = p.stdout
    m = re.search(r'paso=(\d+) direccion=(\d+) fase=(\d+) estela=(\d+) discrepancias=(\d+)', out)
    if not m:
        raise RuntimeError('Salida inesperada del microtest:\n' + out[-3000:])
    res = dict(total=sum(int(m.group(k)) for k in range(1, 5)), mismatch=int(m.group(5)), output=out)
    res['ok'] = p.returncode == 0 and res['mismatch'] == 0
    return res


if __name__ == '__main__':
    res = run(sys.argv[1] if len(sys.argv) > 1 else None)
    print(res['output'])
    print('combinaciones totales:', res['total'], 'OK' if res['ok'] else 'FALLO')
    sys.exit(0 if res['ok'] else 1)
