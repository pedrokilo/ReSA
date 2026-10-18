import sys, collections

rows_file = sys.argv[1]
p = r'D:\Original\gta-reversed-master\docs\hooks.csv'
rows = [l for l in open(rows_file, encoding='utf-8').read().splitlines() if l.strip()]
b = open(p, 'rb').read().decode('utf-8')
nl = '\r\n' if '\r\n' in b else '\n'
L = b.split(nl)
have = {tuple(l.split(',')[:2]) for l in L}
new = [r for r in rows if tuple(r.split(',')[:2]) not in have]
byc = collections.defaultdict(list)
for r in new:
    byc[r.split(',')[0]].append(r)
for c, rs in byc.items():
    idx = [k for k, l in enumerate(L) if l.startswith(c + ',')]
    if idx:
        L[idx[-1] + 1:idx[-1] + 1] = rs
    else:
        while L and L[-1] == '':
            L.pop()
        L += rs + ['']
open(p, 'wb').write(nl.join(L).encode('utf-8'))
print(len(rows), 'filas;', len(new), 'nuevas')
