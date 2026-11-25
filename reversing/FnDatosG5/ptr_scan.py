# palabras de .rdata/.data inicializado del EXE original (y del standalone) que apuntan DENTRO de objetos DatosExe declarados (punteros guardados en tablas)
import struct, sys, os, bisect, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
def secciones(d):
    pe = struct.unpack_from('<I', d, 0x3C)[0]; n = struct.unpack_from('<H', d, pe + 6)[0]; opt = struct.unpack_from('<H', d, pe + 20)[0]; base = struct.unpack_from('<I', d, pe + 52)[0]
    out = []
    for i in range(n):
        o = pe + 24 + opt + i * 40; nm = d[o:o+8].rstrip(b'\0').decode(); vsz, rva, rsz, raw = struct.unpack_from('<IIII', d, o + 8); out.append((nm, base + rva, vsz, raw, rsz))
    return out
exe = sys.argv[1]; d = open(exe, 'rb').read()
objl = E.objsl; st = E.starts
res = []
for nm, va, vsz, raw, rsz in secciones(d):
    if nm not in ('.rdata', '.data'): continue
    for off in range(0, min(vsz, rsz) - 3, 4):
        w = struct.unpack_from('<I', d, raw + off)[0]
        if not (0x8E4000 <= w < 0xCB1000): continue
        j = bisect.bisect_right(st, w) - 1
        if j >= 0 and objl[j][0] <= w < objl[j][0] + max(objl[j][1], 1):
            res.append((nm, va + off, w, objl[j][2], objl[j][3]))
print(len(res), 'palabras apuntan dentro de objetos declarados')
for r in res: print('%s 0x%X -> 0x%X %s%s' % (r[0], r[1], r[2], r[3], ' *' if r[4] else ''))
