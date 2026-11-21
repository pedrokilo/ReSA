"""Prueba final con TODAS las sustituciones a la vez: EXE original contra EXE con todas las funciones confirmadas de confirmacion.csv sustituidas por la biblioteca (patch en vivo).
Uso: python run_todo.py <escenarios deflate> <escenarios png> [semilla]. Los bancos corren en paralelo en trozos de 250 escenarios."""
import csv, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
B = r'D:\Original\wv-30_tmp' + chr(92) + 'b'
rows = [r for r in csv.DictReader(open('confirmacion.csv', encoding='utf8')) if r['sustitucion_en_banco'] == 'identica' and r['control_trampa'] == 'se_ejecuta']
DEF = {'deflateInit_','deflateInit2_','deflateSetDictionary','deflateReset','deflateParams','deflate','deflateEnd','deflateCopy','fill_window','deflate_stored','deflate_fast','deflate_slow','longest_match',
       '_tr_init','_tr_stored_block','_tr_align','_tr_flush_block','build_tree','pqdownheap','scan_tree','send_tree','compress_block','bi_windup','adler32','crc32'}
spec_png = ','.join(f"{r['nombre']}=0x{r['va'][2:].lstrip('0')}" for r in rows)
spec_def = ','.join(f"{r['nombre']}=0x{r['va'][2:].lstrip('0')}" for r in rows if r['nombre'] in DEF)
nd, npg = int(sys.argv[1]), int(sys.argv[2]); seed0 = int(sys.argv[3]) if len(sys.argv) > 3 else 100000
def run(bench, spec, n, seed):
    r = subprocess.run([B + chr(92) + bench, EXE, 'insitu', str(n), str(seed), spec], capture_output=True, text=True, timeout=3600)
    return bench, n, r.stdout.strip().splitlines()[-1] if r.stdout.strip() else ('sin salida (codigo %d)' % r.returncode)
jobs = []
for k in range(0, nd, 250): jobs.append(('bench_deflate.exe', spec_def, min(250, nd - k), seed0 + k))
for k in range(0, npg, 250): jobs.append(('bench_png.exe', spec_png, min(250, npg - k), seed0 + k))
tot = {}
with ThreadPoolExecutor(5) as ex:
    for bench, n, line in ex.map(lambda j: run(j[0], j[1], j[2], j[3]), jobs):
        print(bench, n, line, flush=True)
