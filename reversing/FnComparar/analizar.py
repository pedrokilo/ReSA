# Compara las ejecuciones del original (orig) y del standalone (std) escena por escena: compuestas lado a lado, diferencia de color medio y tabla.
# Uso: python -I analizar.py --orig dir1[,dir2] --std dir1[,dir2] --salida evidencia [--nombre etiqueta] [--completa-si-dif 6]
import sys, os, json, csv
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import png_util as P
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
def cargar(dirs):
    r = {}
    for d in dirs.split(','):
        f = os.path.join(d, 'resultados.jsonl')
        if not os.path.exists(f): continue
        for l in open(f, encoding='utf-8'):
            j = json.loads(l)
            if 'escena' in j: j['_dir'] = d; r[j['escena']] = j
            else: r.setdefault('_' + j['tipo'], []).append(j)
    return r
def region(w, h, rgb, y0, y1):
    n = w * (y1 - y0); seg = rgb[y0 * w * 3:y1 * w * 3]
    return [round(sum(seg[c::3]) / n, 1) for c in range(3)]
def main():
    O = cargar(arg('--orig')); S = cargar(arg('--std')); sal = arg('--salida', os.path.join(aqui, 'evidencia')); os.makedirs(sal, exist_ok=True)
    pref = arg('--nombre', 'par'); umbral = float(arg('--completa-si-dif', '6'))
    filas = []
    for esc in sorted(k for k in O if not k.startswith('_') and k in S):
        o, s = O[esc], S[esc]
        fo, fs = os.path.join(o['_dir'], esc + '.png'), os.path.join(s['_dir'], esc + '.png')
        if not (os.path.exists(fo) and os.path.exists(fs)): continue
        a, b = P.leer(fo), P.leer(fs)
        if a[:2] != b[:2]: filas.append({'escena': esc, 'error': 'tamano distinto %s %s' % (a[:2], b[:2])}); continue
        dm, dabs, dif = P.diferencia(a, b); w, h = a[0], a[1]
        fila = {'escena': esc, 'tipo': o['tipo'], 'media_orig': [round(x, 1) for x in P.media(*a)], 'media_std': [round(x, 1) for x in P.media(*b)],
                'dif_media_rgb': [round(x, 1) for x in dm], 'dif_abs_media': round(dabs, 2),
                'cielo_orig': region(*a, 0, h // 3), 'cielo_std': region(*b, 0, h // 3), 'suelo_orig': region(*a, 2 * h // 3, h), 'suelo_std': region(*b, 2 * h // 3, h),
                'cap_orig_valida': o['cap'] and o['cap'].get('valida'), 'cap_std_valida': s['cap'] and s['cap'].get('valida'),
                'fps_orig': o['fps']['fps'], 'fps_std': s['fps']['fps'],
                'veh_orig': o['conteos']['veh']['usados'], 'veh_std': s['conteos']['veh']['usados'], 'peds_orig': o['conteos']['peds']['usados'], 'peds_std': s['conteos']['peds']['usados'],
                'rnd_orig': o['conteos']['NumRandomCars'], 'rnd_std': s['conteos']['NumRandomCars']}
        completa = dabs >= umbral or esc.startswith(('casa', 'ropero'))
        nom = '%s_%s.png' % (pref, esc)
        if completa:
            W, H, rgb = P.lado_a_lado([a, b]); P.escribir(os.path.join(sal, nom), W, H, rgb)   # orig | std a tamano completo
        else:
            W, H, rgb = P.lado_a_lado([a, b], esc=2); P.escribir(os.path.join(sal, nom), W, H, rgb)   # orig | std a media escala
        fila['archivo'] = nom; fila['completa'] = completa
        filas.append(fila)
    json.dump(filas, open(os.path.join(sal, pref + '_tabla.json'), 'w'), indent=1)
    with open(os.path.join(sal, pref + '_tabla.csv'), 'w', newline='') as f:
        cols = ['escena', 'dif_abs_media', 'dif_media_rgb', 'media_orig', 'media_std', 'cielo_orig', 'cielo_std', 'suelo_orig', 'suelo_std', 'fps_orig', 'fps_std', 'veh_orig', 'veh_std', 'peds_orig', 'peds_std', 'rnd_orig', 'rnd_std', 'archivo']
        w = csv.writer(f); w.writerow(cols)
        for r in filas:
            if 'error' in r: continue
            w.writerow([r.get(c) if not isinstance(r.get(c), list) else '/'.join(str(x) for x in r[c]) for c in cols])
    with open(os.path.join(sal, pref + '_tabla.md'), 'w', encoding='utf-8') as f:
        f.write('| escena | dif. media RGB (std-orig) | dif. abs. media | media orig | media std | cielo orig / std | veh o/s | peds o/s | rnd o/s | fps o/s |' + chr(10) + '|---|---|---:|---|---|---|---|---|---|---|' + chr(10))
        for r in filas:
            if 'error' in r: continue
            j = lambda v: '/'.join(str(round(x)) for x in v)
            f.write('| %s | %s | %.1f | %s | %s | %s / %s | %d/%d | %d/%d | %d/%d | %.0f/%.0f |' % (r['escena'], j(r['dif_media_rgb']), r['dif_abs_media'], j(r['media_orig']), j(r['media_std']), j(r['cielo_orig']), j(r['cielo_std']), r['veh_orig'], r['veh_std'], r['peds_orig'], r['peds_std'], r['rnd_orig'], r['rnd_std'], r['fps_orig'], r['fps_std']) + chr(10))
    print(len(filas), 'pares')
if __name__ == '__main__': main()
