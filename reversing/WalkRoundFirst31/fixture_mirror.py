"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), reconstruye la fila `input` como Core.hpp y la pasa por el oraculo.
Comprueba: ningun caso invalido (el oraculo rechaza ventanas incoherentes), las constantes de arena del C++ (ultima linea) frente a las del oraculo, cobertura frente a los minimos de
run_local.py, y muestra los recuentos.
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
#  this 0x40 + 0x54 = 0x94 <= 0x100 ; objeto 0x20 + 0x20 = 0x40 <= 0x80 ; matriz 0x20 + 0x40 = 0x60 <= 0x100 ; peaton 0x20 + 0x20 = 0x40 <= 0x80 ; matriz del peaton 0x20 + 0x40 = 0x60 <= 0x100 ; ruta 0x20 + 0x64 = 0x84 <= 0x100
ARENAS = {'this_bytes': 0x54, 'this_off': 0x40, 'this_arena': 0x100, 'obj_bytes': 0x20, 'obj_off': 0x20, 'obj_arena': 0x80, 'mat_bytes': 0x40, 'mat_off': 0x20, 'mat_arena': 0x100,
          'ped_bytes': 0x20, 'ped_off': 0x20, 'ped_arena': 0x80, 'pmat_bytes': 0x40, 'pmat_off': 0x20, 'pmat_arena': 0x100, 'route_bytes': 0x64, 'route_off': 0x20, 'route_arena': 0x100,
          'slot': 0x80, 'window': 1}


def build_and_run(cap):
    out = Path(tempfile.mkdtemp(prefix='wrfirst31-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s" %d\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe', cap), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def to_input(row):
    inp = dict(row)
    inp['this_off'] = 0x40
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
