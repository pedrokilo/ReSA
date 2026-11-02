"""Cobertura por sitio de llamada de un comparisons.jsonl: python sites.py <carpeta>  (imprime los sitios bajo el minimo y donde terminan los casos)"""
import sys, json
from collections import Counter
sys.path.insert(0, str(__import__('pathlib').Path(__file__).resolve().parents[1]))
import run_local as r
cnt = Counter(); last = Counter(); pairs = 0; diff = 0
for line in open(sys.argv[1] + '/comparisons.jsonl', encoding='utf-8'):
    row = json.loads(line)
    if row['event'] == 'comparison':
        pairs += 1; diff += not row['equal']
        for a in set(row['sites']): cnt[a] += 1
        last[row['sites'][-1] if row['sites'] else 0] += 1
want = r.expected_sites()
print('pares', pairs, 'diferencias', diff)
print(len(want), 'sitios;', sum(1 for a in want if cnt[a] >= 20), '>=20;', sum(1 for a in want if cnt[a] == 0), 'a 0')
print('bajos', [('%X' % a, cnt[a]) for a in want if cnt[a] < 20])
print('finales', [('%X' % a, c) for a, c in last.most_common(14)])
print('todos', ' '.join('%X:%d' % (a, cnt[a]) for a in want))
