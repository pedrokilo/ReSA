"""Autoprueba del banco Fn236A SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los 16384 casos por el oraculo (fixture_oracle.py) y cuenta la cobertura de ramas
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


def work(rows):
    out = []
    for row in rows:
        e = oracle.expected(row)
        out.append((row['fn'], row['family'], e['marks']))
    return out


if __name__ == '__main__':
    import multiprocessing
    rows = dump_rows()
    limit = int(sys.argv[1]) if len(sys.argv) > 1 else 0
    if limit:
        per = {}
        keep = []
        for r in rows:
            per[r['fn']] = per.get(r['fn'], 0) + 1
            if per[r['fn']] <= limit:
                keep.append(r)
        rows = keep
    chunks = [rows[i:i + 400] for i in range(0, len(rows), 400)]
    with multiprocessing.Pool() as pool:
        res = [x for part in pool.map(work, chunks) for x in part]
    cov = {fn: {} for fn in range(len(oracle.NAMES))}
    fam = {fn: {} for fn in range(len(oracle.NAMES))}
    for fn, family, marks in res:
        fam[fn][family] = fam[fn].get(family, 0) + 1
        for m in marks:
            cov[fn][m] = cov[fn].get(m, 0) + 1
    floor = 50 if not limit else max(1, limit // 300)
    bad = 0
    for fn in range(len(oracle.NAMES)):
        miss = {k: cov[fn].get(k, 0) for k in oracle.KEYS[fn] if cov[fn].get(k, 0) < floor}
        print(fn, oracle.NAMES[fn], len(fam[fn]), 'familias; marcas', json.dumps(dict(sorted(cov[fn].items()))), ('FALTAN: ' + json.dumps(miss)) if miss else '')
        bad += bool(miss)
    print(len(res), 'casos; funciones con marcas insuficientes:', bad)
    print('ok' if not bad else 'REVISAR')
