"""Compuerta previa de HeliUpdate78: valida contra el HARDWARE x87 la funcion HeliIsFarFromPlayer de Heli.cpp (dist_hw.cpp).

Corta de source/game_sa/Entity/Vehicle/Heli.cpp la funcion `static bool HeliIsFarFromPlayer` (produccion TAL CUAL) en heli_slice.inc, compila el microtest de 32 bits con las opciones por defecto del proyecto
(/Od /arch:IA32 y /arch:SSE2 en dos variantes) bajo vcvars32 y lo ejecuta. Exige 0 discrepancias.
Uso:  python -B dist_hw.py [ruta/a/Heli.cpp]      (devuelve 0 si todo coincide)
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
BUILD = HERE / 'artifacts' / 'dist_hw'


def run(heli=None, timeout=3000):
    src = Path(heli) if heli else ROOT / 'source/game_sa/Entity/Vehicle/Heli.cpp'
    text = src.read_bytes().decode('utf-8')
    i = text.index('static bool HeliIsFarFromPlayer')
    j = text.index('\n}\n', i) + 3
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / 'heli_slice.inc').write_bytes(text[i:j].encode('utf-8'))
    results = []
    for name, opts in (('ia32', '/Od /arch:IA32'), ('sse2', '/Od /arch:SSE2'), ('ia32fast', '/Od /arch:IA32 /fp:fast')):
        exe = BUILD / ('dist_hw_%s.exe' % name)
        cmd = f'"{VCVARS}" >nul && cl /nologo {opts} /EHsc /std:c++latest /I"{BUILD}" /Fo"{BUILD / ("dist_hw_%s.obj" % name)}" /Fe"{exe}" "{HERE / "dist_hw.cpp"}"'
        r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, errors='replace')
        if r.returncode != 0 or not exe.exists():
            raise RuntimeError('No compila el microtest (%s):\n' % name + r.stdout[-3000:] + r.stderr[-1000:])
        p = subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)
        m = re.search(r'combinaciones=(\d+) discrepancias=(\d+)', p.stdout)
        if not m:
            raise RuntimeError('Salida inesperada del microtest (%s):\n' % name + p.stdout[-3000:])
        results.append(dict(variant=name, options=opts, combos=int(m.group(1)), mismatch=int(m.group(2)), output=p.stdout, ok=p.returncode == 0 and int(m.group(2)) == 0))
    return results


if __name__ == '__main__':
    res = run(sys.argv[1] if len(sys.argv) > 1 else None)
    for r in res:
        print(r['variant'], r['options'], 'combinaciones', r['combos'], 'discrepancias', r['mismatch'], 'OK' if r['ok'] else 'FALLO')
        if not r['ok']:
            print(r['output'])
    sys.exit(0 if all(r['ok'] for r in res) else 1)
