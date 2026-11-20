"""Resuelve el mapa EXE <-> biblioteca con el propio banco: ejecuta la tabla; para las funciones con diferencias busca con 'bench --find' que funcion del EXE
(entre todas las de exe_funcs.csv) se comporta igual que la de la biblioteca sobre 60 casos aleatorios. Escribe confirmadas.csv con las coincidencias unicas.
Uso: python resolve.py <bench.exe> <ruta del EXE> <tanda.txt>"""
import subprocess, sys, re, csv
from pathlib import Path
HERE = Path(__file__).resolve().parent
bench, exe, tanda = sys.argv[1], sys.argv[2], sys.argv[3]
def run(args, timeout=1500):
    return subprocess.run([bench, exe] + args, capture_output=True, text=True, timeout=timeout)
subprocess.run([sys.executable, str(HERE / 'gen_table.py'), tanda], check=True, cwd=HERE)
r = run([str(HERE / 'tabla.txt'), '--n', '150', '--diffs', '0'])
bad = []; good = []
for l in r.stdout.splitlines():
    m = re.match(r'^(\w+)\s+0x([0-9A-F]+)\s+(\d+)\s+(\d+)', l)
    if m: (bad if int(m.group(4)) else good).append((m.group(1), int(m.group(2), 16)))
    elif 'HANG' in l or 'SIN SIMBOLO' in l: print(l)
for l in open(HERE / 'tabla.txt'):
    t = l.split()
    if t[4] == '0': bad.append((t[0], 0))
print('coinciden', len(good), 'difieren', len(bad))
conf = {}
conff = HERE / 'confirmadas.csv'
if conff.exists():
    for row in csv.reader(open(conff)):
        if row and not row[0].startswith('#'): conf[row[0]] = row[1]
for n, a in good: conf[n] = f'{a:x}'
addrs = [x.strip() for x in (HERE / 'exe_addrs.txt').read_text().split() if x.strip()]
from concurrent.futures import ThreadPoolExecutor
def try_one(args):
    n, ad = args
    f = HERE / f'_a_{ad}.txt'; f.write_text(ad + chr(10))
    for _ in range(3):
        try:
            p = run([str(HERE / 'tabla.txt'), '--find', n, '--direcciones', str(f)], timeout=60)
        except subprocess.TimeoutExpired: break
        if 'fin find' in p.stdout or 'CANDIDATA' in p.stdout: break
    f.unlink()
    return ad if 'CANDIDATA' in p.stdout else None
for n, a in bad:
    with ThreadPoolExecutor(6) as ex: res = list(ex.map(try_one, [(n, ad) for ad in addrs]))
    found = [r for r in res if r]
    print(n, 'actual 0x%X' % a, '->', found, flush=True)
    if len(found) == 1: conf[n] = found[0].lower().lstrip('0')
    elif found: conf['?' + n] = ';'.join(f.lower() for f in found)
with open(conff, 'w', newline='') as f:
    w = csv.writer(f); w.writerow(['# nombre', 'va_exe']);
    for k, v in conf.items(): w.writerow([k, v])
