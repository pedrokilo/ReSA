# Estatico ampliado: citas por literal (0x8E4000-0xCB0FFF) en source/ frente a objetos DatosExe declarados (todos los lotes; marca los de 1-30).
import re, os, glob, json, sys, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objs
R = objs.R; SRC = os.path.join(R, 'source')
D = objs.declared(); L = objs.lotes(30)
# tamanos de objetos: de la tabla del plan (tam) donde exista; si no, tipo escalar
ESC = {'bool':1,'char':1,'int8':1,'uint8':1,'int16':2,'uint16':2,'int32':4,'uint32':4,'int':4,'float':4,'double':8,'int64':8,'uint64':8,'CVector':12,'RwV2d':8,'CVector2D':8,'RwV3d':12,'CRGBA':4,'RwRGBA':4,'CRect':16,'eWeaponType':4,'size_t':4,'long':4,'DWORD':4,'BOOL':4,'HANDLE':4,'void*':4,'CVector2D':8, 'uint32_t':4,'int32_t':4,'uint8_t':1,'int8_t':1,'uint16_t':2,'int16_t':2,'unsigned':4,'short':2}
allplan = {}
import csv
for r in csv.DictReader(open(os.path.join(R, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8')):
    allplan[int(r['va'], 16)] = int(r['tam'] or 0)
# tamanos de clases (VALIDATE_SIZE)
SZ = dict(ESC)
vs = re.compile(r'VALIDATE_SIZE\(\s*([\w:<>, ]+?)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')
for p in glob.glob(os.path.join(SRC, '**', '*.h'), recursive=True):
    for m in vs.finditer(open(p, encoding='utf-8', errors='replace').read()):
        SZ[m.group(1).split('::')[-1]] = int(m.group(2), 0)
def typesize(t):
    t = re.sub(r'\bconst\b|\bvolatile\b', '', t).strip()
    if t.endswith('*'): return 4
    t2 = t.split('::')[-1].strip()
    m = re.fullmatch(r'(?:std::array|notsa::mdarray)\s*<\s*([^,]+?)\s*,\s*([^>]+?)(?:\s*,\s*([^>]+?))?\s*>', t)
    if m:
        e = typesize(m.group(1)); 
        dims = [x for x in (m.group(2), m.group(3)) if x]
        n = 1
        for d in dims:
            d = d.strip().replace("'", '')
            try: n *= int(d, 0)
            except Exception: return None
        return e * n if e else None
    m = re.fullmatch(r'(.+?)\s*((?:\[[^\]]+\])+)', t)
    if m:
        e = typesize(m.group(1)); n = 1
        for d in re.findall(r'\[([^\]]+)\]', m.group(2)):
            try: n *= int(d.strip().replace("'", ''), 0)
            except Exception: return None
        return e * n if e else None
    return SZ.get(t2)
objsl = sorted((va, allplan.get(va, 4) if allplan.get(va) else 4, D[va][0], va in L) for va in D if 0x8E4000 <= va < 0xCB1000)
objsl = [(va, tam, n, inl) for (va, tam, n, inl) in objsl]
starts = [o[0] for o in objsl]
lit = re.compile(r'\b0x0*([0-9A-Fa-f]{6})\b')
sref = re.compile(r'(?:Scoped)?StaticRef\s*<\s*(.+?)\s*>\s*\(\s*0x0*([0-9A-Fa-f]{6,8})')
hits = []
def strip_comments(txt):
    txt = re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), txt, flags=re.S)
    return txt
for p in glob.glob(os.path.join(SRC, '**', '*'), recursive=True):
    if not p.endswith(('.h', '.cpp', '.inl', '.hpp')): continue
    if os.sep + 'DatosExe' + os.sep in p: continue
    txt = strip_comments(open(p, encoding='utf-8', errors='replace').read())
    for ln, l in enumerate(txt.split('\n'), 1):
        c = re.sub(r'//.*', '', l)
        c = re.sub(r'"[^"]*"', '""', c)
        ext = {}
        for m in sref.finditer(c):
            ext[int(m.group(2), 16)] = (typesize(m.group(1)), m.group(1))
        for m in lit.finditer(c):
            a = int(m.group(1), 16)
            if not (0x8E4000 <= a < 0xCB1000): continue
            tail = c[m.end():m.end() + 24]
            idx = bool(re.match(r'\s*[\+\-]\s*\(?\s*[\w\(\)\.]*\s*[\*<\+]', tail)) or bool(re.match(r'\s*\+\s*\w', tail))
            sz, tn = ext.get(a, (None, None))
            e = sz if sz else 1
            # objetos dentro de [a, a+e) o que contienen a
            i = bisect.bisect_right(starts, a) - 1
            rel = []
            if i >= 0 and objsl[i][0] <= a < objsl[i][0] + max(objsl[i][1], 1): rel.append(('cita dentro del objeto', objsl[i]))
            j = bisect.bisect_left(starts, a)
            while j < len(objsl) and objsl[j][0] < a + e:
                if not (objsl[j][0] == a and e <= objsl[j][1]): rel.append(('objeto dentro de la extension', objsl[j]))
                elif objsl[j][0] == a: rel.append(('misma direccion', objsl[j]))
                j += 1
            seen = set()
            for k, o in rel:
                if (k, o[0]) in seen: continue
                seen.add((k, o[0]))
                hits.append(dict(file=os.path.relpath(p, SRC), line=ln, addr=a, kind=k, obj=o[2], va=o[0], tam=o[1], inl=o[3], ext=sz, tn=tn, idx=idx, text=c.strip()[:150]))
json.dump(hits, open('estatico_hits.json', 'w'), indent=0)
import collections
print('hits', len(hits), collections.Counter((h['kind'], h['inl']) for h in hits))
