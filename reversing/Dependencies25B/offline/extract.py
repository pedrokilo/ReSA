"""Extrae de rwcore.cpp, rpanisot.cpp y rpworld.cpp (sin cambios) el texto de las 20 funciones reconstruidas (desde su comentario `// 0xADDR` hasta la llave de cierre)
y las referencias estaticas `static auto& s_...`, y los deja en cand.inc para offline/main.cpp.
Uso: python extract.py [raiz de source/game_sa/RenderWare/rw]   (por omision, la del repositorio)"""
import re
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
rw = Path(sys.argv[1]) if len(sys.argv) > 1 else here.parents[2] / 'source/game_sa/RenderWare/rw'
ADDRS = ['4C9970', '748F20', '748F30', '748F50', '7498E0', '74BE30', '74BE50', '74BEC0', '74C760', '74E1B0', '74E2B0', '74E420', '74FCD0', '74FD00', '751050', '751D60',
         '7522E0', '7588D0', '7588E0', '758900']
out = []
refs = []
for name in ('rwcore.cpp', 'rpanisot.cpp', 'rpworld.cpp'):
    t = (rw / name).read_text(encoding='utf-8')
    refs += re.findall(r'^static auto& s_\w+ += StaticRef<[^;]+;.*$', t, re.M)
    for a in ADDRS:
        m = re.search(r'^// 0x%s\n' % a, t, re.M)
        if not m:
            continue
        end = t.index('\n}\n', m.start()) + 3
        out.append(t[m.start():end])
found = [re.match(r'// 0x(\w+)', x).group(1) for x in out]
assert sorted(found) == sorted(ADDRS), (sorted(set(ADDRS) - set(found)))
(here / 'cand.inc').write_text('\n'.join(refs) + '\n\n' + '\n'.join(out), encoding='utf-8', newline='\n')
print('extraidas', len(out), 'funciones y', len(refs), 'referencias de', rw)
