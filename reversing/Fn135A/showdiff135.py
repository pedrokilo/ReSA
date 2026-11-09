"""Muestra las primeras filas con diferencias (EXE contra DLL) y las que no coinciden con el oraculo. Uso: python showdiff135.py <carpeta> [n]"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as o
d = Path(sys.argv[1]); lim = int(sys.argv[2]) if len(sys.argv) > 2 else 3
shown = {}
nd = {}; no = {}; tot = {}
for l in open(d / 'comparisons.jsonl', encoding='utf-8'):
    r = json.loads(l)
    if r.get('event') != 'comparison':
        continue
    fn = r['function']; tot[fn] = tot.get(fn, 0) + 1
    a, b = r['original'], r['candidate']
    keys = [k for k in set(a) | set(b) if a.get(k) != b.get(k)]
    okeys = [k for k in (a.get('observed', {})) if a['observed'].get(k) != b['observed'].get(k)]
    bad = o.check_row(r)
    if keys or r['fp_difference']:
        nd[fn] = nd.get(fn, 0) + 1
    if bad:
        no[fn] = no.get(fn, 0) + 1
    if (keys or bad) and shown.get(fn, 0) < lim:
        shown[fn] = shown.get(fn, 0) + 1
        print('==', fn, r['case'], 'claves distintas:', keys, 'observed:', okeys, 'fp:', r['fp_difference'])
        print('  entrada a=', r['input']['a'][:12], 's=', r['input']['s'][:60], 't=', r['input']['t'][:40])
        for k in okeys:
            print('   ', k, 'EXE:', str(a['observed'][k])[:400]); print('   ', k, 'DLL:', str(b['observed'][k])[:400])
        if bad:
            print('  oraculo:', bad[0], bad[1][:400])
print('totales', tot); print('EXE!=DLL', nd); print('lados != oraculo', no)
