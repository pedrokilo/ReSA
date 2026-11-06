"""Oraculo independiente (Python) de Fn103A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las siete funciones del lote 103A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente
ampliado, NaN con carga util) se modela con fracciones exactas; fsin/fcos con enteros de punto fijo de 400 bits redondeados a 64 bits de mantisa.

Funciones (desensamblado del EXE; el primer factor o sumando va en el registro de la x87 (fld: un NaN senalizante se silencia al cargarlo) y el segundo en memoria (fmul/fadd/fsub con operando float de
memoria: un NaN senalizante de memoria pierde frente a un NaN del registro; entre dos registros, o con un silencioso de memoria, gana el NaN de mayor significando). S = fsin(a), C = fcos(a) (64 bits de
mantisa, sin redondear a float en la pila) y Sf = float(S):
  0 SetRotateXOnly 0x59AFA0: m0 = 1.0; m1, m2, m4 y m8 a cero; m5 = float(C), m6 = float(S), m9 = float(-S), m10 = float(C); no toca la posicion. 1 SetRotateYOnly 0x59AFE0: m5 = 1.0; m1, m4, m6, m9 a cero;
    m0 = float(C), m2 = float(-S), m8 = float(S), m10 = float(C).
  2 Rotate 0x59B460 (x, y, z): cx, sx, cy, sy, cz, sz; f1 = float(sx), f2 = float(sz), p1 = sz*f1 (sin guardar), f3 = float(cz*f1); f4 = cz*cy - p1*sy, f5 = f3*sy + f2*cy, f6 = -(sy*cx), f7 = -(f2*cx),
    f8 = cz*cx, f9 = cz*sy + p1*cy, f10 = f2*sy - f3*cy (todos guardados como float), cc = cx*cy (sin guardar); filas: (m0, m1, m2) = ((f9*m2 + f7*m1) + f4*m0, (f10*m2 + f8*m1) + f5*m0, (cc*m2 + f1*m1) + f6*m0),
    forward ((f4*m4 + f9*m6) + f7*m5, (f5*m4 + f10*m6) + f8*m5, (f6*m4 + cc*m6) + f1*m5), up (m8..m10) y pos (m12..m14) igual; se lee todo antes de escribir.
  3 ForceUpVector 0x59B7E0 (up por valor): A = up.z*m5 - up.y*m6, B = up.x*m6 - up.z*m4, C = up.y*m4 - up.x*m5 (m4..m6 = forward); m2 = float(C); m4 = float(C*up.y - B*up.z); m5 = float(A*up.z - float(C)*up.x);
    m6 = float(up.x*B - A*up.y); m0 = float(A); m1 = float(B); m8, m9, m10 = up (dwords tal cual).
  4 SetRotate(quat) 0x59BBF0 (q = x, y, z, real en pb): v2 = float(2q); x2x, y2x, z2x, y2y como float, z2y, z2z, y2r, z2r en registro, x2r como float; m0 = 1 - (z2z + y2y), m1 = y2x + z2r, m2 = z2x - y2r, m4 = y2x - z2r,
    m5 = 1 - (z2z + x2x), m6 = x2r + z2y, m8 = y2r + z2x, m9 = z2y - x2r, m10 = 1 - (y2y + x2x).
  5 operator+ 0x59BFA0 (out oculto = po, a = pa, b = pb): para las doce componentes float(a + b) (a en el registro, b en memoria), leidas todas antes de escribir; +0x40 y +0x44 a cero; EAX = po; el relleno
    (+0x0C, +0x1C, +0x2C, +0x3C) es basura de la pila y el banco lo restaura a su valor inicial.
  6 CVector::FromMultiply3x3 0x59C6D0 (this = po, matriz = pa, vector = pb): x = (m8*v.z + m4*v.y) + m0*v.x, y = (m9*v.z + m1*v.x) + m5*v.y, z = (m10*v.z + m2*v.x) + m6*v.y; cada componente se escribe antes de
    calcular la siguiente (la memoria viva).

Reglas de NaN de la x87 medidas en el hardware (micro-prueba con fld/fadd/fmul de memoria y faddp de registros): con un NaN senalizante en el operando de memoria gana el NaN del registro (aunque tenga
menor significando) y, si el registro no es NaN, el senalizante silenciado; entre dos NaN silenciosos gana el de mayor significando; fld convierte los senalizantes en silenciosos; las operaciones invalidas
(inf*0, inf-inf) dan el indefinido 0xFFC00000. El empate de significando con signo distinto (que solo crean los fchs de Rotate) lo gana el NaN de signo positivo (medido con las filas de Rotate: 4246 casos). Angulos con modulo entre 2^19 y 2^62: no se generan.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

INDEF = ('n', 1, 0x400000)


def isnan(a):
    return a[0] == 'n'


def load(bits):
    """fld dword: float -> valor del modelo (los NaN salen silenciados)"""
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return ('n', neg, m | 0x400000) if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def is_snan_bits(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0 and (bits & 0x400000) == 0


def rnd(v, quantum_exp_min=None):
    """redondea al mas cercano, empate a par, a 24 bits de mantisa (con exponente ilimitado salvo quantum_exp_min)"""
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


def pick_nan(a, b):
    """resultado cuando algun operando es NaN: el de mayor significando; si solo hay uno, ese"""
    if isnan(a) and isnan(b):
        if a[2] == b[2]:   # mismo significando y distinto signo (lo crean los fchs de Rotate): medido en el hardware, gana el de signo positivo
            return b if (a[1] == 1 and b[1] == 0) else a
        return a if a[2] > b[2] else b
    return a if isnan(a) else b


def mul(a, b):
    if isnan(a) or isnan(b):
        return pick_nan(a, b)
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return INDEF
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def mul_mem(reg, membits):
    """fmul dword ptr [mem] con el registro en ST0: un NaN senalizante de memoria pierde frente a un NaN del registro"""
    if is_snan_bits(membits):
        if isnan(reg):
            return reg
        return load(membits)
    return mul(reg, load(membits))


def add(a, b):
    if isnan(a) or isnan(b):
        return pick_nan(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else INDEF
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def sqrt24(a):
    if isnan(a):
        return a
    if a[0] == 'i':
        return a if a[1] == 0 else INDEF
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return INDEF
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b puede ser cero: divide-by-zero enmascarado da infinito)"""
    if isnan(a) or isnan(b):
        return pick_nan(a, b)
    if a[0] == 'i':
        return INDEF if b[0] == 'i' else ('i', a[1] ^ (b[2] if b[1] == 0 else (1 if b[1] < 0 else 0)))
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return INDEF
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('i', sa ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def store(a):
    """fstp dword: bits del float (los NaN conservan los 23 bits altos de la carga)"""
    if isnan(a):
        return (a[1] << 31) | 0x7F800000 | a[2]
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
    if r == 0:
        return sign << 31
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo"""
    if isnan(a):
        return ((1 << 63) | (a[2] << 40)).to_bytes(8, 'little') + (((a[1] << 15) | 0x7FFF)).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + ((neg << 15)).to_bytes(2, 'little')
    sign = 1 if v < 0 else 0
    m = abs(v)
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    mant = m / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return int(mant).to_bytes(8, 'little') + ((sign << 15) | (e + 16383)).to_bytes(2, 'little')


def fp_stack(hexstr):
    """(TOP, lista de (st, 10 bytes)) de las posiciones validas de una imagen fnsave normalizada"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return top, valid



MARKS = []
GUARD = 0x40
M32 = 0xFFFFFFFF
ONE = ('f', Fraction(1), 0)
FLT_MAX = Fraction(0xFFFFFF, 1) * Fraction(2) ** 104
FLT_MIN = Fraction(2) ** -126


def neg(a):
    if isnan(a):
        return a
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, neg(b))


def note_mem(reg, membits):
    if is_snan_bits(membits):
        MARKS.append('snan_mem_reg_nan' if isnan(reg) else 'snan_mem_alone')
    elif isnan(reg) and (membits & 0x7F800000) == 0x7F800000 and (membits & 0x7FFFFF):
        MARKS.append('two_qnan')


def mul_m(reg, membits):
    """fmul dword ptr [mem] con el registro en ST0: un NaN senalizante de memoria pierde frente a un NaN del registro (medido en el hardware)"""
    note_mem(reg, membits)
    if is_snan_bits(membits):
        return reg if isnan(reg) else load(membits)
    return mul(reg, load(membits))


def add_m(reg, membits):
    note_mem(reg, membits)
    if is_snan_bits(membits):
        return reg if isnan(reg) else load(membits)
    return add(reg, load(membits))


def sub_m(reg, membits):
    note_mem(reg, membits)
    if is_snan_bits(membits):
        return reg if isnan(reg) else load(membits)
    return sub(reg, load(membits))


def ld(bits):
    return load(bits)


def bytes_of(words):
    out = []
    for w in words:
        out += [(w >> (8 * j)) & 0xFF for j in range(4)]
    return out


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def classify(words, marks):
    nans = [w for w in words if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF)]
    if nans:
        marks.append('nan_in')
    if any(is_snan_bits(w) for w in nans):
        marks.append('snan_in')
    if len(nans) >= 2:
        marks.append('multi_nan')
    if any((w & 0x7FFFFFFF) == 0x7F800000 for w in words):
        marks.append('inf_in')
    if any((w & 0x7FFFFFFF) == 0 for w in words):
        marks.append('zero_in')
    if any(w == 0x80000000 for w in words):
        marks.append('neg_zero_in')
    if any(0 < (w & 0x7FFFFFFF) < 0x00800000 for w in words):
        marks.append('denormal_in')


def mark_out(old_val, new_bits, marks):
    w = new_bits
    if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
        marks.append('out_nan')
    elif (w & 0x7FFFFFFF) == 0x7F800000:
        marks.append('out_inf')
    elif (w & 0x7FFFFFFF) == 0:
        marks.append('out_zero')
        if old_val is not None and not isnan(old_val) and old_val[0] == 'f' and old_val[1] != 0:
            marks.append('out_underflow_zero')
    elif (w & 0x7FFFFFFF) < 0x00800000:
        marks.append('out_denormal')
    else:
        marks.append('out_finite')
    if old_val is not None and not isnan(old_val) and old_val[0] == 'f' and abs(old_val[1]) > FLT_MAX and (w & 0x7FFFFFFF) == 0x7F800000:
        marks.append('out_overflow_inf')


def rawfloat(bits):
    import struct
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def res_mark(r):
    if isnan(r):
        return 'res_nan'
    if r[0] == 'i':
        return 'res_inf'
    if r[1] == 0:
        return 'res_zero'
    if abs(r[1]) > FLT_MAX:
        return 'res_over_float'
    if abs(r[1]) < FLT_MIN:
        return 'res_under_float'
    return 'res_finite'


# ---- seno y coseno exactos (aritmetica entera de punto fijo con 400 bits fraccionarios)
P = 400
ONE_FP = 1 << P


def _atan_inv(n):
    x = ONE_FP // n
    total = x
    n2 = n * n
    k = 1
    sign = -1
    while x:
        x //= n2
        k += 2
        total += sign * (x // k)
        sign = -sign
    return total


PI_FP = 4 * (4 * _atan_inv(5) - _atan_inv(239))


def _sincos_reduced(r):
    """seno y coseno de r (entero de punto fijo, |r| <= pi/4) por Taylor"""
    r2 = (r * r) >> P
    s = term = r
    k = 1
    while term:
        term = -((term * r2) >> P) // ((k + 1) * (k + 2))
        k += 2
        s += term
    c = term = ONE_FP
    k = 0
    while term:
        term = -((term * r2) >> P) // ((k + 1) * (k + 2))
        k += 2
        c += term
    return s, c


def sincos(x):
    """x: Fraction exacta. Devuelve (sin, cos) como Fractions con error < 2^-390."""
    xfp = x * ONE_FP
    xfp = xfp.numerator // xfp.denominator
    half_pi = PI_FP // 2
    n = (2 * xfp + half_pi) // (2 * half_pi)
    r = xfp - n * half_pi
    s, c = _sincos_reduced(r)
    q = n % 4
    if q == 0:
        sn, cs = s, c
    elif q == 1:
        sn, cs = c, -s
    elif q == 2:
        sn, cs = -s, -c
    else:
        sn, cs = -c, s
    return Fraction(sn, ONE_FP), Fraction(cs, ONE_FP)


def rnd_bits(v, bits):
    """redondea al mas cercano, empate a par, a `bits` bits de mantisa (exponente ilimitado)"""
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (e - (bits - 1))
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


_TRIG = {}


def trig(bits):
    """(fsin, fcos) de un float cargado con fld: valores de 64 bits de mantisa (la precision de la x87 para las transcendentes); NaN -> el propio NaN silenciado, infinito -> indefinido,
    |x| >= 2^63 -> la x87 deja el operando (C2 = 1) y sin = cos = x"""
    if bits in _TRIG:
        return _TRIG[bits]
    a = load(bits)
    e = (bits >> 23) & 0xFF
    if isnan(a):
        r = (a, a)
    elif a[0] == 'i':
        r = (INDEF, INDEF)
    elif e >= 127 + 63:
        r = (a, a)
    elif a[1] == 0:
        r = (a, ONE)
    else:
        s, c0 = sincos(a[1])
        r = (('f', rnd_bits(s, 64), 1 if s < 0 else 0), ('f', rnd_bits(c0, 64), 1 if c0 < 0 else 0))
    _TRIG[bits] = r
    return r


def chs(a):
    if isnan(a):
        return ('n', a[1] ^ 1, a[2])
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def classify_angle(w, marks):
    e = (w >> 23) & 0xFF
    m = w & 0x7FFFFF
    if e == 0xFF:
        marks.append('ang_nan' if m else 'ang_inf')
        if m and not (m & 0x400000):
            marks.append('ang_snan')
    elif (w & 0x7FFFFFFF) == 0:
        marks.append('ang_zero')
    elif e == 0:
        marks.append('ang_denormal')
    elif e >= 127 + 63:
        marks.append('ang_huge')
    elif e >= 127 + 13:
        marks.append('ang_large')
    elif e >= 127 + 3:
        marks.append('ang_mid')
    else:
        marks.append('ang_small')
    if w >> 31:
        marks.append('ang_neg')


def sinf(bits):
    """(S, C, Sf) de un angulo: fsin y fcos (64 bits) y el seno guardado como float (fstp a la pila)"""
    sn, cs = trig(bits)
    return sn, cs, store(sn)


def trig_set(bits):
    """(S, C) de un angulo como valores del modelo"""
    sn, cs = trig(bits)
    return sn, cs


def expected(c):
    global MARKS
    MARKS = []
    fn = c['fn']
    words = list(c['mem'])
    pa, pb, po = c['pa'], c['pb'], c['po']
    new = list(words)
    eax_out = False
    events = []
    ins = []
    m = words[po:po + 16]      # matriz de las funciones 0..4 (una sola; se lee entera antes de escribir)
    if fn in (0, 1):
        ab = c['f0']
        classify_angle(ab, MARKS)
        S, C, Sf = sinf(ab)
        ins = [ab]
        if fn == 0:
            res = {0: 0x3F800000, 1: 0, 2: 0, 4: 0, 8: 0}
            for k, r in ((5, C), (6, S), (9, chs(S)), (10, C)):
                res[k] = store(r)
                mark_out(r, res[k], MARKS)
        else:
            res = {5: 0x3F800000, 1: 0, 4: 0, 6: 0, 9: 0}
            for k, r in ((0, C), (2, chs(S)), (8, S), (10, C)):
                res[k] = store(r)
                mark_out(r, res[k], MARKS)
        for k, v in res.items():
            new[po + k] = v
    elif fn == 2:
        xb, yb, zb = c['f0'], c['f1'], c['f2']
        for a in (xb, yb, zb):
            classify_angle(a, MARKS)
        ins = [xb, yb, zb] + m[0:3] + m[4:7] + m[8:11] + m[12:15]
        sx, cx = trig_set(xb)
        sy, cy = trig_set(yb)
        sz, cz = trig_set(zb)
        f1 = store(sx)
        f2 = store(sz)
        p1 = mul_m(sz, f1)
        f3 = store(mul_m(cz, f1))
        f4 = store(sub(mul(cz, cy), mul(p1, sy)))
        f5 = store(add(mul(ld(f3), sy), mul(ld(f2), cy)))
        f6 = store(chs(mul(sy, cx)))
        f7 = store(chs(mul(ld(f2), cx)))
        f8 = store(mul(cz, cx))
        f9 = store(add(mul(cz, sy), mul(p1, cy)))
        f10 = store(sub(mul(ld(f2), sy), mul(ld(f3), cy)))
        cc = mul(cx, cy)
        # right: el orden de sumas es (f9*m2 + f7*m1) + f4*m0; las demas filas (f4*a + f9*c) + f7*b. El primer factor es el valor de la pila y el segundo la memoria.
        x0, y0, z0 = m[0], m[1], m[2]
        out = {0: add(add(mul_m(ld(f9), z0), mul_m(ld(f7), y0)), mul_m(ld(f4), x0)),
               1: add(add(mul_m(ld(f10), z0), mul_m(ld(f8), y0)), mul_m(ld(f5), x0)),
               2: add(add(mul_m(cc, z0), mul_m(ld(f1), y0)), mul_m(ld(f6), x0))}
        for b0 in (4, 8, 12):
            x0, y0, z0 = m[b0], m[b0 + 1], m[b0 + 2]
            out[b0] = add(add(mul_m(ld(f4), x0), mul_m(ld(f9), z0)), mul_m(ld(f7), y0))
            out[b0 + 1] = add(add(mul_m(ld(f5), x0), mul_m(ld(f10), z0)), mul_m(ld(f8), y0))
            out[b0 + 2] = add(add(mul_m(ld(f6), x0), mul_m(cc, z0)), mul_m(ld(f1), y0))
        for k, r in out.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 3:
        ux, uy, uz = c['f0'], c['f1'], c['f2']
        ins = [ux, uy, uz] + m[4:7]
        A = sub(mul_m(ld(uz), m[5]), mul_m(ld(uy), m[6]))
        B = sub(mul_m(ld(ux), m[6]), mul_m(ld(uz), m[4]))
        Cc = sub(mul_m(ld(uy), m[4]), mul_m(ld(ux), m[5]))
        cf = store(Cc)
        D = sub(mul_m(Cc, uy), mul_m(B, uz))
        E = sub(mul_m(A, uz), mul_m(ld(cf), ux))
        F = mul(ld(ux), B)
        G = sub(F, mul_m(A, uy))
        new[po + 2] = cf
        new[po + 4] = store(D)
        new[po + 5] = store(E)
        new[po + 8] = ux
        new[po + 10] = uz
        new[po + 0] = store(A)
        new[po + 6] = store(G)
        new[po + 9] = uy
        new[po + 1] = store(B)
        for k in (0, 1, 2, 4, 5, 6):
            mark_out(None, new[po + k], MARKS)
    elif fn == 4:
        qx, qy, qz, qr = words[pb:pb + 4]
        ins = [qx, qy, qz, qr]
        ONEB = ('f', Fraction(1), 0)
        qxr, qyr = ld(qx), ld(qy)
        v2x = store(add(qxr, qxr))
        v2y = store(add(qyr, qyr))
        v2z = store(add(ld(qz), ld(qz)))
        x2x = store(mul(ld(v2x), qxr))
        y2x = store(mul(ld(v2y), qxr))
        z2x = store(mul(ld(v2z), qxr))
        y2y = store(mul(ld(v2y), qyr))
        z2y = mul(ld(v2z), qyr)
        z2z = mul_m(ld(v2z), qz)
        x2r = store(mul_m(ld(v2x), qr))
        y2r = mul_m(ld(v2y), qr)
        z2r = mul_m(ld(v2z), qr)
        res = {
            0: sub(ONEB, add_m(z2z, y2y)),
            1: add(ld(y2x), z2r),
            2: sub(ld(z2x), y2r),
            4: sub(ld(y2x), z2r),
            5: sub(ONEB, add_m(z2z, x2x)),
            6: add(ld(x2r), z2y),
            8: add_m(y2r, z2x),
            9: sub_m(z2y, x2r),
            10: sub(ONEB, add_m(ld(y2y), x2x)),
        }
        for k, r in res.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 5:
        a_idx = (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)
        ins = [words[pa + k] for k in a_idx] + [words[pb + k] for k in a_idx]
        outv = {}
        for k in a_idx:
            r = add_m(ld(words[pa + k]), words[pb + k])
            outv[k] = (r, store(r))
        for k, (r, bits) in outv.items():
            new[po + k] = bits
            mark_out(r, bits, MARKS)
        new[po + 16] = 0
        new[po + 17] = 0
        for k in (3, 7, 11, 15):     # el relleno es basura de la pila: el banco lo restaura a su valor inicial
            new[po + k] = words[po + k]
        eax_out = True
    else:
        # 6: FromMultiply3x3: this = po; matriz = pa; vector = pb; memoria viva
        ins = [words[pa + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10)] + words[pb:pb + 3]
        terms = (((8, 2), (4, 1), (0, 0)), ((9, 2), (1, 0), (5, 1)), ((10, 2), (2, 0), (6, 1)))
        for comp, (t1, t2, t3) in enumerate(terms):
            p1 = mul_m(ld(new[pa + t1[0]]), new[pb + t1[1]])
            p2 = mul_m(ld(new[pa + t2[0]]), new[pb + t2[1]])
            p3 = mul_m(ld(new[pa + t3[0]]), new[pb + t3[1]])
            r = add(add(p1, p2), p3)
            new[po + comp] = store(r)
            mark_out(r, new[po + comp], MARKS)
    classify(ins, MARKS)
    MARKS.append('mode%d' % c['mode'])
    old_b, new_b = bytes_of(words), bytes_of(new)
    changes = [[0, i, new_b[i]] for i in range(len(new_b)) if old_b[i] != new_b[i]]
    return {'marks': list(MARKS), 'changes': changes, 'st0': None, 'eax_out': eax_out, 'fn': fn, 'po': po, 'events': events}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('events') != e['events']:
        return 'eventos %s != %s' % (str(o.get('events'))[:300], str(e['events'])[:300])
    if e['eax_out']:
        if o.get('exit_eax') != tag(0, 4 * e['po']):
            return 'EAX de salida %s != out' % o.get('exit_eax')
    elif 'exit_eax' in o:
        return 'EAX de salida inesperado'
    top, valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        want = ext80(e['st0'])
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != want:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], want.hex())
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
