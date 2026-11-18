"""Autoprueba del banco Fn239A SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, pasa los casos por el oraculo (emulacion del EXE) y cuenta la cobertura (marcas).

    python -I fixture_selfcheck.py [casos-por-funcion] [--write-keys]
Comprueba que el oraculo ejecuta cada caso hasta el final (pila, registros no volatiles y SEH restaurados). Necesita Visual Studio (vcvars64.bat); solo compila C++ estandar.
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
        try:
            e = oracle.expected(row)
            out.append((row['fn'], row['family'], e['marks'], None))
        except Exception as ex:   # noqa: BLE001
            out.append((row['fn'], row['family'], [], '%s: %s' % (row['cmd'], ex)))
    return out


if __name__ == '__main__':
    import multiprocessing
    rows = dump_rows()
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    limit = int(args[0]) if args else 0
    if limit:
        per = {}
        keep = []
        for r in rows:
            per[r['fn']] = per.get(r['fn'], 0) + 1
            if per[r['fn']] <= limit:
                keep.append(r)
        rows = keep
    chunks = [rows[i:i + 200] for i in range(0, len(rows), 200)]
    with multiprocessing.Pool() as pool:
        res = [x for part in pool.map(work, chunks) for x in part]
    errors = [x[3] for x in res if x[3]]
    cov = {fn: {} for fn in range(len(oracle.NAMES))}
    fam = {fn: {} for fn in range(len(oracle.NAMES))}
    for fn, family, marks, err in res:
        fam[fn][family] = fam[fn].get(family, 0) + 1
        for m in marks:
            cov[fn][m] = cov[fn].get(m, 0) + 1
    floor = 50 if not limit else max(1, limit // 300)
    if '--write-keys' in sys.argv:
        keys = [sorted(k for k, v in cov[fn].items() if k.startswith('cmd:') or k == 'oor' or v >= 50) for fn in range(len(oracle.NAMES))]
        (HERE / 'coverage.json').write_text(json.dumps(keys), encoding='utf-8')
        print('coverage.json escrito:', [len(k) for k in keys])
    bad = 0
    for fn in range(len(oracle.NAMES)):
        miss = {k: cov[fn].get(k, 0) for k in oracle.KEYS[fn] if cov[fn].get(k, 0) < floor}
        print(fn, oracle.NAMES[fn], len(fam[fn]), 'familias;', len(cov[fn]), 'marcas;', ('FALTAN: ' + json.dumps(miss)) if miss else 'cobertura completa')
        bad += bool(miss)
    print(len(res), 'casos; errores del oraculo:', len(errors), '; funciones con marcas insuficientes:', bad)
    for e in errors[:20]:
        print('  ', e)
    print('ok' if not bad and not errors else 'REVISAR')
