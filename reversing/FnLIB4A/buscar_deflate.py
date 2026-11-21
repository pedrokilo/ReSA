"""Busqueda por comportamiento de las funciones internas de deflate/trees del EXE (patch en vivo, ver bench_deflate.cpp).
Para cada candidata A (funciones descubiertas del EXE): 1) trampa (xor eax,eax; ret) -> si el resultado difiere, A se ejecuta y importa;
2) sustituida por cada funcion F de la biblioteca -> si coincide con el EXE puro en todas las semillas, (A, F) es pareja."""
import csv, subprocess, sys, itertools, json
from concurrent.futures import ThreadPoolExecutor
BENCH = r'D:\Original\wv-30_tmp\b\bench_deflate.exe'
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
N = 24
def run(spec, seed=5000, n=N):
    try:
        r = subprocess.run([BENCH, EXE, 'probar', str(n), str(seed), spec], capture_output=True, timeout=60)
        return int(r.stdout.decode().strip().split()[-1]) if r.stdout.strip() else 2
    except subprocess.TimeoutExpired:
        return 2
    except Exception:
        return 2
rows = [r for r in csv.DictReader(open('../FnLIB2A/exe_funcs.csv'))]
cands = [int(r['va'], 16) for r in rows if 0x7df400 <= int(r['va'], 16) <= 0x7e1100 or 0x7e77a0 <= int(r['va'], 16) <= 0x7e9300]
FN = ['lm_init','putShortMSB','flush_pending','read_buf','fill_window','longest_match','deflate_stored','deflate_fast','deflate_slow',
      '_tr_init','_tr_stored_block','_tr_align','_tr_flush_block','_tr_tally','init_block','pqdownheap','gen_bitlen','gen_codes','build_tree','scan_tree',
      'send_tree','build_bl_tree','send_all_trees','compress_block','set_data_type','bi_reverse','bi_flush','bi_windup','copy_block','compress','compress2','adler32','crc32']
known = {'deflateInit_':0x7DF480,'deflateInit2_':0x7DF4B0,'deflateSetDictionary':0x7DF7B0,'deflateReset':0x7DF8B0,'deflateParams':0x7DF9C0,'deflate':0x7DFA90,'deflateEnd':0x7DFF00,'deflateCopy':0x7DFFB0}
cands = [a for a in cands if a not in known.values()]
def trap(a): return a, run(f'trampa=0x{a:X}')
with ThreadPoolExecutor(6) as ex: tr = dict(ex.map(trap, cands))
exec_ = [a for a in cands if tr[a] != 0]
print('candidatas que se ejecutan (trampa difiere):', ' '.join(f'{a:X}' for a in exec_), flush=True)
pairs = [(a, f) for a in exec_ for f in FN]
def tryp(p):
    a, f = p; return p, run(f'{f}=0x{a:X}')
res = {}
with ThreadPoolExecutor(6) as ex:
    for (a, f), r in ex.map(tryp, pairs):
        if r == 0: res.setdefault(a, []).append(f)
for a in exec_: print(f'{a:X}', res.get(a, []))
json.dump({f'{a:X}': res.get(a, []) for a in exec_}, open('buscar_deflate.json', 'w'))
