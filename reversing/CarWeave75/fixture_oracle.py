"""Oraculo independiente (Python) de CarWeave75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x4325C0..0x432B0A, lo que debe observarse
(rejilla de celdas, secuencia completa de llamadas a los barridos con sus argumentos, escrituras programadas, valor devuelto en ST0, codigo de escaneo final) y lo compara con las filas
de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga de un NaN la valida el microtest de hardware bounds_nan_hw.py); todo lo demas,
bit a bit. La secuencia de eventos se resume en un hash FNV-1a por palabra de 32 bits (con los NaN de posiciones de coma flotante reducidos a 0x7FC00000) y se comparan ademas los primeros 48.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import floor, isqrt

M32 = 0xFFFFFFFF
NAN = ('n',)
KEEP = 48
REPEAT_BASE = 0xB992B8
INIT_BITS = 0xC61C3F9A


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


K25 = load(0x40200000)
ONE = load(0x3F800000)
TWO = load(0x40000000)
K12 = load(0x41400000)
K002 = load(0x3CA3D70A)
K60 = load(0x42700000)
HALF = load(0x3F000000)
PI = load(0x40490FDB)
TWO_PI = load(0x40C90FDB)
NEG_PI = load(0xC0490FDB)
HALF_PI = load(0x3FC90FDB)
TOL = load(0x3DA3D70A)
ZERO = ('f', Fraction(0), 0)
LAST = {}


def ftol_floor(a):
    """floor (CRT) + _ftol2: palabra baja con signo del entero de 64 bits; NaN, infinito o |v| >= 2^63 -> 0"""
    if a == NAN or a[0] == 'i':
        return 0
    f = floor(a[1])
    if abs(f) >= 1 << 63:
        return 0
    w = f & M32
    return w - (1 << 32) if w & 0x80000000 else w


def sector(fv):
    reg = add(mul(fv, K002), K60)
    return ftol_floor(reg), ftol_floor(st(reg))


def bounds(c):
    if c['has_mat']:
        px, py = load(c['mpos_x']), load(c['mpos_y'])
    else:
        px, py = load(c['ppos_x']), load(c['ppos_y'])
    vx, vy = load(c['vx']), load(c['vy'])
    root = fsqrt(add(mul(vx, vx), mul(vy, vy)))
    t = add(mul(root, K25), ONE)
    if not lt(t, TWO):
        t = TWO
    t2 = mul(mul(t, load(c['scale'])), K12)
    xmin, xmax = store(sub(px, t2)), store(add(t2, px))
    ymin, ymax = store(sub(py, t2)), store(add(t2, py))
    out = dict(xmin=xmin, ymin=ymin, xmax=xmax, ymax=ymax)
    first, second = sector(reload(xmin))
    out['minX'] = 0 if first <= 0 else second
    first, second = sector(reload(ymin))
    out['minY'] = 0 if first <= 0 else second
    first, second = sector(reload(xmax))
    out['maxX'] = 0x77 if first >= 0x77 else second
    first, second = sector(reload(ymax))
    out['maxY'] = 0x77 if first >= 0x77 else second
    return out


def tag_vehicle():
    return 0x10000000 + (0 << 20) + 0x40


def tag_phys(c):
    if c['phys'] == 0:
        return 0
    if c['phys'] == 1:
        return 0x10000000 + (2 << 20) + 0x40
    return 0xDEAD1234


class Fnv:
    def __init__(self):
        self.h = 2166136261

    def word(self, w):
        self.h = ((self.h ^ (w & M32)) * 16777619) & M32


def canon(bits):
    return 0x7FC00000 if bits is None or is_nan(bits) else bits


def simulate(c):
    """secuencia de eventos (cada uno: kind, [palabras], mascara de floats, [bits modelados con None = NaN]), llamadas totales y valores finales de los dos barridos"""
    b = bounds(c)
    LAST.clear()
    LAST.update(b)
    events = []
    clears = 0
    if (c['scan'] & 0xFFFF) >= 0xFFFF:
        events.append((4, [0], 0, [0]))
        clears = 1
    s50 = s40 = c['arg3']
    prev50 = prev40 = INIT_BITS
    calls = 0
    veh, phys = tag_vehicle(), tag_phys(c)
    sweeps = 0
    while not (eq(load(prev50), load(s50)) and eq(load(prev40), load(s40))):
        prev40, prev50 = s40, s50
        sweeps += 1
        if sweeps > 100000:
            raise RuntimeError('demasiados barridos')
        for y in range(b['minY'], b['maxY'] + 1):
            for x in range(b['minX'], b['maxX'] + 1):
                lst = REPEAT_BASE + (((y & 15) * 16 + (x & 15)) * 12)
                seq = [(1, lst)]
                if (c['style'] & 0xFF) != 6:
                    seq.append((2, lst + 4))
                seq.append((3, lst + 8))
                seq.append((1, lst))
                for kind, lp in seq:
                    if kind == 3:
                        words = [calls, lp, veh, b['xmin'], b['ymin'], b['xmax'], b['ymax'], s50, s40, 1]
                        mask = 0x1F8
                    else:
                        words = [calls, lp, veh, phys, b['xmin'], b['ymin'], b['xmax'], b['ymax'], s50, s40, 1]
                        mask = 0x3F0
                    events.append((kind, words, mask, None))
                    for w in c['writes']:
                        if w[0] == calls:
                            if w[1] & 1:
                                s50 = w[2]
                            if w[1] & 2:
                                s40 = w[3]
                    calls += 1
    LAST.update(calls=calls, sweeps=sweeps, clears=clears, s50=s50, s40=s40)
    return events, calls, clears, s50, s40


def final(c, s50bits, s40bits):
    """(bits del resultado o None si NaN, rama)"""
    t, h = load(c['arg3']), load(c['arg4'])
    s50, s40 = load(s50bits), load(s40bits)
    n = 0

    def guard():
        nonlocal n
        n += 1
        if n > 2000000:
            raise RuntimeError('bucle de normalizacion sin fin')
    x = sub(h, t)
    while lt(x, NEG_PI):
        guard()
        x = add(x, TWO_PI)
    while gt(x, PI):
        guard()
        x = sub(x, TWO_PI)
    v = add(mul(x, HALF), t)
    a = store(v)
    if lt(v, NEG_PI):
        while True:
            guard()
            v = add(v, TWO_PI)
            if not lt(v, NEG_PI):
                break
        a = store(v)
    if gt(v, PI):
        while True:
            guard()
            v = sub(v, TWO_PI)
            if not gt(v, PI):
                break
        a = store(v)
    w1 = sub(s50, v)
    w2 = sub(s40, reload(a))
    for which in (1, 2):
        w = w1 if which == 1 else w2
        while lt(w, NEG_PI):
            guard()
            w = add(w, TWO_PI)
        while gt(w, PI):
            guard()
            w = sub(w, TWO_PI)
        if lt(w, ZERO):
            w = fchs(w)
        if which == 1:
            w1 = w
        else:
            w2 = w
    LAST.update(a=w1, b=w2, x=x, v=v, A=reload(a))
    if gt(w1, HALF_PI) and gt(w2, HALF_PI):
        LAST['branch'] = 'A'
        return a, 'A'
    d = sub(w1, w2)
    if lt(d, ZERO):
        d = fchs(d)
    if lt(d, TOL):
        LAST['branch'] = 'near'
        return s40bits, 'near'
    if lt(w1, w2):
        LAST['branch'] = 's50'
        return s50bits, 's50'
    LAST['branch'] = 's40'
    return s40bits, 's40'


def ext_value(hexs):
    """valor de un registro x87 de 80 bits (hex de 10 bytes, little endian) como Fraction, o NAN / ('i', signo)"""
    raw = bytes.fromhex(hexs)
    mant = int.from_bytes(raw[:8], 'little')
    se = int.from_bytes(raw[8:10], 'little')
    sign = se >> 15
    exp = se & 0x7FFF
    if exp == 0x7FFF:
        return ('i', sign) if (mant << 1) & ((1 << 64) - 1) == 0 else NAN
    if mant == 0:
        return ('f', Fraction(0), sign)
    v = Fraction(mant) * Fraction(2) ** (exp - 16383 - 63)
    return ('f', -v if sign else v, sign)


def check_side(c, o):
    """cadena vacia si el lado observado coincide con el modelo"""
    events, calls, clears, s50, s40 = simulate(c)
    res, branch = final(c, s50, s40)
    if o['calls'] != calls or o['clears'] != clears:
        return 'llamadas %d/%d != %d/%d' % (o['calls'], o['clears'], calls, clears)
    if o['ev_count'] != len(events):
        return 'numero de eventos %d != %d' % (o['ev_count'], len(events))
    h = Fnv()
    for kind, words, mask, _ in events:
        h.word(kind)
        h.word(len(words))
        for i, w in enumerate(words):
            h.word(canon(w) if (mask >> i) & 1 else w)
    if o['ev_hash'] != h.h:
        # buscar el primer evento distinto para el mensaje
        got = o['events']
        for i, (kind, words, mask, _) in enumerate(events[:KEEP]):
            if i >= len(got):
                break
            g = got[i]
            exp = [kind] + [canon(w) if (mask >> j) & 1 else w for j, w in enumerate(words)]
            have = [g[0]] + [canon(w) if (mask >> j) & 1 else w for j, w in enumerate(g[1:])]
            if exp != have:
                return 'evento %d: %s != %s' % (i, have, exp)
        return 'hash de eventos %x != %x (eventos %d)' % (o['ev_hash'], h.h, len(events))
    # los primeros 48 con los NaN modelados solo como NaN
    for i, (kind, words, mask, _) in enumerate(events[:KEEP]):
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
    scan = c['scan'] & 0xFFFF
    want_scan = 1 if scan >= 0xFFFF else scan + 1
    if o['scan_after'] != want_scan:
        return 'codigo de escaneo %d != %d' % (o['scan_after'], want_scan)
    if res is None or is_nan(res):
        return 'resultado NaN inesperado'
    got = ext_value(o['ret_st0'])
    want = load(res)
    if got != want and not (got[0] == 'f' and want[0] == 'f' and got[1] == want[1] == 0 and got[2] == want[2]):
        return 'resultado %s != %s (rama %s)' % (got, want, branch)
    ch = o['changes']
    if ch[0] or ch[1] or ch[2]:
        return 'cambios en vehiculo, matriz o fisico: %s' % (ch,)
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
