import json, re, os, glob, sys, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
H = {k: [[a,b,c,d,e] for a,b,c,d,e in v] for k, v in json.load(open('bin_verif.json')).items()}
inl = {o[2]: o for o in E.objsl if o[3]}
# referencias por nombre en source (fuera de DatosExe)
txt = ''
files = [p for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True) if p.endswith(('.h', '.cpp', '.inl', '.hpp')) and os.sep + 'DatosExe' + os.sep not in p]
cont = {p: E.strip_comments(open(p, encoding='utf-8', errors='replace').read()) for p in files}
allt = '\n'.join(cont.values())
names = set(inl)
cnt = collections.Counter(m.group(0) for m in re.finditer(r'\b[bvs][lL_]?[0-9A-Fa-f_]*[0-9A-F]{6}\b|\b(?:b|v|sl_)[0-9A-F]{6}\b', allt) if m.group(0) in names)
out = []
for n in sorted(inl, key=lambda n: inl[n][0]):
    hits = H.get(n, [])
    dentro = [h for h in hits if h[0] == 'dentro']
    if not dentro: continue
    out.append((n, inl[n][0], inl[n][1], cnt.get(n, 0), len(dentro), sorted(set(h[3] for h in dentro))))
print(len(out), 'objetos de lotes 1-30 con referencias de codigo DENTRO de su extension')
conn = [o for o in out if o[3] > 0]
print('de ellos con referencias por nombre en source:', len(conn))
for n, va, tam, c, nh, fs in out:
    print('%s 0x%X tam=%d nombre_refs=%d hits=%d %s' % (n, va, tam, c, nh, [f[:60] for f in fs[:6]]))
json.dump(out, open('cruce.json', 'w'))
