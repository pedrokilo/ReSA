# Estatico 3: literales .bss pasados como argumento a ayudantes/lambdas (base de un bloque) o con otra forma que no sea StaticRef<T>(0x..):
# objetos propios que empiezan dentro de (base, base+ventana). Posible acceso 'base + desplazamiento' escondido.
import re, glob, os, json, bisect, sys, csv
raiz = sys.argv[1]; mapa = sys.argv[2]; ventana = int(sys.argv[3]) if len(sys.argv) > 3 else 128
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objmap
aqui = os.path.dirname(os.path.abspath(__file__))
sizes = {int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_prev.json'))).items()}
syms = objmap.cargar(mapa); objs = []
for i, (va, nom, ea, ns, f) in enumerate(syms):
    if ns not in ('Bss', 'R5') or nom.startswith('?kConstruct'): continue
    sz = sizes.get(ea) or max(1, min(syms[i + 1][0] - va, 16)); objs.append((ea, sz, nom.split('@')[0][1:]))
objs.sort(); st = [o[0] for o in objs]
plan = {int(r['va'], 16): r['lote'] for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8'))}
call = re.compile(r'\b([A-Za-z_]\w*)\s*(?:<[^>]*>)?\s*\(\s*(0x[0-9A-Fa-f]{6,8})\b')
EXC = {'StaticRef', 'ScopedStaticRef', 'RH_ScopedInstall', 'RH_ScopedVirtualInstall', 'RH_ScopedOverloadedInstall', 'RH_ScopedGlobalInstall', 'RH_ScopedVMTInstall', 'RH_ScopedCategory', 'if', 'sizeof', 'static_assert'}
res = {}
for p in glob.glob(os.path.join(raiz, 'source', '**', '*'), recursive=True):
    if not p.endswith(('.cpp', '.h', '.inl', '.hpp')): continue
    rel = os.path.relpath(p, os.path.join(raiz, 'source')).replace(chr(92), '/')
    if rel.startswith('game_sa/DatosExe/'): continue
    for ln, line in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
        c = line.split('//')[0]
        for m in call.finditer(c):
            fn, a = m.group(1), int(m.group(2), 16)
            if fn in EXC or not (0x8E4000 <= a < 0xCA0000): continue
            j = bisect.bisect_left(st, a + 1); near = []
            while j < len(objs) and objs[j][0] < a + ventana: near.append(objs[j]); j += 1
            # el objeto que empieza justo en 'a' o contiene 'a' lo cubre estatico.py
            if near: res.setdefault((fn, a), []).append((rel, ln, near, c.strip()[:110]))
print('base literal como argumento de ayudante con objetos propios en (base, base+%d):' % ventana, len(res))
for (fn, a), v in sorted(res.items(), key=lambda kv: kv[0][1]):
    near = v[0][2]
    print('0x%X %s(...) x%d %s:%d -> %s | %s' % (a, fn, len(v), v[0][0], v[0][1], ', '.join('%s@+0x%X(%d,%s)' % (o[2], o[0] - a, o[1], plan.get(o[0], '?')) for o in near[:5]), v[0][3]))
