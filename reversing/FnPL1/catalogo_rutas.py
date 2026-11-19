# PL1: reconstruye las rutas Categoria/Clase de los hooks leyendo el fuente (RH_ScopedCategory / RH_Scoped*Class / RH_ScopedNamespace)
import re, os, json, collections, sys
raiz = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'source'))
cat_re = re.compile(r'RH_ScopedCategory\(\s*"([^"]+)"\s*\)|RH_ScopedCategoryGlobal\(\)')
cls_re = re.compile(r'RH_Scoped(?:Virtual)?Class\(\s*([A-Za-z_][\w:<>]*)|RH_ScopedNamedClass\(\s*[^,]+,\s*"?([^")]+)"?\s*\)|RH_ScopedNamespaceName\(\s*"([^"]+)"|RH_ScopedNamespace\(\s*([A-Za-z_]\w*)')
rutas = collections.defaultdict(set)   # categoria -> clases
for d, _, fs in os.walk(raiz):
    for f in fs:
        if not f.endswith(('.cpp', '.h', '.hpp')): continue
        t = open(os.path.join(d, f), encoding='utf-8', errors='ignore').read()
        if 'RH_Scoped' not in t: continue
        ev = []
        for m in cat_re.finditer(t): ev.append((m.start(), 'cat', m.group(1) or 'Global'))
        for m in cls_re.finditer(t): ev.append((m.start(), 'cls', next(g for g in m.groups() if g)))
        ev.sort()
        for i, (p, k, v) in enumerate(ev):
            if k != 'cls': continue
            sig = next((e for e in ev[i+1:] if e[1] == 'cat'), None)
            ant = next((e for e in reversed(ev[:i]) if e[1] == 'cat'), None)
            if sig and sig[0] - p < 250: cat = sig[2]
            elif ant: cat = ant[2]
            elif sig: cat = sig[2]
            else: continue
            rutas[cat].add(v)
out = {c: sorted(v) for c, v in sorted(rutas.items())}
json.dump(out, open(os.path.join(os.path.dirname(__file__), 'rutas.json'), 'w'), indent=1)
print(len(out), 'categorias', sum(len(v) for v in out.values()), 'rutas')
