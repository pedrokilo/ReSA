"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar), reconstruye la fila `input` como Core.hpp y la pasa por el oraculo.
Comprueba: ningun caso invalido (el oraculo rechaza ventanas incoherentes o sin ranura libre), cobertura frente a los minimos de run_local.py, y muestra los recuentos.
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


def build_and_run(cap):
    out = Path(tempfile.mkdtemp(prefix='psseq28-'))
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
    inp['refs'] = {'head': 5, 'next': 6}
    win = row['pool']['win']
    pp = []
    for j in range(row['refs_len']):
        v = 0xA5A50000 + j * 0x10   # el valor concreto no importa al oraculo (no es una etiqueta)
        if row['dup'] and len(win) >= 2 and j == row['dup_node']:
            v = 'W1+12'
        pp.append(v)
    inp['node_pp'] = pp
    return inp


def main(cap=500):
    rows = build_and_run(cap)
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
