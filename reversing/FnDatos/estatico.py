# Estatico ampliado: citas por literal (0x858000-0xCB0FFF) en source/ (fuera de DatosExe/) que caen dentro de la extension de un objeto propio .bss,
# cualquier forma (StaticRef, cast, ayudante, literal con suma de indice...). Extension del objeto: sizeof compilado (sizes_prev.json) o hueco del .map.
import re, glob, os, json, csv, bisect, sys, collections
raiz = sys.argv[1]; mapa = sys.argv[2]
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objmap
sizes = {int(k): v for k, v in json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'sizes_prev.json'))).items()}
syms = objmap.cargar(mapa)
objs = []
for i, (va, nom, ea, ns, f) in enumerate(syms):
    if ns not in ('Bss', 'R5') or nom.startswith('?kConstruct'): continue
    sz = sizes.get(ea)
    if sz is None:
        nxt = syms[i + 1][0] if i + 1 < len(syms) else va + 4; sz = max(1, nxt - va); src = 'hueco'
        if sz > 0x100000: sz = 4096
    else: src = 'sizeof'
    objs.append((ea, sz, nom, src))
objs.sort(); st = [o[0] for o in objs]
plan = {int(r['va'], 16): r['lote'] for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8'))}
def lote_de(a):
    # lote del objeto del plan que contiene a (o el mas cercano por debajo)
    return plan.get(a, '?')
lit = re.compile(r'0x([0-9A-Fa-f]{6,8})\b')
res = []
for p in glob.glob(os.path.join(raiz, 'source', '**', '*'), recursive=True):
    if not p.endswith(('.cpp', '.h', '.inl', '.hpp')): continue
    rel = os.path.relpath(p, os.path.join(raiz, 'source')).replace(chr(92), '/')
    if rel.startswith('game_sa/DatosExe/'): continue
    for ln, line in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
        code = line.split('//')[0]
        if 'RH_Scoped' in code: continue
        for m in lit.finditer(code):
            a = int(m.group(1), 16)
            if not (0x8E4000 <= a < 0xCA0000): continue
            i = bisect.bisect_right(st, a) - 1
            if i < 0: continue
            o = objs[i]
            if o[0] <= a < o[0] + o[1]:
                res.append((rel, ln, a, o, code.strip()[:140]))
print('citas por literal dentro de objetos propios .bss:', len(res))
for rel, ln, a, o, c in res:
    print('%s:%d 0x%X -> %s @0x%X +%d (sz %d %s) lote %s | %s' % (rel, ln, a, o[2].split('@')[0][1:], o[0], a - o[0], o[1], o[3], lote_de(o[0]), c))
