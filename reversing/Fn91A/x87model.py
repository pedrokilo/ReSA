"""Modelo numerico de la x87 con palabra de control de 24 bits (exponente extendido) y utilidades del banco Fn91A (copia del modelo del banco Fn88B: fracciones exactas, valores especiales).

Valores: Fraction (finito no nulo), ('z', signo), ('i', signo), ('n', signo, mantisa de 23 bits con el bit 22 puesto: NaN silencioso).
"""
import struct
from fractions import Fraction
from math import floor, isqrt  # noqa: F401

M32 = 0xFFFFFFFF


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    """Rand de Fixtures.hpp de Fn91A"""
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 91001) & M32)


def region_bytes(mode, seed, start, count):
    """bytes [start, start+count) de una region rellenada con FillRegion de Fixtures.hpp (0 y 5 ruido, 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5)"""
    if mode == 1:
        return bytes(count)
    if mode == 2:
        return bytes([0xFF]) * count
    if mode == 3:
        return bytes([0xCD]) * count
    if mode == 4:
        return bytes([0xA5]) * count
    out = bytearray()
    first = start // 4
    last = (start + count + 3) // 4
    for w in range(first, last):
        out += struct.pack('<I', rnd(seed, w))
    off = start - first * 4
    return bytes(out[off:off + count])


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con exponente extendido
# Valores: Fraction (finito no nulo), ('z', signo), ('i', signo), ('n', signo, mantisa de 23 bits con el bit 22 puesto: NaN silencioso)
class Ambiguous(Exception):
    """dos NaN con el mismo significando y distinto signo: la x87 no define cual elige"""


DEFAULT_NAN = ('n', 1, 0x400000)


def from_bits(b):
    s = b >> 31
    e = (b >> 23) & 0xFF
    m = b & 0x7FFFFF
    if e == 0xFF:
        return ('n', s, m | 0x400000) if m else ('i', s)
    if e == 0:
        return ('z', s) if m == 0 else Fraction(-m if s else m, 1 << 149)
    v = Fraction(m | 0x800000) * (Fraction(2) ** (e - 150))
    return -v if s else v


def r24(x):
    """Redondeo a 24 bits de mantisa (mitad al par), exponente sin limite."""
    if x == 0:
        return x
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return s * n * ulp


def r24v(x):
    """r24 de un resultado exacto: cero exacto no pasa por aqui."""
    return r24(x)


def to_bits(v):
    """fstp dword: redondeo a float (mitad al par, subnormales, desborde a infinito)."""
    if isinstance(v, tuple):
        if v[0] == 'z':
            return v[1] << 31
        if v[0] == 'i':
            return (v[1] << 31) | 0x7F800000
        return (v[1] << 31) | 0x7F800000 | (v[2] & 0x7FFFFF)
    s = 1 if v < 0 else 0
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (max(e, -126) - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    r = n * ulp
    if r >= Fraction(2) ** 128:
        return (s << 31) | 0x7F800000
    if n == 0:
        return s << 31
    if r < Fraction(2) ** -126:
        return (s << 31) | n
    # normal: n*ulp = m * 2^(E-23), con 2^23 <= m < 2^24
    ee = r.numerator.bit_length() - r.denominator.bit_length()
    while Fraction(2) ** ee > r:
        ee -= 1
    while Fraction(2) ** (ee + 1) <= r:
        ee += 1
    mant = int(r / (Fraction(2) ** (ee - 23))) & 0x7FFFFF
    return (s << 31) | ((ee + 127) << 23) | mant


def f32(v):
    """valor tras guardarlo como float y volver a cargarlo"""
    return from_bits(to_bits(v))


def is_nan(a):
    return isinstance(a, tuple) and a[0] == 'n'


def is_inf(a):
    return isinstance(a, tuple) and a[0] == 'i'


def is_zero(a):
    return isinstance(a, tuple) and a[0] == 'z'


def pick_nan(a, b):
    if is_nan(a) and is_nan(b):
        if a[2] == b[2]:
            if a[1] != b[1]:
                raise Ambiguous
            return a
        return a if a[2] > b[2] else b
    return a if is_nan(a) else b


def sign_of(a):
    if isinstance(a, tuple):
        return a[1]
    return 1 if a < 0 else 0


def neg(a):
    if isinstance(a, tuple):
        return (a[0], 1 - a[1]) + a[2:]
    return -a


def add(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    if is_inf(a) and is_inf(b):
        return a if a[1] == b[1] else DEFAULT_NAN
    if is_inf(a):
        return a
    if is_inf(b):
        return b
    if is_zero(a) and is_zero(b):
        return a if a[1] == b[1] else ('z', 0)
    if is_zero(a):
        return r24(b)
    if is_zero(b):
        return r24(a)
    s = a + b
    if s == 0:
        return ('z', 0)
    return r24(s)


def sub(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    return add(a, neg(b))


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    s = sign_of(a) ^ sign_of(b)
    if is_inf(a) or is_inf(b):
        if is_zero(a) or is_zero(b):
            return DEFAULT_NAN
        return ('i', s)
    if is_zero(a) or is_zero(b):
        return ('z', s)
    return r24(a * b)


def div(a, b):
    """solo con divisor finito no nulo y dividendo finito"""
    if is_zero(a):
        return ('z', sign_of(a) ^ sign_of(b))
    return r24(a / b)


def sqrt24(a):
    if is_nan(a):
        return a
    if is_zero(a):
        return a
    if is_inf(a):
        return a if a[1] == 0 else DEFAULT_NAN
    if a < 0:
        return DEFAULT_NAN
    P = 90
    from math import isqrt
    N = a * (4 ** P)
    n = N.numerator // N.denominator
    s = isqrt(n)
    if Fraction(s * s) == N:
        return r24(Fraction(s, 2 ** P))
    return r24(Fraction(2 * s + 1, 2 ** (P + 1)))     # entre s y s+1: el bit pegajoso evita falsos empates



def ftol(a):
    """_ftol2 del EXE (0x821B40): trunca a 64 bits (fistp qword + correccion) y el llamador usa EAX = 32 bits bajos con signo; NaN, infinito o |valor| >= 2^63 dan 0x8000000000000000 -> 0"""
    if isinstance(a, tuple):
        return 0
    n = a.numerator // a.denominator if a >= 0 else -((-a.numerator) // a.denominator)
    if abs(n) >= 1 << 63:
        return 0
    low = n & 0xFFFFFFFF
    return low - (1 << 32) if low >= 1 << 31 else low




def fi(n):
    """fild de un entero: exacto"""
    return ('z', 0) if n == 0 else Fraction(n)


def ld(bits):
    return from_bits(bits & M32)


def bits(v):
    """fstp dword"""
    return to_bits(v)


def fix0(x):
    return ('z', 0) if x == 0 else x


def num(a):
    """valor comparable (Fraction o +-inf) o None si es NaN"""
    if isinstance(a, tuple):
        if a[0] == 'n':
            return None
        if a[0] == 'z':
            return Fraction(0)
        return float('inf') if a[1] == 0 else float('-inf')
    return a


def lt(a, b):
    x, y = num(a), num(b)
    return x is not None and y is not None and x < y


def gt(a, b):
    x, y = num(a), num(b)
    return x is not None and y is not None and x > y


def div_int(a, n):
    """a / n (fidiv): n entero, posiblemente cero"""
    if is_nan(a):
        return a
    if n == 0:
        if is_zero(a):
            return DEFAULT_NAN
        return ('i', sign_of(a))
    neg_n = 1 if n < 0 else 0
    if is_inf(a):
        return ('i', a[1] ^ neg_n)
    if is_zero(a):
        return ('z', a[1] ^ neg_n)
    return r24(a / Fraction(n))


def floor_val(v):
    if isinstance(v, tuple):
        return v
    return fix0(Fraction(floor(v)))


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def idiv(a, b):
    """idiv de 32 bits: cociente truncado hacia cero"""
    q = abs(a) // abs(b)
    return -q if (a < 0) != (b < 0) else q


def crc32(data, value=0):
    import zlib
    return zlib.crc32(bytes(data), value) & M32


def div2(a, b):
    """a / b con b finito y no nulo; a de cualquier clase"""
    if is_nan(a):
        return a
    if is_inf(a):
        return ('i', a[1] ^ (1 if b < 0 else 0))
    if is_zero(a):
        return ('z', a[1] ^ (1 if b < 0 else 0))
    return r24(a / b)
