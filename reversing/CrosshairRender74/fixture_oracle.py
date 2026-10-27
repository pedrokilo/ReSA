"""Oraculo independiente (Python) de CrosshairRender74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x56EF90..0x56F1F6, lo que debe observarse
(estados de render, los 60 sprites de la estela con su color, posicion y tamano, y los historiales globales de X, Y y hora) y lo compara con las filas de comparisons.jsonl.

La aritmetica float es la del x87 del EXE (24 bits, exponente sin limite, guardados `fstp dword` con desnormales; fsin/fcos con math.sin/math.cos y consumidas en la misma operacion).
Los NaN solo se comparan como «es NaN» (la carga la valida el microtest nan_rules_hw de PlayerCrosshair74 contra el hardware); todo lo demas, bit a bit.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import os
import sys
from fractions import Fraction

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'PlayerCrosshair74'))
from fixture_x87 import NAN, load, mul, add, store, mk  # noqa: E402

M32 = 0xFFFFFFFF
K1 = load(0x3BC90FDB)
K02 = load(0x3E4CCCCD)
ONE = load(0x3F800000)
HALF = load(0x3F000000)
PI2 = load(0x3FC90FDB)
PI4 = load(0x3F490FDB)


def fint(i):
    return mk(Fraction(i))


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def sin_of(a):
    return NAN if a == NAN or a[0] == 'i' else (a if a[1] == 0 else mk(Fraction(math.sin(float(a[1])))))


def cos_of(a):
    return NAN if a == NAN or a[0] == 'i' else mk(Fraction(math.cos(float(a[1]))))


def reload(bits):
    return NAN if bits is None else load(bits)


def phase(time):
    a1 = mul(fint(time & 0x3FF), K1)
    s = mul(sin_of(a1), K02)
    return store(add(s, ONE))


def point(xb, yb, width, height, ph, radius, corner):
    ang = add(mul(fint(corner), PI2), PI4)
    rad = fint(radius)
    phv = reload(ph)
    cp = mul(mul(cos_of(ang), rad), phv)
    yt = mul(mul(add(reload(yb), ONE), fint(s32(height))), HALF)
    y = store(add(cp, yt))
    sp = mul(mul(sin_of(ang), rad), phv)
    xt = mul(mul(add(reload(xb), ONE), fint(s32(width))), HALF)
    x = store(add(sp, xt))
    return x, y


def hist_after(c):
    """historiales (X, Y, T) tras la llamada y el desplazamiento de 0x56F027..0x56F08B"""
    X = [c['hx%d' % i] for i in range(10)]
    Y = [c['hy%d' % i] for i in range(10)]
    T = [c['ht%d' % i] for i in range(10)]
    base = 5 * c['player']
    for k in range(4):
        to = base + 4 - k
        X[to] = X[to - 1]
        Y[to] = Y[to - 1]
        T[to] = Y[3 - k]            # direccion absoluta: Y del jugador 0 + 0xC - 4k
    X[base] = c['tx']
    Y[base] = c['ty']
    T[base] = c['time']
    return X, Y, T


def expected(c):
    """(eventos [lista de listas; los sprites con coordenadas None = cualquier NaN], historial final X, Y, T)"""
    if (c['active'] & 0xFF) == 0:
        return [], None
    ev = [[1, 8, 0], [1, 6, 0], [1, 12, 1], [1, 10, 5], [1, 11, 8], [1, 1, c['raster']]]
    X, Y, T = hist_after(c)
    base = 5 * c['player']
    r, g, b = (0x32, 0, 0xFF) if c['player'] != 0 else (0, 0xFF, 0x32)
    for layer in range(5):
        ph = phase(T[base])
        for j in range(3):
            radius = 20 + 10 * j
            for i in range(4):
                x, y = point(X[base + layer], Y[base + layer], c['width'], c['height'], ph, radius, i)
                ev.append(('sprite', x, y, r, g, b))
        r >>= 1
        g >>= 1
        b >>= 1
    ev += [[1, 8, 1], [1, 6, 1]]
    return ev, (X, Y, T)


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def cmp_bits(got, want):
    return is_nan(got) if want is None else got == want


def hist_hex(X, Y, T):
    return b''.join(v.to_bytes(4, 'little') for v in T + Y + X).hex()


def main(path):
    bad = checked = sprites = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        ev, hist = expected(c)
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            ok = True
            why = ''
            got = o['events']
            if len(got) != len(ev):
                ok = False
                why = 'numero de eventos %d != %d' % (len(got), len(ev))
            else:
                for g, w in zip(got, ev):
                    if isinstance(w, tuple):
                        _, x, y, r, gg, b = w
                        sprites += 1
                        # [2, x, y, z=100, w=15, h=15, r, g, b, intensidad 255, rz 0.01, rotacion 0, alfa 255]
                        if not (g[0] == 2 and cmp_bits(g[1], x) and cmp_bits(g[2], y) and g[3:6] == [0x42C80000, 0x41700000, 0x41700000] and g[6:9] == [r, gg, b] and g[9:] == [255, 0x3C23D70A, 0, 255]):
                            ok = False
                            why = 'sprite %s != %s' % (g, w)
                    elif g != w:
                        ok = False
                        why = 'evento %s != %s' % (g, w)
            if o['changes'] != [[], []]:
                ok = False
                why = why or 'cambios en las arenas %s' % (o['changes'],)
            gotH = o['history']
            if hist is None:
                X0 = [c['hx%d' % i] for i in range(10)]
                Y0 = [c['hy%d' % i] for i in range(10)]
                T0 = [c['ht%d' % i] for i in range(10)]
                wantH = hist_hex(X0, Y0, T0)
            else:
                wantH = hist_hex(*hist)
            if gotH != wantH:
                ok = False
                why = why or 'historiales'
            if not ok:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad, 'sprites', sprites)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
