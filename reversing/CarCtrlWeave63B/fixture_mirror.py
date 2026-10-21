"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion
y el Rand de C++ frente al de Python (aqui no hay Rand en el oraculo: solo se comprueba que la fixture es coherente) y mide la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py        (imprime la cobertura medida de entrada; falla si algun minimo de entrada no se alcanza)
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
# Constantes de disposicion segun el desensamblado (ver README.md)
LAYOUT = {'ents': 7, 'nodes': 6, 'ent_words': 0x180, 'mat_words': 0x14, 'cm_words': 0xC, 'mi_words': 0x10, 'intel_words': 0x40, 'guard': 4, 'pos': 4, 'matrix': 0x14, 'model': 0x22, 'flags': 0x1C, 'scan': 0x2C,
          'b36': 0x36, 'speed': 0x44, 'style': 0x3B9, 'tempact': 0x3BB, 'temptime': 0x3BC, 'ctrl': 0x3DB, 'b42b': 0x42B, 'driver': 0x460, 'intel': 0x47C, 'd514': 0x514, 'd584': 0x584, 'dfc': 0xFC,
          'vtype': 0x590, 'timer': 0xB7CB84, 'scan_addr': 0xB7CD78, 'focus': 0xB7CD74, 'players': 0xB7CD98, 'remote': 0xD8}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcw63b-'))
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
        if k == 'R':
            continue
        per[names[int(k)]].append(json.loads(js))
    cov = {}
    for name in names:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            try:
                want, stats = oracle.expected(name, row, n)
            except Exception as e:
                raise SystemExit('Fixture incoherente en %s caso %d: %r' % (name, n, e))
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
        short = run_local.check_minimums(cov, observed=False)
    except ImportError:
        short = {}
    for name, c in cov.items():
        print(name, json.dumps(dict(sorted(c.items()))))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
