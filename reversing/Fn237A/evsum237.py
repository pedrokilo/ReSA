import json, sys, collections
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import bank_spec as S
d = Path(sys.argv[1])
miss = collections.Counter(); extra = collections.Counter(); bad = collections.Counter(); tot = 0; nbad = 0
cmds = collections.defaultdict(set)
for line in open(d / 'comparisons.jsonl', encoding='utf-8'):
    if '"comparison"' not in line: continue
    r = json.loads(line); tot += 1
    if r['equal']: continue
    nbad += 1
    a = collections.Counter(e[0] for e in r['original']['observed']['events']); b = collections.Counter(e[0] for e in r['candidate']['observed']['events'])
    for k in (a - b): miss[S.COLLABS[k]['name']] += 1; cmds[S.COLLABS[k]['name']].add(r['input']['cmd'])
    for k in (b - a): extra[S.COLLABS[k]['name']] += 1
    if a == b: bad[r['input']['cmd']] += 1
print('pares', tot, 'con diferencias', nbad)
print('faltan en DLL:', {k: (v, sorted(cmds[k])) for k, v in miss.items()})
print('sobran en DLL:', dict(extra))
print('mismos eventos pero otra cosa, por comando:', dict(bad))
