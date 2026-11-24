# Resume datos_dyn.json (sonda_datos.py): objetos propios .bss cuya memoria del EXE se ha escrito (escritor por direccion), con el lote del plan y el contenido del objeto propio.
import json, sys, csv, re, os
raiz = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
plan = {}
for r in csv.DictReader(open(os.path.join(raiz, 'docs/DatosExe/plan-objetos.csv'), encoding='utf-8')): plan[int(r['va'], 16)] = (r['lote'], r['propietario'])
d = json.load(open(sys.argv[1]))
print('t=%.0f s, leido=%d bytes, objetos vigilados=%d, final=%s, con memoria del EXE escrita=%d' % (d['t'], d['leido'], d['objetos'], d['final'], len(d['hallazgos'])))
for h in sorted(d['hallazgos'], key=lambda h: int(h['ea'], 16)):
    ea = int(h['ea'], 16); l = plan.get(ea, ('?', ''))
    nm = h['nom'].split('@')[0][1:]
    print('%s %-9s sz=%-5d lote %-8s t1=%-6s nz=%-5d exe=%s obj=%s | %s' % (h['ea'], nm, h['sz'], l[0], h['primera_t'], h['exe_alguna_vez_nz'], h['exe_ahora'][:48], h['obj'][:48], l[1][:40]))
