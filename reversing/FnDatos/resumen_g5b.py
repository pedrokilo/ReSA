# Resumen por objeto de los lotes DE05-31..61: combina el volcado dinamico (datos_dyn.json de una o varias ejecuciones) con el estatico (usos por nombre de la auditoria anterior).
# Uso: python -I resumen_g5b.py <salida.csv> <datos_dyn.json> [...]
import sys, json, csv, re, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import objmap
aqui = os.path.dirname(os.path.abspath(__file__)); raiz = os.path.abspath(os.path.join(aqui, '..', '..'))
mapa = os.environ.get('MAPA', r'D:\Original\wv-datos3\reversing\FnFase8\salida\gta_sa_standalone.map')
sizes = {int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_prev.json'))).items()}; sizes.update({int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_slots.json'))).items()})
refs = {o['addr']: bool(o['refs'] or o['aliasrefs']) for o in json.load(open(r'D:\Original\sim_out\aud2\objs2.json'))}
plan = {}
for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8')):
    m = re.match(r'DE05-(\d+)$', r['lote'])
    if r['grupo'] == 'G5' and m and 31 <= int(m.group(1)) <= 61: plan[int(r['va'], 16)] = r['lote']
syms = {x[2]: x for x in objmap.cargar(mapa) if x[3] in ('Bss', 'R5') and not x[1].startswith('?kConstruct')}
exe_nz = {}; obj_nz = set()
for f in sys.argv[2:]:
    d = json.load(open(f))
    for a in d.get('obj_nz', []): obj_nz.add(a)
    for h in d['hallazgos']:
        ea = int(h['ea'], 16); real = sizes.get(ea) or h['sz']; offs = [o for o in h['offs_nz'] if o < real]
        if offs: exe_nz[ea] = max(exe_nz.get(ea, 0), len(offs))
out = []; import collections; cnt = collections.Counter()
for ea in sorted(plan):
    if ea not in syms: cnt['solo direccion del EXE (sin objeto propio)'] += 1; continue
    nom = syms[ea][1].split('@')[0][1:]
    if ea in exe_nz: cat = 'EXE escrito (escritor por direccion)'
    elif ea in obj_nz: cat = 'verificado: objeto escrito por nombre y EXE intacto'
    else: cat = 'no ejercitado (ni objeto ni EXE escritos)'
    cnt[cat] += 1; out.append((hex(ea), nom, plan[ea], sizes.get(ea, ''), refs.get(ea, ''), cat))
with open(sys.argv[1], 'w', newline='', encoding='utf-8') as g:
    w = csv.writer(g); w.writerow(['va', 'objeto', 'lote', 'tam', 'usos_por_nombre', 'categoria']); w.writerows(out)
for k, v in cnt.items(): print(v, k)
print('total plan', len(plan))
