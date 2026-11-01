"""Oraculo independiente (Python) de Car84A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42DE80..0x42EC81, lo que debe observarse de
CCarCtrl::PickNextNodeRandomly (traza de colaboradores EN ORDEN con sus argumentos, escrituras en el piloto y el vehiculo, estado de las tablas del mundo de caminos, estaticas de la secuencia, reloj y semilla
de rand()) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Las arenas (vehiculo, nodos, enlaces de nodo, enlaces de navegacion y enlaces de carretera) se reconstruyen byte a byte con el mismo relleno pseudoaleatorio que Fixtures.hpp (Mix / Rand / FillWords) y las
mismas escrituras de las fixtures; el resultado esperado se compara con los cambios observados respecto a la imagen de referencia.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); cada operacion se redondea y los valores guardados con fstp dword se
redondean a float.  rand() del CRT del EXE: semilla * 0x343FD + 0x269EC3, resultado (semilla >> 16) & 0x7FFF; srand fija la semilla.

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
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 84001) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        w = rand_fix(seed, i // 4)
        out += w.to_bytes(4, 'little')[:min(4, nbytes - i)]
    return out


SLOTS, NODES_PER, LINKS_PER, NAVI_PER, NAVNODES_PER, NODE_BYTES, LINK_BYTES = 3, 4, 16, 16, 8, 0x1C, 14
VEH_WIN = 0x5A0
SIZES = dict(veh=GUARD * 2 + VEH_WIN, node=GUARD * 2 + SLOTS * NODES_PER * NODE_BYTES, nl=GUARD * 2 + SLOTS * LINKS_PER * 4, navi=GUARD * 2 + SLOTS * NAVI_PER * 2,
             nn=GUARD * 2 + SLOTS * NAVNODES_PER * LINK_BYTES)
SEED_ADD = dict(veh=1, node=2, nl=3, navi=4, nn=5)


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


VEH_MASK = ((0x36, 1), (0xB8, 1), (0x45E, 2), (0x4A4, 1), (0x390, 12), (0x39C, 4), (0x3A0, 4), (0x3A4, 6), (0x3AC, 8), (0x3B4, 5), (0x3B9, 3), (0x3BC, 4), (0x3C8, 4), (0x3DB, 1), (0x3E0, 1))
FIELDS = dict(m36=(0x36, 1), mb8=(0xB8, 1), m45e=(0x45E, 2), m4a4=(0x4A4, 1), m390=(0x390, 4), m394=(0x394, 4), m398=(0x398, 4), m39c=(0x39C, 4), m3a0=(0x3A0, 4), m3a4=(0x3A4, 2), m3a6=(0x3A6, 2),
              m3a8=(0x3A8, 2), m3ac=(0x3AC, 4), m3b0=(0x3B0, 4), m3b4=(0x3B4, 1), m3b5=(0x3B5, 1), m3b6=(0x3B6, 1), m3b7=(0x3B7, 1), m3b8=(0x3B8, 1), m3b9=(0x3B9, 1), m3ba=(0x3BA, 1), m3bb=(0x3BB, 1),
              m3bc=(0x3BC, 4), m3c8=(0x3C8, 4), m3db=(0x3DB, 1), m3e0=(0x3E0, 1))

MUT_SPECIAL = (0, 0x80000000, 1, 0x7F800000, 0xFF800000, 0xFFC00000, 0x7F7FFFFF, 0x3F800000)
K001, K0125, K54, K256, K1000, K05, KWIDTH = 0x3C23D70A, 0x3E000000, 0x40ACCCCD, 0x43800000, 0x447A0000, 0x3F000000, 0x3C3DA12F


def mut_float(v):
    if v & 3 == 0:
        return MUT_SPECIAL[(v >> 2) % 8]
    import struct
    return struct.unpack('<I', struct.pack('<f', float((v % 2001) - 1000) / 8.0))[0]


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
        v = self.veh
        v.put(0, 4, 0)
        for key, off, size in (('seed16', 0x45E, 2), ('s36', 0x36, 1), ('b8', 0xB8, 1), ('created', 0x4A4, 1), ('cur', 0x390, 4), ('start', 0x394, 4), ('end0', 0x398, 4), ('field_c', 0x39C, 4), ('scale', 0x3A0, 4),
                               ('cur_link', 0x3A4, 2), ('next_link', 0x3A6, 2), ('prev_link', 0x3A8, 2), ('t3ac', 0x3AC, 4), ('t3b0', 0x3B0, 4), ('sm4', 0x3B4, 1), ('sm5', 0x3B5, 1), ('sm6', 0x3B6, 1),
                               ('lane7', 0x3B7, 1), ('lane8', 0x3B8, 1), ('style', 0x3B9, 1), ('mission', 0x3BA, 1), ('tempact', 0x3BB, 1), ('temp_time', 0x3BC, 4), ('speed', 0x3C8, 4), ('f3db', 0x3DB, 1),
                               ('f50', 0x3E0, 1)):
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
        for m in (self.veh, self.node, self.nl, self.navi, self.nn):
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
    if kind == 1:
        veh.put(0x36, 1, v)
    elif kind == 2:
        veh.put(0x390, 4, mutation_addr(w, v))
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
        veh.put(0x394, 4, mutation_addr(w, v))
    elif kind == 9:
        veh.put(0x3C8, 4, mut_float(v))
    elif kind == 10:
        veh.put(0x3E0, 1, v)
    elif kind == 11:
        veh.put(0x39C, 4, v)
    elif kind == 12:
        veh.put(0x3A0, 4, v)
    elif kind == 13:
        veh.put(0x3DB, 1, v)
    elif kind == 14:
        veh.put(0x4A4, 1, v)
    elif kind == 15:
        st['time'] = v & M32
    elif kind in (16, 17):
        a = veh.get(0x394, 4)
        s = w.slot(a & 0xFFFF)
        if s is not None and (a >> 16) < NODES_PER:
            w.node.put((s * NODES_PER + (a >> 16)) * NODE_BYTES + (0x1A if kind == 16 else 0x18), 1, v)
    elif kind == 18:
        veh.put(0x398, 4, v)
    elif kind == 19:
        veh.put(0x3B9, 1, v)
        veh.put(0x3BA, 1, v >> 8)
        veh.put(0x3BB, 1, v >> 16)
    elif kind == 20:
        veh.put(0x3A8, 2, v)
        veh.put(0x3AC, 4, v)


def lane_offset(flags, width):
    """0x44DB00 (CCarPathLink::OneWayLaneOffset): x87 a 24 bits"""
    opp = flags & 7
    half = load(K05)
    if opp == 0:
        return sub(half, mul(fromint((flags >> 3) & 7), half))
    if flags & 0x38 == 0:
        return sub(half, mul(fromint(opp), half))
    return add(mul(fromint(width), load(KWIDTH)), half)


# ---------------------------------------------------------------- modelo de PickNextNodeRandomly (0x42DE80..0x42EC81)
def expected(c):
    """devuelve dict(v, changes, node_changes, events, statics, time, rand_state, marks)"""
    w = World(c)
    veh = w.veh
    st = dict(time=c['now'], rs=c['rseed'] & M32, ev=0, fpd=0, ian=0, statics=[x & M32 for x in c['st']], events=[], marks=[])
    marks = st['marks']

    def mark(m):
        marks.append(m)

    def now():
        return st['time']

    def after():
        k = st['ev']
        st['ev'] += 1
        if k == c['mut_at']:
            mark('mut_applied')
            mark('mut_kind_%d' % c['mut_kind'])
            apply_mut(w, st, c['mut_kind'], c['mut_val'])

    def rand():
        st['rs'] = (st['rs'] * 0x343FD + 0x269EC3) & M32
        return (st['rs'] >> 16) & 0x7FFF

    def ev(kind, *vals):
        st['events'].append([kind] + [x & M32 for x in vals])

    def g32(o):
        return veh.get(o, 4)

    def finish():
        return result(w, st)

    seed16 = c['seed16'] & 0xFFFF
    if seed16:
        st['rs'] = seed16
        mark('seed_srand')
    cur0 = g32(0x390)
    if not w.loaded(cur0 & 0xFFFF):
        mark('early_cur')
        return finish()
    nxt = g32(0x394)
    if not w.loaded(nxt & 0xFFFF):
        mark('early_next')
        return finish()
    entry = veh.get(0x3A6, 2)
    if not w.loaded(entry >> 10):
        mark('early_link')
        return finish()
    mark('entered')
    nn = w.node.get(w.node_off(nxt) + 0x18, 1) & 0xF
    num_links = nn
    lo = w.link_off(entry)
    lflags = w.nn.get(lo + 0xB, 1)
    to_next = w.nn.get(lo + 4, 2) == (nxt & 0xFFFF) and w.nn.get(lo + 6, 2) == (nxt >> 16)
    if to_next:
        lanes = lflags & 7
        one_way = (lflags & 0x38) == 0
        mark('entry_to_next')
    else:
        lanes = (lflags >> 3) & 7
        one_way = (lflags & 7) == 0
    mark('one_way' if one_way else 'two_way')
    lane8 = s8(veh.get(0x3B8, 1))
    mask = 0
    if lane8 == 0:
        mask = 4
    if lane8 == lanes - 1:
        mask |= 2
    if lanes < 3 or mask == 0:
        mask |= 1
    mark('mask_%d' % mask if mask in (1, 2, 4) else 'mask_mixed')
    veh.put(0x398, 4, g32(0x390))
    veh.put(0x390, 4, g32(0x394))
    ev(2, 0, g32(0x390), g32(0x394), g32(0x398), veh.get(0x36, 1), now())
    r = c['rtnt'] & 0xFF
    after()
    if r:
        mask = 1
        mark('turn_true')
    ev(1, num_links, now())
    st['statics'][2] = num_links & M32
    st['statics'][1] = c['seq_off'] & M32
    st['statics'][0] = (st['statics'][0] & 0xFFFFFF00) | (c['seq_other'] & 0xFF)
    after()
    if num_links == 0:
        mark('n_zero')

    def fse(i):
        length = s32(st['statics'][2])
        offset = st['statics'][1]
        if st['statics'][0] & 0xFF:
            numerator = (i + offset) & M32
            mark('seq_other')
        else:
            numerator = (length - i + offset) & M32
        n = s32(numerator)
        q = abs(n) % abs(length)
        return -q if n < 0 else q

    def base_index(i):
        return s16(w.node.get(w.node_off(nxt) + 0x10, 2)) + fse(i)

    def cand_at(index):
        s = w.slot(nxt & 0xFFFF)
        return w.nl.get((s * LINKS_PER + index) * 4, 4), s

    def navi_at(index):
        s = w.slot(nxt & 0xFFFF)
        return w.navi.get((s * NAVI_PER + index) * 2, 2)

    def bad_link(word):
        o = w.link_off(word)
        f = w.nn.get(o + 0xB, 1)
        if w.nn.get(o + 4, 2) == (nxt & 0xFFFF) and w.nn.get(o + 6, 2) == (nxt >> 16):
            return (f & 0x38) == 0
        return (f & 7) == 0

    chosen = None
    for i in range(num_links):
        index = base_index(i)
        cand, _ = cand_at(index)
        veh.put(0x394, 4, cand)
        if not w.loaded(cand & 0xFFFF):
            mark('l1_cand_unloaded')
            continue
        k = st['fpd']
        st['fpd'] += 1
        kk = min(k, 15)
        ev(3, cur0, g32(0x390), cand, 1, g32(0x394), g32(0x390), veh.get(0x36, 1), now())
        reversed_ = c['fpd_out'][kk] & 0xFF
        direction = c['fpd'][kk] & 0xFF
        after()
        link = navi_at(index)
        if not w.loaded(link >> 10):
            mark('l1_link_unloaded')
            continue
        if (veh.get(0x36, 1) & 0xF8) == 0x10 and reversed_:
            mark('l1_reversed_skip')
            continue
        lo = w.link_off(link)
        f = w.nn.get(lo + 0xB, 1)
        tn = w.nn.get(lo + 4, 2) == (nxt & 0xFFFF) and w.nn.get(lo + 6, 2) == (nxt >> 16)
        first_free = (f & 0x38) == 0 if tn else (f & 7) == 0
        second_free = (f & 7) == 0 if tn else (f & 0x38) == 0
        k2 = st['ian']
        st['ian'] += 1
        kk2 = min(k2, 15)
        ev(4, 0, cur0, nxt, cand, 1 if first_free else 0, g32(0x394), now())
        ok = c['ian'][kk2] & 0xFF
        after()
        if not ok:
            mark('l1_ian_false')
            continue
        if not (direction & mask):
            mark('l1_mask_fail')
            continue
        if not one_way or not second_free:
            chosen = link
            mark('loop1_success')
            break
        mark('l1_oneway_skip')
    if chosen is None:
        for i in range(num_links):
            index = base_index(i)
            cand, _ = cand_at(index)
            veh.put(0x394, 4, cand)
            if not w.loaded(cand & 0xFFFF):
                continue
            link = navi_at(index)
            if not w.loaded(link >> 10):
                continue
            bad = bad_link(link)
            if (cand & 0xFFFF) == (cur0 & 0xFFFF) and (cand >> 16) == (cur0 >> 16):
                mark('l2_same_as_previous')
                continue
            if bad:
                mark('l2_bad')
                continue
            cb = w.node.get(w.node_off(cand) + 0x18, 1)
            pb = w.node.get(w.node_off(cur0) + 0x18, 1)
            if not (cb & 0x20) or (pb & 0x20):
                chosen = link
                mark('loop2_success')
                break
            mark('l2_bit20')
    if chosen is None:
        for i in range(num_links):
            index = base_index(i)
            cand, _ = cand_at(index)
            veh.put(0x394, 4, cand)
            if not w.loaded(cand & 0xFFFF):
                continue
            link = navi_at(index)
            if not w.loaded(link >> 10):
                continue
            bad = bad_link(link)
            if (cand & 0xFFFF) == (cur0 & 0xFFFF) and (cand >> 16) == (cur0 >> 16):
                continue
            if not bad:
                chosen = link
                mark('loop3_success')
                break
            mark('l3_bad')
    if chosen is None:
        veh.put(0x394, 4, cur0)
        chosen = veh.get(0x3A6, 2)
        mark('fallback')
    # volver al nodo anterior
    start = g32(0x394)
    if (cur0 & 0xFFFF) == (start & 0xFFFF) and (cur0 >> 16) == (start >> 16):
        mark('uturn')
        if (veh.get(0x36, 1) & 0xF8) != 0x18:
            veh.put(0x36, 1, (veh.get(0x36, 1) & 7) | 0x18)
            veh.put(0x3BB, 1, 0)
            veh.put(0x3AC, 4, now() + 0x7D0)
            veh.put(0x3B0, 4, now())
            veh.put(0xB8, 1, 0)
            mark('uturn_set')
    start = g32(0x394)
    kind = w.node.get(w.node_off(start) + 0x1A, 1) >> 4
    if kind == 10:
        mark('kind10')
        status = veh.get(0x36, 1)
        veh.put(0x3BB, 1, 1)
        veh.put(0x3BC, 4, now() + 0x2710)
        if (status & 0xF8) == 0x10:
            ev(5, 0x390, 0, now())
            mark('modspeed')
            after()
    elif kind in (1, 2):
        mark('kind%d' % kind)
        veh.put(0x36, 1, (veh.get(0x36, 1) & 7) | 0x18)
        veh.put(0x3BB, 1, 0)
        veh.put(0x3AC, 4, now() + 0x7D0)
        stamp = now()
        veh.put(0x3BA, 1, 0x2D if kind == 2 else 0x2E)
        veh.put(0x3B0, 4, stamp)
        veh.put(0xB8, 1, 0)
        veh.put(0x3B9, 1, 0)
    current = g32(0x390)
    if (w.node.get(w.node_off(current) + 0x1A, 1) & 0xF0) == 0x90:
        if veh.get(0x4A4, 1) != 2:
            mark('kind9')
            status = veh.get(0x36, 1)
            veh.put(0x3BB, 1, 1)
            veh.put(0x3BC, 4, now() + 0x1194)
            if (status & 0xF8) == 0x10:
                ev(5, 0x390, 0, now())
                mark('modspeed')
                after()
        else:
            mark('kind9_mission')
    # desplazamiento de enlaces
    cur_old = veh.get(0x3A4, 2)
    scale = g32(0x3A0)
    next_old = veh.get(0x3A6, 2)
    total = (g32(0x39C) + scale) & M32
    sign_cur = veh.get(0x3B5, 1)
    veh.put(0x3A8, 2, cur_old)
    sign_next = veh.get(0x3B6, 1)
    veh.put(0x3A4, 2, next_old)
    next_lane = veh.get(0x3B8, 1)
    veh.put(0x3B5, 1, sign_next)
    veh.put(0x39C, 4, total)
    veh.put(0x3B4, 1, sign_cur)
    veh.put(0x3B7, 1, next_lane)
    veh.put(0x3A6, 2, chosen)
    start = g32(0x394)
    below = (nxt & 0xFFFF) < (start & 0xFFFF) or ((nxt & 0xFFFF) == (start & 0xFFFF) and (nxt >> 16) < (start >> 16))
    veh.put(0x3B6, 1, 0xFF if below else 1)
    mark('below' if below else 'not_below')
    if not w.loaded(chosen >> 10):
        lanes_next = 1
        mark('lanes_unloaded')
    else:
        f = w.nn.get(w.link_off(chosen) + 0xB, 1)
        lanes_next = (f >> 3) & 7 if below else f & 7
    # direcciones
    sc = fromint(s8(veh.get(0x3B5, 1)))
    sn = fromint(s8(veh.get(0x3B6, 1)))
    cl = w.link_off(veh.get(0x3A4, 2))
    nl_ = w.link_off(chosen)

    def dir_(off, scale_):
        return mul(mul(fromint(s8(w.nn.get(off, 1))), load(K001)), scale_)
    cdx, cdy = dir_(cl + 8, sc), dir_(cl + 9, sc)
    ndx, ndy = dir_(nl_ + 8, sn), dir_(nl_ + 9, sn)

    def stored(a):
        return reload(store(a))
    cdx, cdy, ndx, ndy = stored(cdx), stored(cdy), stored(ndx), stored(ndy)
    start = g32(0x394)
    current = g32(0x390)
    so, co = w.node_off(start), w.node_off(current)

    def pos(off):
        return mul(fromint(s16(w.node.get(off, 2))), load(K0125))
    dx = sub(pos(so + 8), pos(co + 8))
    dy = sub(pos(so + 0xA), pos(co + 0xA))
    d2 = add(mul(dx, dx), mul(dy, dy))
    if gt(d2, load(K256)):
        mark('dist_far')
        counter = (veh.get(0x3E0, 1) - 1) & 0xFF
        veh.put(0x3E0, 1, counter)
        if counter == 0:
            mark('counter_zero')
            veh.put(0x3E0, 1, (rand() & 3) + 4)
            rnd_ = rand()
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
    if not (lane < lanes_next - 1):
        lane = lanes_next - 1
    lane = s8(lane)
    veh.put(0x3B8, 1, lane if lane > 0 else 0)
    flags = veh.get(0x3DB, 1)
    if flags & 8:
        veh.put(0x3B8, 1, 0)
        mark('flag8')
    elif flags & 0x10:
        last = lanes_next - 1
        veh.put(0x3B8, 1, 0 if last < 0 else last)
        mark('flag10')
    if (veh.get(0x36, 1) & 0xF8) != 0x10:
        mark('status_not2')
        return finish()
    mark('status2')
    off_cur = lane_offset(w.nn.get(cl + 0xB, 1), w.nn.get(cl + 0xA, 1))
    width_cur = stored(mul(add(off_cur, fromint(s8(veh.get(0x3B7, 1)))), load(K54)))
    off_next = lane_offset(w.nn.get(nl_ + 0xB, 1), w.nn.get(nl_ + 0xA, 1))
    width_next = mul(add(off_next, fromint(s8(veh.get(0x3B8, 1)))), load(K54))

    def lpos(off, k):
        return mul(fromint(s16(w.nn.get(off + k, 2))), load(K0125))
    ay = stored(sub(lpos(nl_, 2), mul(width_next, ndx)))
    ax = stored(add(lpos(nl_, 0), mul(width_next, ndy)))
    sy = stored(sub(lpos(cl, 2), mul(width_cur, cdx)))
    sx = stored(add(lpos(cl, 0), mul(width_cur, cdy)))
    ev(6, store(sx), store(sy), 0, store(ax), store(ay), 0, store(cdx), store(cdy), store(ndx), store(ndy), now())
    ratio = load(c['csf'])
    after()
    speed = load(veh.get(0x3C8, 4))
    quotient = div(load(K1000), speed)
    value = ftol2(mul(ratio, quotient))
    if c['csf'] & 0x7F800000 == 0x7F800000:
        mark('csf_special')
    if veh.get(0x3C8, 4) & 0x7F800000 in (0, 0x7F800000):
        mark('speed_special')
    veh.put(0x3A0, 4, value & M32)
    if not (value > 10):
        value = 10
        mark('scale_clamped')
    veh.put(0x3A0, 4, value & M32)
    return finish()


def result(w, st):
    veh = w.veh
    v = {k: veh.get(o, s) for k, (o, s) in FIELDS.items()}
    return dict(v=v, changes=veh.changes(VEH_MASK), node_changes=[w.node.changes(), w.nl.changes(), w.navi.changes(), w.nn.changes()], events=st['events'], statics=st['statics'],
                time=st['time'] & M32, rand_state=st['rs'], marks=st['marks'])


def check_side(c, o, e):
    for k, val in e['v'].items():
        if o['v'].get(k) != val:
            return 'v.%s: %s != %s' % (k, o['v'].get(k), val)
    if o['changes'] != e['changes']:
        return 'changes del vehiculo: %s != %s' % (o['changes'][:6], e['changes'][:6])
    if o['node_changes'] != e['node_changes']:
        return 'changes del mundo: %s != %s' % (o['node_changes'], e['node_changes'])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o['statics'] != e['statics']:
        return 'estaticas %s != %s' % (o['statics'], e['statics'])
    if o['time'] != e['time']:
        return 'reloj %s != %s' % (o['time'], e['time'])
    if o['rand_state'] != e['rand_state']:
        return 'rand %s != %s' % (o['rand_state'], e['rand_state'])
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
