"""Resuelve direcciones propias (VA) a simbolos del .map y direcciones originales a su fila de funciones-vivas.csv.
Uso: python -I resolver.py <map> <va> [<va>...]"""
import re, sys, bisect, csv
from pathlib import Path
raiz = Path(__file__).resolve().parents[2]
mp, *vas = sys.argv[1:]
pub = []
for l in open(mp, encoding='utf-8', errors='replace'):
    m = re.match(r'\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+f?\s*i?\s+(\S+)', l)
    if m: pub.append((int(m.group(2), 16), m.group(1), m.group(3)))
pub.sort()
keys = [p[0] for p in pub]
vivas = {}
f = raiz / 'docs/CodigoVivoExe/funciones-vivas.csv'
if f.exists():
    for r in csv.DictReader(open(f, encoding='utf-8')):
        try: vivas[int(r.get('va') or '0', 16)] = r
        except Exception: pass
for v in vas:
    a = int(v, 16)
    if a < 0xCB1000:
        print(hex(a), 'ORIGINAL', vivas.get(a, ''))
        continue
    i = bisect.bisect_right(keys, a) - 1
    print(hex(a), pub[i][1], '+0x%X' % (a - pub[i][0]), pub[i][2])
