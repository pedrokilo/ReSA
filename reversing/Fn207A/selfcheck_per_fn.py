"""Cobertura de ramas por funcion (marcas de KEYS en fixture_oracle.py) sobre los 16384 casos de cada funcion, sin juego:  python -I selfcheck_per_fn.py"""
import json
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
import fixture_selfcheck as sc

rows = sc.dump_rows()
cov = {fn: {} for fn in range(len(oracle.NAMES))}
fam = {fn: {} for fn in range(len(oracle.NAMES))}
for row in rows:
    e = oracle.expected(row)
    for m in set(e['marks']):
        cov[row['fn']][m] = cov[row['fn']].get(m, 0) + 1
    fam[row['fn']][row['family']] = fam[row['fn']].get(row['family'], 0) + 1
bad = False
for fn, name in enumerate(oracle.NAMES):
    miss = {k: cov[fn].get(k, 0) for k in oracle.KEYS[fn] if cov[fn].get(k, 0) < 50}
    extra = {k: v for k, v in cov[fn].items() if k not in oracle.KEYS[fn]}
    print(fn, name, 'FALTAN' if miss else 'ok', json.dumps(miss) if miss else '', 'fuera de KEYS:' + json.dumps(extra) if extra else '')
    bad |= bool(miss)
print('familias', sorted({len(f) for f in fam.values()}))
sys.exit(1 if bad else 0)
