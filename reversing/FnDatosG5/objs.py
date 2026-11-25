# Objetos DatosExe de los lotes DE05-1..30 (plan-lotes.csv): nombre, tipo, VA, tam y direccion propia (mapa de enlace)
import csv, re, glob, os, json, sys
R = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
def lotes(maxl=30):
    rows = list(csv.DictReader(open(os.path.join(R, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8')))
    out = {}
    for r in rows:
        m = re.fullmatch(r'DE05-(\d+)', r['lote'])
        if m and 1 <= int(m.group(1)) <= maxl: out[int(r['va'], 16)] = (int(m.group(1)), int(r['tam'] or 0), r['tipos'], r['formas'], r['tam_origen'])
    return out
decl = re.compile(r'^\s*extern\s+(?:const\s+)?([\w:<>\*\s,\'\[\]]+?)\s+(\w+)((?:\[[^\]]*\])*)\s*;\s*//\s*0x([0-9A-Fa-f]+)')
tmpl = re.compile(r'template <class T> T& (\w+)\(\);\s*//\s*0x([0-9A-Fa-f]+)')
def declared():
    d = {}
    for h in glob.glob(os.path.join(R, 'source/game_sa/DatosExe/*.h')):
        for l in open(h, encoding='utf-8', errors='replace'):
            m = decl.match(l)
            if m: d[int(m.group(4), 16)] = (m.group(2), (m.group(1).strip() + m.group(3)), os.path.basename(h)); continue
            m = tmpl.search(l)
            if m: d[int(m.group(2), 16)] = (m.group(1), 'F', os.path.basename(h))
    return d
def mapa(path):
    syms = {}
    for l in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'\s*0003:[0-9a-f]+\s+\?(\w+)@(?:Bss|Data)@DatosExe@@3\S+\s+([0-9a-f]{8})', l)
        if m: syms[m.group(1)] = int(m.group(2), 16)
    return syms
if __name__ == '__main__':
    L = lotes(); D = declared()
    print('objetos plan 1..30:', len(L), 'declarados hoy:', sum(1 for a in L if a in D))
    print('sin declarar:', [hex(a) for a in L if a not in D][:20])

def mapa2(path):
    """nombre -> direccion propia (b<DIR>/v<DIR>/sl_<DIR> de DatosExe en Bss o Data)"""
    syms = {}
    for l in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'\s*0003:[0-9a-f]+\s+\?(\w+)@(?:Bss|Data)@DatosExe@@3\S+\s+([0-9a-f]{8})', l)
        if m: syms[m.group(1)] = int(m.group(2), 16)
    return syms
def tabla(mapfile, maxl=30):
    L = lotes(maxl); D = declared(); S = mapa2(mapfile); out = []
    for va, (lote, tam, tipos, formas, to) in sorted(L.items()):
        if va not in D: continue
        n, t, h = D[va]; key = ('sl_%X' % va).replace('sl_', 'sl_%06X' % va) if False else None
        own = S.get(n)
        if own is None and t == 'F':
            own = S.get('sl_%06X' % va)
        out.append(dict(va=va, name=n, type=t, hdr=h, lote=lote, tam=tam, tam_origen=to, own=own))
    return out
