# Estadistica de la fase lluvia: media y desviacion del color medio por escena (punto:hora) en cada ejecucion. Uso: python -I estad_lluvia.py <dir orig> <dir std> [...]
import json, sys, statistics as S
def cargar(d):
    esc = {}
    for l in open(d + '/resultados.jsonl'):
        r = json.loads(l)
        if r.get('tipo') != 'lluvia': continue
        k = (r.get('punto', 3), r.get('hora', 18)); esc.setdefault(k, []).append(r)
    return esc
res = [cargar(d) for d in sys.argv[1:]]
for k in sorted(set().union(*[set(r) for r in res])):
    print('escena punto %d hora %02d' % k)
    for d, r in zip(sys.argv[1:], res):
        rs = r.get(k, [])
        if not rs: continue
        g = [x['cap']['media_rgb'][1] for x in rs]
        rafagas = sum(1 for x in rs if x['clima_mem'].get('LightningBurst') or x['clima_mem'].get('LightningFlash'))
        print('  %-28s n=%2d  G media %.1f  sd %.1f  min %.1f  max %.1f  mediana %.1f  (con rafaga/flash en memoria: %d)  valores %s' % (d.split('/')[-1][:28], len(g), S.mean(g), S.pstdev(g), min(g), max(g), S.median(g), rafagas, [round(v) for v in g]))
