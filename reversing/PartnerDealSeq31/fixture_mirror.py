"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), reconstruye la fila `input` como Core.hpp y la pasa por el oraculo.
Comprueba: ningun caso invalido (el oraculo rechaza ventanas incoherentes y pools sin ranura libre), las constantes de arena del C++ (ultima linea) frente a las del oraculo,
cobertura frente a los minimos de run_local.py, y muestra los recuentos.
Uso:  python -I fixture_mirror.py [capacidad]      (por defecto 500, la del pool de tareas del juego)
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
# Tamanos de arena que el oraculo da por supuestos (suma a mano guarda + objeto + guarda):
#  this 0x40 + 0x74 + 0x4C = 0x100 ; partner 0x40 + 0x40 + 0x80 = 0x100 ; nodos 0x20 + 3*8 + 0x48 = 0x80
ARENAS = {'this_bytes': 0x74, 'this_off': 0x40, 'this_arena': 0x100, 'ent_bytes': 0x40, 'ent_off': 0x40, 'ent_arena': 0x100,
          'node_size': 8, 'node_off': 0x20, 'node_arena': 0x80, 'slot': 0x80, 'window': 3}
assert 0x40 + 0x74 + 0x4C == 0x100 and 0x40 + 0x40 + 0x80 == 0x100 and 0x20 + 3 * 8 + 0x48 == 0x80


def build_and_run(cap):
    out = Path(tempfile.mkdtemp(prefix='pcdeal31-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s" %d\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe', cap), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def to_input(row):
    inp = dict(row)
    inp['refs_off'] = oracle.REFS_OFF
    inp['this_off'] = 0x40
    inp['refs'] = {'chain': [5, 6, 7, 8]}
    win = row['pool']['win']
    pp = []
    for j in range(row['refs_len']):
        v = 0xA5A50000 + j * 0x10 + row['pp_rand'][j]
        if j == row['dup_idx'] and row['dup_kind'] == 1 and len(win) >= 2:
            v = 'W1+12'
        pp.append(v)
    inp['node_pp'] = pp
    return inp


def main(cap=500):
    rows = build_and_run(cap)
    sizes = rows.pop()['sizes']
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s != %s' % (sizes, ARENAS))
    assert len(rows) == oracle.CASES, len(rows)
    cov = {}
    for n, row in enumerate(rows):
        want, stats = oracle.expected(oracle.NAME, to_input(row), n)
        for k, v in stats.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
    import run_local
    short = {k: (cov.get(k, 0), m) for k, m in run_local.MINIMUMS[oracle.NAME].items() if cov.get(k, 0) < m}
    print(json.dumps(dict(cases=len(rows), coverage=dict(sorted(cov.items())), shortfalls=short), indent=1))
    if short:
        raise SystemExit('cobertura insuficiente: %s' % short)
    print('ok')


if __name__ == '__main__':
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 500)
