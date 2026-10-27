"""Compuerta previa de HeliSwat74: valida contra el HARDWARE x87 la emulacion de NaN de HeliSwat74WorldPosCpp (nan_rules_hw.cpp).

Corta de source/game_sa/X87Math.cpp el bloque «// --- Lote 73» (hasta el final: incluye HeatFire73Raw/Op, que el bloque «Lote 74» reutiliza) en nan_rules_slice.inc (produccion TAL CUAL), compila el microtest de 32 bits con las opciones de
X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast) bajo vcvars32 y lo ejecuta. Exige 0 discrepancias de resultado, 0 de bandera IE y 0 de cadena completa.
Uso:  python -B nan_rules_hw.py [ruta/a/X87Math.cpp]      (devuelve 0 si todo coincide; imprime los recuentos de combinaciones)
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
    pop = '#pragma float_control(pop)'
    i = text.index('// --- Lote 73 (HeatFire73)')
    i_end = text.index(pop, text.index('HeatFire73SpawnCpp(const float* moveSpeedIn', i)) + len(pop)
    j = text.index('// --- Lote 74 (HeliSwat74)')
    j_end = text.index(pop, j) + len(pop)
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / 'nan_rules_slice.inc').write_bytes(('#include <cstdint>\n#include <cmath>\n' + text[i:i_end] + '\n' + text[j:j_end] + '\n').encode('utf-8'))
    exe = BUILD / 'nan_rules_hw.exe'
    cmd = f'"{VCVARS}" >nul && cl /nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / "nan_rules_hw.obj"}" /Fe"{exe}" "{HERE / "nan_rules_hw.cpp"}"'
    r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, errors='replace')
    if r.returncode != 0 or not exe.exists():
        raise RuntimeError('No compila el microtest:\n' + r.stdout[-3000:] + r.stderr[-1000:])
    p = subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)
    out = p.stdout
    m1 = re.search(r'parte1_combinaciones=(\d+) discrepancias=(\d+) discrepancias_IE=(\d+)', out)
    m2 = re.search(r'parte2_combinaciones=(\d+) discrepancias=(\d+)', out)
    if not m1 or not m2:
        raise RuntimeError('Salida inesperada del microtest:\n' + out[-3000:])
    res = dict(part1=int(m1.group(1)), part1_mismatch=int(m1.group(2)), part1_ie_mismatch=int(m1.group(3)), part2=int(m2.group(1)), part2_mismatch=int(m2.group(2)), output=out)
    res['ok'] = p.returncode == 0 and not (res['part1_mismatch'] or res['part1_ie_mismatch'] or res['part2_mismatch'])
    return res


if __name__ == '__main__':
    res = run(sys.argv[1] if len(sys.argv) > 1 else None)
    print(res['output'])
    print('combinaciones totales:', res['part1'] + res['part2'], 'OK' if res['ok'] else 'FALLO')
    sys.exit(0 if res['ok'] else 1)
