# literales cuya direccion queda 1..96 bytes ANTES de un objeto declarado (bloques base+offset, ayudantes) y cuyo contexto no fija el tamano
import re, os, glob, sys, bisect, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
out = []
for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True):
    if not p.endswith(('.h', '.cpp', '.inl', '.hpp')) or os.sep + 'DatosExe' + os.sep in p: continue
    txt = E.strip_comments(open(p, encoding='utf-8', errors='replace').read())
    for ln, l in enumerate(txt.split('\n'), 1):
        c = re.sub(r'//.*', '', l); c = re.sub(r'"[^"]*"', '""', c)
        ext = {}
        for m in E.sref.finditer(c): ext[int(m.group(2), 16)] = E.typesize(m.group(1))
        for m in E.lit.finditer(c):
            a = int(m.group(1), 16)
            if not (0x8E4000 <= a < 0xCB1000): continue
            if ext.get(a): continue   # tamano conocido: lo cubre estatico.py
            j = bisect.bisect_left(E.starts, a)
            near = []
            while j < len(E.objsl) and E.objsl[j][0] <= a + 96:
                if E.objsl[j][0] != a or True: near.append(E.objsl[j])
                j += 1
            if near: out.append((a, os.path.relpath(p, E.SRC), ln, c.strip()[:130], near))
print(len(out))
for a, f, ln, c, near in sorted(out):
    inl = [n for n in near if n[3]]
    if not inl: continue
    print('0x%X %s:%d | %s -> %s' % (a, f, ln, c[:110], ' '.join('%s@+%d' % (n[2], n[0]-a) for n in inl[:4])))
