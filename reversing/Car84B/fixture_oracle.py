"""Oraculo independiente (Python) de Car84B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x426EF0..0x427736, lo que debe observarse de
CCarCtrl::PickNextNodeToChaseCar (el evento de DoPathSearch con todos sus argumentos, el valor devuelto, las escrituras en el piloto y el vehiculo, el estado de las tablas del mundo de caminos, la matriz,
la zona del clima, el reloj y la semilla de rand()) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Las arenas (vehiculo, matriz, nodos, enlaces de nodo, enlaces de navegacion y enlaces de carretera) se reconstruyen byte a byte con el mismo relleno pseudoaleatorio que Fixtures.hpp (Mix / Rand /
FillWords) y las mismas escrituras de las fixtures; el resultado esperado se compara con los cambios observados respecto a la imagen de referencia.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); cada operacion se redondea y los valores guardados con fstp dword se
redondean a float. fpatan (arcotangente de GetATanOfXY) se calcula con 90 digitos decimales y no se redondea hasta la operacion siguiente (la x87 trabaja con 64 bits). rand() del CRT del EXE:
semilla * 0x343FD + 0x269EC3, resultado (semilla >> 16) & 0x7FFF; srand fija la semilla.

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
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 84002) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        w = rand_fix(seed, i // 4)
        out += w.to_bytes(4, 'little')[:min(4, nbytes - i)]
    return out


SLOTS, NODES_PER, LINKS_PER, NAVI_PER, NAVNODES_PER, NODE_BYTES, LINK_BYTES = 3, 4, 16, 16, 8, 0x1C, 14
VEH_WIN = 0x5A0
MAT_WIN = 0x80
SIZES = dict(veh=GUARD * 2 + VEH_WIN, node=GUARD * 2 + SLOTS * NODES_PER * NODE_BYTES, nl=GUARD * 2 + SLOTS * LINKS_PER * 4, navi=GUARD * 2 + SLOTS * NAVI_PER * 2,
             nn=GUARD * 2 + SLOTS * NAVNODES_PER * LINK_BYTES, mat=GUARD * 2 + MAT_WIN)
SEED_ADD = dict(veh=1, node=2, nl=3, navi=4, nn=5, mat=6)


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


VEH_MASK = ((4, 12), (0x14, 4), (0x22, 2), (0x390, 12), (0x39C, 4), (0x3A0, 4), (0x3A4, 6), (0x3B4, 5), (0x3BA, 1), (0x3DB, 1), (0x3E0, 1), (0x594, 4))
FIELDS = dict(m04=(4, 4), m08=(8, 4), m0c=(0xC, 4), m22=(0x22, 2), m45e=(0x45E, 2), m390=(0x390, 4), m394=(0x394, 4), m398=(0x398, 4), m39c=(0x39C, 4), m3a0=(0x3A0, 4), m3a4=(0x3A4, 2), m3a6=(0x3A6, 2),
              m3a8=(0x3A8, 2), m3b4=(0x3B4, 1), m3b5=(0x3B5, 1), m3b6=(0x3B6, 1), m3b7=(0x3B7, 1), m3b8=(0x3B8, 1), m3ba=(0x3BA, 1), m3db=(0x3DB, 1), m3e0=(0x3E0, 1), m594=(0x594, 4))

MUT_SPECIAL = (0, 0x80000000, 1, 0x7F800000, 0xFF800000, 0xFFC00000, 0x7F7FFFFF, 0x3F800000)
K0125, K256, K05, K3, K10 = 0x3E000000, 0x43800000, 0x3F000000, 0x40400000, 0x41200000
KPI, KTWOPI, KNEGPI, KHALFPI, KTHREEHALF, KM1 = 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x3FC90FDB, 0x4096CBE4, 0xBF800000
KSEARCH50, KSEARCH, KUNK = 0x42480000, 0x497423FE, 0x497423FE


def mut_float(v):
    v &= M32
    if v & 3 == 0:
        return MUT_SPECIAL[(v >> 2) % 8]
    import struct
    return struct.unpack('<I', struct.pack('<f', float((v % 2001) - 1000) / 8.0))[0]


# ---------------------------------------------------------------- arcotangente (fpatan(x, 1) con precision mucho mayor que la de 64 bits de la x87)
from decimal import Decimal, getcontext
getcontext().prec = 90


def _atan_small(x):
    x2 = x * x
    term = x
    s = x
    k = 1
    eps = Decimal(10) ** -85
    while True:
        term *= -x2
        k += 2
        t = term / k
        if abs(t) < eps:
            return s
        s += t


def _pi():
    return 16 * _atan_small(Decimal(1) / 5) - 4 * _atan_small(Decimal(1) / 239)


PI_DEC = _pi()


def atan_dec(x):
    if x == 0:
        return Decimal(0)
    neg = x < 0
    x = abs(x)
    inv = x > 1
    if inv:
        x = 1 / x
    k = 0
    while x > Decimal('0.05'):
        x = x / (1 + (1 + x * x).sqrt())
        k += 1
    r = _atan_small(x) * (2 ** k)
    if inv:
        r = PI_DEC / 2 - r
    return -r if neg else r


def fpatan(a):
    """fld a; fld1; fpatan: atan(a / 1) SIN redondear a 24 bits (la x87 de 64 bits)"""
    if a == NAN:
        return NAN
    if a[0] == 'i':
        v = Fraction(PI_DEC / 2)
        return ('f', v if a[1] == 0 else -v, a[1])
    v = a[1]
    if v == 0:
        return ('f', Fraction(0), a[2])
    r = atan_dec(Decimal(v.numerator) / Decimal(v.denominator))
    f = Fraction(r)
    return ('f', f, 1 if f < 0 else 0)


def le(a, b):
    """a <= b ordenado (fcom + test ah,41h / jp): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) <= num(b)


def is_zero(a):
    return a != NAN and a[0] == 'f' and a[1] == 0


def atan_of_xy(x, y):
    """CGeneral::GetATanOfXY (0x53CC70): x, y ya cargados. Devuelve ST0 al terminar (sin guardar)"""
    zero = load(0)
    if is_zero(x) and is_zero(y):
        return zero
    ax = neg(x) if lt(x, zero) else x
    ay = neg(y) if lt(y, zero) else y
    xpos = gt(x, zero)
    ypos = gt(y, zero)
    m1 = load(KM1)
    if lt(ax, ay):
        if ypos:
            if xpos:
                return sub(load(KHALFPI), fpatan(div(x, y)))
            return add(fpatan(mul(div(m1, y), x)), load(KHALFPI))
        if xpos:
            return add(fpatan(mul(div(m1, y), x)), load(KTHREEHALF))
        return sub(load(KTHREEHALF), fpatan(div(x, y)))
    if ypos:
        if xpos:
            return fpatan(div(y, x))
        return sub(load(KPI), fpatan(mul(div(m1, x), y)))
    if xpos:
        return sub(load(KTWOPI), fpatan(mul(div(m1, x), y)))
    return add(fpatan(div(y, x)), load(KPI))


class Unsafe(Exception):
    """la fixture haria leer al EXE fuera de las tablas del banco"""


class World:
    def __init__(self, c):
        self.c = c
        self.veh = Mem('veh', c['seed'])
        self.node = Mem('node', c['seed'])
        self.nl = Mem('nl', c['seed'])
        self.navi = Mem('navi', c['seed'])
        self.nn = Mem('nn', c['seed'])
        self.mat = Mem('mat', c['seed'])
        self.matptr = bool(c['has_mat'])
        v = self.veh
        v.put(0, 4, 0)
        for k in range(3):
            self.mat.put(0x30 + 4 * k, 4, c['mat'][k])
            v.put(4 + 4 * k, 4, c['place'][k])
        v.put(0x14, 4, 1 if self.matptr else 0)
        v.put(0x22, 2, c['model'])
        for key, off, size in (('seed16', 0x45E, 2), ('cur', 0x390, 4), ('start', 0x394, 4), ('end0', 0x398, 4), ('field_c', 0x39C, 4), ('scale', 0x3A0, 4), ('cur_link', 0x3A4, 2), ('next_link', 0x3A6, 2),
                               ('prev_link', 0x3A8, 2), ('sm4', 0x3B4, 1), ('sm5', 0x3B5, 1), ('sm6', 0x3B6, 1), ('lane7', 0x3B7, 1), ('lane8', 0x3B8, 1), ('mission', 0x3BA, 1), ('f3db', 0x3DB, 1),
                               ('f50', 0x3E0, 1), ('subtype', 0x594, 4)):
            v.put(off, size, c[key])
        n = self.node
        for i in range(SLOTS * NODES_PER):
            o = i * NODE_BYTES
            n.put(o + 8, 2, c['node'][i * 3] & 0xFFFF)
            n.put(o + 0xA, 2, c['node'][i * 3] >> 16)
            n.put(o + 0x10, 2, c['node'][i * 3 + 1] & 0xFFFF)
            n.put(o + 0x18, 1, (c['node'][i * 3 + 1] >> 16) & 0xFF)
            n.put(o + 0x19, 1, c['node'][i * 3 + 1] >> 24)
            n.put(o + 0x1A, 1, c['node'][i * 3 + 2])
        for s in range(SLOTS):
            for k in range(LINKS_PER):
                self.nl.put((s * LINKS_PER + k) * 4, 4, c['nlink'][s * LINKS_PER + k])
            for k in range(NAVI_PER):
                self.navi.put((s * NAVI_PER + k) * 2, 2, c['navi'][s * NAVI_PER + k])
        for i in range(SLOTS * NAVNODES_PER):
            o = i * LINK_BYTES
            w0, w1, w2 = c['lk'][i * 3:i * 3 + 3]
            self.nn.put(o, 2, w0 & 0xFFFF)
            self.nn.put(o + 2, 2, w0 >> 16)
            self.nn.put(o + 4, 2, w1 & 0xFFFF)
            self.nn.put(o + 6, 2, w1 >> 16)
            self.nn.put(o + 8, 1, w2)
            self.nn.put(o + 9, 1, w2 >> 8)
            self.nn.put(o + 0xA, 1, w2 >> 16)
            self.nn.put(o + 0xB, 1, w2 >> 24)
        for m in (self.veh, self.node, self.nl, self.navi, self.nn, self.mat):
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


def mutation_addr(w, v):
    c = w.c
    lst = [c['areas'][s] | (j << 16) for s in range(SLOTS) if c['loaded'][s] for j in range(NODES_PER)]
    return lst[v % len(lst)] if lst else 0


def mutation_link(w, v):
    c = w.c
    lst = [j | (c['areas'][s] << 10) for s in range(SLOTS) if c['loaded'][s] for j in range(NAVNODES_PER)]
    return lst[v % len(lst)] if lst else 0


def apply_mut(w, st, kind, v):
    veh = w.veh
    c = w.c
    v &= M32
    start = c['start']
    s0 = w.slot(start & 0xFFFF)
    so = (s0 * NODES_PER + (start >> 16)) * NODE_BYTES if s0 is not None else 0
    if kind == 1:
        veh.put(0x390, 4, mutation_addr(w, v))
    elif kind == 2:
        veh.put(0x394, 4, mutation_addr(w, v))
    elif kind == 3:
        veh.put(0x3A6, 2, mutation_link(w, v))
    elif kind == 4:
        veh.put(0x3A4, 2, mutation_link(w, v))
    elif kind == 5:
        veh.put(0x3B8, 1, v)
    elif kind == 6:
        veh.put(0x3B7, 1, v)
    elif kind == 7:
        veh.put(0x3B5, 1, v)
        veh.put(0x3B6, 1, v >> 8)
    elif kind == 8:
        veh.put(0x3BA, 1, v)
    elif kind == 9:
        veh.put(0x3E0, 1, v)
    elif kind == 10:
        veh.put(0x3DB, 1, v)
    elif kind == 11:
        veh.put(0x594, 4, 10 if (v & 3) == 0 else v)
    elif kind == 12:
        veh.put(0x39C, 4, v)
    elif kind == 13:
        veh.put(0x3A0, 4, v)
    elif kind == 14:
        st['zone'] = 0 if (v & 3) == 0 else 4 if (v & 3) == 1 else (v >> 2) & 0xFF
    elif kind == 15:
        w.matptr = False
        veh.put(0x14, 4, 0)
    elif kind == 16:
        veh.put(4, 4, mut_float(v))
        veh.put(8, 4, mut_float((v * 5 + 1) & M32))
    elif kind == 17:
        w.mat.put(0x30, 4, mut_float(v))
        w.mat.put(0x34, 4, mut_float((v + 7) & M32))
    elif kind == 18:
        w.node.put(so + 0x10, 2, v % (17 - (w.node.get(so + 0x18, 1) & 0xF)))
    elif kind == 19:
        w.node.put(so + 0x18, 1, (w.node.get(so + 0x18, 1) & 0xF0) | (v % (17 - (w.node.get(so + 0x10, 2) & 0xFFFF))))
    elif kind == 20:
        w.node.put(so + 8, 2, v & 0xFFFF)
        w.node.put(so + 0xA, 2, (v >> 16) & 0xFFFF)


# ---------------------------------------------------------------- modelo de PickNextNodeToChaseCar (0x426EF0..0x427736)
def expected(c):
    """devuelve dict(v, ret, changes, node_changes, events, statics, time, rand_state, zone, mat_pos, marks)"""
    w = World(c)
    veh = w.veh
    st = dict(rs=c['rseed'] & M32, ev=0, zone=c['zone'] & 0xFFFF, statics=[x & M32 for x in c['st']], events=[], marks=[], ret=0)
    marks = st['marks']
    zero = load(0)

    def mark(m):
        marks.append(m)

    def g32(o):
        return veh.get(o, 4)

    def finish(ret):
        st['ret'] = ret
        return result(w, st)

    def position():
        """el EXE relee el puntero de la matriz donde la usa"""
        if w.matptr:
            return w.mat.get(0x30, 4), w.mat.get(0x34, 4)
        return veh.get(4, 4), veh.get(8, 4)

    seed16 = c['seed16'] & 0xFFFF
    if seed16:
        st['rs'] = seed16
        mark('seed_srand')
    cur0 = g32(0x390)
    nxt = g32(0x394)
    nxt_off = w.node_off(nxt)
    nxt_slot = w.slot(nxt & 0xFFFF)
    mark('entered')
    mark('matrix' if w.matptr else 'no_matrix')
    # ---- DoPathSearch
    zone = st['zone']
    near_zone = zone in (0, 4)
    mark('zone_0_4' if near_zone else 'zone_other')
    px = store(mul(fromint(s16(w.node.get(nxt_off + 8, 2))), load(K0125)))
    py = store(mul(fromint(s16(w.node.get(nxt_off + 0xA, 2))), load(K0125)))
    pz = store(mul(fromint(s16(w.node.get(nxt_off + 0xC, 2))), load(K0125)))
    vortex = (c['model'] & 0xFFFF) == 0x21B
    if vortex:
        mark('vortex')
    search = KSEARCH50 if near_zone else KSEARCH
    st['events'].append([1, 1, 0, px, py, pz, nxt, c['dest'][0], c['dest'][1], c['dest'][2], 1, 1, 2, 1, search, 0, KUNK, 0, 1, 1 if vortex else 0, 0, zone, cur0, nxt])
    st['events'][-1] = [x & M32 for x in st['events'][-1]]
    count = s16(c['cnt'])
    ps = [c['ps0'], c['ps1']]
    dist = c['ps_dist']
    # (el falso escribe los nodos solo si la cuenta lo pide)
    if st['ev'] == c['mut_at']:
        mark('mut_applied')
        mark('mut_kind_%d' % c['mut_kind'])
        apply_mut(w, st, c['mut_kind'], c['mut_val'])
    st['ev'] += 1
    zone = st['zone']
    if zone in (0, 4):
        if count == 0:
            mark('count0_true')
            return finish(1)
        px_, py_ = position()
        dx = sub(load(c['dest'][0]), load(px_))
        dy = sub(load(c['dest'][1]), load(py_))
        length = sqrt24(add(mul(dy, dy), mul(dx, dx)))
        if lt(mul(length, load(K3)), load(dist)):
            mark('direct_true')
            return finish(1)
        mark('direct_false')
    if count == 1:
        mark('cnt1')
    elif count == 2:
        mark('cnt2')
    else:
        mark('cnt_other')

    chosen = None
    index = 0
    found = False

    def search_link(target):
        base = s16(w.node.get(nxt_off + 0x10, 2))
        i = 0
        while True:
            idx = base + s16(i & 0xFFFF)
            if idx < 0 or idx >= LINKS_PER:
                raise Unsafe('busqueda de enlace')
            if w.nl.get((nxt_slot * LINKS_PER + idx) * 4, 4) == (target & M32):
                return i
            i += 1
    if count in (1, 2):
        if ps[0] != nxt:
            chosen = ps[0]
            index = search_link(ps[0])
            found = True
            mark('neighbor_first')
        elif count == 2:
            if ps[1] != nxt:
                chosen = ps[1]
                index = search_link(ps[1])
                found = True
                mark('neighbor_second')
            else:
                mark('neighbor_equal_second')
        else:
            mark('neighbor_equal_first')
    if not found:
        mark('general')
        px_, py_ = position()
        hy = reload(store(sub(load(c['dest'][1]), load(py_))))
        hx = reload(store(sub(load(c['dest'][0]), load(px_))))
        heading_bits = store(atan_of_xy(hx, hy))
        heading = reload(heading_bits)
        num_links = w.node.get(nxt_off + 0x18, 1) & 0xF
        if num_links == 0:
            mark('numlinks_zero')
        best = load(K10)
        for i in range(num_links):
            base = s16(w.node.get(nxt_off + 0x10, 2))
            idx = base + i
            if idx < 0 or idx >= LINKS_PER:
                raise Unsafe('enlace del nodo')
            cand = w.nl.get((nxt_slot * LINKS_PER + idx) * 4, 4)
            if cand == cur0 and num_links > 1:
                mark('cand_skip_current')
                continue
            if not w.loaded(cand & 0xFFFF):
                mark('cand_skip_unloaded')
                continue
            co = w.node_off(cand)

            def pos(off, k):
                return mul(fromint(s16(w.node.get(off + k, 2))), load(K0125))
            xx = reload(store(sub(pos(co, 8), pos(nxt_off, 8))))
            yy = reload(store(sub(pos(co, 0xA), pos(nxt_off, 0xA))))
            diff = sub(atan_of_xy(xx, yy), heading)
            if gt(diff, load(KPI)):
                mark('wrap_pos')
            if lt(diff, load(KNEGPI)):
                mark('wrap_neg')
            guard = 0
            while gt(diff, load(KPI)):
                diff = sub(diff, load(KTWOPI))
                guard += 1
                if guard > 100:
                    raise Unsafe('normalizacion')
            while lt(diff, load(KNEGPI)):
                diff = add(diff, load(KTWOPI))
                guard += 1
                if guard > 100:
                    raise Unsafe('normalizacion')
            if lt(diff, zero):
                diff = neg(diff)
            if diff == NAN:
                mark('cand_nan')
            if le(diff, best):
                if not lt(diff, best):
                    mark('tie_update')
                best = reload(store(diff))
                index = i
                chosen = cand
                mark('cand_update')
            else:
                mark('cand_not_better')
    if chosen is None:
        mark('none_chosen')
        chosen = 0xFFFF | (0xFFFF << 16)
    # ---- desplazamiento de nodos, enlaces, signos y carril (el EXE relee actual y siguiente)
    old_cur = g32(0x390)
    old_nxt = g32(0x394)
    veh.put(0x390, 4, old_nxt)
    scale = g32(0x3A0)
    total = (g32(0x39C) + scale) & M32
    veh.put(0x398, 4, old_cur)
    sign_cur = veh.get(0x3B5, 1)
    lane_next = veh.get(0x3B8, 1)
    link_cur = veh.get(0x3A4, 2)
    link_next = veh.get(0x3A6, 2)
    sign_next = veh.get(0x3B6, 1)
    veh.put(0x394, 4, chosen)
    veh.put(0x39C, 4, total)
    veh.put(0x3A8, 2, link_cur)
    veh.put(0x3A4, 2, link_next)
    veh.put(0x3B4, 1, sign_cur)
    veh.put(0x3B5, 1, sign_next)
    veh.put(0x3B7, 1, lane_next)
    base = s16(w.node.get(nxt_off + 0x10, 2))
    idx = base + s16(index & 0xFFFF)
    if idx < 0 or idx >= NAVI_PER:
        raise Unsafe('enlace de navegacion')
    veh.put(0x3A6, 2, w.navi.get((nxt_slot * NAVI_PER + idx) * 2, 2))

    def valid_node(word):
        return word != 0xFFFF and w.loaded(word)

    def valid_link(word):
        return word != 0xFFFF and w.loaded(word >> 10)
    if not (valid_link(veh.get(0x3A4, 2)) and valid_link(veh.get(0x3A6, 2)) and valid_node(veh.get(0x390, 2)) and valid_node(veh.get(0x394, 2))):
        veh.put(0x3DC, 1, veh.get(0x3DC, 1) | 1)
        mark('stop_true')
        return finish(1)
    now = g32(0x394)
    below = (nxt & 0xFFFF) < (now & 0xFFFF) or ((nxt & 0xFFFF) == (now & 0xFFFF) and (nxt >> 16) < (now >> 16))
    veh.put(0x3B6, 1, 0xFF if below else 1)
    mark('below' if below else 'not_below')
    nl_ = w.link_off(veh.get(0x3A6, 2))
    flags = w.nn.get(nl_ + 0xB, 1)
    lanes = flags & 7 if below else (flags >> 3) & 7
    cl = w.link_off(veh.get(0x3A4, 2))

    def lpos(off, k):
        return mul(fromint(s16(w.nn.get(off + k, 2))), load(K0125))
    ddx = sub(lpos(nl_, 0), lpos(cl, 0))
    ddy = sub(lpos(nl_, 2), lpos(cl, 2))
    squared = add(mul(ddx, ddx), mul(ddy, ddy))
    if gt(squared, load(K256)):
        mark('dist_far')
        if veh.get(0x3BA, 1) in (2, 4, 0x0F, 0x34, 0x11, 0x36, 0x2B, 0x3D):
            mark('mission_skip')
        else:
            counter = (veh.get(0x3E0, 1) - 1) & 0xFF
            veh.put(0x3E0, 1, counter)
            if counter == 0:
                mark('counter_zero')
                st['rs'] = (st['rs'] * 0x343FD + 0x269EC3) & M32
                veh.put(0x3E0, 1, ((st['rs'] >> 16) & 3) + 4)
                st['rs'] = (st['rs'] * 0x343FD + 0x269EC3) & M32
                rnd_ = (st['rs'] >> 16) & 0x7FFF
                lane = veh.get(0x3B8, 1)
                if rnd_ >= 0x3FFF:
                    veh.put(0x3B8, 1, lane - 1)
                    mark('lane_dec')
                else:
                    veh.put(0x3B8, 1, lane + 1)
                    mark('lane_inc')
    else:
        mark('dist_near')
    lane = s8(veh.get(0x3B8, 1))
    if not (lane < lanes - 1):
        lane = lanes - 1
    lane = s8(lane)
    veh.put(0x3B8, 1, lane if lane > 0 else 0)
    f = veh.get(0x3DB, 1)
    if f & 8:
        veh.put(0x3B8, 1, 0)
        mark('flag8')
    elif f & 0x10 or veh.get(0x594, 4) == 10:
        mark('flag10' if f & 0x10 else 'sub10')
        last = lanes - 1
        veh.put(0x3B8, 1, 0 if last < 0 else last)
    return finish(0)


def result(w, st):
    veh = w.veh
    v = {k: veh.get(o, s) for k, (o, s) in FIELDS.items()}
    if v['m394'] & 0xFFFF == 0xFFFF:
        v['m394'] &= 0xFFFF
    v['m14nz'] = w.matptr
    return dict(v=v, ret=st['ret'], changes=veh.changes(VEH_MASK), node_changes=[w.node.changes(), w.nl.changes(), w.navi.changes(), w.nn.changes(), w.mat.changes(((0x30, 12),))],
                mat_pos=[w.mat.get(0x30, 4), w.mat.get(0x34, 4), w.mat.get(0x38, 4)], events=st['events'], statics=st['statics'], time=0, rand_state=st['rs'], zone=st['zone'], marks=st['marks'])


def check_side(c, o, e):
    for k, val in e['v'].items():
        if o['v'].get(k) != val:
            return 'v.%s: %s != %s' % (k, o['v'].get(k), val)
    if o['ret'] != e['ret']:
        return 'retorno %s != %s' % (o['ret'], e['ret'])
    if o['changes'] != e['changes']:
        return 'changes del vehiculo: %s != %s' % (o['changes'][:6], e['changes'][:6])
    if o['node_changes'] != e['node_changes']:
        return 'changes del mundo: %s != %s' % (o['node_changes'], e['node_changes'])
    if o['mat_pos'] != e['mat_pos']:
        return 'posicion de la matriz %s != %s' % (o['mat_pos'], e['mat_pos'])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o['statics'] != e['statics']:
        return 'estaticas %s != %s' % (o['statics'], e['statics'])
    if o['rand_state'] != e['rand_state']:
        return 'rand %s != %s' % (o['rand_state'], e['rand_state'])
    if o['zone'] & 0xFFFF != e['zone']:
        return 'zona %s != %s' % (o['zone'], e['zone'])
    return None


def check_row(row):
    c = row['input']
    try:
        e = expected(c)
    except Unsafe as ex:
        return 'entrada', 'la fixture lee fuera de las tablas: %s' % ex
    e['time'] = c['now'] & M32
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['time'] != e['time']:
            return side, 'reloj %s != %s' % (o['time'], e['time'])
        why = check_side(c, o, e)
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
