"""Segunda pasada de la busqueda por comportamiento (ver buscar_png.py): solo candidatas y funciones de la biblioteca que la primera dejo sin pareja, con mas escenarios."""
import csv, subprocess, sys, json, re
from concurrent.futures import ThreadPoolExecutor
BENCH = r'D:\Original\wv-30_tmp\b\bench_png.exe'
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
def run(spec, seed, n):
    try:
        r = subprocess.run([BENCH, EXE, 'probar', str(n), str(seed), spec], capture_output=True, timeout=40)
        return int(r.stdout.decode().strip().split()[-1]) if r.stdout.strip() else 2
    except Exception: return 2
prev = json.load(open('buscar_png.json'))
matched_a = {int(a, 16) for a, v in prev.items() if v}
matched_f = {f for v in prev.values() for f in v}
src = open('bench_png.cpp', encoding='utf8').read()
FN = [f for f in sorted(set(re.findall(r'LS\((\w+)\)', src))) if f not in matched_f]
rows = list(csv.DictReader(open('../FnLIB2A/exe_funcs.csv')))
done = set()
for r in csv.DictReader(open('../FnLIB2A/estado_funciones.csv')):
    if r['va_exe']: done.add(int(r['va_exe'], 16))
cands = [int(r['va'], 16) for r in rows if 0x7cf940 <= int(r['va'], 16) < 0x7df480 and int(r['va'], 16) not in matched_a and int(r['va'], 16) not in done]
N0 = int(sys.argv[1]) if len(sys.argv) > 1 else 100
def trap(a): return a, run(f'trampa=0x{a:X}', 7000, N0)
with ThreadPoolExecutor(6) as ex: tr = dict(ex.map(trap, cands))
exec_ = [a for a in cands if tr[a] != 0]
print('candidatas sin pareja que se ejecutan:', ' '.join(f'{a:X}' for a in exec_), flush=True)
print('funciones sin pareja:', ' '.join(FN), flush=True)
pairs = [(a, f) for a in exec_ for f in FN]
def tryp(p): a, f = p; return p, run(f'{f}=0x{a:X}', 7000, N0)
res = {}
with ThreadPoolExecutor(6) as ex:
    for (a, f), r in ex.map(tryp, pairs):
        if r == 0: res.setdefault(a, []).append(f)
for a in exec_: print(f'{a:X}', res.get(a, []))
json.dump({f'{a:X}': res.get(a, []) for a in exec_}, open('buscar_png2.json', 'w'))
