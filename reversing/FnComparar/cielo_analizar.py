# Analiza las escenas de la fase 'cielo' de comparar.py: empareja por nombre de escena las ejecuciones de dos modos (o dos repeticiones del mismo),
# mide la diferencia numerica del cielo (zona superior sin el HUD) y escribe composiciones lado a lado.
# Uso: python -I cielo_analizar.py par  <dirA> <dirB> [--out carpeta] [--comp]      (A = referencia, B = candidato; diferencia = B - A)
#      python -I cielo_analizar.py ruido <dir_orig1> <dir_orig2> ...                 (ruido del original contra si mismo, por escena)
import sys, os, json, glob, statistics
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import png_util as P

W, H = 800, 600
def zona(x, y):          # cielo: franja superior (y<300), sin el HUD de arriba a la derecha (reloj, dinero, salud, aire) ni el texto del borde
    return y < 300 and not (x >= 540 and y < 150)
def mascara():
    return [zona(x, y) for y in range(H) for x in range(W)]
MASK = mascara()

def cargar(d):
    r = {}
    f = os.path.join(d, 'resultados.jsonl')
    if not os.path.exists(f): return r
    for ln in open(f, encoding='utf-8'):
        try: j = json.loads(ln)
        except Exception: continue
        if j.get('tipo') == 'cielo' and j.get('cap') and j['cap'].get('valida'):
            r.setdefault(j['escena'], []).append((j, os.path.join(d, j['escena'] + '.png')))
    return r

def med(rgb, mask):
    n = sum(mask); s = [0, 0, 0]
    for i, mk in enumerate(mask):
        if mk:
            for c in range(3): s[c] += rgb[3 * i + c]
    return [x / n for x in s]

def metricas(a, b):
    """a, b = (w,h,rgb); devuelve dif. con signo b-a por canal en el cielo, |dif| media, % de pixeles con |dif|>24 (suma de canales/3), y rugosidad (desv. media local) de cada una"""
    ra, rb = a[2], b[2]; n = sum(MASK); sd = [0, 0, 0]; ab = 0; gr = 0
    for i, mk in enumerate(MASK):
        if not mk: continue
        t = 0
        for c in range(3):
            d = rb[3 * i + c] - ra[3 * i + c]; sd[c] += d; t += abs(d)
        ab += t / 3
        if t / 3 > 24: gr += 1
    return {'dif_signo': [round(x / n, 2) for x in sd], 'dif_abs': round(ab / n, 2), 'pct_gt24': round(100.0 * gr / n, 2), 'media_a': [round(x, 1) for x in med(ra, MASK)], 'media_b': [round(x, 1) for x in med(rb, MASK)],
            'rug_a': round(rugosidad(a), 2), 'rug_b': round(rugosidad(b), 2)}

def rugosidad(im):
    """media del |gradiente| horizontal+vertical de la luminancia en el cielo (mide si hay formas/barras o un degradado liso)"""
    w, h, rgb = im; tot = 0; n = 0
    for y in range(0, 300, 2):
        for x in range(0, 540, 2):
            i = y * w + x
            l0 = rgb[3 * i] + rgb[3 * i + 1] + rgb[3 * i + 2]
            l1 = rgb[3 * (i + 1)] + rgb[3 * (i + 1) + 1] + rgb[3 * (i + 1) + 2]
            l2 = rgb[3 * (i + w)] + rgb[3 * (i + w) + 1] + rgb[3 * (i + w) + 2]
            tot += abs(l1 - l0) + abs(l2 - l0); n += 1
    return tot / n / 3

def main():
    modo = sys.argv[1]
    if modo == 'par':
        A = cargar(sys.argv[2]); B = cargar(sys.argv[3]); out = sys.argv[sys.argv.index('--out') + 1] if '--out' in sys.argv else None
        comp = '--comp' in sys.argv
        if out: os.makedirs(out, exist_ok=True)
        filas = []
        for esc in sorted(set(A) & set(B)):
            ia = P.leer(A[esc][0][1]); ib = P.leer(B[esc][0][1])
            m = metricas(ia, ib); m['escena'] = esc; filas.append(m)
            print(esc, json.dumps(m))
            if comp and out:
                dif = P.diferencia(ia, ib)[2]
                W2, H2, rgb = P.lado_a_lado([ia, ib, (ia[0], ia[1], dif)], esc=1)
                P.escribir(os.path.join(out, esc + '.png'), W2, H2, rgb, 6)
        json.dump(filas, open(os.path.join(out, 'metricas.json'), 'w') if out else sys.stdout, indent=0)
    elif modo == 'ruido':
        runs = [cargar(d) for d in sys.argv[2:]]
        for esc in sorted(set.intersection(*[set(r) for r in runs])):
            ims = [P.leer(r[esc][0][1]) for r in runs]; res = []
            for i in range(1, len(ims)):
                m = metricas(ims[0], ims[i]); res.append((m['dif_abs'], m['dif_signo']))
            print(esc, res)
if __name__ == '__main__': main()
