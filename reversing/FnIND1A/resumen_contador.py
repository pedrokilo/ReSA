r"""IND1A: resume el CSV del contador (n,funcion,llamada_desde,quien_llama,ms) en una tabla Markdown con la funcion propia
que llama (resuelta con el .map del exe) y la region de cada funcion original.
Uso: python -I resumen_contador.py <ind1a_funciones.csv> <gta_reversed_exe.map> <salida.md> [titulo]
"""
import bisect, csv, re, sys
csv_p, map_p, out_p = sys.argv[1:4]
titulo = sys.argv[4] if len(sys.argv) > 4 else 'Funciones originales ejecutadas'
fs = []
for l in open(map_p, encoding='latin1'):
    m = re.match(r'\s+0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8}) f.*?(\S+)\s*$', l)
    if m: fs.append((int(m.group(3), 16), m.group(2), m.group(4)))
fs.sort(); keys = [f[0] for f in fs]
def propio(a):
    i = bisect.bisect_right(keys, a) - 1
    return fs[i][1] if i >= 0 else '?'
def region(a):
    if a < 0x750000: return 'juego (0x401000-0x74FFFF)'
    if a < 0x820000: return 'RenderWare/D3D (0x750000-0x81FFFF)'
    return 'CRT y bibliotecas (0x820000+)'
rows = list(csv.DictReader(open(csv_p)))
reg = {}
for r in rows: reg[region(int(r['funcion'], 16))] = reg.get(region(int(r['funcion'], 16)), 0) + 1
o = [f'# {titulo}', '', f'{len(rows)} funciones originales distintas (primera entrada). Por region: ' + ', '.join(f'{k}: {v}' for k, v in sorted(reg.items())), '',
     '| n | funcion original | region | llamada desde | quien llama | ms |', '|---:|---|---|---|---|---:|']
for r in rows:
    a = int(r['funcion'], 16); c = int(r['llamada_desde'], 16)
    q = r['quien_llama']
    if q == 'propio': q = 'propio: `' + propio(c) + '`'
    elif q.startswith('original'): q = 'original (' + region(c).split(' ')[0] + ')'
    o.append(f'| {r["n"]} | `0x{a:08X}` | {region(a).split(" (")[0]} | `0x{c:08X}` | {q} | {r["ms"]} |')
open(out_p, 'w', encoding='utf-8').write('\n'.join(o) + '\n')
print(len(rows), reg)
