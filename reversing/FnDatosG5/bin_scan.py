# Referencias por direccion fija (VA del EXE original) dentro del codigo del standalone: escanea .text del PE buscando palabras de 32 bits que caen en
# [VA, VA+tam) de un objeto DatosExe declarado (o hasta 16 bytes antes: bloque base+offset), y las atribuye a la funcion del mapa de enlace.
import re, struct, sys, os, bisect, json, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objs, estatico as E
exe = sys.argv[1]; mp = sys.argv[2]
d = open(exe, 'rb').read()
pe = struct.unpack_from('<I', d, 0x3C)[0]
nsec = struct.unpack_from('<H', d, pe + 6)[0]; opt = struct.unpack_from('<H', d, pe + 20)[0]; base = struct.unpack_from('<I', d, pe + 52)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + i * 40
    name = d[o:o+8].rstrip(b'\0').decode(); vsz, rva, rsz, raw = struct.unpack_from('<IIII', d, o + 8); ch = struct.unpack_from('<I', d, o + 36)[0]
    secs.append((name, rva, vsz, raw, rsz, ch))
print([(s[0], hex(s[1]), hex(s[2])) for s in secs])
# funciones del mapa
fn = []
for l in open(mp, encoding='utf-8', errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if m: fn.append((int(m.group(2), 16), m.group(1)))
fn.sort(); fa = [a for a, _ in fn]
def funcion(va):
    i = bisect.bisect_right(fa, va) - 1
    return fn[i][1] if i >= 0 else '?'
objl = E.objsl; st = E.starts
lo, hi = 0x8E4000 - 16, 0xCB1000
res = collections.defaultdict(list)
for name, rva, vsz, raw, rsz, ch in secs:
    if not ch & 0x20000000: continue   # solo codigo
    seg = d[raw:raw + min(rsz, vsz)]
    # busca cualquier palabra alineada a byte: 3 bytes 'xx yy 0..' => valor 0x00B?xxxx; patron: byte alto 0x00 y segundo byte entre 0x8E..0xCB
    idx = []; v = {}
    pat = re.compile(b'(?=(..[' + bytes([0x8e]) + b'-' + bytes([0xcb]) + b']' + bytes([0]) + b'))', re.S)
    for m in pat.finditer(seg):
        i0 = m.start(); val = struct.unpack('<I', m.group(1))[0]
        if lo <= val < hi: idx.append(i0); v[i0] = val
    for i in idx:
        val = v[i]; j = bisect.bisect_right(st, val) - 1
        hit = None
        if j >= 0 and objl[j][0] <= val < objl[j][0] + max(objl[j][1], 1): hit = ('dentro', objl[j])
        else:
            k = bisect.bisect_left(st, val)
            if k < len(objl) and objl[k][0] - val <= 16: hit = ('antes+%d' % (objl[k][0] - val), objl[k])
        if hit:
            va = base + rva + int(i)
            res[hit[1][2]].append((hit[0], val, va, funcion(va), bytes(seg[max(0, int(i)-2):int(i)+4]).hex()))
json.dump({k: v for k, v in res.items()}, open('bin_hits.json', 'w'))
print('objetos con referencias en codigo:', len(res), 'referencias:', sum(len(v) for v in res.values()))
