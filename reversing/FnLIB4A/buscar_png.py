"""Busqueda por comportamiento de las funciones internas de deflate/trees del EXE (patch en vivo, ver bench_deflate.cpp).
Para cada candidata A (funciones descubiertas del EXE): 1) trampa (xor eax,eax; ret) -> si el resultado difiere, A se ejecuta y importa;
2) sustituida por cada funcion F de la biblioteca -> si coincide con el EXE puro en todas las semillas, (A, F) es pareja."""
import csv, subprocess, sys, itertools, json
from concurrent.futures import ThreadPoolExecutor
BENCH = r'D:\Original\wv-30_tmp\b\bench_png.exe'
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
N = 24
def run(spec, seed=5000, n=N):
    if True:
        pass
    try:
        r = subprocess.run([BENCH, EXE, 'probar', str(n), str(seed), spec], capture_output=True, timeout=25)
        return int(r.stdout.decode().strip().split()[-1]) if r.stdout.strip() else 2
    except subprocess.TimeoutExpired:
        return 2
    except Exception:
        return 2
rows = [r for r in csv.DictReader(open('../FnLIB2A/exe_funcs.csv'))]
import re
src = open('bench_png.cpp', encoding='utf8').read()
FN = sorted(set(re.findall(r'LS\((\w+)\)', src)))
cands = [int(r['va'], 16) for r in rows if 0x7cf600 <= int(r['va'], 16) < 0x7df480]
N0 = int(sys.argv[1]) if len(sys.argv) > 1 else 14
def trap(a): return a, run(f'trampa=0x{a:X}', 3000, N0)
with ThreadPoolExecutor(6) as ex: tr = dict(ex.map(trap, cands))
exec_ = [a for a in cands if tr[a] != 0]
print('candidatas que se ejecutan:', len(exec_), ' '.join(f'{a:X}' for a in exec_), flush=True)
pairs = [(a, f) for a in exec_ for f in FN]
def tryp(p):
    a, f = p; return p, run(f'{f}=0x{a:X}', 3000, N0)
res = {}
with ThreadPoolExecutor(6) as ex:
    for (a, f), r in ex.map(tryp, pairs):
        if r == 0: res.setdefault(a, []).append(f)
for a in exec_: print(f'{a:X}', res.get(a, []))
json.dump({f'{a:X}': res.get(a, []) for a in exec_}, open('buscar_png.json', 'w'))
