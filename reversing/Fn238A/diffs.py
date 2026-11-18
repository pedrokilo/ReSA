"""Resume en flujo las diferencias de una corrida: python -I diffs.py <comparisons.jsonl> [max]"""
import json, sys, collections
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as O
cnt = collections.Counter(); shown = collections.Counter(); orc = collections.Counter()
mx = int(sys.argv[2]) if len(sys.argv) > 2 else 2
with open(sys.argv[1], encoding='utf-8') as fh:
    for line in fh:
        if not line.startswith('{"accepted"'):
            continue
        r = json.loads(line)
        cmd = r['input']['args'][0]
        o, c = r['original']['observed'], r['candidate']['observed']
        res = O.check_row(r)
        if r['equal'] and not res:
            continue
        key = (r['function'].split('::')[-1], cmd, 'EXEvsDLL' if not r['equal'] else 'oraculo')
        cnt[key] += 1
        if shown[key] < mx:
            shown[key] += 1
            print('==', key, 'caso', r['case'])
            if not r['equal']:
                for k in o:
                    if o[k] != c[k]:
                        if k == 'changes':
                            for ar in 'ABCD':
                                a = {x[0]: x[1] for x in o[k][ar]}; b = {x[0]: x[1] for x in c[k][ar]}
                                dd = [(kk, a.get(kk), b.get(kk)) for kk in sorted(set(a) | set(b)) if a.get(kk) != b.get(kk)]
                                if dd: print('  changes', ar, dd[:10])
                        else:
                            print('  ', k, 'EXE', str(o[k])[:500]); print('  ', k, 'DLL', str(c[k])[:500])
            if res:
                print('  oraculo', res[0], res[1][:500])
print(dict(cnt))
