import sys, json
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_selfcheck as s, fixture_oracle as o
rows = s.dump_rows()
cov = {}
for r in rows:
    e = o.expected(r)
    d = cov.setdefault(r['fn'], {})
    for m in set(e['marks']):
        d[m] = d.get(m, 0) + 1
bad = 0
for fn in range(len(o.NAMES)):
    need = o.KEYS[fn]
    low = {k: cov.get(fn, {}).get(k, 0) for k in need if cov.get(fn, {}).get(k, 0) < 200}
    if low:
        print(fn, o.NAMES[fn], low); bad += 1
print('funciones con alguna marca < 200:', bad)
