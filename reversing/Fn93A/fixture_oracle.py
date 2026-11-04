"""Oraculo independiente (Python) de Fn93A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de cada funcion del lote 93A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas
y la memoria con arenas rellenas con el mismo generador que el banco (Fixtures.hpp: Mix/Rand/FillWords), de modo que los bytes cambiados se predicen exactos (los bytes escritos con el mismo valor no cuentan).

Funcion 0, CCarPathLink::OneWayLaneOffset (0x44DB00; thiscall, ECX = enlace, `ret`; float en ST0): opp = bits 0..2 y same = bits 3..5 del byte +0xB.
  opp == 0: 0.5 - same * 0.5; si no, same == 0: 0.5 - opp * 0.5; si no: (byte +0xA * 0.011574074) + 0.5 (0x858EEC; cada operacion redondeada a 24 bits).
Funcion 1, CPathFind::RemoveBadStartNode (0x44E4F0; thiscall, `ret 14h`; void): contador (palabra con signo) < 2: nada; areas de los dos primeros nodos de la ruta cargadas (m_pPathNodes[area] != 0, +0x804);
  t1 = float(y1 - py); (y0 - py) * t1 + (x1 - px) * (x0 - px) < 0 (ordenado): contador - 1 (palabra) y nodos[i] = nodos[i + 1] para i < contador nuevo.
Funcion 2, CPathFind::StoreInteriorNodeLink (0x450FB0; cdecl, 6 argumentos): enlace[contador] = (area B, nodo B) en m_pNodeLinks[ranura + 0x40]; longitud = max(1, trunc(min(255, dist))) con dist = sqrt((dz^2 + dy^2) + dx^2)
  entre el nodo A (m_pPathNodes[a1], indice de byte con signo a2) y el B (m_pPathNodes[a3], a4); interseccion = 0; nibble bajo de las banderas (+0x18) del nodo (m_pPathNodes[ranura + 0x40], el mismo indice de A) + 1 sin llevarse; contador + 1.
Funcion 3, CPathFind::CalcDistToAnyConnectingLinks (0x44F190; thiscall, `ret 10h`; float en ST0): mejor = 1.0e6 (0x497423FE); por cada enlace (nibble bajo de +0x18; enlace m_pNodeLinks[area][base + i]):
  area destino cargada -> DistToLineSqr(nodo, destino, pos) (falso); si no (mejor < resultado) ordenado, mejor = resultado. Devuelve sqrt(mejor).
Funcion 4, CPathFind::FindNodeClosestInRegion (0x44F2C0; thiscall, `ret 2Ch`; void): area sin nodos: nada; tipo (byte) 0: nodos [0, vehiculos); 1: [vehiculos, total); otro: nada. Salta (bit 0x20 de +0x18 con bLowTraffic),
  (bit 2 de +0x19 con bUnkn) y los de bit de agua distinto de bBoats (byte). puntuacion D = (((|z - pz| * 3) + |y - py|) + |x - px|) * 0.3 (registro); si D < *dist: r = CalcDist (falso), E = r * 0.2 + float(D);
  si E < *dist: *dist = float(E), *out = (area, indice).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

NAN = ('n',)


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


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / jne): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jp: cero solo si es igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero aqui)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]





M32 = 0xFFFFFFFF
GUARD = 0x40
K_HALF = load(0x3F000000)
K_WIDTH = load(0x3C3DA12F)       # 0.011574074 (0x858EEC)
K255 = load(0x437F0000)
K3 = load(0x40400000)
K03 = load(0x3E99999A)           # 0.3f (0x858C24)
K02 = load(0x3E4CCCCD)           # 0.2f (0x858CC4)
K1E6 = load(0x497423FE)          # 1.0e6 aproximado del EXE (0x497423FE)
ZERO_F = load(0)
BANK_WIN = 0x1C * 12
WINS = {
    0: [0x10],
    1: [0x940, 0x40, 0x10] + [BANK_WIN] * 3,
    2: [BANK_WIN] * 3 + [0xA0, 0x30, 0x30, 0x10],
    3: [0xB70, 0x40, 0xA0] + [BANK_WIN] * 3,
    4: [0x1200, 0x10, 0x10] + [BANK_WIN] * 3,
}
TGT_AREA = (3, 30)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def lt(a, b):
    return gt(b, a)


def fabs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s16(v):
    v &= 0xFFFF
    return v - 65536 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


# ---------------------------------------------------------------------------------------------- memoria: arenas con el mismo relleno que el banco (Fixtures.hpp: Mix / Rand / FillWords)
def mix(x):
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand32(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 93001) & M32)


class Arena:
    def __init__(self, win, seed):
        self.win = win
        n = GUARD + win + GUARD
        data = bytearray()
        for i in range(0, n, 4):
            data += (rand32(seed, i // 4)).to_bytes(4, 'little')
        self.m = data[:n]

    def put(self, off, size, v):
        v &= (1 << (8 * size)) - 1
        self.m[GUARD + off:GUARD + off + size] = v.to_bytes(size, 'little')

    def get(self, off, size):
        return int.from_bytes(self.m[GUARD + off:GUARD + off + size], 'little')

    def snap(self):
        self.r = bytearray(self.m)


def build(fn, c):
    sd = (c['seed'] * 13 + 50) & M32
    arenas = []
    for w in WINS[fn]:
        arenas.append(Arena(w, sd))
        sd = (sd + 1) & M32
    return arenas


def put_nodes(banks, c, count):
    np = c['np']
    per_bank = 8 * 3
    for b in range(3):
        for i in range(count):
            for k in range(3):
                banks[b].put(i * 0x1C + 8 + 2 * k, 2, np[b * per_bank + i * 3 + k] if count == 8 else np[(b * 12 + i) * 3 + k])


def raw(v):
    """int16 crudo / 8 como float exacto del x87"""
    v = s16(v)
    return ('f', Fraction(v, 8), 0)


def changes_of(arenas):
    out = []
    for idx, a in enumerate(arenas):
        for i in range(len(a.m)):
            if a.m[i] != a.r[i]:
                out.append([idx, i - GUARD, a.m[i]])
    return out


def ext80(a):
    """ST0 del x87 (imagen FNSAVE, 10 bytes) de un valor del modelo"""
    if a == NAN:
        return bytes([0, 0, 0, 0, 0, 0, 0, 0xC0, 0xFF, 0xFF]).hex()
    if a[0] == 'i':
        return (bytes(7) + bytes([0x80]) + (0x7FFF | (a[1] << 15)).to_bytes(2, 'little')).hex()
    v, neg = a[1], a[2]
    if v == 0:
        return (bytes(8) + (neg << 15).to_bytes(2, 'little')).hex()
    mag = abs(v)
    sign = 1 if v < 0 else 0
    e = mag.numerator.bit_length() - mag.denominator.bit_length()
    if Fraction(2) ** e > mag:
        e -= 1
    elif Fraction(2) ** (e + 1) <= mag:
        e += 1
    mant = mag / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return (int(mant).to_bytes(8, 'little') + ((e + 16383) | (sign << 15)).to_bytes(2, 'little')).hex()


# ---------------------------------------------------------------------------------------------- funcion 0: CCarPathLink::OneWayLaneOffset
def expected0(c):
    marks = []
    lanes = c['lanes'] & 0xFF
    opp, same = lanes & 7, (lanes >> 3) & 7
    if opp == 0:
        marks.append('opp0')
        marks.append('same%d' % same)
        v = sub(K_HALF, mul(load(float_bits(same)), K_HALF))
    elif lanes & 0x38 == 0:
        marks.append('same0')
        v = sub(K_HALF, mul(load(float_bits(opp)), K_HALF))
    else:
        marks.append('width')
        w = c['width'] & 0xFF
        marks.append('width0' if w == 0 else ('width255' if w == 255 else 'widthmid'))
        v = add(mul(load(float_bits(w)), K_WIDTH), K_HALF)
    if lanes & 0xC0:
        marks.append('high_bits')
    return {'changes': [], 'events': [], 'st0': ext80(v), 'marks': marks}


def float_bits(i):
    import struct
    return struct.unpack('<I', struct.pack('<f', float(i)))[0]


# ---------------------------------------------------------------------------------------------- funcion 1: CPathFind::RemoveBadStartNode
def slot_banks(assign):
    """asignaciones (area, banco o None) en orden: la ultima gana"""
    m = {}
    for area, bank in assign:
        m[area] = bank
    return m


def expected1(c):
    marks = []
    arenas = build(1, c)
    this_a, nodes_a, count_a = arenas[0], arenas[1], arenas[2]
    banks = arenas[3:]
    put_nodes(banks, c, 8)
    a0, a1 = c['areaof'][0], c['areaof'][1]
    slots = slot_banks([(a0, c['bank0'] if c['loaded0'] else None), (a1, c['bank1'] if c['loaded1'] else None)])
    for area, bank in slots.items():
        this_a.put(0x804 + 4 * area, 4, 0 if bank is None else tag_ptr(1, bank))   # los punteros reales no importan: solo si es nulo
    for k in range(16):
        nodes_a.put(4 * k, 4, (c['areaof'][k] & 0xFFFF) | (c['idof'][k] << 16))
    count_a.put(0, 4, c['cnt'])
    for a in arenas:
        a.snap()
    ev = []
    cnt = s16(c['cnt'])
    if cnt < 2:
        marks.append('count_lt2')
        return {'changes': [], 'events': ev, 'marks': marks}
    if slots[a0] is None:
        marks.append('area0_unloaded')
        return {'changes': [], 'events': ev, 'marks': marks}
    if slots[a1] is None:
        marks.append('area1_unloaded')
        return {'changes': [], 'events': ev, 'marks': marks}
    n0, n1 = c['idof'][0], c['idof'][1]
    b0, b1 = banks[slots[a0]], banks[slots[a1]]
    x0, y0 = raw(b0.get(n0 * 0x1C + 8, 2)), raw(b0.get(n0 * 0x1C + 10, 2))
    x1, y1 = raw(b1.get(n1 * 0x1C + 8, 2)), raw(b1.get(n1 * 0x1C + 10, 2))
    px, py = load(c['pos'][0] & M32), load(c['pos'][1] & M32)
    t1 = reload(bits(sub(y1, py)))
    a = mul(sub(y0, py), t1)
    b = mul(sub(x1, px), sub(x0, px))
    s = add(a, b)
    if s == NAN:
        marks.append('nan')
    if not lt(s, ZERO_F):
        marks.append('not_negative')
        return {'changes': [], 'events': ev, 'marks': marks}
    marks.append('removed')
    marks.append('count%d' % cnt)
    remaining = cnt - 1
    count_a.put(0, 2, remaining & 0xFFFF)
    for i in range(remaining):
        nodes_a.put(4 * i, 4, nodes_a.get(4 * (i + 1), 4))
    return {'changes': changes_of(arenas), 'events': ev, 'marks': marks}


def tag_ptr(arena, bank):
    return 0x70000000 + bank   # solo no nulo


# ---------------------------------------------------------------------------------------------- funcion 2: CPathFind::StoreInteriorNodeLink
def expected2(c):
    marks = []
    arenas = build(2, c)
    banks = arenas[0:3]
    links, lens, ints, ctr = arenas[3], arenas[4], arenas[5], arenas[6]
    put_nodes(banks, c, 8)
    idx5 = 0x40 + c['a5']
    slots = slot_banks([(c['a1'], c['bankA']), (c['a3'], c['bankB']), (idx5, c['bankF'])])
    ia = s8(c['a2'])
    banks[c['bankF']].put(ia * 0x1C + 0x18, 1, c['flaginit'])
    ctr.put(0, 4, c['ctr'])
    for a in arenas:
        a.snap()
    eff_a, eff_b, eff_f = banks[slots[c['a1']]], banks[slots[c['a3']]], banks[slots[idx5]]
    n = c['ctr']
    a4 = c['a4']
    links.put(4 * n, 2, c['a3'])
    links.put(4 * n + 2, 2, a4)
    ax, ay, az = [raw(eff_a.get(ia * 0x1C + 8 + 2 * k, 2)) for k in range(3)]
    bx, by, bz = [raw(eff_b.get(a4 * 0x1C + 8 + 2 * k, 2)) for k in range(3)]
    dx, dy, dz = sub(ax, bx), sub(ay, by), sub(az, bz)
    d = sqrt24(add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx)))
    if gt(d, K255):
        marks.append('clamped')
        d = K255
    whole = int(d[1]) if d[0] == 'f' else 0
    al = whole & 0xFF
    length = 1 if al < 1 else al
    marks.append('len1' if length == 1 else ('len255' if length == 255 else 'lenmid'))
    if whole == 0:
        marks.append('under1')
    if ia < 0:
        marks.append('negative_index')
    if c['a2'] & 0xFFFFFF00:
        marks.append('index_junk')
    if c['a1'] == c['a3'] and ia == a4:
        marks.append('same_node')
    lens.put(n, 1, length)
    ints.put(n, 1, 0)
    old = eff_f.get(ia * 0x1C + 0x18, 1)
    if old & 0xF == 0xF:
        marks.append('nibble_wrap')
    eff_f.put(ia * 0x1C + 0x18, 1, (old & 0xF0) | ((old + 1) & 0x0F))
    ctr.put(0, 4, (n + 1) & M32)
    return {'changes': changes_of(arenas), 'events': [], 'marks': marks}


# ---------------------------------------------------------------------------------------------- funcion 3: CPathFind::CalcDistToAnyConnectingLinks
def expected3(c):
    marks = []
    arenas = build(3, c)
    this_a, node_a, linkarr = arenas[0], arenas[1], arenas[2]
    banks = arenas[3:]
    put_nodes(banks, c, 8)
    base = s16(c['base'])
    for k in range(3):
        node_a.put(8 + 2 * k, 2, c['np'][(2 * 8 + 7) * 3 + k])
    node_a.put(0x10, 2, c['base']); node_a.put(0x12, 2, c['narea']); node_a.put(0x18, 1, c['nflags'])
    for k in range(16):
        linkarr.put(4 * (base + k), 4, TGT_AREA[c['lkarea'][k] & 1] | (c['lknode'][k] << 16))
    for a in arenas:
        a.snap()
    pos = [c['pos'][k] & M32 for k in range(3)]
    best = K1E6
    calls = 0
    ev = []
    nlinks = c['nflags'] & 0xF
    if nlinks == 0:
        marks.append('no_links')
    from_pos = [raw(c['np'][(2 * 8 + 7) * 3 + k]) for k in range(3)]
    from_bits = [bits(v) for v in from_pos]
    for i in range(nlinks):
        area_idx = c['lkarea'][i] & 1
        if not c['loaded'][area_idx]:
            marks.append('unloaded')
            continue
        marks.append('call')
        bank = c['areabank'][area_idx]
        nid = c['lknode'][i]
        to_bits = [bits(raw(c['np'][(bank * 8 + nid) * 3 + k])) for k in range(3)]
        ev.append([1] + from_bits + to_bits + pos)
        result = load(c['dist'][min(calls, 15)] & M32)
        calls += 1
        if not lt(best, result):
            marks.append('replaced')
            if result == NAN:
                marks.append('nan_result')
            best = result
    marks.append('final_nan' if best == NAN else 'final_ok')
    if best != NAN and best[0] == 'f' and best[1] == 0:
        marks.append('final_zero')
    return {'changes': changes_of(arenas), 'events': ev, 'st0': ext80(sqrt24(best)), 'marks': marks}


# ---------------------------------------------------------------------------------------------- funcion 4: CPathFind::FindNodeClosestInRegion
def expected4(c):
    marks = []
    arenas = build(4, c)
    this_a, out_a, dist_a = arenas[0], arenas[1], arenas[2]
    banks = arenas[3:]
    put_nodes(banks, c, 12)
    bank = banks[c['bank']]
    for i in range(12):
        bank.put(i * 0x1C + 0x18, 1, c['fl18'][i])
        bank.put(i * 0x1C + 0x19, 1, c['fl19'][i])
    out_a.put(0, 4, c['outinit'])
    dist_a.put(0, 4, c['outdist'])
    for a in arenas:
        a.snap()
    ev = []
    if not c['loaded']:
        marks.append('area_unloaded')
        return {'changes': [], 'events': ev, 'marks': marks}
    nv, nn = s32(c['numveh']), s32(c['numnodes'])
    nt = c['ntype'] & 0xFF
    if nt == 0:
        marks.append('type0')
        first, end = 0, nv
    elif nt == 1:
        marks.append('type1')
        first, end = nv, nn
    else:
        marks.append('type_other')
        return {'changes': [], 'events': ev, 'marks': marks}
    pos = [c['pos'][k] & M32 for k in range(3)]
    px, py, pz = load(pos[0]), load(pos[1]), load(pos[2])
    out_dist = load(c['outdist'] & M32)
    out_bits = c['outdist'] & M32
    calls = 0
    i = first
    if not first < end:
        marks.append('empty')
    while i < end:
        f18, f19 = c['fl18'][i] & 0xFF, c['fl19'][i] & 0xFF
        if (c['fllow'] & 0xFF) and f18 & 0x20:
            marks.append('skip_low')
            i += 1
            continue
        if (c['flunk'] & 0xFF) and f19 & 0x02:
            marks.append('skip_unk')
            i += 1
            continue
        if (c['flboat'] & 0xFF) != (f18 >> 7):
            marks.append('skip_boat')
            i += 1
            continue
        marks.append('candidate')
        X, Y, Z = [raw(c['np'][(c['bank'] * 12 + i) * 3 + k]) for k in range(3)]
        zt = mul(fabs(sub(Z, pz)), K3)
        s1 = add(zt, fabs(sub(Y, py)))
        s2 = add(s1, fabs(sub(X, px)))
        D = mul(s2, K03)
        stored = reload(bits(D))
        if not lt(D, out_dist):
            marks.append('score_not_better')
            i += 1
            continue
        marks.append('score_better')
        ev.append([1, tag(0, 0), tag(3 + c['bank'], i * 0x1C)] + pos)
        r = load(c['dist'][min(calls, 15)] & M32)
        calls += 1
        E = add(mul(r, K02), stored)
        if lt(E, out_dist):
            marks.append('improved')
            out_bits = bits(E)
            out_dist = reload(out_bits)
            dist_a.put(0, 4, out_bits)
            out_a.put(0, 2, c['area'])
            out_a.put(2, 2, i)
        else:
            marks.append('total_not_better')
        i += 1
    return {'changes': changes_of(arenas), 'events': ev, 'marks': marks}


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4}[c['fn']](c)


def check_side(c, o, e):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if 'st0' in e and o.get('st0') != e['st0']:
        return 'st0: %s != %s' % (o.get('st0'), e['st0'])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
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
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
