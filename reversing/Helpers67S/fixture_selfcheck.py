"""Autoprueba cruzada del banco Helpers67S SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 16384 filas `input` y las constantes de arena,
comprueba que las constantes coinciden con las del oraculo (LAYOUT de fixture_oracle.py), pasa cada fila por el oraculo y calcula la cobertura frente a los minimos de run_local.py.

    python -I fixture_selfcheck.py            # imprime la cobertura y 'ok'
Necesita Visual Studio (vcvars64.bat); solo compila C++ estandar, no el proyecto.
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines()]


def coverage(rows):
    layout = rows[0]['layout']
    for key, value in oracle.LAYOUT.items():
        assert layout[key] == value, ('constante de arena distinta', key, layout[key], value)
    assert layout['cases'] == oracle.CASES
    cov = {}
    for row in rows[1:]:
        want, stats = oracle.expected(oracle.NAMES[0], row, row['n'])
        cov['f0_stack_seed'] = cov.get('f0_stack_seed', 0) + 1
        for key, value in stats.items():
            if key == 'family':
                k = 'f0_fam%d' % value
                cov[k] = cov.get(k, 0) + 1
            elif value:
                cov['f0_%s' % key] = cov.get('f0_%s' % key, 0) + 1
    return cov


if __name__ == '__main__':
    rows = dump_rows()
    assert len(rows) == oracle.CASES + 1
    assert [r['n'] for r in rows[1:]] == list(range(oracle.CASES))
    cov = coverage(rows)
    print(json.dumps(dict(sorted(cov.items())), indent=1))
    import run_local
    short = run_local.check_minimums(cov)
    assert not short, short
    print('ok')
