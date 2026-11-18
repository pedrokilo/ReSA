"""Muestra las diferencias EXE/DLL (original vs candidate) de los pares que difieren. Uso: python -I diffrows.py <comparisons.jsonl> [max]"""
import json, sys
def diff(a, b, path=''):
    out = []
    if isinstance(a, dict) and isinstance(b, dict):
        for k in sorted(set(a) | set(b)):
            out += diff(a.get(k), b.get(k), path + '/' + str(k))
    elif isinstance(a, list) and isinstance(b, list) and len(a) == len(b):
        for i, (x, y) in enumerate(zip(a, b)):
            out += diff(x, y, path + '[%d]' % i)
    elif a != b:
        out.append((path, a, b))
    return out
n = 0
seen = {}
for line in open(sys.argv[1], encoding='utf-8'):
    if '"event":"comparison"' not in line:
        continue
    r = json.loads(line)
    if r['original'] != r['candidate']:
        seen[r['function']] = seen.get(r['function'], 0) + 1
        if n < int(sys.argv[2] if len(sys.argv) > 2 else 5):
            n += 1
            print('PAR', r['pair'], r['function'], r['case'], 'familia', r['input']['family'])
            for p, x, y in diff(r['original'], r['candidate'])[:12]:
                print('   ', p, str(x)[:200], '|', str(y)[:200])
print('pares distintos por funcion', seen)
