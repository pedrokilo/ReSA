# Clasifica los objetos segun lo visto en datos_snap.json (VA = direccion del EXE, own = objeto propio)
import json, sys, glob
T = {o['name']: o for o in json.load(open(sys.argv[1]))}
res = {'A_ambos_distintos': [], 'B_solo_propio': [], 'C_solo_VA': [], 'D_iguales_no_cero': [], 'E_nada': []}
tot = {}
for f in sys.argv[2:]:
    for n, L in json.load(open(f)).items():
        t = tot.setdefault(n, {'va': set(), 'own': set(), 'pares': set()})
        for a, b, tt in L:
            t['va'].add(a); t['own'].add(b); t['pares'].add((a, b))
for n, t in tot.items():
    z = lambda h: set(h) <= {'0'}
    va_cambia = any(not z(a) for a in t['va']); own_cambia = any(not z(b) for b in t['own'])
    dist = any(a != b for a, b in t['pares'])
    if va_cambia and own_cambia and dist: k = 'A_ambos_distintos'
    elif own_cambia and not va_cambia: k = 'B_solo_propio'
    elif va_cambia and not own_cambia: k = 'C_solo_VA'
    elif va_cambia and own_cambia: k = 'D_iguales_no_cero'
    else: k = 'E_nada'
    res[k].append(n)
print('objetos medidos:', len(tot), 'de', len(T))
for k, v in res.items(): print(k, len(v))
for k in ('A_ambos_distintos', 'C_solo_VA', 'D_iguales_no_cero'):
    print('---', k)
    for n in res[k]:
        o = T[n]; t = tot[n]
        print(n, hex(o['va']), 'tam', o['tam'], 'lote', o['lote'], '| VA', sorted(t['va'])[:3], '| own', sorted(t['own'])[:3])
json.dump(res, open('clasif.json', 'w'))
