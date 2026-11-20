"""Ordena las funciones del EXE por numero de diferencias contra una funcion de la biblioteca (diagnostico). Uso: python rank.py <bench.exe> <EXE> <nombre> [n]"""
import re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
HERE = Path(__file__).resolve().parent
bench, exe, name = sys.argv[1:4]; n = sys.argv[4] if len(sys.argv) > 4 else '300'
line = [l.split() for l in open(HERE / 'tabla.txt') if l.split()[0] == name][0]
addrs = [a.strip() for a in (HERE / 'exe_addrs.txt').read_text().split() if a.strip()]
def one(a):
    t = list(line); t[4] = a; f = HERE / f'_r_{a}.txt'; f.write_text(' '.join(t) + chr(10))
    try: r = subprocess.run([bench, exe, str(f), '--n', n, '--diffs', '0', '--filtro', name], capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired: f.unlink(); return (10**9, a)
    f.unlink()
    m = re.search(r'^%s\s+0x[0-9A-F]+\s+(\d+)\s+(\d+)' % name, r.stdout, re.M)
    return (int(m.group(2)) if m else 10**9, a)
with ThreadPoolExecutor(6) as ex: res = sorted(ex.map(one, addrs))
print(res[:8])
