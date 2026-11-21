"""Segunda confirmacion de las funciones que la primera pasada dejo como `no_ejercida` o `fallo` (funciones raras en los escenarios): mas escenarios y, para deflate/crc32,
tambien el banco de escritura PNG. Actualiza confirmacion.csv."""
import csv, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
B = r'D:\Original\wv-30_tmp' + chr(92) + 'b'
rows = list(csv.DictReader(open('confirmacion.csv', encoding='utf8')))
def run(bench, spec, n, seed):
    try:
        r = subprocess.run([B + chr(92) + bench, EXE, 'probar', str(n), str(seed), spec], capture_output=True, timeout=1800)
        return int(r.stdout.decode().strip().split()[-1])
    except Exception:
        return 2
DEF = {'deflateReset'}
NP = int(sys.argv[1])
def work(r):
    nm = r['nombre']; va = r['va'][2:].lstrip('0')
    res = {}
    benches = ['bench_png.exe'] + (['bench_deflate.exe'] if nm in DEF else [])
    for b in benches:
        n = NP if b == 'bench_png.exe' else 400
        res[b] = (run(b, f'{nm}=0x{va}', n, 60000), run(b, f'trampa=0x{va}', n, 60000))
    return r, res
todo = [r for r in rows if r['sustitucion_en_banco'] != 'identica' or r['control_trampa'] != 'se_ejecuta']
with ThreadPoolExecutor(6) as ex:
    for r, res in ex.map(work, todo):
        oks = [v[0] for v in res.values() if v[1] != 0]       # sustituciones en los bancos donde la funcion se ejecuta
        executed = bool(oks)
        allid = executed and all(o == 0 for o in oks)
        r['sustitucion_en_banco'] = 'identica' if allid else ('difiere' if executed else r['sustitucion_en_banco'])
        r['control_trampa'] = 'se_ejecuta' if executed else 'no_ejercida'
        print(r['va'], r['nombre'], res, r['sustitucion_en_banco'], r['control_trampa'], flush=True)
w = csv.DictWriter(open('confirmacion.csv', 'w', newline='', encoding='utf8'), fieldnames=list(rows[0].keys()))
w.writeheader(); w.writerows(rows)
