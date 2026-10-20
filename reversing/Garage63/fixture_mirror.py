"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion
y los valores del falso (RetVal de C++ frente a ret_val de Python) y la cobertura frente a los minimos de run_local.py.
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
# Constantes de disposicion segun el desensamblado (ver README.md): CGarage 0xD8; izquierda 0x28, derecha 0x2C, frente 0x30, fondo 0x34, z 0x08, topZ 0x1C, puerta 0x38, tiempo 0x3C, tipo 0x4C,
# estado 0x4D, banderas 0x4E; entidad: posicion +4, matriz +0x14, byte +0x36, velocidad +0x44, banderas +0x428, tipo de vehiculo +0x590; vehiculo 0xA18 (imul 0xA18); esfera 0x14 bytes; pool ds:[0xB74494]
LAYOUT = {'garage': 0xD8, 'veh': 0xA18, 'max_veh': 8, 'left': 0x28, 'right': 0x2C, 'front': 0x30, 'back': 0x34, 'posz': 0x08, 'topz': 0x1C, 'door': 0x38, 'time': 0x3C, 'type': 0x4C, 'state': 0x4D,
          'flags': 0x4E, 'pool': 0xB74494, 'pos': 4, 'matrix': 0x14, 'b36': 0x36, 'speed': 0x44, 'f428': 0x428, 'vtype': 0x590, 'sph_stride': 0x14, 'sph_words': 5, 'mat_words': 24}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcg63-'))
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
            seed, kk, val = json.loads(js)
            if oracle.rand(seed, kk * 4 + 2) != val:
                raise SystemExit('Rand distinto entre C++ y Python: semilla %d k %d: %d / %d' % (seed, kk, val, oracle.rand(seed, kk * 4 + 2)))
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
        short = run_local.check_minimums(cov)
    except ImportError:
        short = {}
    for name, c in cov.items():
        print(name, json.dumps(dict(sorted(c.items()))))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
