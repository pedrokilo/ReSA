"""Clasifica las diferencias objeto/direccion de comparar_objetos.py (diferencias.csv).
Para cada objeto que difiere en alguna instantanea cuenta (sin comentarios, fuera de DatosExe/):
  nombre = usos por nombre del objeto;  lit = citas por literal de la direccion (o de una direccion interior);
  viva = funciones vivas del EXE que tocan la direccion (docs/DatosExe/objetos.csv)
Clase: A = usado por nombre Y por literal (dato en dos sitios probable); B = solo literal (el objeto sobra);
       C = solo nombre (la direccion del EXE no la lee el C++; mirar funciones vivas); D = ni uno ni otro.
Uso: python clasificar_diferencias.py <repo> <diferencias.csv> [salida.csv]
"""
import csv, os, re, sys, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import comparar_objetos as c

repo, difcsv = sys.argv[1], sys.argv[2]
salida = sys.argv[3] if len(sys.argv) > 3 else None
dec = c.leer_decls(repo)
por_nombre = {(d['ns'], d['name']): d for d in dec.values()}


def sin_comentarios(t):
    t = re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), t, flags=re.S)
    return re.sub(r'//[^\n]*', '', t)


textos = {}
for root, _, fs in os.walk(os.path.join(repo, 'source')):
    if os.sep + 'Imagen' in root:
        continue
    for f in fs:
        if f.endswith(('.cpp', '.h', '.inl', '.hpp')):
            p = os.path.join(root, f)
            textos[p] = sin_comentarios(open(p, encoding='utf-8', errors='replace').read())
fuera = {p: t for p, t in textos.items() if os.sep + 'DatosExe' + os.sep not in p}
lits = []
lit = re.compile(r'0[xX]0*([0-9A-Fa-f]{6})\b')
for p, t in fuera.items():
    for m in lit.finditer(t):
        a = int(m.group(1), 16)
        if 0x858000 <= a < 0xCB1000:
            lits.append((a, os.path.relpath(p, repo).replace(chr(92), '/')))
lits.sort()
lva = [x[0] for x in lits]
vivas = {}
for x in csv.DictReader(open(os.path.join(repo, 'docs/DatosExe/objetos.csv'), encoding='utf-8')):
    vivas[int(x['va'], 16)] = int(x['funcs_exe_vivas'])
vv = sorted(vivas)

filas = []
for r in csv.DictReader(open(difcsv, encoding='utf-8')):
    cols = [k for k in r if k.startswith('dif_t')]
    if not any(r[k] == '1' for k in cols):
        continue
    va, tam = int(r['va'], 16), int(r['tam'])
    nombre = r['nombre']
    pat = re.compile(r'\b%s\b' % re.escape(nombre))
    nn = sum(len(pat.findall(t)) for t in fuera.values())
    i = bisect.bisect_left(lva, va)
    ls = []
    while i < len(lva) and lva[i] < va + max(tam, 1):
        ls.append(lits[i][1])
        i += 1
    j = bisect.bisect_left(vv, va)
    nv = 0
    while j < len(vv) and vv[j] < va + max(tam, 1):
        nv += vivas[vv[j]]
        j += 1
    cl = 'A' if nn and ls else 'B' if ls else 'C' if nn else 'D'
    filas.append([r['va'], r['ns'], nombre, r['tipo'], tam, r['exacto'], ','.join(r[k] for k in cols), nn, len(ls), ';'.join(sorted(set(ls)))[:80], nv, cl, r['obj_final'][:32], r['mem_final'][:32]])
filas.sort(key=lambda x: (x[11], x[0]))
cab = ['va', 'ns', 'nombre', 'tipo', 'tam', 'exacto', 'dif', 'usos_nombre', 'citas_literal', 'archivos_literal', 'fn_vivas', 'clase', 'obj', 'mem']
if salida:
    with open(salida, 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f)
        w.writerow(cab)
        w.writerows(filas)
from collections import Counter
print(Counter(x[11] for x in filas))
for x in filas:
    if x[11] in ('A',):
        print(x[:12], x[12], x[13])
