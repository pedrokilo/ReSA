# Estatico 2: (a) sumas de literales '0xBASE + 0xOFF' evaluadas; (b) base literal + indice no constante: objetos propios (.bss) que empiezan dentro de BASE..BASE+ventana.
import re, glob, os, json, bisect, sys, csv
raiz = sys.argv[1]; mapa = sys.argv[2]; ventana = int(sys.argv[3]) if len(sys.argv) > 3 else 512
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objmap
aqui = os.path.dirname(os.path.abspath(__file__))
sizes = {int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_prev.json'))).items()}
syms = objmap.cargar(mapa); objs = []
for i, (va, nom, ea, ns, f) in enumerate(syms):
    if ns not in ('Bss', 'R5') or nom.startswith('?kConstruct'): continue
    sz = sizes.get(ea) or max(1, min(syms[i + 1][0] - va, 4096)); objs.append((ea, sz, nom.split('@')[0][1:]))
objs.sort(); st = [o[0] for o in objs]
plan = {int(r['va'], 16): r['lote'] for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8'))}
H = r'0x[0-9A-Fa-f]{6,8}'
suma = re.compile(r'(' + H + r')\s*\+\s*(0x[0-9A-Fa-f]{1,5}|\d{1,5})\b(?!\s*[*\w(])')
base_idx = re.compile(r'(' + H + r')\s*\+\s*(?:static_cast<[^>]+>\(|\()?\s*[A-Za-z_]')
base_idx2 = re.compile(r'(' + H + r')\s*\+\s*(?:static_cast<\w+>\()?[A-Za-z_]')
out_a, out_b = [], []
for p in glob.glob(os.path.join(raiz, 'source', '**', '*'), recursive=True):
    if not p.endswith(('.cpp', '.h', '.inl', '.hpp')): continue
    rel = os.path.relpath(p, os.path.join(raiz, 'source')).replace(chr(92), '/')
    if rel.startswith('game_sa/DatosExe/'): continue
    for ln, line in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
        c = line.split('//')[0]
        if 'RH_Scoped' in c: continue
        for m in suma.finditer(c):
            a = int(m.group(1), 16) + int(m.group(2), 0)
            if 0x8E4000 <= a < 0xCA0000:
                i = bisect.bisect_right(st, a) - 1
                if i >= 0 and objs[i][0] <= a < objs[i][0] + objs[i][1]:
                    out_a.append((rel, ln, a, objs[i], c.strip()[:130]))
        for m in list(base_idx.finditer(c)) + list(base_idx2.finditer(c)):
            b = int(m.group(1), 16)
            if not (0x8E4000 <= b < 0xCA0000): continue
            j = bisect.bisect_left(st, b)
            near = []
            while j < len(objs) and objs[j][0] < b + ventana:
                near.append(objs[j]); j += 1
            # base dentro de un objeto no es 'cerca' (ya lo cubre estatico.py)
            if near: out_b.append((rel, ln, b, near, c.strip()[:130]))
print('(a) sumas literal+literal que caen dentro de un objeto propio:', len(out_a))
for rel, ln, a, o, c in out_a: print('  %s:%d 0x%X -> %s @0x%X (lote %s) | %s' % (rel, ln, a, o[2], o[0], plan.get(o[0], '?'), c))
print('(b) base literal + indice, objetos propios en [base, base+%d):' % ventana, len(out_b))
for rel, ln, b, near, c in out_b:
    print('  %s:%d base 0x%X -> %s | %s' % (rel, ln, b, ', '.join('%s@+0x%X(%d,lote %s)' % (o[2], o[0] - b, o[1], plan.get(o[0], '?')) for o in near[:6]), c))
