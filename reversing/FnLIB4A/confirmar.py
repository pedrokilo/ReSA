"""Confirmacion final por funcion (patch en vivo): para cada fila de mapa_final.csv, N escenarios con la funcion sustituida por la de la biblioteca (resultado 0 = identico al EXE)
y con la trampa `xor eax,eax; ret` (resultado distinto de 0 = la funcion se ejecuta en los escenarios: control negativo). Escribe confirmacion.csv."""
import csv, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
B = r'D:\Original\wv-30_tmp\b'
rows = list(csv.DictReader(open('mapa_final.csv', encoding='utf8')))
DEF = {'deflateInit_','deflateInit2_','deflateSetDictionary','deflateReset','deflateParams','deflate','deflateEnd','deflateCopy','fill_window','deflate_stored','deflate_fast','deflate_slow','longest_match',
       '_tr_init','_tr_stored_block','_tr_align','_tr_flush_block','build_tree','pqdownheap','scan_tree','send_tree','compress_block','bi_windup','adler32'}
def run(bench, spec, n, seed):
    try:
        r = subprocess.run([B + chr(92) + bench, EXE, 'probar', str(n), str(seed), spec], capture_output=True, timeout=900)
        return int(r.stdout.decode().strip().split()[-1])
    except Exception: return 2
NDEF, NPNG = int(sys.argv[1]), int(sys.argv[2])
def work(r):
    nm = r['nombre']; va = r['va'][2:].lstrip('0'); bench = 'bench_deflate.exe' if nm in DEF else 'bench_png.exe'
    n = NDEF if nm in DEF else NPNG; seed = 40000
    ok = run(bench, f'{nm}=0x{va}', n, seed); tr = run(bench, f'trampa=0x{va}', min(n, 120), seed)
    return r, ok, tr
out = []
with ThreadPoolExecutor(6) as ex:
    for r, ok, tr in ex.map(work, rows):
        out.append([r['va'], r['tam_exe'], r['nombre'], r['archivo'], 'identica' if ok == 0 else ('difiere' if ok == 1 else 'fallo'), 'se_ejecuta' if tr != 0 else 'no_ejercida', r['llamadas'], r['saltos'], r['punteros']])
        print(out[-1], flush=True)
w = csv.writer(open('confirmacion.csv', 'w', newline='', encoding='utf8'))
w.writerow(['va', 'tam_exe', 'nombre', 'archivo', 'sustitucion_en_banco', 'control_trampa', 'llamadas', 'saltos', 'punteros'])
w.writerows(out)
