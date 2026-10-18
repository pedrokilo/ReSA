import csv, re, os

ROOT = r'D:\Original\gta-reversed-master\reversing\OriginalDependencies2026-10-17'
rd = lambda n: list(csv.DictReader(open(os.path.join(ROOT, n), encoding='utf-8-sig', newline='')))
targets = {r['address']: r for r in rd('targets.csv')}
cls = {r['address']: r for r in rd('classification.csv')}
wq = {r['address']: r for r in rd('work-queue.csv')}


def to_int(x, d=0):
    try:
        return int(x)
    except Exception:
        return d


def sublib(r, t):
    ident = (r['identity'] + ' ' + r['notes'] + ' ' + r.get('evidence', '')).lower()
    note = r['notes']
    name = r['identity'].lower()
    if re.search(r'qtml|quicktime', ident):
        return 'QuickTime/QTML'
    if re.search(r'd3d9', name):
        return 'Controlador D3D9 de RenderWare'
    m = re.search(r'por (L\d|override)', note)
    rule = m.group(1) if m else ''
    if rule in ('L1', 'L3'):
        return 'RenderWare SDK'
    if rule == 'L2' or int(r['address'], 16) >= 0x821000:
        return 'CRT / runtime estático'
    return 'Revisión manual (' + (rule or 'sin regla') + ')'


def tier_game(w):
    score = to_int(w['score'])
    risky = to_int(w['uncatalogued_game_callees']) + to_int(w['indirect_calls']) + to_int(w['x87_instructions']) + to_int(w['switch_tables']) + to_int(w['seh'])
    b = to_int(w['body_bytes'])
    if b <= 60 and risky == 0:
        return '1 Muy fácil'
    if b <= 150 and risky <= 2:
        return '2 Fácil'
    if b <= 500:
        return '3 Media'
    if b <= 1500:
        return '4 Difícil'
    return '5 Muy difícil'


def tier_lib(b):
    if b <= 40:
        return '1 Muy fácil'
    if b <= 150:
        return '2 Fácil'
    if b <= 500:
        return '3 Media'
    if b <= 1500:
        return '4 Difícil'
    return '5 Muy difícil'


rows = []
for a, r in cls.items():
    t = targets[a]
    b = to_int(t.get('body_bytes'))
    if r['category'] == 'game':
        w = wq.get(a)
        if w is None:
            continue
        rows.append(dict(categoria='juego', subcategoria=w['implementation'], address=a, identity=r['identity'], bytes=b,
                         score=to_int(w['score']), nivel=tier_game(w),
                         riesgos=f"callees_sin_catalogar={w['uncatalogued_game_callees']};indirectas={w['indirect_calls']};x87={w['x87_instructions']};switch={w['switch_tables']};seh={w['seh']};biblioteca={w['library_calls']}",
                         fuente=r['source']))
    elif r['category'] == 'library':
        rows.append(dict(categoria='biblioteca', subcategoria=sublib(r, t), address=a, identity=r['identity'], bytes=b,
                         score=b, nivel=tier_lib(b), riesgos='', fuente=r['source']))
rows.sort(key=lambda x: (x['categoria'], x['score'], x['address']))
out = os.path.join(ROOT, 'lista-por-dificultad.csv')
with open(out, 'w', encoding='utf-8', newline='') as f:
    w = csv.DictWriter(f, fieldnames=['rango', 'categoria', 'nivel', 'subcategoria', 'address', 'identity', 'bytes', 'score', 'riesgos', 'fuente'])
    w.writeheader()
    for cat in ('juego', 'biblioteca'):
        for i, r in enumerate([x for x in rows if x['categoria'] == cat], 1):
            w.writerow(dict(rango=i, **r))
import collections
for cat in ('juego', 'biblioteca'):
    c = collections.Counter(x['nivel'] for x in rows if x['categoria'] == cat)
    print(cat, sum(c.values()), sorted(c.items()))
c = collections.Counter(x['subcategoria'] for x in rows if x['categoria'] == 'biblioteca')
print(c)
