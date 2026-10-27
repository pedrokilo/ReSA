"""Oraculo independiente (Python) de FxWood73: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x49EE10..0x49F032, lo que debe observarse
(traza de AddParticle, sorteos consumidos y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida);
indices: 0 posicion, 1 direccion, 2 matriz de la camara, 3 sistema A (g_fx.m_Blood inicial), 4 sistema B, 5 objeto this.
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
M32 = 0xFFFFFFFF


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


# ---- rand() falso del banco (misma definicion que Fixtures.hpp: RandValue) ----
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 73073) & M32)


EDGE = [0, 1, 4999, 5000, 5001, 9999, 10000, 10001, 19999, 20000, 32767, 0x7FFFFFFF, 0xFFFFFFFF, 0x80000000, 0x80000001, 0xFFFFD8F0, 0xFFFFFFF0, 0xFFFF0001]


def rand_value(mode, seed, k):
    if mode == 1:
        return EDGE[rnd(seed, k) % len(EDGE)]
    if mode == 2:
        return rnd(seed, k + 5000)
    if mode == 3:
        return 5000 + 10000 * (rnd(seed, k) % 3) if (k % 7) >= 4 else rnd(seed, k) & 0x7FFF
    return rnd(seed, k) & 0x7FFF


# ---- aritmetica ----
def f32(bits):
    return struct_unpack(bits)


def struct_unpack(bits):
    import struct
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


class V:
    """valor de la x87: kind 'f' (finito, v Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits ya silenciado)"""
    __slots__ = ('kind', 'v', 'neg', 'bits')

    def __init__(self, kind, v=Fraction(0), neg=False, bits=0):
        self.kind, self.v, self.neg, self.bits = kind, v, neg, bits


def from_bits(b):
    if is_nan(b):
        return V('nan', bits=b | 0x400000)       # fld dword silencia el NaN senalizante
    if is_inf(b):
        return V('inf', neg=bool(b >> 31))
    e = (b >> 23) & 0xFF
    m = b & 0x7FFFFF
    if e == 0:
        v = Fraction(m, 1 << 149)
    else:
        v = Fraction(m | 0x800000) * Fraction(2) ** (e - 150)
    neg = bool(b >> 31)
    return V('f', -v if neg else v, neg)


def exp2floor(a):
    n, d = a.numerator, a.denominator
    e = n.bit_length() - d.bit_length()
    if Fraction(n, d) < Fraction(2) ** e:
        e -= 1
    return e


def round_q(a, quantum):
    """a (>0) a un multiplo de quantum, mitad al par"""
    q = a / quantum
    m = q.numerator // q.denominator
    r = q - m
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and m % 2 == 1):
        m += 1
    return m


def r24(x):
    """redondeo a 24 bits con exponente extendido"""
    if x.kind != 'f':
        return x
    if x.v == 0:
        return x
    a = abs(x.v)
    e = exp2floor(a)
    m = round_q(a, Fraction(2) ** (e - 23))
    val = m * Fraction(2) ** (e - 23)
    return V('f', -val if x.v < 0 else val, x.v < 0)


def to_f32_bits(x):
    if x.kind == 'nan':
        return x.bits
    if x.kind == 'inf':
        return 0xFF800000 if x.neg else 0x7F800000
    if x.v == 0:
        return 0x80000000 if x.neg else 0
    sign = 0x80000000 if x.v < 0 else 0
    a = abs(x.v)
    e = exp2floor(a)
    if e < -126:
        m = round_q(a, Fraction(1, 1 << 149))
        return sign | m
    m = round_q(a, Fraction(2) ** (e - 23))
    if m == 1 << 24:
        m >>= 1
        e += 1
    if e > 127:
        return sign | 0x7F800000
    return sign | ((e + 127) << 23) | (m - (1 << 23))


def add(a, b):
    if a.kind == 'nan':
        return a
    if b.kind == 'nan':
        return b
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf' and a.neg != b.neg:
            return V('nan', bits=0xFFC00000)
        return a if a.kind == 'inf' else b
    s = a.v + b.v
    if s == 0:
        return V('f', Fraction(0), a.neg and b.neg if a.v == 0 and b.v == 0 else False)
    return r24(V('f', s, s < 0))


def neg_v(a):
    if a.kind == 'nan':
        return a
    if a.kind == 'inf':
        return V('inf', neg=not a.neg)
    return V('f', -a.v, not a.neg if a.v == 0 else a.v > 0)


def sub(a, b):
    return add(a, neg_v(b)) if not (a.kind == 'f' and b.kind == 'f' and a.v == b.v and a.v != 0) else V('f', Fraction(0), False)


def mul(a, b):
    if a.kind == 'nan':
        return a
    if b.kind == 'nan':
        return b
    sign = (a.neg if a.kind == 'inf' else (a.v < 0 or (a.v == 0 and a.neg))) != (b.neg if b.kind == 'inf' else (b.v < 0 or (b.v == 0 and b.neg)))
    if a.kind == 'inf' or b.kind == 'inf':
        other = b if a.kind == 'inf' else a
        if other.kind == 'f' and other.v == 0:
            return V('nan', bits=0xFFC00000)
        return V('inf', neg=sign)
    p = a.v * b.v
    if p == 0:
        return V('f', Fraction(0), sign)
    return r24(V('f', p, p < 0))


def gt625(a):
    """comparacion ordenada: con NaN es falso (el EXE no sale)"""
    if a.kind == 'nan':
        return False
    if a.kind == 'inf':
        return not a.neg
    return a.v > 625


def far_away(cam, pos):
    c = [from_bits(b) for b in cam]
    p = [from_bits(b) for b in pos]
    d = [r24(sub(c[i], p[i])) for i in range(3)]       # dx, dy, dz
    sq = [r24(mul(x, x)) for x in d]
    s = r24(add(sq[0], sq[2]))
    s = r24(add(s, sq[1]))
    return gt625(s)


INV = from_bits(0x38D1B717)    # 1e-4f (0x858FC4)
FOUR = from_bits(0x40800000)   # 4.0f (0x858B90)
TWO = from_bits(0x40000000)    # 2.0f (0x858CA0)


def signed32(u):
    return u - (1 << 32) if u & 0x80000000 else u


def cdiv_rem(u):
    """cdq; idiv 10000: resto con el signo del dividendo"""
    a = signed32(u)
    q = abs(a) // 10000
    r = abs(a) - q * 10000
    return -r if a < 0 else r


def from_int(i):
    return V('f', Fraction(i), False)


def scaled(rem, scale_bits, off_bits):
    t = r24(mul(from_int(rem), INV))
    t = r24(mul(t, from_bits(scale_bits)))
    return to_f32_bits(r24(add(t, from_bits(off_bits))))


def jitter(rem, vel_bits):
    t = r24(mul(from_int(rem), INV))
    t = r24(mul(t, FOUR))
    t = r24(sub(t, TWO))
    return to_f32_bits(r24(add(t, from_bits(vel_bits))))


def expected(c):
    """(rand_calls, particle_calls, events, slot_tag)"""
    amount = signed32(c['amount'])
    if far_away_pair(c):
        return 0, 0, [], tag(3)
    if amount <= 0:
        return 0, 0, [], tag(3)
    ev = []
    k = 0
    slot = 0
    for _ in range(amount):
        draws = []
        for _j in range(7):
            draws.append(cdiv_rem(rand_value(c['rand_mode'], c['rand_seed'], k)))
            k += 1
        red = scaled(draws[0], 0x3DF5C290, 0x3E051EB8)
        green = scaled(draws[1], 0x3CF5C294, 0x3DF5C28F)
        blue = scaled(draws[2], 0x3CF5C290, 0x3D23D70A)
        size = scaled(draws[3], 0x3E99999A, 0x3F333333)
        vel = [to_f32_bits(r24(mul(from_bits(c['dir'][i]), FOUR))) for i in range(3)]
        vel = [jitter(draws[4 + i], vel[i]) for i in range(3)]
        ev.append([31, tag(3 + slot), tag(0)] + list(c['pos']) + vel + [0, red, green, blue, 0x3F800000, size, 0, 0x3F800000, 0xBF800000, c['light'], 0x3F19999A, 0])
        if c['swap_slot']:
            slot ^= 1
    return k, amount, ev, tag(3 + slot)


def far_away_pair(c):
    return far_away(c['cam'], c['pos'])


def check_self():
    # 1e-4f * 5000 -> 0.5 exacto tras el redondeo a 24 bits: el desplazamiento aleatorio vale cero y un desnormal de direccion se conserva
    assert to_f32_bits(r24(mul(from_int(5000), INV))) == 0x3F000000
    assert jitter(5000, 0x00000004) == 0x00000004
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert cdiv_rem(0x80000000) == -3648 and cdiv_rem(0xFFFFFFFF) == -1 and cdiv_rem(32767) == 2767
    assert scaled(0, 0x3DF5C290, 0x3E051EB8) == 0x3E051EB8
    print('ok')


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        rand_calls, particles, ev, slot = expected(c)
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            if o['rand_calls'] != rand_calls or o['particle_calls'] != particles or o['events'] != ev or o['blood_slot'] != slot or any(o['changes']):
                bad += 1
                if bad <= 5:
                    print('DIFIERE', side, row['case'], 'rand', o['rand_calls'], rand_calls, 'part', o['particle_calls'], particles, '\n  events', o['events'][:2], '\n  model ', ev[:2], '\n  slot', o['blood_slot'], slot, 'changes', o['changes'])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    if len(sys.argv) == 1:
        check_self()
        sys.exit(0)
    sys.exit(main(sys.argv[1]))
