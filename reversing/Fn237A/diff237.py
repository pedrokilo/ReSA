"""Muestra las diferencias EXE/DLL (y contra el oraculo) de una corrida del banco Fn237A:  python diff237.py <carpeta-de-resultados> [n] [--all]"""
import json, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as O
import bank_spec as S
def short(x):
    t = json.dumps(x)
    return t if len(t) < 700 else t[:700] + '...'
def evname(ev): return [[S.COLLABS[e[0]]['name']] + e[1:] for e in ev]
d = Path(sys.argv[1]); n = int(sys.argv[2]) if len(sys.argv) > 2 else 5
shown = 0; seen_cmd = set()
for line in open(d / 'comparisons.jsonl', encoding='utf-8'):
    if '"event":"comparison"' not in line and '"event": "comparison"' not in line: continue
    r = json.loads(line)
    if r['equal'] and '--all' not in sys.argv: 
        e = O.expected(r['input'])
        bad = O.check_row(r, e)
        if not bad: continue
        print('PAR', r['pair'], 'cmd', r['input']['cmd'], 'ORACULO distinto:', bad[0]); print('   ', short(bad[1])); shown += 1
        if shown >= n: break
        continue
    c = r['input']['cmd']
    if c in seen_cmd and '--all' not in sys.argv: continue
    seen_cmd.add(c)
    print('PAR', r['pair'], 'cmd', c, 'fn', r['function'], 'caso', r['case'])
    o, k = r['original'], r['candidate']
    for key in o:
        if o[key] != k[key]:
            if key == 'observed':
                for kk in o['observed']:
                    if o['observed'][kk] != k['observed'][kk]:
                        a, b = o['observed'][kk], k['observed'][kk]
                        if kk == 'events': a, b = evname(a), evname(b)
                        print('   EXE', kk, short(a)); print('   DLL', kk, short(b))
            else: print('   ', key, short(o[key]), '|', short(k[key]))
    print('   params', r['input']['params'][:10])
    shown += 1
    if shown >= n: break
