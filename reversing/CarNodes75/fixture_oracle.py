"""Oraculo independiente (Python) de CarNodes75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42BD20..0x42C249, lo que debe observarse
(secuencia completa de llamadas a los colaboradores con sus argumentos, y los dos CNodeAddress de salida) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga de un NaN la valida el microtest de hardware nodes_nan_hw.py); todo lo demas,
bit a bit. La secuencia de eventos se resume en un hash FNV-1a por palabra de 32 bits (con los NaN de posiciones de coma flotante reducidos a 0x7FC00000) y se comparan ademas los primeros 48.
Salidas: con area 0xFFFF el EXE deja en el nodo un resto de pila que ningun llamador lee (se registra como nulo en ambos lados).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import floor, isqrt

M32 = 0xFFFFFFFF
NAN = ('n',)
KEEP = 48


def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v):
    """redondea al par a 24 bits de mantisa (exponente ilimitado)"""
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (e - 23)
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def rnd_float(v):
    """redondea al par a un float (con desnormales): cuanto minimo 2**-149"""
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = max(e - 23, -149)
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def fchs(a):
    if a == NAN:
        return a
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, fchs(b))


def fsqrt(a):
    """fsqrt a 24 bits (redondeo al par)"""
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if a[1] else a
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    e = (v.numerator.bit_length() - v.denominator.bit_length()) // 2
    while Fraction(2) ** (2 * e) > v:
        e -= 1
    while Fraction(2) ** (2 * (e + 1)) <= v:
        e += 1
    q = Fraction(2) ** (e - 23)
    t = v / (q * q)
    k0 = isqrt(t.numerator // t.denominator)
    half = (Fraction(2 * k0 + 1, 2)) ** 2
    k = k0 + (1 if (t > half or (t == half and k0 % 2 == 1)) else 0)
    return ('f', k * q, 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN (cualquier carga)"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd_float(mag)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def reload(bits):
    return NAN if bits is None else load(bits)


def st(a):
    """guardado como float y recargado"""
    return reload(store(a))


def cmp(a, b):
    if a == NAN or b == NAN:
        return None

    def key(v):
        if v[0] == 'i':
            return Fraction(10) ** 400 * (-1 if v[1] else 1)
        return v[1]
    ka, kb = key(a), key(b)
    return (ka > kb) - (ka < kb)


def gt(a, b):
    return cmp(a, b) == 1


def lt(a, b):
    return cmp(a, b) == -1


def eq(a, b):
    return cmp(a, b) == 0


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0




class Fnv:
    def __init__(self):
        self.h = 2166136261

    def word(self, w):
        self.h = ((self.h ^ (w & M32)) * 16777619) & M32


def canon(bits):
    return 0x7FC00000 if bits is None or is_nan(bits) else bits


K200 = load(0x43480000)
K550 = load(0x44098000)
K150 = load(0x43160000)
K3 = load(0x40400000)
K5 = load(0x40A00000)
ONE = load(0x3F800000)
INIT_BEST = load(0x47C34FF3)
THEPATHS = 0x96F050
LAST = {}


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def intf(i):
    """entero de 16 bits * 0.125 (exacto)"""
    return ('f', Fraction(i, 8), 1 if i < 0 else 0)


def lookup(c, area):
    for sl in c['slots']:
        if sl[0] == area:
            return sl
    return None


def simulate(c):
    """eventos (kind, palabras, mascara de floats), salida 1 (area, nodo | None), salida 2, numero de llamadas a DistToLine y Normalise"""
    px_b, py_b, pz_b = (c['mpos_x'], c['mpos_y'], c['mpos_z']) if c['has_mat'] else (c['ppos_x'], c['ppos_y'], c['ppos_z'])
    posx, posy, posz = load(px_b), load(py_b), load(pz_b)
    events = []
    events.append((7, [THEPATHS, px_b], 0x2))
    events.append((8, [THEPATHS, py_b], 0x2))
    rx, ry = c['region_x'], c['region_y']
    events.append((9, [THEPATHS, rx], 0))
    events.append((10, [THEPATHS, ry], 0))
    dxr = st(sub(posx, load(c['xcoord'])))
    dyr = sub(posy, load(c['ycoord']))
    rxs, rys = s32(rx), s32(ry)
    low_x = s32(rxs - 1) if lt(dxr, K200) else rxs
    low_y = s32(rys - 1) if lt(dyr, K200) else rys
    high_x = s32(rxs + 1) if gt(dxr, K550) else rxs
    high_y = s32(rys + 1) if gt(dyr, K550) else rys
    min_x, min_y = max(low_x, 0), max(low_y, 0)
    high_x, high_y = min(high_x, 7), min(high_y, 7)
    LAST.clear()
    LAST.update(min_x=min_x, min_y=min_y, high_x=high_x, high_y=high_y, scanned=0, nodes_in_range=0, links=0, links_valid=0, best_changes=0, dists=0)
    best = INIT_BEST
    out1 = (0xFFFF, None)
    out2 = (0xFFFF, None)
    nd = nn = 0
    fwd = (load(c['fwd_x']), load(c['fwd_y']), load(c['fwd_z']))
    z3_pos = store(mul(posz, K3))
    for x in range(min_x, high_x + 1):
        if min_y > high_y:
            continue
        for y in range(min_y, high_y + 1):
            area = x + y * 8
            sl = lookup(c, area)
            if sl is None or not sl[1]:
                continue
            count = s32(sl[2])
            if count <= 0:
                continue
            LAST['scanned'] += 1
            nodes, table = sl[4], {i: v for i, v in sl[5]}
            for k in range(count):
                n = nodes[k]
                cx, cy, cz = intf(n[0]), intf(n[1]), intf(n[2])
                dx = st(sub(cx, posx))
                dy = sub(cy, posy)
                dz = sub(cz, posz)
                dist = fsqrt(add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx)))
                if not lt(dist, K150):
                    continue
                LAST['nodes_in_range'] += 1
                nl = n[4] & 0xF
                base = s16(n[3])
                for j in range(nl):
                    link = table[base + j]
                    LAST['links'] += 1
                    tsl = lookup(c, link & 0xFFFF)
                    if tsl is None or not tsl[1]:
                        continue
                    LAST['links_valid'] += 1
                    tn = tsl[4][link >> 16]
                    tx, ty, tz = intf(tn[0]), intf(tn[1]), intf(tn[2])
                    events.append((11, [nd, store(cx), store(cy), store(mul(cz, K3)), store(tx), store(ty), store(mul(tz, K3)), px_b, py_b, z3_pos], 0x3FE))
                    distv = load(c['dist'][nd % 8])
                    nd += 1
                    delta = (store(sub(tx, cx)), store(sub(ty, cy)), store(sub(tz, cz)))
                    events.append((12, [nn, delta[0], delta[1], delta[2]], 0xE))
                    nv = [load(v) for v in c['norm'][nn % 8]]
                    nn += 1
                    dot = add(add(mul(nv[2], fwd[2]), mul(nv[1], fwd[1])), mul(nv[0], fwd[0]))
                    score = add(mul(sub(ONE, dot), K5), distv)
                    if lt(score, best):
                        best = st(score)
                        out1 = (area & 0xFFFF, k & 0xFFFF)
                        out2 = (link & 0xFFFF, link >> 16)
                        LAST['best_changes'] += 1
    LAST.update(dists=nd, best=best)
    return events, out1, out2, nd, nn


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def check_side(c, o):
    events, out1, out2, nd, nn = simulate(c)
    if o['dist_calls'] != nd or o['norm_calls'] != nn:
        return 'llamadas %d/%d != %d/%d' % (o['dist_calls'], o['norm_calls'], nd, nn)
    if o['ev_count'] != len(events):
        return 'numero de eventos %d != %d' % (o['ev_count'], len(events))
    h = Fnv()
    for kind, words, mask in events:
        h.word(kind)
        h.word(len(words))
        for i, w in enumerate(words):
            h.word(canon(w) if (mask >> i) & 1 else w)
    if o['ev_hash'] != h.h:
        got = o['events']
        for i, (kind, words, mask) in enumerate(events[:KEEP]):
            if i >= len(got):
                break
            g = got[i]
            exp = [kind] + [canon(w) if (mask >> j) & 1 else w for j, w in enumerate(words)]
            have = [g[0]] + [canon(w) if (mask >> j) & 1 else w for j, w in enumerate(g[1:])]
            if exp != have:
                return 'evento %d: %s != %s' % (i, have, exp)
        return 'hash de eventos %x != %x (eventos %d)' % (o['ev_hash'], h.h, len(events))
    for i, (kind, words, mask) in enumerate(events[:KEEP]):
        g = o['events'][i]
        if g[0] != kind or len(g) != len(words) + 1:
            return 'evento %d: forma %s' % (i, g)
        for j, w in enumerate(words):
            gw = g[1 + j]
            if (mask >> j) & 1 and w is None:
                if not is_nan(gw):
                    return 'evento %d palabra %d deberia ser NaN: %x' % (i, j, gw)
            elif (mask >> j) & 1 and is_nan(w):
                if not is_nan(gw):
                    return 'evento %d palabra %d deberia ser NaN: %x' % (i, j, gw)
            elif gw != w:
                return 'evento %d palabra %d: %x != %x' % (i, j, gw, w)
    if len(o['events']) != min(len(events), KEEP):
        return 'primeros eventos'
    for name, want in (('out1', out1), ('out2', out2)):
        got = o[name]
        if got[0] != want[0] or got[1] != want[1]:
            return '%s %s != %s' % (name, got[:2], want)
        if got[2]:
            return '%s cambios fuera del objeto: %s' % (name, got[2])
    ch = o['changes']
    if any(ch):
        return 'cambios en vehiculo, matriz, nodos o tablas: %s' % (ch,)
    return ''


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        for side in ('original', 'candidate'):
            why = check_side(c, row[side]['observed'])
            if why:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
