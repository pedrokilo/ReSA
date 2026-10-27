"""Aritmetica x87 a 24 bits (palabra de control del juego: mantisa de 24 bits, redondeo al par, exponente sin limite) con Fraction, compartida por los oraculos de los bancos del lote 74.

Un valor es NAN, ('i', signo) o ('f', Fraction, signo) (el signo solo importa para el cero). `store` devuelve los bits del float guardado con fstp dword (con desnormales), o None si es NaN.
"""
from fractions import Fraction

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


def mk(v):
    """valor exacto (Fraction) como operando: el signo de un cero positivo es 0"""
    return ('f', v, 1 if v < 0 else 0)


def is_zero(a):
    return a[0] == 'f' and a[1] == 0


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        if is_zero(a) or is_zero(b):
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


def div(a, b):
    if a == NAN or b == NAN:
        return NAN
    sa = a[1] if a[0] == 'i' else a[2]
    sb = b[1] if b[0] == 'i' else b[2]
    if a[0] == 'i':
        return NAN if b[0] == 'i' else ('i', sa ^ sb)
    if b[0] == 'i':
        return ('f', Fraction(0), sa ^ sb)
    if b[1] == 0:
        return NAN if a[1] == 0 else ('i', sa ^ sb)
    return fin(a[1] / b[1], sa ^ sb)


def lt0(a):
    if a == NAN:
        return False
    if a[0] == 'i':
        return a[1] == 1
    return a[1] < 0


def store(a):
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
