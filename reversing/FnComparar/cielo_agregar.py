# Agrega 2+ ejecuciones del original y 2+ del standalone: por escena, media del rectangulo de cielo por canal, ruido original-original, ruido std-std y diferencia entre modos
# (|dif| por pixel: dentro de cada modo frente a cruzada). Uso: python -I cielo_agregar.py <out.json> --o dirO1 dirO2 ... --s dirS1 dirS2 ...
import sys, os, json, itertools
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import png_util as P, cielo_analizar as C
a = sys.argv; out = a[1]; i = a.index('--o'); j = a.index('--s'); O = [C.cargar(d) for d in a[i + 1:j]]; S = [C.cargar(d) for d in a[j + 1:]]
esc = sorted(set.intersection(*[set(r) for r in O + S]))
res = []
for e in esc:
    imo = [P.leer(r[e][0][1]) for r in O]; ims = [P.leer(r[e][0][1]) for r in S]
    mo = [C.med(x[2], C.MASK) for x in imo]; ms = [C.med(x[2], C.MASK) for x in ims]
    def ab(x, y): return C.metricas(x, y)['dif_abs']
    ruo = [ab(x, y) for x, y in itertools.combinations(imo, 2)]; rus = [ab(x, y) for x, y in itertools.combinations(ims, 2)]
    cru = [ab(x, y) for x in imo for y in ims]
    mean = lambda L: [sum(v[c] for v in L) / len(L) for c in range(3)]
    d = [round(p - q, 2) for p, q in zip(mean(ms), mean(mo))]
    sp_o = max(abs(x[c] - y[c]) for x, y in itertools.combinations(mo, 2) for c in range(3)) if len(mo) > 1 else 0
    sp_s = max(abs(x[c] - y[c]) for x, y in itertools.combinations(ms, 2) for c in range(3)) if len(ms) > 1 else 0
    res.append({'escena': e, 'media_o': [round(x, 1) for x in mean(mo)], 'media_s': [round(x, 1) for x in mean(ms)], 'dif_medias_s_menos_o': d, 'ruido_media_oo': round(sp_o, 2), 'ruido_media_ss': round(sp_s, 2),
                'absdif_oo': [round(x, 2) for x in ruo], 'absdif_ss': [round(x, 2) for x in rus], 'absdif_os': [round(x, 2) for x in cru]})
json.dump(res, open(out, 'w'), indent=0)
for r in res:
    print(r['escena'][2:], 'dif', r['dif_medias_s_menos_o'], 'ruidoOO', r['ruido_media_oo'], 'ruidoSS', r['ruido_media_ss'], '|d| oo', r['absdif_oo'], 'ss', r['absdif_ss'], 'os', r['absdif_os'])
