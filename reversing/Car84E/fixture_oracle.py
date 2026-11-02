"""Oraculo independiente (Python) de Car84E: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x424210..0x424CD7, lo que debe observarse de
CCarCtrl::GenerateCarCreationCoors2 (los eventos de Find2NodesForCarCreation, FindNodeClosestToCoors, IsSphereVisible y ThisNodeWillLeadIntoADeadEnd con todos sus argumentos, el valor devuelto, el punto de
salida, los dos nodos y la fraccion, las estaticas del EXE, la bandera de "mas coches", gString, el estado de las tablas del mundo de caminos, el reloj y la semilla de rand()) y lo compara con las filas de
comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Las arenas (nodos, enlaces de nodo, enlaces de navegacion, enlaces de carretera y salidas) se reconstruyen byte a byte con el mismo relleno pseudoaleatorio que Fixtures.hpp (Mix / Rand / FillWords) y las
mismas escrituras de las fixtures; el resultado esperado se compara con los cambios observados respecto a la imagen de referencia.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); cada operacion se redondea y los valores guardados con fstp dword se
redondean a float. rand() del CRT del EXE: semilla * 0x343FD + 0x269EC3, resultado (semilla >> 16) & 0x7FFF.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)

# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
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


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def sqrt24(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
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


def fromint(j):
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def lt(a, b):
    """a < b ordenado (fcomp + test ah,5 / jp): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) < num(b)


def gt(a, b):
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v




def div(a, b):
    """fdiv: a / b (24 bits, exponente ampliado)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN
    sa = a[1] if a[0] == 'i' else a[2]
    sb = b[1] if b[0] == 'i' else b[2]
    if a[0] == 'i':
        return ('i', sa ^ sb)
    if b[0] == 'i':
        return ('f', Fraction(0), sa ^ sb)
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', sa ^ sb)
    return fin(a[1] / b[1], sa ^ sb)


def ftol2(a):
    """_ftol2 (0x821B40): int64 truncado; NaN, infinito o fuera de rango dan 0x8000000000000000. Se usa EAX (dword bajo, con signo)"""
    if a == NAN or a[0] == 'i':
        return 0
    v = a[1]
    n = v.numerator // v.denominator if v >= 0 else -((-v.numerator) // v.denominator)
    if abs(n) >= 1 << 63:
        return 0
    n &= (1 << 64) - 1
    low = n & M32
    return low - (1 << 32) if low & 0x80000000 else low


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v




def le(a, b):
    """a <= b ordenado (fcom + test ah,41h / jp): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) <= num(b)


def fabs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def store_bits(a):
    """fstp dword: bits del float; un NaN (unico patron de las fixtures o invalido de la x87) se guarda como el indefinido 0xFFC00000"""
    b = store(a)
    return 0xFFC00000 if b is None else b


def st(a):
    """fstp dword + fld dword: el valor redondeado a float"""
    return reload(store(a))


# ---------------------------------------------------------------- generador de fixtures (Mix / Rand / FillWords de Fixtures.hpp: solo para reproducir el relleno de las arenas)
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand_fix(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 84005) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        w = rand_fix(seed, i // 4)
        out += w.to_bytes(4, 'little')[:min(4, nbytes - i)]
    return out


SLOTS, NODES_PER, LINKS_PER, NAVI_PER, NAVNODES_PER, NODE_BYTES, LINK_BYTES = 3, 4, 16, 16, 8, 0x1C, 14
OUT_WIN = 0x20
GSTRING_LEN = 96
SIZES = dict(node=GUARD * 2 + SLOTS * NODES_PER * NODE_BYTES, nl=GUARD * 2 + SLOTS * LINKS_PER * 4, navi=GUARD * 2 + SLOTS * NAVI_PER * 2,
             nn=GUARD * 2 + SLOTS * NAVNODES_PER * LINK_BYTES, out=GUARD * 2 + OUT_WIN)
SEED_ADD = dict(node=2, nl=3, navi=4, nn=5, out=6)


class Mem:
    """arena con guarda: los desplazamientos son relativos al objeto (ventana)"""

    def __init__(self, name, seed):
        self.b = fill_words(SIZES[name], (seed * 13 + SEED_ADD[name]) & M32)
        self.ref = None

    def put(self, off, size, v):
        self.b[GUARD + off:GUARD + off + size] = (v & ((1 << (8 * size)) - 1)).to_bytes(size, 'little')

    def get(self, off, size):
        return int.from_bytes(self.b[GUARD + off:GUARD + off + size], 'little')

    def snap(self):
        self.ref = bytes(self.b)

    def changes(self, mask=()):
        a = bytearray(self.b)
        r = bytearray(self.ref)
        for off, ln in mask:
            for i in range(ln):
                a[GUARD + off + i] = 0
                r[GUARD + off + i] = 0
        return [[i - GUARD, a[i]] for i in range(len(a)) if a[i] != r[i]]


MUT_SPECIAL = (0, 0x80000000, 1, 0x7F800000, 0xFF800000, 0xFFC00000, 0x7F7FFFFF, 0x3F800000)
K0 = 0
K1, KHALF, K10, K40, K230, K0125, K15, K5 = 0x3F800000, 0x3F000000, 0x41200000, 0x42200000, 0x43660000, 0x3E000000, 0x3FC00000, 0x40A00000


def mut_float(v):
    v &= M32
    if v & 3 == 0:
        return MUT_SPECIAL[(v >> 2) % 8]
    import struct
    return struct.unpack('<I', struct.pack('<f', float((v % 2001) - 1000) / 8.0))[0]


class Unsafe(Exception):
    """la fixture haria leer al EXE fuera de las tablas del banco"""


class World:
    def __init__(self, c):
        self.c = c
        self.node = Mem('node', c['seed'])
        self.nl = Mem('nl', c['seed'])
        self.navi = Mem('navi', c['seed'])
        self.nn = Mem('nn', c['seed'])
        self.out = Mem('out', c['seed'])
        n = self.node
        for i in range(SLOTS * NODES_PER):
            o = i * NODE_BYTES
            n.put(o + 8, 2, c['node'][i * 3] & 0xFFFF)
            n.put(o + 0xA, 2, c['node'][i * 3] >> 16)
            n.put(o + 0xC, 2, c['node'][i * 3 + 1] & 0xFFFF)
            n.put(o + 0x10, 2, c['node'][i * 3 + 1] >> 16)
            n.put(o + 0x12, 2, c['node'][i * 3 + 2] & 0xFFFF)
            n.put(o + 0x18, 1, (c['node'][i * 3 + 2] >> 16) & 0xFF)
            n.put(o + 0x19, 1, c['node'][i * 3 + 2] >> 24)
        for s in range(SLOTS):
            for k in range(LINKS_PER):
                self.nl.put((s * LINKS_PER + k) * 4, 4, c['nlink'][s * LINKS_PER + k])
            for k in range(NAVI_PER):
                self.navi.put((s * NAVI_PER + k) * 2, 2, c['navi'][s * NAVI_PER + k])
        for i in range(SLOTS * NAVNODES_PER):
            o = i * LINK_BYTES
            w0, w1 = c['lk'][i * 2:i * 2 + 2]
            self.nn.put(o, 2, w0 & 0xFFFF)
            self.nn.put(o + 2, 2, w0 >> 16)
            self.nn.put(o + 0xB, 1, w1)
        for m in (self.node, self.nl, self.navi, self.nn, self.out):
            m.snap()

    def slot(self, area):
        for s in range(SLOTS):
            if self.c['loaded'][s] and self.c['areas'][s] == area:
                return s
        return None

    def loaded(self, area):
        return self.slot(area) is not None

    def node_off(self, addr):
        s = self.slot(addr & 0xFFFF)
        if s is None or (addr >> 16) >= NODES_PER:
            raise Unsafe('nodo')
        return (s * NODES_PER + (addr >> 16)) * NODE_BYTES

    def link_off(self, word):
        s = self.slot(word >> 10)
        if s is None or (word & 0x3FF) >= NAVNODES_PER:
            raise Unsafe('enlace')
        return (s * NAVNODES_PER + (word & 0x3FF)) * LINK_BYTES

    def pos(self, off, k):
        """int16 * 0.125f (0x858C48)"""
        return mul(fromint(s16(self.node.get(off + k, 2))), load(K0125))


def pick_addr(c, v):
    v &= M32
    return c['areas'][v % SLOTS] | (((v >> 8) % NODES_PER) << 16)


def apply_mut(w, S, kind, v):
    c = w.c
    v &= M32
    if kind == 1:
        for i in range(SLOTS * NODES_PER):
            o = i * NODE_BYTES
            w.node.put(o + 8, 2, (v + i * 37) & M32)
            w.node.put(o + 0xA, 2, ((v >> 8) - i * 53) & M32)
            w.node.put(o + 0xC, 2, ((v >> 16) + i * 11) & M32)
    elif kind == 2:
        for i in range(SLOTS * NODES_PER):
            o = i * NODE_BYTES
            w.node.put(o + 0x18, 1, (w.node.get(o + 0x18, 1) & 0x0F) | (v & 0xF0))
    elif kind == 3:
        w.out.put(0, 4, mut_float(v))
    elif kind == 4:
        S['A'] = pick_addr(c, v)
    elif kind == 5:
        S['time'] = v
    elif kind == 6:
        for i in range(SLOTS * NODES_PER):
            w.node.put(i * NODE_BYTES + 0x10, 2, (v + i) % 8)
    elif kind == 7:
        for s in range(SLOTS):
            for k in range(LINKS_PER):
                w.nl.put((s * LINKS_PER + k) * 4, 4, pick_addr(c, v + s * 16 + k))


def tag_node(off):
    return 0x10000000 + GUARD + off


# ---------------------------------------------------------------- modelo de GenerateCarCreationCoors2 (0x424210..0x424CD7)
def expected(c):
    """devuelve dict(ret, out, out_changes, node_changes, events, statics, statics2, busy, gstring, time, rand_state, marks)"""
    w = World(c)
    S = dict(guard=c['guard'] & M32, nextTime=c['next_time'] & M32, last=[x & M32 for x in c['last']], A=c['sa'] & M32, B=c['sb'] & M32, C=c['sc'] & M32, D=c['sd'] & M32,
             seq=[x & M32 for x in c['seq']], busy=c['busy'] & 0xFF, time=c['now'] & M32, rs=c['rseed'] & M32, ev=0, find2=0, vis=0, dead=0, events=[], marks=[],
             gstring=fill_words(GSTRING_LEN, (c['seed'] * 13 + 7) & M32), ret=0)
    marks = S['marks']

    def mark(m):
        marks.append(m)

    def rand():
        S['rs'] = (S['rs'] * 0x343FD + 0x269EC3) & M32
        return (S['rs'] >> 16) & 0x7FFF

    def ev(kind, *vals):
        S['events'].append([kind] + [x & M32 for x in vals])

    def after():
        if S['ev'] == c['mut_ev']:
            mark('mut_applied')
            mark('mut_kind_%d' % c['mut_kind'])
            apply_mut(w, S, c['mut_kind'], c['mut_val'])
        S['ev'] += 1

    def finish(ret):
        S['ret'] = ret
        return result(w, S)

    zero, one = load(K0), load(K1)
    px, py, pz = (load(b) for b in c['pos'])
    pos_bits = [b & M32 for b in c['pos']]
    mark('entered')
    # ---- estaticas con guarda
    for bit, key in ((1, 'A'), (2, 'B'), (4, 'C'), (8, 'D')):
        if not S['guard'] & bit:
            S['guard'] |= bit
            S[key] = (S[key] & 0xFFFF0000) | 0xFFFF
            mark('guard_init')
    if not S['guard'] & 0x10:
        S['guard'] |= 0x10
    accum = zero
    dx = sub(px, load(S['last'][0]))
    dy = sub(py, load(S['last'][1]))
    length = sqrt24(add(mul(dx, dx), mul(dy, dy)))
    by_len = gt(length, load(K10))
    by_time = (S['time'] & M32) > S['nextTime']
    if by_len or by_time:
        mark('refresh_len' if by_len else 'refresh_time')
        ev(1, 1, *pos_bits, 0x969104, 0x969100, 1, S['time'], 0)
        S['A'], S['B'] = c['f2'][0] & M32, c['f2'][1] & M32
        S['find2'] += 1
        after()
        ev(1, 1, *pos_bits, 0x9690FC, 0x9690F8, 0, S['time'], 1)
        S['C'], S['D'] = c['f2'][2] & M32, c['f2'][3] & M32
        S['find2'] += 1
        after()
        S['nextTime'] = (S['time'] + 5000) & M32
        S['last'] = list(pos_bits)
        first = S['A']
        if (first & 0xFFFF) != 0xFFFF and w.loaded(first & 0xFFFF):
            mark('busy_block')
            fo = w.node_off(first)
            area_f = w.node.get(fo + 0x12, 2)
            sl = w.slot(area_f)
            if sl is None:
                raise Unsafe('area propia del nodo')
            base = s16(w.node.get(fo + 0x10, 2))
            if base < 0 or base >= NAVI_PER:
                raise Unsafe('base de enlaces')
            word = w.navi.get((sl * NAVI_PER + base) * 2, 2)
            if w.loaded(word >> 10):
                lo = w.link_off(word)
                flags = w.nn.get(lo + 0xB, 1)
                if (flags & 7) < 2 and (flags & 0x38) < 0x10:
                    S['busy'] = 0
                    mark('busy_narrow')
                else:
                    lx = mul(fromint(s16(w.nn.get(lo, 2))), load(K0125))
                    ly = mul(fromint(s16(w.nn.get(lo + 2, 2))), load(K0125))
                    ddx = sub(lx, px)
                    ddy = sub(ly, py)
                    d = sqrt24(add(mul(ddy, ddy), mul(ddx, ddx)))
                    if lt(d, load(K40)):
                        S['busy'] = 1
                        if (w.node.get(fo + 0x19, 1) & 0x30) < 0x20:
                            S['busy'] = 0
                            mark('busy_near_clear')
                        else:
                            mark('busy_near_set')
                    else:
                        S['busy'] = 0
                        mark('busy_far')
            else:
                mark('busy_skip_area')
        else:
            mark('busy_skip_first')
    else:
        mark('no_refresh')
    # ---- nodo de partida
    a6 = load(c['a6'])
    a7 = load(c['a7'])
    r = rand()
    if (r & 0xF) == 4 and not c['a13']:
        mark('start_closest')
        ev(2, 1, *pos_bits, 0, 0x42480000, 0, 0, 0, 1, 1)
        cur = c['closest'] & M32
        after()
        a6 = st(mul(a6, load(K15)))
    else:
        if (r & 0xF) == 4:
            mark('start_blocked13')
        r2 = rand()
        if r2 & 3:
            cur = S['A'] if c['a12'] else S['C']
            mark('start_ac')
        else:
            cur = S['B'] if c['a12'] else S['D']
            mark('start_bd')
    if (cur & 0xFFFF) == 0xFFFF or not w.loaded(cur & 0xFFFF):
        mark('start_invalid')
        return finish(0)
    history = [cur]
    arg11 = None
    while lt(accum, load(K230)):
        off = w.node_off(cur)
        sl = w.slot(cur & 0xFFFF)
        nl = w.node.get(off + 0x18, 1) & 0xF
        if nl == 0:
            raise Unsafe('sin enlaces')
        mark('loop_iter')
        S['seq'][2] = nl
        offset = rand() % nl
        S['seq'][1] = offset
        way = (rand() >> 4) & 1
        S['seq'][0] = (S['seq'][0] & 0xFFFFFF00) | way
        descending = offset + nl
        chosen = None
        for i in range(nl):
            idx = ((i + offset) if way else descending) % nl
            descending -= 1
            lidx = s16(w.node.get(off + 0x10, 2)) + idx
            if lidx < 0 or lidx >= LINKS_PER:
                raise Unsafe('indice de enlace')
            nxt = w.nl.get((sl * LINKS_PER + lidx) * 4, 4)
            if not w.loaded(nxt & 0xFFFF):
                mark('link_unloaded')
                continue
            word = w.navi.get((sl * NAVI_PER + lidx) * 2, 2)
            if not w.loaded(word >> 10):
                mark('link_navi_unloaded')
                continue
            w.node_off(nxt)
            if nxt in history:
                mark('link_seen')
                continue
            chosen = nxt
            break
        if chosen is None:
            mark('no_link')
            return finish(0)
        nxt = chosen
        noff = w.node_off(nxt)
        cx, cy = w.pos(off, 8), w.pos(off, 0xA)
        nx, ny = w.pos(noff, 8), w.pos(noff, 0xA)
        dxc = st(sub(px, cx))
        dyc = sub(py, cy)
        distc = st(sqrt24(add(mul(dyc, dyc), mul(dxc, dxc))))
        dxn = st(sub(px, nx))
        dyn = sub(py, ny)
        distn = st(sqrt24(add(mul(dyn, dyn), mul(dxn, dxn))))
        advance = False
        accepted = False
        nflags = w.node.get(noff + 0x18, 1)
        cflags = w.node.get(off + 0x18, 1)
        if ((nflags & 0x20) or (cflags & 0x20)) and c['a12']:
            mark('skip_flags')
            advance = True
        else:
            bc, bn = sub(distc, a6), sub(distn, a6)
            if lt(mul(bc, bn), zero):
                mark('a1_straddle')
                aC = st(neg(bc) if lt(bc, zero) else bc)
                aN = neg(bn) if lt(bn, zero) else bn
                cz, nz = w.pos(off, 0xC), w.pos(noff, 0xC)
                cxT, cyT, czT = st(mul(cx, aN)), mul(cy, aN), mul(cz, aN)
                nxT, nyT, nzT = st(mul(nx, aC)), mul(ny, aC), st(mul(nz, aC))
                sx, sy, sz = st(add(nxT, cxT)), st(add(nyT, cyT)), add(nzT, czT)
                inv = div(one, add(aN, aC))
                invs = st(inv)
                ox, oy, oz = store_bits(mul(sx, inv)), store_bits(mul(sy, inv)), store_bits(mul(sz, inv))
                w.out.put(0, 4, ox)
                w.out.put(4, 4, oy)
                w.out.put(8, 4, oz)
                ev(3, 1, ox, oy, oz, K5, 1)
                vis = c['vis'][S['vis'] & 3] & 1
                S['vis'] += 1
                after()
                if vis:
                    arg11 = store_bits(mul(invs, aC))
                    w.out.put(0x18, 4, arg11)
                    accepted = True
                    mark('a1_accept')
                else:
                    mark('a1_hidden')
            if not accepted:
                fc, fn = sub(distc, a7), sub(distn, a7)
                if lt(mul(fc, fn), zero):
                    mark('a2_straddle')
                    aC = st(neg(fc) if lt(fc, zero) else fc)
                    aN = st(neg(fn) if lt(fn, zero) else fn)
                    cpx, cpy, cpz = w.pos(off, 8), w.pos(off, 0xA), w.pos(off, 0xC)
                    cxT, cyT, czT = st(mul(aN, cpx)), st(mul(aN, cpy)), st(mul(aN, cpz))
                    npx, npy, npz = w.pos(noff, 8), w.pos(noff, 0xA), w.pos(noff, 0xC)
                    nxT, nyT, nzT = mul(aC, npx), mul(aC, npy), st(mul(aC, npz))
                    sx, sy, sz = st(add(nxT, cxT)), add(nyT, cyT), add(nzT, czT)
                    inv = div(one, add(aN, aC))
                    invs = st(inv)
                    ox, oy, oz = store_bits(mul(sx, inv)), store_bits(mul(sy, inv)), store_bits(mul(sz, inv))
                    w.out.put(0, 4, ox)
                    w.out.put(4, 4, oy)
                    w.out.put(8, 4, oz)
                    ev(3, 1, ox, oy, oz, K5, 1)
                    vis = c['vis'][S['vis'] & 3] & 1
                    S['vis'] += 1
                    after()
                    if not vis:
                        arg11 = store_bits(mul(invs, aC))
                        w.out.put(0x18, 4, arg11)
                        accepted = True
                        mark('a2_accept')
                    else:
                        mark('a2_visible')
                else:
                    mark('a2_no_straddle')
            if accepted:
                cxs, cys, czs = w.pos(off, 8), w.pos(off, 0xA), w.pos(off, 0xC)
                nxs, nys, nzs = w.pos(noff, 8), w.pos(noff, 0xA), w.pos(noff, 0xC)
                dxl = st(sub(cxs, nxs))
                dyl = sub(cys, nys)
                dz = fabs(sub(czs, nzs))
                half = mul(sqrt24(add(mul(dxl, dxl), mul(dyl, dyl))), load(KHALF))
                if lt(half, dz):
                    mark('slope_fail')
                    advance = True
            else:
                advance = True
        if not advance:
            r = rand()
            if r & 8:
                n1, n2 = cur, nxt
                mark('final_set')
            else:
                n1, n2 = nxt, cur
                a = load(w.out.get(0x18, 4))
                w.out.put(0x18, 4, store_bits(sub(one, a)))
                mark('final_flip')
            w.out.put(0x10, 4, n1)
            w.out.put(0x14, 4, n2)
            o2 = w.node_off(n2)
            o1 = w.node_off(n1)
            if w.node.get(o2 + 0x18, 1) & 0x10:
                mark('dead_check')
                ev(4, 1, tag_node(o2), tag_node(o1), n1, n2)
                d = c['dead'][S['dead'] & 1] & 1
                S['dead'] += 1
                after()
                if d and c['a12']:
                    mark('dead_reject')
                    return finish(0)
            ox, oy = load(w.out.get(0, 4)), load(w.out.get(4, 4))
            dx = sub(ox, px)
            dy = sub(oy, py)
            inv = div(one, sqrt24(add(mul(dy, dy), mul(dx, dx))))
            dot = add(mul(mul(inv, dy), load(c['a3'])), mul(mul(inv, dx), load(c['rad'])))
            good = gt(dot, load(c['a4'])) if c['f5'] else le(dot, load(c['a4']))
            if not good:
                mark('dot_reject')
                return finish(0)
            text = ('tell Obbe it happened again %d/%d %d/%d' % (n1 & 0xFFFF, n1 >> 16, n2 & 0xFFFF, n2 >> 16)).encode() + b'\0'
            S['gstring'][:len(text)] = text
            mark('ret_true')
            return finish(1)
        # ---- se avanza al nodo siguiente
        mark('advance')
        history.append(nxt)
        cur_old = cur
        cur = nxt
        cxr, cyr = w.pos(off, 8), w.pos(off, 0xA)
        nxr, nyr = w.pos(noff, 8), w.pos(noff, 0xA)
        dxl = st(sub(nxr, cxr))
        dyl = sub(nyr, cyr)
        ln = sqrt24(add(mul(dxl, dxl), mul(dyl, dyl)))
        accum = st(add(ln, accum))
        if len(history) >= 30:
            mark('limit30')
            return finish(0)
    mark('limit230')
    return finish(0)


def result(w, S):
    out = w.out
    return dict(ret=S['ret'], out=[out.get(0, 4), out.get(4, 4), out.get(8, 4), out.get(0x10, 4), out.get(0x14, 4), out.get(0x18, 4)], out_changes=out.changes(((0, 12), (0x10, 12))),
                node_changes=[w.node.changes(), w.nl.changes(), w.navi.changes(), w.nn.changes()], events=S['events'], statics=list(S['seq']),
                statics2=[S['guard'], S['nextTime'], S['last'][0], S['last'][1], S['last'][2], S['A'], S['B'], S['C'], S['D']], busy=S['busy'], gstring=bytes(S['gstring']).hex(),
                time=S['time'] & M32, rand_state=S['rs'], marks=S['marks'])


def check_side(c, o, e):
    for k in ('ret', 'out', 'out_changes', 'node_changes', 'statics', 'statics2', 'busy', 'gstring', 'time', 'rand_state'):
        if o[k] != e[k]:
            return '%s: %s != %s' % (k, o[k] if k != 'node_changes' else str(o[k])[:200], e[k] if k != 'node_changes' else str(e[k])[:200])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    try:
        e = expected(c)
    except Unsafe as ex:
        return 'entrada', 'la fixture lee fuera de las tablas: %s' % ex
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
