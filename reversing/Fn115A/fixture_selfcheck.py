"""Autoprueba del banco Fn115A SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los 16384 casos por el oraculo (fixture_oracle.py) y cuenta la cobertura de ramas
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
    rows = dump_rows()
    cov, fam = {}, {}
    per = {}
    for row in rows:
        e = oracle.expected(row, row)
        for m in set(e['marks']):
            cov[(row['fn'], m)] = cov.get((row['fn'], m), 0) + 1
        fam[row['family']] = fam.get(row['family'], 0) + 1
    print(len(rows), 'casos;', len(fam), 'familias', sorted(set(fam.values())))
    out = {}
    for (fn, m), v in sorted(cov.items()):
        out.setdefault(str(fn), {})[m] = v
    print(json.dumps(out))
    if len(sys.argv) > 1 and sys.argv[1] == '--write':
        print('(use los valores impresos para coverage.json)')
    print('ok')
