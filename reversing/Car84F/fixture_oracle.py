"""Oraculo independiente (Python) de Car84F: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42B7D0..0x42BC49, lo que debe observarse de
CCarCtrl::GenerateOneEmergencyServicesCar (los eventos de los falsos con todos sus argumentos y el estado del vehiculo en cada uno, el vehiculo devuelto, los bytes cambiados del vehiculo y de su matriz
respecto a la imagen que deja el constructor falso, los contadores de ambulancias y bomberos y el byte del jugador en foco) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Las arenas (nodos, vehiculo, matriz y jugador) se reconstruyen byte a byte con el mismo relleno pseudoaleatorio que Fixtures.hpp (Mix / Rand / FillWords) y las mismas escrituras de las fixtures.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); cada operacion se redondea y los valores guardados con fstp dword se
redondean a float.

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




def rand_fix(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 84006) & M32)


SLOTS, NODES_PER, NODE_BYTES, CALLS = 3, 4, 0x1C, 5
VEH_WIN, MAT_WIN, PLAYER_WIN = 0x988, 0x48, 0x10
SIZES = dict(node=GUARD * 2 + SLOTS * NODES_PER * NODE_BYTES, veh=GUARD * 2 + VEH_WIN, mat=GUARD * 2 + MAT_WIN, player=GUARD * 2 + PLAYER_WIN)
SEED_ADD = dict(node=2, veh=8, mat=9, player=10)
VEH_TAG = 0x10000000 + GUARD          # Tag(objeto de la arena del vehiculo): arena 0 de la tabla, desplazamiento kGuard
K1, K0125, KUNIT = 0x3F800000, 0x3E000000, 0x3F800000
GROUND_NONE = 0x4E6E6B28              # 1e9
RAD, A4, A67 = 0x3F34FDF4, 0xBF800000, 0x43200000


class Mem:
    """arena con guarda: los desplazamientos son relativos al objeto (ventana)"""

    def __init__(self, name, seed):
        self.b = fill_words(SIZES[name], (seed * 13 + SEED_ADD[name]) & M32)
        self.ref = bytes(self.b)

    def put(self, off, size, v):
        self.b[GUARD + off:GUARD + off + size] = (v & ((1 << (8 * size)) - 1)).to_bytes(size, 'little')

    def get(self, off, size):
        return int.from_bytes(self.b[GUARD + off:GUARD + off + size], 'little')

    def snap(self):
        self.ref = bytes(self.b)

    def changes(self):
        return [[i - GUARD, self.b[i]] for i in range(len(self.b)) if self.b[i] != self.ref[i]]


class Unsafe(Exception):
    """la fixture haria leer al EXE fuera de las tablas del banco"""


def quiet(bits):
    """fld dword + fstp dword: un NaN senalizador sale silenciado"""
    return (bits | 0x00400000) if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x007FFFFF) else bits


def isnan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x007FFFFF) != 0


def expected(c):
    marks = []
    events = []

    def mark(name):
        marks.append(name)

    def ev(kind, *vals):
        events.append([kind] + [v & M32 for v in vals])

    mark('entered')
    veh = Mem('veh', c['seed'])
    mat = Mem('mat', c['seed'])
    node = Mem('node', c['seed'])
    player = Mem('player', c['seed'])
    S = dict(amb=c['amb'] & M32, fire=c['fire'] & M32)
    areas = c['areas']
    for i in range(SLOTS * NODES_PER):
        node.put(i * NODE_BYTES + 0xC, 2, c['node_z'][i])
        node.put(i * NODE_BYTES + 0x18, 1, c['node_flag'][i])
    for k in range(3):
        player.put(4 * k, 4, c['player'][k])
    for m in (veh, mat, node, player):
        m.snap()

    def slot_of(addr):
        a, n = addr & 0xFFFF, addr >> 16
        if a not in areas or n >= NODES_PER:
            raise Unsafe('nodo fuera de las ranuras: %x' % addr)
        return areas.index(a) * NODES_PER + n

    def snap_vals():
        v = [mat.get(o, 4) for o in (0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38)]
        v += [veh.get(0x44, 4), veh.get(0x48, 4), veh.get(0x4C, 4), veh.get(0x428, 1), veh.get(0x42D, 1), S['amb'], S['fire']]
        return v

    def result(ret):
        return dict(ret=ret, veh_changes=veh.changes(), mat_changes=mat.changes(), node_changes=node.changes(), player_changes=player.changes(), events=events,
                    statics=[S['amb'] & M32, S['fire'] & M32], pif=c['pif'] & 0xFF, marks=marks)

    # ---- primera llamada del EXE: jugador (0x56E250 con el byte 0xB7CD74)
    ev(1, c['pif'] & 0xFF)
    pl = [c['player'][0], c['player'][1], c['player'][2]]
    n1w, n2w = 0xFFFF, 0xFFFF            # solo las areas: las palabras de nodo no se leen antes de la primera llamada
    org = None
    frac = 0
    found = False
    attempts = 0
    gen = 0
    fo = 0
    k_found = 0
    while True:
        if attempts >= 5:
            mark('top_fail')
            return result(0)
        found = False
        k = min(gen, CALLS - 1)
        ev(2, gen, pl[0], pl[1], pl[2], RAD, RAD, A4, 1, A67, A67, 0, 0, n1w & 0xFFFF, n2w & 0xFFFF)
        gen += 1
        org = [c['gorg'][3 * k], c['gorg'][3 * k + 1], c['gorg'][3 * k + 2]]
        n1w, n2w, frac = c['gn1'][k], c['gn2'][k], c['gfrac'][k]
        if c['gret'][k] & 1:
            idx = slot_of(n1w)
            flag = node.get(idx * NODE_BYTES + 0x18, 1)
            if not flag & 0x80:
                kf = min(fo, CALLS - 1)
                ev(3, fo, org[0], org[1], org[2], 0x41200000, 1, 2, 0, 0, 1, 1, 0, 0)
                fo += 1
                count = s16(c['cnt'][kf])
                if count == 0:
                    found = True
                    k_found = attempts
                    mark('count_zero')
                else:
                    mark('count_nonzero')
                    if (c['cnt'][kf] & 0xFF) == 0:
                        mark('count_low_byte_zero')
            else:
                mark('water_skip')
        else:
            mark('gen_false')
        attempts += 1
        if found:
            break
    if attempts >= 5:
        mark('fifth_fail')
        return result(0)
    mark('found_at_%d' % k_found)
    # ---- reserva y constructor (falsos)
    ev(5, 0x988)
    ev(6, VEH_TAG, c['model'], 1, 1)
    veh.snap()
    mat.snap()
    ox, oy, oz = org
    # posicion provisional (la matriz siempre existe en los fixtures): x como palabra, y y z por la x87
    mat.put(0x30, 4, ox)
    mat.put(0x34, 4, quiet(oy))
    mat.put(0x38, 4, quiet(oz))
    # orientacion hacia `posn` en el plano
    one = load(K1)
    dx = st(sub(load(c['posn'][0]), load(ox)))
    dy = st(sub(load(c['posn'][1]), load(oy)))
    length = sqrt24(add(mul(dy, dy), mul(dx, dx)))
    dy_bits = store_bits(dy)
    if length != NAN and length[0] == 'f' and length[1] == 0:
        mark('len_zero')
        fx_bits, fy_bits, ry_bits = K1, dy_bits, 0xBF800000
    else:
        mark('len_pos')
        inv = st(div(one, length))
        r1 = mul(inv, dx)
        r2 = mul(inv, dy)
        fx_bits, fy_bits = store_bits(r1), store_bits(r2)
        ry_bits = 0x7FC00000 if r1 == NAN else store_bits(neg(r1))
    mat.put(0x10, 4, fx_bits)
    mat.put(0x14, 4, fy_bits)
    mat.put(0x18, 4, 0)
    mat.put(0x00, 4, fy_bits)
    mat.put(0x04, 4, ry_bits)
    mat.put(0x08, 4, 0)
    mat.put(0x20, 4, 0)
    mat.put(0x24, 4, 0)
    mat.put(0x28, 4, K1)
    # altura del punto: (1 - t) * z1' + z2' * t
    z1 = mul(fromint(s16(node.get(slot_of(n1w) * NODE_BYTES + 0xC, 2))), load(K0125))
    z2 = mul(fromint(s16(node.get(slot_of(n2w) * NODE_BYTES + 0xC, 2))), load(K0125))
    t = load(frac)
    zpoint = store_bits(add(mul(sub(one, t), z1), mul(z2, t)))
    if isnan(frac) or (frac & 0x7F800000) == 0x7F800000:
        mark('frac_special')
    # suelo: dos disparos verticales (+1000 y -1000)
    ground = GROUND_NONE
    ev(4, 0, ox, oy, zpoint, 0x447A0000, 1, 0, 0, 0, 1, 0)
    if c['phit'][0] & 1:
        ground = c['pz'][0]
        mark('ground_first')
    ev(4, 1, ox, oy, zpoint, 0xC47A0000, 1, 0, 0, 0, 1, 0)
    if c['phit'][1] & 1:
        zp = load(zpoint)
        d2 = sub(load(c['pz'][1]), zp)
        if lt(d2, load(0)):
            d2 = neg(d2)
        d1 = sub(load(ground), zp)
        if lt(d1, load(0)):
            d1 = neg(d1)
        if lt(d2, d1):
            ground = c['pz'][1]
            mark('ground_second')
        else:
            mark('ground_second_rejected')
            # el empate solo existe tras el redondeo a 24 bits: con diferencias exactas la segunda seria mas cercana
            def exact(bits):
                a, b = load(bits), zp
                if a == NAN or b == NAN or a[0] == 'i' or b[0] == 'i':
                    return None
                return abs(a[1] - b[1])
            e2 = exact(c['pz'][1])
            e1 = exact(ground)
            if e1 is not None and e2 is not None and e2 < e1:
                mark('ground_tie_x87')
    if not (c['phit'][0] & 1) and not (c['phit'][1] & 1):
        mark('ground_none')
    if ground == GROUND_NONE:
        mark('ground_1e9')
        ev(13, VEH_TAG, 1, *snap_vals())
        return result(0)
    # posicion final: x, y del punto, z = distancia al centro de masa + suelo
    ev(7, VEH_TAG)
    zf = store_bits(add(load(c['dist']), load(ground)))
    mat.put(0x30, 4, ox)
    mat.put(0x34, 4, quiet(oy))
    mat.put(0x38, 4, quiet(zf))
    veh.put(0x44, 4, 0)
    veh.put(0x48, 4, 0)
    veh.put(0x4C, 4, 0)
    ev(8, VEH_TAG, *snap_vals())
    ev(9, VEH_TAG, *snap_vals())
    model = c['model'] & M32
    handled = True
    if model == 407:
        mark('fire')
        veh.put(0x428, 1, veh.get(0x428, 1) | 4)
        S['fire'] = (S['fire'] + 1) & M32
        ev(10, VEH_TAG, *snap_vals())
    elif model == 416:
        mark('ambulance')
        veh.put(0x428, 1, veh.get(0x428, 1) | 2)
        S['amb'] = (S['amb'] + 1) & M32
        ev(11, VEH_TAG, *snap_vals())
    elif model in (427, 523, 596, 597, 598, 599):
        mark('police')
        ev(12, VEH_TAG, 0, *snap_vals())
    else:
        mark('default_model')
        handled = False
    if handled:
        veh.put(0x42D, 1, veh.get(0x42D, 1) | 0x80)
    mark('success')
    for x in list(c['posn']) + list(c['player']) + org + [c['dist'], frac]:
        if isnan(x):
            mark('nan_input')
        elif (x & 0x7F800000) == 0x7F800000:
            mark('inf_input')
        elif (x & 0x7F800000) == 0 and (x & 0x7FFFFF):
            mark('denormal_input')
        elif x == 0x80000000:
            mark('negzero_input')
    return result(VEH_TAG)


def check_side(c, o, e):
    for k in ('ret', 'veh_changes', 'mat_changes', 'node_changes', 'player_changes', 'statics', 'pif'):
        if o[k] != e[k]:
            return '%s: %s != %s' % (k, str(o[k])[:300], str(e[k])[:300])
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
