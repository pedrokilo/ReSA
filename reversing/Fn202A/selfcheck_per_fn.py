import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_selfcheck as sc, fixture_oracle as oracle
rows = sc.dump_rows()
cov = {}
for row in rows:
    e = oracle.expected(row)
    d = cov.setdefault(row['fn'], {})
    for m in set(e['marks']): d[m] = d.get(m, 0) + 1
for fn in sorted(cov):
    need = oracle.KEYS[fn]
    low = {k: cov[fn].get(k, 0) for k in need if cov[fn].get(k, 0) < 50}
    print(fn, oracle.NAMES[fn], 'BAJO:', low if low else 'ok')
