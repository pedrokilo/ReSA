"""Oraculo independiente (Python) de HeliSwat74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C69C0..0x6C6D24 (EXE 1.0 US), lo que debe observarse
(compuertas, traza de colaboradores EN ORDEN, posicion de la cuerda y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga depende de la forma de la instruccion); todo lo demas, bit a bit.
La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp), asi que los bytes cambiados se comparan exactamente.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + guarda + desplazamiento; indices: 0 helicoptero (guarda 0x200), 1 objetivo, 2 matriz del helicoptero, 3 matriz del objetivo,
4 ped, 5 inteligencia, 6 clump, 7 secuencia, 8 tarea de cuerda, 9 tarea de merodeo.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 74074  # lo fija cada oraculo


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + SALT) & M32)


def fill(size, seed):
    out = bytearray(size)
    for i in range(0, size, 4):
        w = rand(seed, i // 4)
        n = min(4, size - i)
        out[i:i + n] = w.to_bytes(4, 'little')[:n]
    return out


class Arena:
    """guarda + ventana + guarda; objeto en `guard`. put32/put8 como el banco; changes() = [[desplazamiento respecto al objeto, byte nuevo]]"""

    def __init__(self, win, seed, guard=0x40):
        self.guard = guard
        self.m = fill(guard + win + guard, seed)
        self.ref = None

    def put32(self, off, v):
        self.m[self.guard + off:self.guard + off + 4] = (v & M32).to_bytes(4, 'little')

    def put8(self, off, v):
        self.m[self.guard + off] = v & 0xFF

    def get32(self, off):
        return int.from_bytes(self.m[self.guard + off:self.guard + off + 4], 'little')

    def get8(self, off):
        return self.m[self.guard + off]

    def snap(self):
        self.ref = bytes(self.m)

    def changes(self):
        return [[i - self.guard, self.m[i]] for i in range(len(self.m)) if self.m[i] != self.ref[i]]


def tag(idx, off, guard=0x40):
    return 0x10000000 + (idx << 20) + guard + off


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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
    """redondea al par a 24 bits (exponente ilimitado), o con cuanto minimo 2**quantum_exp_min (desnormales del float)"""
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


def fchs(a):
    if a == NAN:
        return a
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, fchs(b))


def sqrt(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if a[1] else a
    if a[1] == 0:
        return a
    if a[1] < 0:
        return NAN
    s = 80
    n = (a[1] * (1 << (2 * s))).__floor__()
    r = isqrt(n)
    exact = r * r == n and (a[1] * (1 << (2 * s))).denominator == 1
    root = Fraction(r, 1 << s) if exact else Fraction(2 * r + 1, 1 << (s + 1))
    return ('f', rnd(root), 0)


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
    """un valor guardado como float y vuelto a cargar (NaN se mantiene NaN)"""
    return NAN if bits is None else load(bits)


def num(a):
    """valor para comparar: None si es NaN, +-inf como float, o Fraction"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return float('-inf') if a[1] else float('inf')
    return a[1]


def gt(a, b):
    """a > b ordenado (falso con NaN)"""
    x, y = num(a), num(b)
    return x is not None and y is not None and x > y


def lt(a, b):
    x, y = num(a), num(b)
    return x is not None and y is not None and x < y


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def same_float(got, want):
    """want: bits o None (NaN = cualquier carga)"""
    if want is None:
        return is_nan(got)
    return got == want


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_tiny(v):
    return v != NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < Fraction(2) ** -126


def is_signed_zero(v, neg):
    return v != NAN and v[0] == 'f' and v[1] == 0 and v[2] == neg

# ================================================================================================ SendDownSwat (0x6C69C0)
B50, B01, B25, B0 = 0x42480000, 0x3DCCCCCD, 0x40200000, 0
HELI_GUARD = 0x200
TAG = {'heli': 0, 'tgt': 1, 'matH': 2, 'matT': 3, 'ped': 4, 'intel': 5, 'clump': 6, 'seq': 7, 'rope': 8, 'wander': 9}
LAST = {}


def vec(c, p):
    return [load(c[p + '_x']), load(c[p + '_y']), load(c[p + '_z'])]


def rotate(c, rel):
    """0x59C790 + 0x411A00: rotacion (up*rz, fwd*ry, right*rx en el orden del EXE) y suma de la posicion del helicoptero; devuelve los bits (None = NaN) y los intermedios"""
    M = lambda k: load(c[k])
    right, fwd, up = [M('m_rx'), M('m_ry'), M('m_rz')], [M('m_fx'), M('m_fy'), M('m_fz')], [M('m_ux'), M('m_uy'), M('m_uz')]
    rx, ry, rz = rel
    x = add(add(mul(up[0], rz), mul(fwd[0], ry)), mul(right[0], rx))
    y = add(add(mul(up[1], rz), mul(right[1], rx)), mul(fwd[1], ry))
    z = add(add(mul(up[2], rz), mul(right[2], rx)), mul(fwd[2], ry))
    v = [reload(store(x)), reload(store(y)), reload(store(z))]
    hel = vec(c, 'hel')
    out = [store(add(hel[i], v[i])) for i in range(3)]
    LAST.clear()
    LAST.update(x=x, y=y, z=z, v=v, out=out)
    return out


def expected(c):
    """(pasa, eventos, cambios por arena, bits de la posicion de la cuerda o None)"""
    seed = c['seed']
    A = lambda k, win, g=0x40: Arena(win, (seed * 13 + k) & M32, g)
    heli, ped = A(1, 0xA18, HELI_GUARD), A(5, 0x500)
    seq, rope, wander = A(8, 0x40), A(9, 0x1C), A(10, 0x50)
    heli.put8(0x9B9, c['num_swat'])
    ped.put32(0x1C, c['ped_flags'])
    for a in (heli, ped, seq, rope, wander):
        a.snap()
    changes = [[], [], [], [], [], [], [], [], [], []]
    ev = []
    num = c['num_swat'] & 0xFF
    if num == 0 or (c['phys'] & 0x100) or (c['stream'] & 0xFF) != 1:
        return False, ev, changes, None
    ev.append([1])
    if c['rnd'] & 0x7F:
        return False, ev, changes, None
    hel, tgt, spd = vec(c, 'hel'), vec(c, 'tgt'), vec(c, 'spd')
    d = [sub(hel[i], tgt[i]) for i in range(3)]
    dist = sqrt(add(add(mul(d[0], d[0]), mul(d[1], d[1])), mul(d[2], d[2])))
    if gt(dist, load(B50)):
        return False, ev, changes, None
    speed = sqrt(add(add(mul(spd[0], spd[0]), mul(spd[1], spd[1])), mul(spd[2], spd[2])))
    if gt(speed, load(B01)):
        return False, ev, changes, None
    heli_tag = tag(TAG['heli'], 0, HELI_GUARD)
    ev.append([2, heli_tag, num - 1])
    V = rotate(c, vec(c, 'rel'))
    ev.append([3, V[0], V[1], V[2], 0, 0])
    ground = load(c['ground'])
    diff = sub(tgt[2], ground)
    if lt(diff, load(B0)):
        diff = fchs(diff)
    if not lt(diff, load(B25)):
        return False, ev, changes, V
    ev.append([4, V[0], V[1], V[2], 0xF00D0001, 0, 0])
    if c['water'] and not lt(load(c['water_lvl']), ground):
        return False, ev, changes, V
    rope_id = tag(TAG['heli'], num - 1, HELI_GUARD)
    ev.append([5, rope_id, 8, V[0], V[1], V[2], 0, 0, 0, 0, 20000])
    ev.append([6, 6, 3, 0xF00D0001, V[0], V[1], V[2], 1])
    ev.append([7, 0x40])
    seq_tag, rope_tag, wander_tag = tag(TAG['seq'], 0), tag(TAG['rope'], 0), tag(TAG['wander'], 0)
    seq_p = 0 if c['null_seq'] else seq_tag
    if seq_p:
        ev.append([8, seq_tag])
        seq.put32(0, 0xC0DE0008)
    ev.append([7, 0x1C])
    rope_p = 0 if c['null_rope'] else rope_tag
    if rope_p:
        ev.append([9, rope_tag, rope_id, heli_tag])
        rope.put32(0, 0xC0DE0009)
    ev.append([10, seq_p, rope_p])
    ev.append([7, 0x50])
    wander_p = 0 if c['null_wander'] else wander_tag
    if wander_p:
        ev.append([11, 0, 8])
        ev.append([12, wander_tag, 4, c['range']])
        wander.put32(0, 0xC0DE000C)
    ev.append([10, seq_p, wander_p])
    ev.append([13, tag(TAG['intel'], 4), seq_p, 3, 0])
    ev.append([14, tag(TAG['clump'], 0), 0, 0x98, 0x40800000])
    # memoria: ocupantes - 1 (byte), estado 0xAA en la ranura nueva (indice sin signo), ped sin colision, marcas de los constructores
    new = (num - 1) & 0xFF
    heli.put8(0x9B9, new)
    heli.put8(0x9BA + new, 0xAA)
    ped.put32(0x1C, c['ped_flags'] & ~1 & M32)
    changes[0], changes[4], changes[7], changes[8], changes[9] = heli.changes(), ped.changes(), seq.changes(), rope.changes(), wander.changes()
    return True, ev, changes, V


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, pasa)"""
    c = row['input']
    passes, ev, changes, V = expected(c)
    bad = []
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['ret_al'] != (1 if passes else 0):
            bad.append((side, 'resultado'))
        got = o['events']
        if len(got) != len(ev):
            bad.append((side, 'numero de eventos %d != %d' % (len(got), len(ev))))
        else:
            for g, w in zip(got, ev):
                if len(g) != len(w) or g[0] != w[0] or any(not same_float(gv, wv) if wv is None else gv != wv for gv, wv in zip(g[1:], w[1:])):
                    bad.append((side, 'evento %s != %s' % (g, w)))
                    break
        if o['changes'] != changes:
            bad.append((side, 'cambios %s != %s' % (str(o['changes'])[:200], str(changes)[:200])))
    return bad, passes


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        why, _ = check_row(row)
        checked += 1
        if why:
            bad += 1
            if bad <= 8:
                print('DIFIERE', row['case'], why[:3])
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
