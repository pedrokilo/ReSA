# Estatico 4: constantes con nombre = literal .bss (constexpr/const/define) usadas con aritmetica (NOMBRE + ..., NOMBRE[...], NOMBRE * ...): objetos propios que empiezan dentro de (base, base+ventana).
import re, glob, os, json, bisect, sys, csv
raiz = sys.argv[1]; mapa = sys.argv[2]; ventana = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x800
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objmap
aqui = os.path.dirname(os.path.abspath(__file__))
sizes = {int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_prev.json'))).items()}
sizes.update({int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_slots.json'))).items()})
syms = objmap.cargar(mapa); objs = []
for i, (va, nom, ea, ns, f) in enumerate(syms):
    if ns not in ('Bss', 'R5') or nom.startswith('?kConstruct'): continue
    sz = sizes.get(ea) or max(1, min(syms[i + 1][0] - va, 16)); objs.append((ea, sz, nom.split('@')[0][1:]))
objs.sort(); st = [o[0] for o in objs]
plan = {int(r['va'], 16): r['lote'] for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8'))}
decl = re.compile(r'\b([A-Za-z_]\w*)\s*(?:=|\{)\s*(?:\([\w\s:<>*]+\)\s*)?(0x[0-9A-Fa-f]{6,8})u?\b')
define = re.compile(r'#\s*define\s+([A-Za-z_]\w*)\s+\(?(0x[0-9A-Fa-f]{6,8})')
files = [p for p in glob.glob(os.path.join(raiz, 'source', '**', '*'), recursive=True) if p.endswith(('.cpp', '.h', '.inl', '.hpp')) and 'game_sa' + os.sep + 'DatosExe' not in p]
consts = {}
for p in files:
    rel = os.path.relpath(p, os.path.join(raiz, 'source')).replace(chr(92), '/')
    for ln, line in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
        c = line.split('//')[0]
        for rx in (decl, define):
            for m in rx.finditer(c):
                a = int(m.group(2), 16)
                if 0x8E4000 <= a < 0xCA0000 and m.group(1) not in ('Raw',): consts.setdefault((rel, m.group(1)), (a, ln, c.strip()[:100]))
print('constantes con nombre = literal .bss:', len(consts))
uso = re.compile(r'\b(%s)\b\s*(\+|\[|-|\*)')
hits = []
byfile = {}
for (rel, name), (a, ln, c) in consts.items(): byfile.setdefault(rel, []).append((name, a, ln, c))
for rel, lst in byfile.items():
    txt = open(os.path.join(raiz, 'source', rel), encoding='utf-8', errors='replace').read().splitlines()
    for name, a, ln, c in lst:
        rx = re.compile(r'\b' + re.escape(name) + r'\b\s*(\+|\[|\*|-)')
        n = [i + 1 for i, l in enumerate(txt) if rx.search(l.split('//')[0]) and i + 1 != ln]
        if not n: continue
        j = bisect.bisect_left(st, a)
        near = []
        # objeto que contiene a o empieza en (a, a+ventana)
        i0 = bisect.bisect_right(st, a) - 1
        if i0 >= 0 and objs[i0][0] <= a < objs[i0][0] + objs[i0][1]: near.append(objs[i0])
        while j < len(objs) and objs[j][0] < a + ventana: near.append(objs[j]); j += 1
        if near: hits.append((rel, name, a, ln, n[:3], near[:6], c))
print('con aritmetica y objetos propios cerca:', len(hits))
for rel, name, a, ln, n, near, c in sorted(hits, key=lambda h: h[2]):
    print('%s:%d %s=0x%X usos %s -> %s' % (rel, ln, name, a, n, ', '.join('%s@+0x%X(%d,%s)' % (o[2], o[0] - a, o[1], plan.get(o[0], '?')) for o in near)))
