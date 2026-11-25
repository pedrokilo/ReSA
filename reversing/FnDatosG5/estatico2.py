# citas StaticRef con tipo de tamano NO calculable (o forma con indice): objetos declarados que quedan justo detras (posible solape)
import re, os, glob, sys, bisect, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
SRC = E.SRC
res = []
for p in glob.glob(os.path.join(SRC, '**', '*'), recursive=True):
    if not p.endswith(('.h', '.cpp', '.inl', '.hpp')) or os.sep + 'DatosExe' + os.sep in p: continue
    txt = E.strip_comments(open(p, encoding='utf-8', errors='replace').read())
    for ln, l in enumerate(txt.split('\n'), 1):
        c = re.sub(r'//.*', '', l)
        for m in E.sref.finditer(c):
            a = int(m.group(2), 16)
            if not (0x8E4000 <= a < 0xCB1000): continue
            sz = E.typesize(m.group(1))
            if sz is None:
                res.append((a, m.group(1), os.path.relpath(p, SRC), ln))
print(len(res), 'citas con tamano no calculable')
for a, t, f, ln in sorted(res):
    j = bisect.bisect_left(E.starts, a)
    nxt = []
    while j < len(E.objsl) and E.objsl[j][0] < a + 0x4000 and len(nxt) < 3:
        nxt.append('%s@+0x%X%s' % (E.objsl[j][2], E.objsl[j][0] - a, '*' if E.objsl[j][3] else '')); j += 1
    if nxt: print('0x%X %s %s:%d -> %s' % (a, t[:50], f, ln, ' '.join(nxt)))
