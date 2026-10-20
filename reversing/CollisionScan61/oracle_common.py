"""Aritmetica float32 para los oraculos del banco EventScanners61.

El EXE trabaja en x87 con la palabra de control a 24 bits de mantisa: cada suma, resta o producto de dos floats se redondea a 24 bits (como un float) y,
mientras no se salga del rango de float, el resultado es identico al de una operacion SSE de precision simple. Aqui cada operacion se calcula en double
(exacto para +, -, * de dos floats: 53 >= 2*24+2) y se redondea a float; los desbordamientos dan infinito. Los fixtures evitan los exponentes extremos
(donde el x87 conservaria el rango extendido), pero incluyen NaN e infinitos, que se propagan igual.
"""
import math
import struct


def bits_to_f(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def f_to_bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def f32(x):
    if x != x or x in (math.inf, -math.inf):
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def add(a, b):
    return f32(a + b)


def sub(a, b):
    return f32(a - b)


def mul(a, b):
    # inf * 0 en Python da nan (sin excepcion)
    return f32(a * b)


def neg(a):
    return -a


def lt(a, b):
    """fcomp + test ah,5 + jp: verdadero solo si a < b (falso con NaN)."""
    return a < b


def gt(a, b):
    return a > b


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


def u32(v):
    return v & 0xFFFFFFFF
