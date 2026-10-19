"""Modelo x87 de 24 bits con exponente extendido, comun a los bancos GaragesAddOne30 y GaragesImpound30 (copia identica en ambas carpetas).

Valores V: 'f' finito (Fraction exacta; neg = signo del cero), 'inf', 'nan' (bits float32; silenciosos salvo un operando de MEMORIA senal, ver mem()) y 'unk' (NaN de carga util o signo desconocidos: se exige EXE == DLL).
fadd/fsub/fmul/fdiv/fsqrt redondean al par a 24 bits de mantisa sin limite de exponente (PC = 24); fstp dword redondea al rango float32. fld dword de un NaN senal lo silencia.
NaN (Intel SDM vol. 1, tabla 4-7, x87): una operacion con un unico NaN devuelve ese NaN (silenciado); SNaN y QNaN -> el QNaN; dos QNaN -> el de mayor significando (mismo significando y
signos opuestos: 'unk'); operacion invalida -> indefinido real 0xFFC00000. `fld dword` silencia un NaN senal al cargarlo (load); un operando de memoria de fadd/fsub/fsubr/... NO se silencia antes de
operar (mem): solo importa cuando el otro operando (registro, siempre silencioso) tambien es NaN.
"""
import math
from fractions import Fraction

INDEFINITE = 0xFFC00000
BIG = Fraction(2) ** 2000


class V:
    __slots__ = ('kind', 'val', 'neg', 'bits', 'snan')

    def __init__(self, kind, val=Fraction(0), neg=False, bits=0, snan=False):
        self.kind, self.val, self.neg, self.bits, self.snan = kind, val, neg, bits, snan

    def is_nan(self):
        return self.kind in ('nan', 'unk')

    def sign(self):
        return self.neg if (self.kind == 'inf' or (self.kind == 'f' and self.val == 0)) else self.val < 0

    def is_zero(self):
        return self.kind == 'f' and self.val == 0


def load(bits):
    """fld dword: exacta (subnormales tal cual); un NaN senal se carga silenciado."""
    bits &= 0xFFFFFFFF
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man:
            return V('nan', bits=bits | 0x400000)
        return V('inf', neg=bool(sign))
    if exp == 0:
        val = Fraction(man, 1 << 149)
    else:
        val = Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127))
    if sign:
        val = -val
    return V('f', val, neg=bool(sign))


def mem(bits):
    """Operando de memoria float32 de una instruccion aritmetica (fadd m32...): igual que load, pero un NaN senal conserva sus bits y la marca snan."""
    bits &= 0xFFFFFFFF
    if (bits >> 23) & 0xFF == 0xFF and bits & 0x7FFFFF and not bits & 0x400000:
        return V('nan', bits=bits, snan=True)
    return load(bits)


def _quiet(v):
    return V('nan', bits=v.bits | 0x400000) if v.kind == 'nan' and v.snan else v


def _exp2(a):
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    return e


def r24(x):
    """Redondeo exacto (al par) de un racional a 24 bits de mantisa, exponente sin limite."""
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    ulp = Fraction(2) ** (_exp2(a) - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


def nan_pick(a, b):
    """a: operando registro (silencioso); b: segundo operando (puede ser un NaN senal de memoria)."""
    if a.kind == 'unk' or b.kind == 'unk':
        return V('unk')
    if a.is_nan() and b.is_nan():
        if a.snan != b.snan:
            return _quiet(b if a.snan else a)        # SNaN y QNaN: el QNaN
        ma, mb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if ma > mb:
            return _quiet(a)
        if mb > ma:
            return _quiet(b)
        if (a.bits >> 31) == (b.bits >> 31):
            return _quiet(a)
        return V('unk')
    return _quiet(a if a.is_nan() else b)


def indefinite():
    return V('nan', bits=INDEFINITE)


def neg(v):
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return V('inf', neg=not v.neg)
    if v.val == 0:
        return V('f', Fraction(0), neg=not v.neg)
    return V('f', -v.val, neg=v.val > 0)


def add(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf':
            return a if a.neg == b.neg else indefinite()
        return a if a.kind == 'inf' else b
    s = a.val + b.val
    if s == 0:
        if a.val == 0 and b.val == 0:
            return V('f', Fraction(0), neg=a.neg and b.neg)
        return V('f', Fraction(0), neg=False)
    s = r24(s)
    return V('f', s, neg=s < 0)


def sub(a, b):
    return add(a, neg(b))


def mul(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf' or b.kind == 'inf':
        if a.is_zero() or b.is_zero():
            return indefinite()
        return V('inf', neg=sg)
    p = a.val * b.val
    if p == 0:
        return V('f', Fraction(0), neg=sg)
    p = r24(p)
    return V('f', p, neg=p < 0)


def div(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf':
        return indefinite() if b.kind == 'inf' else V('inf', neg=sg)
    if b.kind == 'inf':
        return V('f', Fraction(0), neg=sg)
    if b.val == 0:
        return indefinite() if a.val == 0 else V('inf', neg=sg)
    if a.val == 0:
        return V('f', Fraction(0), neg=sg)
    q = r24(a.val / b.val)
    return V('f', q, neg=q < 0)


def fsqrt24(p):
    """sqrt de un racional positivo redondeada al par a 24 bits (exacta, con bit pegajoso)."""
    p = Fraction(p)
    k = 400
    n = p * (Fraction(2) ** (2 * k))
    ni = n.numerator // n.denominator
    r = math.isqrt(ni)
    inexact = r * r != n
    v = Fraction(2 * r + 1, 2) if inexact else Fraction(r)
    return r24(v / (Fraction(2) ** k))


def sqrt(a):
    if a.kind in ('nan', 'unk'):
        return a
    if a.kind == 'inf':
        return indefinite() if a.neg else a
    if a.val == 0:
        return a
    if a.val < 0:
        return indefinite()
    return V('f', fsqrt24(a.val))


def store(v):
    """fstp dword: bits float32 (None = desconocido)."""
    if v.kind == 'unk':
        return None
    if v.kind == 'nan':
        return v.bits | 0x400000
    if v.kind == 'inf':
        return 0xFF800000 if v.neg else 0x7F800000
    if v.val == 0:
        return 0x80000000 if v.neg else 0
    sign = 0x80000000 if v.val < 0 else 0
    a = abs(v.val)
    quantum = Fraction(2) ** (max(_exp2(a), -126) - 23)
    m = a / quantum
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    res = q * quantum
    if res >= Fraction(2) ** 128:
        return sign | 0x7F800000
    if res == 0:
        return sign
    exp = _exp2(res)
    if exp < -126:
        return sign | int(res / Fraction(2) ** -149)
    return sign | ((exp + 127) << 23) | int((res / Fraction(2) ** exp - 1) * (1 << 23))


def fl(v):
    """store + fld: el valor tras pasar por memoria como float32."""
    b = store(v)
    return V('unk') if b is None else load(b)


def key(v):
    if v.kind == 'inf':
        return -BIG if v.neg else BIG
    return v.val


class Unknown(Exception):
    pass


def lt(a, b):
    """a < b ordenado (con NaN: falso, como fcom + test ah,5). Con valores desconocidos lanza Unknown."""
    if a.kind == 'unk' or b.kind == 'unk':
        raise Unknown()
    if a.is_nan() or b.is_nan():
        return False
    return key(a) < key(b)


def gt(a, b):
    return lt(b, a)


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def match_hex(obs, want):
    """Compara cadenas hexadecimales de bytes con comodines '??'."""
    if len(obs) != len(want):
        return False
    return all(want[i:i + 2] == '??' or want[i:i + 2] == obs[i:i + 2] for i in range(0, len(want), 2))
