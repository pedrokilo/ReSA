import sys
sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
from recorte import decode
for ruta in sys.argv[1:]:
    w, h, f = decode(ruta)
    W, H = 1024, 768
    def nb(y, x0, x1): return sum(1 for x in range(x0, x1) if max(f[y][3*x:3*x+3]) > 8)
    def nbc(x, y0, y1): return sum(1 for y in range(y0, y1) if max(f[y][3*x:3*x+3]) > 8)
    # margenes negros: arriba (filas 0..90), izquierda (cols 0..85), debajo del mapa (filas 675..H sin texto: columnas 0..80)
    filas = [(y, nb(y, 0, W)) for y in range(0, 92)]
    cols = [(x, nbc(x, 0, H)) for x in range(0, 88)]
    print(ruta, 'filas superiores con pixeles no negros:', [(y, n) for y, n in filas if n], '| columnas izquierdas:', [(x, n) for x, n in cols if n][:12])
