"""Autoprueba del banco Fn91A SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los 16384 casos por el oraculo (fixture_oracle.py) y cuenta la cobertura de ramas
(marcas) y por familia. Ayuda a calibrar coverage.json y a comprobar que cada marca de KEYS (run_local.py) aparece.

    python -I fixture_selfcheck.py
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


if __name__ == '__main__':
    import time
    t0 = time.time()
    rows = dump_rows()
    cov, fam = {}, {}
    for row in rows:
        e = oracle.expected(row)
        fn = row['fn']
        for m in set(e['marks']):
            cov.setdefault(fn, {})
            cov[fn][m] = cov[fn].get(m, 0) + 1
        fam.setdefault(fn, {})
        fam[fn][row['family']] = fam[fn].get(row['family'], 0) + 1
    print(len(rows), 'casos;', {fn: len(f) for fn, f in fam.items()}, 'familias', 'segundos', round(time.time() - t0))
    for fn in sorted(cov):
        print(fn, json.dumps(dict(sorted(cov[fn].items()))))
    print('ok')
