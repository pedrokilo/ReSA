#!/usr/bin/env python3
"""Compara las jerarquias HAnim (plugin 0x11E) de los DFF de peds de una instalacion de GTA SA: numero de nodos, ids de hueso y flags push/pop.
Uso: python analizar_peds.py <carpeta de GTA SA> [ids de modelo...]   (lee data/peds.ide, models/gta3.img y models/player.img; no modifica nada)"""
import struct, sys, re, collections, pathlib
root = pathlib.Path(sys.argv[1])
def img_dir(p):
    f = open(p, 'rb'); h = f.read(8); n, = struct.unpack('<I', h[4:]); ent = {}
    d = f.read(32 * n)
    for i in range(n):
        off, a, b = struct.unpack_from('<IHH', d, i * 32); name = d[i*32+8:i*32+32].split(b'\0')[0].decode('latin1').lower()
        ent[name] = (off * 2048, (b or a) * 2048)
    return f, ent
imgs = [img_dir(root / 'models' / 'gta3.img'), img_dir(root / 'models' / 'player.img')]
def getdff(name):
    for f, ent in imgs:
        if name.lower() + '.dff' in ent:
            off, sz = ent[name.lower() + '.dff']; f.seek(off); return f.read(sz)
    return None
def chunks(d, p, end):
    while p + 12 <= end:
        t, s, v = struct.unpack_from('<III', d, p); yield t, p + 12, s; p += 12 + s
def hanims(d):
    out = []
    def walk(p, end, depth):
        for t, q, s in chunks(d, p, end):
            if t == 0x11E and s >= 12:
                ver, nid, nn = struct.unpack_from('<III', d, q)
                nodes = []
                if nn:
                    fl, ks = struct.unpack_from('<II', d, q + 12)
                    for i in range(nn): nodes.append(struct.unpack_from('<III', d, q + 20 + i * 12))
                    out.append((nid, nn, fl, ks, nodes))
                else: out.append((nid, 0, 0, 0, []))
            elif t in (0x10, 0x0E, 0x03, 0x14, 0x07):  # clump, framelist, extension, geometry list, geometry
                walk(q, q + s, depth + 1)
    walk(0, len(d), 0); return out
ids = {int(x) for x in sys.argv[2:]}
rows = []
for line in open(root / 'data' / 'peds.ide', encoding='latin1'):
    m = re.match(r'\s*(\d+)\s*,\s*([^,\s]+)\s*,\s*([^,\s]+)', line)
    if m and (not ids or int(m.group(1)) in ids): rows.append((int(m.group(1)), m.group(3)))
sig = collections.defaultdict(list)
for mid, nm in rows:
    d = getdff(nm)
    if d is None: sig['(sin DFF en gta3.img/player.img)'].append((mid, nm)); continue
    try: hs = [h for h in hanims(d) if h[1] > 0]
    except Exception as e: sig['(error al leer: %s)' % e].append((mid, nm)); continue
    if not hs: sig['(sin jerarquia HAnim)'].append((mid, nm)); continue
    nid, nn, fl, ks, nodes = hs[0]
    s = 'nodos=%d keyFrameSize=0x%x flags=0x%x ids=%s nodeflags=%s' % (nn, ks, fl, ','.join(str(n[0]) for n in nodes), ''.join(str(n[2]) for n in nodes))
    sig[s].append((mid, nm))
for s, l in sorted(sig.items(), key=lambda x: -len(x[1])):
    print('%4d modelos | %s' % (len(l), s)); print('        ', ', '.join('%d:%s' % x for x in l[:12]) + (' ...' if len(l) > 12 else ''))
