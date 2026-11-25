# vuelve a cruzar todas las StaticRef con el tamano ampliado: objetos declarados que caen DENTRO de la extension (distinta direccion) o que coinciden
import re, os, glob, sys, bisect, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tam_ext, estatico as E
res = []; sin = []
for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True):
    if not p.endswith(('.h', '.cpp', '.inl', '.hpp')) or os.sep + 'DatosExe' + os.sep in p: continue
    txt = E.strip_comments(open(p, encoding='utf-8', errors='replace').read())
    for ln, l in enumerate(txt.split('\n'), 1):
        c = re.sub(r'//.*', '', l)
        for m in E.sref.finditer(c):
            a = int(m.group(2), 16)
            if not (0x8E4000 <= a < 0xCB1000): continue
            sz = E.typesize(m.group(1))
            if sz is None: sin.append((a, m.group(1), os.path.relpath(p, E.SRC), ln)); continue
            j = bisect.bisect_left(E.starts, a)
            while j < len(E.objsl) and E.objsl[j][0] < a + sz:
                o = E.objsl[j]
                if not (o[0] == a and sz <= o[1]) or True:
                    res.append((a, sz, m.group(1)[:50], os.path.relpath(p, E.SRC), ln, o))
                j += 1
            # objeto que contiene la cita
            i = bisect.bisect_right(E.starts, a) - 1
            if i >= 0 and E.objsl[i][0] < a < E.objsl[i][0] + E.objsl[i][1]: res.append((a, sz, m.group(1)[:50], os.path.relpath(p, E.SRC), ln, E.objsl[i]))
print('citas sin tamano ahora:', len(sin))
for r in sorted(set((a, sz, t, f, ln, o[2], o[0], o[1], o[3]) for a, sz, t, f, ln, o in res)):
    a, sz, t, f, ln, n, va, tam, inl = r
    flag = '*' if inl else ' '
    kind = 'MISMA' if va == a else ('DENTRO-ext' if va > a else 'cita-dentro')
    print('%s %s %s 0x%X+0x%X %s -> %s 0x%X tam=%d | %s:%d' % (flag, kind, t, a, sz, '', n, va, tam, f, ln))
json.dump(sin, open('sin_tam.json', 'w'))
