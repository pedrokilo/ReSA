import sys
sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
from recorte import decode
a = decode(sys.argv[1]); b = decode(sys.argv[2]); W, H = 1024, 768
dif = []
for y in range(H):
    fa, fb = a[2][y], b[2][y]
    if fa[:3 * W] == fb[:3 * W]: continue
    for x in range(W):
        if max(abs(fa[3*x+i] - fb[3*x+i]) for i in range(3)) > 24: dif.append((x, y))
print('pixeles distintos (>24):', len(dif))
from collections import Counter
cx = Counter(x for x, y in dif); cy = Counter(y for x, y in dif)
print('columnas con mas diferencias:', cx.most_common(8)); print('filas con mas diferencias:', cy.most_common(8))
