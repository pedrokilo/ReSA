"""Oraculo independiente del banco CarCtrlSteer63f (lote 63, CCarCtrl parte B1, tanda 6): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++),
  0x428040 DealWithBend_Racing (vehicle, l1, l2, l3, l4, a6, a7, a8, a9, a10, o11, o12, o13, o14, pos)  -> bool en AL
incluidas la funcion de desplazamiento de carril 0x44DB00 y la interseccion de dos rectas 0x4226F0 (que en el banco corren de verdad en el EXE y en la DLL), con aritmetica float de 24 bits
(x87 con PC=24 == float IEEE para + - * / y raiz; el ultimo fpatan se resta sin redondear antes). `expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el
mismo formato que registra Core.hpp. Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct
from fractions import Fraction

CASES = 4096
NAMES = ['CCarCtrl::DealWithBend_Racing']
NAME = NAMES[0]
K_ATAN = 1
OUT_WORDS = 8
AREA_A, AREA_B = 7, 20
LINK = 0xE


def f32(x):
    if isinstance(x, Fraction):
        return f32_exact(x)
    if x != x:
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def f32_exact(q):
    if q == 0:
        return 0.0
    sign = -1 if q < 0 else 1
    q = abs(q)
    e = q.numerator.bit_length() - q.denominator.bit_length()
    while Fraction(2) ** e > q:
        e -= 1
    while Fraction(2) ** (e + 1) <= q:
        e += 1
    shift = e - 23
    if e < -126:
        shift = -149
    scaled = q / (Fraction(2) ** shift)
    n = scaled.numerator // scaled.denominator
    rem = scaled - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (n & 1)):
        n += 1
    v = n * (Fraction(2) ** shift)
    if v >= Fraction(2) ** 128:
        return sign * math.inf
    return sign * float(v)


def bits(f):
    return struct.unpack('<I', struct.pack('<f', f))[0] if f == f else 0x7FC00000


def flt(u):
    return struct.unpack('<f', struct.pack('<I', u & 0xFFFFFFFF))[0]


def dflt(u):
    return struct.unpack('<d', struct.pack('<Q', u))[0]


def fadd(a, b): return f32(a + b)
def fsub(a, b): return f32(a - b)
def fmul(a, b): return f32(a * b)
def fdiv(a, b):
    if b == 0:
        return math.nan if (a == 0 or a != a) else math.copysign(math.inf, a) * (math.copysign(1.0, b))
    return f32(a / b)
def fsqrt(a):
    if a != a or a < 0:
        return math.nan
    return f32(math.sqrt(a))


C_PI = flt(0x40490FDB)
C_2PI = flt(0x40C90FDB)
C_K50 = flt(0x40ACCCCD)     # 5.4 (0x858C50)
C_K8 = flt(0x3E000000)      # 0.125
C_K01 = flt(0x3C23D70A)     # 0.01
C_KWID = flt(0x3C3DA12F)    # 0.0115740737 (0x858EEC)
C_K637 = flt(0x3F22F983)    # 0.6366197 (0x858FB8)
C_K033 = flt(0x3D088889)    # 0.0333333 (0x858F10)
C_KCC8 = flt(0x3F19999A)    # 0.6
C_01 = flt(0x3DCCCCCD)

kCanon = 'nan'


def canon(v):
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return kCanon
    return v


def canon_tree(x):
    if isinstance(x, list):
        return [canon_tree(i) for i in x]
    return canon(x)


def i16(b, o):
    v = b[o] | (b[o + 1] << 8)
    return v - 0x10000 if v & 0x8000 else v


def i8(b, o):
    return b[o] - 256 if b[o] & 0x80 else b[o]


def lane_offset(link):
    """0x44DB00"""
    flags, width = link[0xB], link[0xA]
    opp, same = flags & 7, (flags >> 3) & 7
    if opp == 0:
        return fsub(0.5, fmul(float(same), 0.5))
    if (flags & 0x38) == 0:
        return fsub(0.5, fmul(float(opp), 0.5))
    return fadd(fmul(float(width), C_KWID), 0.5)


def intersect(a1, a2, a3, a4, a5, a6, a7, a8):
    """0x4226F0 desensamblado -> (x, y)"""
    den = fsub(fmul(a3, a8), fmul(a4, a7))
    if den == 0.0:
        t = 0.0
    else:
        t = fdiv(fsub(fmul(fsub(a5, a1), a8), fmul(fsub(a6, a2), a7)), den)
    return fadd(fmul(a3, t), a1), fadd(fmul(t, a4), a2)


def bend(inp, stats):
    out = [0] * OUT_WORDS
    out[:8] = inp['outpre']
    ev = []
    globals_ = []

    def ret(value):
        changes = [['O', 0, w, out[w]] for w in range(OUT_WORDS) if out[w] != inp['outpre'][w]]
        return {'events': canon_tree(ev), 'nevents': len(ev), 'changes': canon_tree(changes), 'globals': globals_, 'ret': value}

    raws = [a & 0xFFFF for a in inp['addr']]
    if raws[0] == 0xFFFF:
        stats['inv1'] = True
        return ret(0)
    if raws[1] == 0xFFFF:
        stats['inv2'] = True
        return ret(0)
    bike = inp['sub'] == 10
    ang_min = flt(0x3EB2B8C3) if bike else flt(0x3F060A92)
    ang_wide = flt(0x3F32B8C3) if bike else flt(0x3F860A92)
    limit = 50.0 if bike else 40.0
    out[1] = bits(1.0)
    loaded = lambda raw: inp['loaded'][0 if (raw >> 10) == AREA_A else 1]
    if not loaded(raws[0]):
        stats['unloaded'] = True
        return ret(0)
    if not loaded(raws[1]):
        stats['unloaded'] = True
        return ret(0)

    def link(raw):
        array = 0 if (raw >> 10) == AREA_A else 1
        o = (raw & 0x3FF) * LINK
        return inp['links'][array][o:o + LINK]

    l1, l2 = link(raws[0]), link(raws[1])
    pos = lambda l, o: fmul(float(i16(l, o)), C_K8)
    l1x, l1y = pos(l1, 0), pos(l1, 2)
    vx, vy = flt(inp['mpos'][0]), flt(inp['mpos'][1])
    dy, dx = fsub(l1y, vy), fsub(l1x, vx)
    d14 = fsqrt(fadd(fmul(dy, dy), fmul(dx, dx)))
    out[3] = bits(d14)
    if d14 > 60.0:
        stats['far'] = True
        return ret(0)

    def close(a, b):
        ay, ax = fsub(pos(a, 2), pos(b, 2)), fsub(pos(a, 0), pos(b, 0))
        return fadd(fmul(ax, ax), fmul(ay, ay)) < 100.0

    def argbyte(u):
        b = u & 0xFF
        return b - 256 if b & 0x80 else b
    a6, a7, a8, a9 = [argbyte(u) for u in inp['args']]
    select = a7
    last = l2
    if raws[2] != 0xFFFF:
        l3 = link(raws[2])
        if close(last, l3):
            select = a8
            last = l3
            stats['chain3'] = True
        if raws[3] != 0xFFFF:
            l4 = link(raws[3])
            if close(last, l4):
                select = a9
                last = l4
                stats['chain4'] = True
    ex, ey = pos(last, 0), pos(last, 2)
    m1, m2 = float(a6), float(select)
    v1x, v1y = fmul(fmul(float(i8(l1, 8)), C_K01), m1), fmul(fmul(float(i8(l1, 9)), C_K01), m1)
    v2x, v2y = fmul(fmul(float(i8(last, 8)), C_K01), m2), fmul(fmul(float(i8(last, 9)), C_K01), m2)

    tx, ty = flt(inp['mtop'][0]), flt(inp['mtop'][1])
    ev.append([K_ATAN, bits(tx), bits(ty)])
    H = f32(dflt(inp['atan'][0]))
    ev.append([K_ATAN, bits(v2x), bits(v2y)])
    G = dflt(inp['atan'][1])
    if G != G or H != H:
        d = math.nan
    elif math.isinf(G):
        d = f32(H - G)
    else:
        d = f32_exact(Fraction(H) - Fraction(G))
    n = 0
    while d > C_PI:
        d = fsub(d, C_2PI); n += 1
        assert n < 100000
        stats['loop_hi'] = True
    while d < -C_PI:
        d = fadd(d, C_2PI); n += 1
        assert n < 100000
        stats['loop_lo'] = True
    absd = -d if d < 0 else d
    if absd < ang_min:
        stats['small_angle'] = True
        return ret(0)
    lanes = lambda l: ((l[0xB] >> 3) & 7) + (l[0xB] & 7)
    if lanes(last) >= 4 and lanes(l1) >= 4:
        stats['wide'] = True
        if absd < ang_wide:
            stats['wide_small'] = True
            return ret(0)
    out[2] = bits(d)

    def points(l, dirx, diry):
        off = lane_offset(l)
        opp, same = float(l[0xB] & 7), float((l[0xB] >> 3) & 7)
        o_t = fadd(off, fsub(opp, 1.0))
        s_t = fadd(off, fsub(same, 1.0))
        return (fmul(fmul(o_t, diry), C_K50), fmul(fmul(o_t, dirx), C_K50), fmul(fmul(s_t, diry), C_K50), fmul(fmul(s_t, dirx), C_K50))
    p0, p1, p2, p3 = points(l1, v1x, v1y)
    q1x, q1y = fadd(p0, l1x), fsub(l1y, p1)
    r1x, r1y = fsub(l1x, p2), fadd(p3, l1y)
    a1, b1, c1, d1 = fsub(q1y, ey), fsub(q1x, ex), fsub(r1y, ey), fsub(r1x, ex)
    dq, dr = fadd(fmul(b1, b1), fmul(a1, a1)), fadd(fmul(d1, d1), fmul(c1, c1))
    if dr > dq:
        ax, ay, bx, by = q1x, q1y, r1x, r1y
    else:
        ax, ay, bx, by = r1x, r1y, q1x, q1y
    ddx, ddy = fsub(bx, ax), fsub(by, ay)
    length = fsqrt(fadd(fmul(ddy, ddy), fmul(ddx, ddx)))
    if length > 11.0:
        stats['clamp11'] = True
        s = fdiv(11.0, length)
        bx, by = fadd(fmul(s, ddx), ax), fadd(fmul(s, ddy), ay)
    p0, p1, p2, p3 = points(last, v2x, v2y)
    q2x, q2y = fadd(p0, ex), fsub(ey, p1)
    r2x, r2y = fsub(ex, p2), fadd(p3, ey)
    a2, b2, c2, d2 = fsub(q2y, l1y), fsub(q2x, l1x), fsub(r2y, l1y), fsub(r2x, l1x)
    dq2, dr2 = fadd(fmul(b2, b2), fmul(a2, a2)), fadd(fmul(d2, d2), fmul(c2, c2))
    if dr2 > dq2:
        cx, cy = q2x, q2y
    else:
        cx, cy = r2x, r2y
    ddvx, ddvy = fsub(v1x, v2x), fsub(v1y, v2y)
    absx, absy = (-ddvx if ddvx < 0 else ddvx), (-ddvy if ddvy < 0 else ddvy)
    if absx < 0.1 and not (absy == 0.0):
        stats['parallel'] = True
        out[4], out[5] = bits(ax), bits(ay)
        px_, py_ = ax, ay
    else:
        stats['intersect'] = True
        px_, py_ = intersect(ax, ay, v1x, v1y, cx, cy, v2x, v2y)
        out[4], out[5] = bits(px_), bits(py_)

    def turn():
        sc = fmul(d, C_K637)
        a = -sc if sc < 0 else sc
        return 1.0 if 1.0 < a else a
    if d14 < limit:
        stats['near_limit'] = True
        out[1] = bits(fsub(1.0, fmul(fmul(fdiv(fsub(limit, d14), limit), turn()), C_KCC8)))
    p = fmul(flt(inp['arg10']), C_K033)
    if 1.0 < p:
        p = 1.0
    elif flt(0x3E4CCCCD) > p:
        p = flt(0x3E4CCCCD)
    thr = fmul(fmul(p, turn()), 20.0)
    if d14 < thr:
        stats['to_pos'] = True
        ev.append([K_ATAN, bits(fsub(px_, vx)), bits(fsub(py_, vy))])
        out[0] = bits(f32(dflt(inp['atan'][2])))
    else:
        stats['to_b'] = True
        ev.append([K_ATAN, bits(fsub(bx, vx)), bits(fsub(by, vy))])
        out[0] = bits(f32(dflt(inp['atan'][2])))
    return ret(1)


def expected(name, inp, case):
    stats = {}
    if name != NAME:
        raise RuntimeError('funcion inesperada ' + name)
    want = bend(inp, stats)
    stats['fn0'] = True
    stats['family%d_fn0' % inp['fam']] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': canon_tree(obs['globals']), 'ret': obs['ret']}


def selftest():
    # 0x44DB00: carriles 0 -> 0.5 - same*0.5 ; opp=2 same=0 -> 0.5 - 1.0 ; con ambos: width*K + 0.5
    link = bytearray(LINK)
    link[0xB] = 0 | (3 << 3)
    assert lane_offset(link) == -1.0
    link[0xB] = 2
    assert lane_offset(link) == -0.5
    link[0xB] = 2 | (1 << 3); link[0xA] = 16
    assert abs(lane_offset(link) - (0.5 + 16 * 0.0115740737)) < 1e-6
    # 0x4226F0: rectas y = x (desde (0,0) con direccion (1,1)) y x = 4 (desde (4,0) con direccion (0,1)) se cortan en (4,4)
    assert intersect(0.0, 0.0, 1.0, 1.0, 4.0, 0.0, 0.0, 1.0) == (4.0, 4.0)
    assert intersect(0.0, 0.0, 1.0, 0.0, 0.0, 5.0, 1.0, 0.0) == (0.0, 0.0)    # paralelas: t = 0
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
