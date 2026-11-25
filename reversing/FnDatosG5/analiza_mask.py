# Analiza bss_mask.bin / bss_last.bin (volcado de TODO el .bss en la direccion del EXE, OR de bits vistos): objetos DatosExe declarados (lotes 1-30)
# cuya direccion del EXE tiene bytes distintos de cero (alguien escribio por literal), con el tamano real del tipo cuando se puede calcular.
# Uso: python analiza_mask.py <carpeta_ejecucion> [<carpeta> ...]
import sys, os, json, re, glob, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tam_ext, estatico as E
LO = 0x8E4000
T = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'objetos_lotes1_30.json')))
files = [p for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True) if p.endswith(('.h', '.cpp', '.inl', '.hpp')) and os.sep + 'DatosExe' + os.sep not in p]
allt = '\n'.join(E.strip_comments(open(p, encoding='utf-8', errors='replace').read()) for p in files)
names = {t['name'] for t in T}
cnt = collections.Counter(m.group(0) for m in re.finditer(r'\b(?:b|v|sl_)[0-9A-F]{6}\b', allt) if m.group(0) in names)
mask = bytearray(LO * 0 + 0xCB1000 - LO)
for d in sys.argv[1:]:
    p = os.path.join(d, 'bss_mask.bin')
    if not os.path.exists(p): continue
    b = open(p, 'rb').read()
    for i, x in enumerate(b[:len(mask)]): mask[i] |= x
# tamano real
decl = {}
for h in glob.glob(os.path.join(E.R, 'source/game_sa/DatosExe/*.h')):
    for l in open(h, encoding='utf-8', errors='replace'):
        m = re.match(r'\s*extern\s+(?:const\s+)?(.+?)\s+(\w+)((?:\[[^\]]*\])*)\s*;\s*//\s*0x([0-9A-Fa-f]+)', l)
        if m: decl[m.group(2)] = (m.group(1) + m.group(3)).strip()
sal = []
for t in T:
    n = t['name']; ty = decl.get(n)
    real = None
    if ty and ty != 'F': real = E.typesize(ty)
    sz = max(t['tam'], real or 0)
    va = t['va']; a = va - LO
    if a < 0 or a + sz > len(mask): continue
    nz = sum(1 for x in mask[a:a + sz] if x)
    if nz: sal.append((va, n, sz, real, nz, cnt.get(n, 0)))
print('objetos con la direccion del EXE distinta de cero:', len(sal))
for va, n, sz, real, nz, c in sorted(sal):
    print('%s 0x%X ext=%d(real=%s) bytes_no_cero=%d refs_por_nombre=%d' % (n, va, sz, real, nz, c))
