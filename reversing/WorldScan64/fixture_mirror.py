"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion
y los valores del falso (atan2 del falso de C++ frente al de Python) y la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py        (imprime la cobertura medida; falla si algun minimo no se alcanza)
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')
# Constantes de disposicion segun el oraculo / el desensamblado: Y[4] en 0xC81598, X[4] en 0xC815A8, contador en 0xC815B8; (0xC815BC - 0xC81598) / 4 = 9 palabras
LAYOUT = {'region_base': 0xC81598, 'region_words': 9, 'y_word': 0, 'x_word': 4, 'count_word': 8, 'max_events': 700, 'point_slots': 8}
assert (0xC815BC - 0xC81598) // 4 == 9 and (0xC815A8 - 0xC81598) // 4 == 4 and (0xC815B8 - 0xC81598) // 4 == 8
assert oracle.REGION_WORDS == 9 and oracle.Y0 == 0 and oracle.X0 == 4 and oracle.CNT == 8 and oracle.MAX_EVENTS == 700


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcws64-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def coverage():
    lines = build_and_run()
    sizes = json.loads(lines.pop().split('\t', 1)[1])
    if sizes != LAYOUT:
        raise SystemExit('Disposicion distinta entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), LAYOUT.get(k)) for k in LAYOUT if sizes.get(k) != LAYOUT.get(k)})
    names = oracle.NAMES
    per = {n: [] for n in names}
    for l in lines:
        k, js = l.split('\t', 1)
        if k == 'A':
            xb, yb, rb = json.loads(js)
            import math
            want = oracle.bits(oracle.f32(math.atan2(oracle.flt(yb), oracle.flt(xb))))
            if want != rb:
                raise SystemExit('atan2 distinto entre el C++ (falso del banco) y Python: x=%d y=%d: %d / %d' % (xb, yb, rb, want))
            continue
        per[names[int(k)]].append(json.loads(js))
    cov = {}
    for name in names:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            want, stats = oracle.expected(name, row, n)
            for key, v in stats.items():
                if v is True:
                    c[key] = c.get(key, 0) + 1
            c['family%d' % row['fam']] = c.get('family%d' % row['fam'], 0) + 1
            c['stack_seed'] = c.get('stack_seed', 0) + 1
        cov[name] = c
    return cov


if __name__ == '__main__':
    cov = coverage()
    try:
        import run_local
        short = run_local.check_minimums(cov)
    except ImportError:
        short = {}
    for name, c in cov.items():
        print(name, json.dumps(dict(sorted(c.items()))))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
