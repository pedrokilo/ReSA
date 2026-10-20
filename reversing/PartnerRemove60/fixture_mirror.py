"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo y la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py [numero de bloques de animacion, por defecto 120; el juego usa el valor de 0xB4EA30]
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
# Tamanos de arena que se dan por supuestos (suma a mano guarda + objeto + guarda): this 0x40 + 0x70 + 0x50 = 0x100
ARENAS = {'this_bytes': 0x70, 'this_off': 0x40, 'this_arena': 0x100, 'name_at': 0x5E, 'flag_at': 0x6E, 'block_size': 0x20, 'loaded_at': 0x10,
          'blocks_base': 0xB5D4A0, 'num_blocks_at': 0xB4EA30, 'model_base': 0x63E7, 'keep': 8}
assert 0x40 + 0x70 + 0x50 == 0x100


def build_and_run(nb):
    out = Path(tempfile.mkdtemp(prefix='pcpartnerremove60-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def main(nb=120):
    rows = build_and_run(nb)
    sizes = rows.pop()['sizes']
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s != %s' % (sizes, ARENAS))
    assert len(rows) == oracle.CASES, len(rows)
    cov = {}
    for n, row in enumerate(rows):
        want, stats = oracle.expected(oracle.NAME, row, n)
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
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 120)
