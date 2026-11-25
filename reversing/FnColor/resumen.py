import sys,json
for l in open(sys.argv[1],encoding='utf-8'):
    if 'ESTADO' not in l: continue
    d=json.loads(l[l.index('ESTADO')+7:])
    print(d['tag'],'lights',d.get('ms_lightsOn[0..3]'),'cur',d.get('ms_currentCol[0..3]'),'inv',d.get('bInvertLook4Pad'),'cheats',d['cheats_activos'],'veh',[(v['modelo'],v['colores']) for v in d['vehiculos']][:7])
