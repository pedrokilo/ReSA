"""Autoprueba del banco Fn607B SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los 16384 casos de cada funcion por el oraculo (fixture_oracle.py: debe poder modelarlos
sin salirse de las arenas) y cuenta la cobertura de ramas (marcas) y por familia. Ayuda a calibrar las marcas de KEYS (run_local.py): cada una debe aparecer al menos COVER_FLOOR veces.

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
    for row in rows:
        e = oracle.expected(row)
        fn = row['fn']
        for m in set(e['marks']):
            cov.setdefault(fn, {})[m] = cov.setdefault(fn, {}).get(m, 0) + 1
        fam.setdefault(fn, {})[row['family']] = fam.setdefault(fn, {}).get(row['family'], 0) + 1
    ok = True
    for fn in sorted(fam):
        print(oracle.NAMES[fn], sum(fam[fn].values()), 'casos;', len(fam[fn]), 'familias', sorted(set(fam[fn].values())))
        print(json.dumps(dict(sorted(cov[fn].items()))))
        short = [k for k in oracle.KEYS[fn] if cov[fn].get(k, 0) < 50]
        if short:
            ok = False
            print('  FALTA COBERTURA:', short)
    print('ok' if ok else 'cobertura insuficiente')
    sys.exit(0 if ok else 1)
