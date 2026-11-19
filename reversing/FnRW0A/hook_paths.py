"""Construye la lista de rutas de hooks (Categoria/Clase) leyendo las fuentes, para --unhook / --unhook-except del arnes."""
import collections
import glob
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
paths = collections.defaultdict(set)
for f in glob.glob(str(ROOT / 'source' / '**' / '*.cpp'), recursive=True):
    t = open(f, encoding='utf-8', errors='replace').read()
    if 'RH_Scoped' not in t:
        continue
    toks = re.findall(r'RH_Scoped(CategoryGlobal|Category|ClassName|Class|VirtualClass|NamespaceName)\(([^)]*)\)|(void\s+[\w:]*InjectHooks\w*\s*\()', t)
    cat = cls = None
    for k, a, fn in toks:
        if fn:
            cat = cls = None
            continue
        a = a.split(',')[0].strip().strip('"')
        if k == 'CategoryGlobal':
            cat = 'Global'
        elif k == 'Category':
            cat = a
        else:
            cls = a
        if cat and cls:
            paths[cat + '/' + cls].add(Path(f).relative_to(ROOT).as_posix())
out = {k: sorted(v) for k, v in sorted(paths.items())}
(ROOT / 'reversing' / 'FnRW0A' / 'hook_paths.json').write_text(json.dumps(out, indent=0), encoding='utf-8')
print(len(out), 'rutas')
rx = re.compile(sys.argv[1], re.I) if len(sys.argv) > 1 else None
if rx:
    print([k for k in out if rx.search(k) or any(rx.search(x) for x in out[k])])
