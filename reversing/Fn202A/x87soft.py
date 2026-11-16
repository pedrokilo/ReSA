"""x87 por software (24 bits de precision de mantisa, exponente ampliado) para el oraculo de Fn202A.

Modela lo que el banco necesita de la FPU del EXE con la palabra de control del juego (precision 24 bits, redondeo al mas cercano, excepciones enmascaradas):
fld/fstp de float, fmul, fadd, fsub, fchs, fsqrt, fcom, fpatan, fsin y fcos. Los resultados de fmul/fadd/fsub/fsqrt se redondean a 24 bits de mantisa SIN limite de exponente; fpatan, fsin y
fcos devuelven 64 bits de mantisa (precision extendida, redondeo al mas cercano a partir de mpmath); el almacenamiento a float redondea al float (con denormales y desbordamiento a infinito).
Los valores son Fraction exactos (finitos no nulos), ceros con signo, infinitos y NaN (con su significando extendido de 64 bits). Si una operacion cae en un caso no modelado (dos NaN de igual
significando y distinto signo, argumentos fuera de rango razonable de fsin/fcos) se lanza Unmodeled y el oraculo marca la fila como `unmodeled` (solo se exige igualdad EXE/DLL).
"""
from fractions import Fraction
import math
import struct

import mpmath

mpmath.mp.prec = 400


class Unmodeled(Exception):
    pass


M63 = 1 << 63
INDEFINITE = (1, 0xC000000000000000)     # NaN real indefinido: signo 1, significando 1100...0


class V:
    __slots__ = ('kind', 'sign', 'mag', 'sig')

    def __init__(self, kind, sign=0, mag=None, sig=None):
        self.kind, self.sign, self.mag, self.sig = kind, sign, mag, sig     # kind: 'f' finito no nulo (mag > 0), 'z', 'i', 'n' (sig = significando extendido de 64 bits)

    def __repr__(self):
        return 'V(%s,%d,%s,%s)' % (self.kind, self.sign, self.mag, hex(self.sig) if self.sig is not None else None)

    def neg(self):
        return V(self.kind, self.sign ^ 1, self.mag, self.sig)

    def is_nan(self):
        return self.kind == 'n'

    def frac(self):
        """valor exacto (Fraction) de un finito o cero"""
        if self.kind == 'z':
            return Fraction(0)
        return -self.mag if self.sign else self.mag


def nan_indef():
    return V('n', INDEFINITE[0], None, INDEFINITE[1])


def from_f32(bits):
    bits &= 0xFFFFFFFF
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    frac = bits & 0x7FFFFF
    if exp == 255:
        if frac == 0:
            return V('i', sign)
        return V('n', sign, None, M63 | (1 << 62) | (frac << 40))      # fld de un SNaN lo convierte en QNaN (bit de silencio)
    if exp == 0:
        if frac == 0:
            return V('z', sign)
        return V('f', sign, Fraction(frac) * Fraction(1, 1 << 149))
    return V('f', sign, Fraction((1 << 23) | frac) * (Fraction(2) ** (exp - 150)))


def _msb(n):
    return n.bit_length()


def round_frac(x, bits):
    """redondea un Fraction > 0 a `bits` bits de mantisa (al mas cercano, empate al par), sin limite de exponente"""
    if x == 0:
        return x
    p, q = x.numerator, x.denominator
    # exponente e tal que 2^(e-1) <= x < 2^e
    e = _msb(p) - _msb(q)
    if Fraction(p, q) >= Fraction(2) ** e:
        e += 1
    elif Fraction(p, q) < Fraction(2) ** (e - 1):
        e -= 1
    # x = m * 2^(e - bits), m en [2^(bits-1), 2^bits)
    shift = bits - e
    scaled = x * (Fraction(2) ** shift)
    n = scaled.numerator // scaled.denominator
    rem = scaled - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (n & 1)):
        n += 1
    return Fraction(n) * (Fraction(2) ** (-shift))


def r24(v):
    if v.kind != 'f':
        return v
    return V('f', v.sign, round_frac(v.mag, 24))


def to_f32(v):
    """fstp dword: bits del float"""
    if v.kind == 'z':
        return v.sign << 31
    if v.kind == 'i':
        return (v.sign << 31) | 0x7F800000
    if v.kind == 'n':
        return (v.sign << 31) | 0x7F800000 | ((v.sig >> 40) & 0x7FFFFF)
    x = v.mag
    # el mas cercano con denormales: la unidad minima es 2^-149 para valores < 2^-126
    if x < Fraction(1, 1 << 126):
        n = x * (Fraction(1 << 149))
        k = n.numerator // n.denominator
        rem = n - k
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (k & 1)):
            k += 1
        return (v.sign << 31) | k            # k puede llegar a 2^23 = el menor normal: la codificacion sigue valiendo
    r = round_frac(x, 24)
    if r >= Fraction(2) ** 128:
        return (v.sign << 31) | 0x7F800000
    e = _msb(r.numerator) - _msb(r.denominator)
    if r >= Fraction(2) ** e:
        e += 1
    # r = m * 2^(e-24), m en [2^23, 2^24)
    m = r / (Fraction(2) ** (e - 24))
    m = m.numerator // m.denominator
    return (v.sign << 31) | ((e - 1 + 127) << 23) | (m & 0x7FFFFF)


def _nan_pick(a, b):
    if a.kind == 'n' and b.kind == 'n':
        if a.sig > b.sig:
            return a
        if b.sig > a.sig:
            return b
        if a.sign != b.sign:
            raise Unmodeled('dos NaN iguales de distinto signo')
        return a
    return a if a.kind == 'n' else b


def _quiet(v):
    return V('n', v.sign, None, v.sig | (1 << 62))


def mul(a, b):
    if a.kind == 'n' or b.kind == 'n':
        return _quiet(_nan_pick(a, b))
    sign = a.sign ^ b.sign
    if a.kind == 'i' or b.kind == 'i':
        if a.kind == 'z' or b.kind == 'z':
            return nan_indef()
        return V('i', sign)
    if a.kind == 'z' or b.kind == 'z':
        return V('z', sign)
    return r24(V('f', sign, a.mag * b.mag))


def add(a, b):
    if a.kind == 'n' or b.kind == 'n':
        return _quiet(_nan_pick(a, b))
    if a.kind == 'i' or b.kind == 'i':
        if a.kind == 'i' and b.kind == 'i' and a.sign != b.sign:
            return nan_indef()
        return a if a.kind == 'i' else b
    if a.kind == 'z' and b.kind == 'z':
        return V('z', a.sign & b.sign)
    x = a.frac() + b.frac()
    if x == 0:
        return V('z', 0)
    return r24(V('f', 1 if x < 0 else 0, abs(x)))


def sub(a, b):
    return add(a, b.neg())


def sqrt(a):
    if a.kind == 'n':
        return _quiet(a)
    if a.kind == 'z':
        return a
    if a.sign:
        return nan_indef()
    if a.kind == 'i':
        return a
    # raiz exacta a 80 bits con pegajoso
    p, q = a.mag.numerator, a.mag.denominator
    # sqrt(p/q) = sqrt(p*q)/q
    n = p * q
    shift = 200
    s = math.isqrt(n << (2 * shift))
    exact = s * s == n << (2 * shift)
    root = Fraction(s, (1 << shift) * q)
    if not exact:
        root += Fraction(1, (1 << (shift + 1)) * q)       # pegajoso: un poco por encima de la raiz truncada para romper empates
    return V('f', 0, round_frac(root, 24))


def compare(a, b):
    """'lt', 'gt', 'eq' o 'un' (a frente a b)"""
    if a.kind == 'n' or b.kind == 'n':
        return 'un'
    def key(v):
        if v.kind == 'i':
            return (math.inf if not v.sign else -math.inf)
        return v.frac()
    ka, kb = key(a), key(b)
    if ka < kb:
        return 'lt'
    if ka > kb:
        return 'gt'
    return 'eq'


def _from_mp(x):
    m = mpmath.mpf(x)
    if m == 0:
        return V('z', 0)
    s, man, exp, _bc = m._mpf_
    f = Fraction(int(man)) * (Fraction(2) ** int(exp))
    return V('f', 1 if s else 0, round_frac(f, 64))


def _to_mp(v):
    f = v.frac()
    return mpmath.mpf(f.numerator) / mpmath.mpf(f.denominator)


PI = mpmath.pi


def fpatan(y, x):
    """atan2(y, x) (ST1 = y, ST0 = x)"""
    if y.kind == 'n' or x.kind == 'n':
        return _quiet(_nan_pick(y, x))
    sy, sx = y.sign, x.sign
    def pi_mul(num, den, sign):
        r = _from_mp(PI * num / den)
        return V(r.kind, sign, r.mag)
    if y.kind == 'z':
        if sx:
            return pi_mul(1, 1, sy)            # atan2(+-0, x < 0 o -0) = +-pi
        return V('z', sy)
    if y.kind == 'i':
        if x.kind == 'i':
            return pi_mul(1, 4, sy) if not sx else pi_mul(3, 4, sy)
        return pi_mul(1, 2, sy)
    # y finito no nulo
    if x.kind == 'i':
        if not sx:
            return V('z', sy)
        return pi_mul(1, 1, sy)
    if x.kind == 'z':
        return pi_mul(1, 2, sy)
    r = _from_mp(mpmath.atan2(_to_mp(y), _to_mp(x)))
    return r


def _check_trig_range(a):
    m = a.mag
    if m >= Fraction(1 << 20):
        raise Unmodeled('fsin/fcos fuera del rango modelado')


def fsin(a):
    if a.kind == 'n':
        return _quiet(a)
    if a.kind == 'i':
        return nan_indef()
    if a.kind == 'z':
        return a
    if a.mag >= Fraction(1 << 63):
        return a                                  # fuera de rango: el operando queda como estaba (C2 = 1)
    _check_trig_range(a)
    r = _from_mp(mpmath.sin(_to_mp(a)))
    return r


def fcos(a):
    if a.kind == 'n':
        return _quiet(a)
    if a.kind == 'i':
        return nan_indef()
    if a.kind == 'z':
        return V('f', 0, Fraction(1))
    if a.mag >= Fraction(1 << 63):
        return a
    _check_trig_range(a)
    return _from_mp(mpmath.cos(_to_mp(a)))
