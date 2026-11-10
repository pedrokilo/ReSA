"""Autoprueba del banco Fn709CF0 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los 16384 casos por el oraculo (fixture_oracle.py) y cuenta la cobertura de ramas
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
    cov = {}
    fam = {}
    for inp in rows:
        e = oracle.expected(inp)
        for m in set(e['marks']):
            cov[m] = cov.get(m, 0) + 1
        fam[inp['family']] = fam.get(inp['family'], 0) + 1
    print('casos', len(rows), 'familias', len(fam), 'min por familia', min(fam.values()))
    keys = oracle.KEYS[0]
    for k in sorted(cov):
        print('  %-18s %6d%s' % (k, cov[k], '' if k in keys else '  (no esta en KEYS)'))
    missing = [k for k in keys if k not in cov]
    print('KEYS sin cubrir:', missing)
    sys.exit(1 if missing else 0)
