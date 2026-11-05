"""Oraculo independiente (Python) de Fn99A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote 99A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente
ampliado, NaN con carga util) se modela con fracciones exactas; fsin/fcos con enteros de punto fijo de 400 bits redondeados a 64 bits de mantisa.

Funciones (desensamblado del EXE; en las que combinan operandos, el primer factor o sumando va en el registro de la x87 (fld: un NaN senalizante se silencia al cargarlo) y el segundo en memoria
(fmul/fadd/fsub con operando float de memoria: un NaN senalizante de memoria pierde frente a un NaN del registro; entre dos registros, o con un silencioso de memoria, gana el NaN de mayor significando):
  0 CVector::Difference 0x40FE00: this.k = left.k - right.k (left en el registro), componente a componente en orden x, y, z (un solapamiento ve las escrituras anteriores).
  1 CMatrix::SetUnity 0x59AE70: 1.0 en +0x00, +0x14 y +0x28; ceros en +0x04, +0x08, +0x10, +0x18, +0x20, +0x24, +0x30, +0x34 y +0x38. 2 ResetOrientation 0x59AEA0: igual sin la posicion.
  3 SetScale(f) 0x59AED0: f (como dword) en +0x00, +0x14 y +0x28; ceros en los demas de las filas y la posicion. 4 SetScale(x,y,z) 0x59AF00: x, y, z en +0x00, +0x14, +0x28; ceros igual.
  5 SetTranslateOnly 0x59AF80: los tres dwords de pila en +0x30, +0x34, +0x38.
  6 SetRotate(x,y,z) 0x59B120: posicion a cero; fsin/fcos sin redondear a float salvo cos(y), sin(z) y cos(z)*sin(x), que se guardan en la pila como float; once productos y sumas a 24 bits (ver expected).
  7 Invert(in, out) 0x59B920: pone a cero +0x30..+0x38 de la salida, copia la rotacion transpuesta (dwords) y calcula la posicion sobre la propia salida: p = in.pos.x*o.right.x ..., tres terminos por
    componente y producto final por -1.0 (0x858C1C); todo sobre la memoria viva (un solapamiento ve las escrituras anteriores); EAX = out.
  8 MatrixMultiplyExe 0x59BE30: out = a * b con cada elemento (r1*m1 + r2*m2) + r3*m3 [+ a.pos]; lee todo antes de escribir; +0x40 y +0x44 a cero; el relleno de la salida no se compara; EAX = out.
  9 Multiply3x3VMExe 0x59C810: x = (m1*v.y + m2*v.z) + v.x*m0; y = (m5*v.y + m4*v.x) + m6*v.z; z = (m9*v.y + m8*v.x) + m10*v.z; lee todo antes de escribir; EAX = out.

Reglas de NaN de la x87 medidas en el hardware (micro-prueba con fld/fadd/fmul de memoria y faddp de registros): con un NaN senalizante en el operando de memoria gana el NaN del registro (aunque tenga
menor significando) y, si el registro no es NaN, el senalizante silenciado; entre dos NaN silenciosos gana el de mayor significando; fld convierte los senalizantes en silenciosos; las operaciones invalidas
(inf*0, inf-inf) dan el indefinido 0xFFC00000. NO se modela el empate de significando con signo distinto (los fixtures lo evitan). Angulos de SetRotate con modulo entre 2^19 y 2^62: no se generan.

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
        return a if a[2] >= b[2] else b
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


def expected(c):
    global MARKS
    MARKS = []
    fn = c['fn']
    words = list(c['mem'])
    pa, pb, po = c['pa'], c['pb'], c['po']
    new = list(words)
    eax_out = False
    ins = []
    ONES = (0, 5, 10)
    if fn == 0:
        # CVector::Difference: this.k = left.k - right.k, componente a componente (lee y escribe cada una antes de pasar a la siguiente)
        ins = words[pa:pa + 3] + words[pb:pb + 3]
        for k in range(3):
            r = sub_m(ld(new[pa + k]), new[pb + k])
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
            if r[0] == 'f' and r[1] == 0 and (new[pa + k] & 0x7FFFFFFF) and (words[pb + k] & 0x7FFFFFFF):
                MARKS.append('cancel_zero')
    elif fn in (1, 2):
        for k in ONES:
            new[po + k] = 0x3F800000
        for k in (1, 2, 4, 6, 8, 9) + ((12, 13, 14) if fn == 1 else ()):
            new[po + k] = 0
    elif fn == 3:
        ins = [c['f0']]
        for k in ONES:
            new[po + k] = c['f0']
        for k in (1, 2, 4, 6, 8, 9, 12, 13, 14):
            new[po + k] = 0
    elif fn == 4:
        ins = [c['f0'], c['f1'], c['f2']]
        new[po + 0], new[po + 5], new[po + 10] = c['f0'], c['f1'], c['f2']
        for k in (1, 2, 4, 6, 8, 9, 12, 13, 14):
            new[po + k] = 0
    elif fn == 5:
        ins = [c['f0'], c['f1'], c['f2']]
        new[po + 12], new[po + 13], new[po + 14] = c['f0'], c['f1'], c['f2']
    elif fn == 6:
        xb, yb, zb = c['f0'], c['f1'], c['f2']
        for w in (xb, yb, zb):
            classify_angle(w, MARKS)
        sx, cx = trig(xb)
        sy, cy = trig(yb)
        sz, cz = trig(zb)
        cyf = store(cy)                      # fstp [esp+4]
        szf = store(sz)                      # fst [esp+8]
        p1 = mul(sz, sx)                     # fmul st, st(3)
        p2 = mul(cz, sx)                     # fmul st, st(4)
        czsxf = store(p2)                    # fstp [esp+0Ch]
        p3 = mul_m(cz, cyf)                  # fmul [esp+4]
        p4 = mul(p1, sy)
        q1 = sub(p3, p4)
        p5 = mul(ld(czsxf), sy)
        p6 = mul_m(ld(szf), cyf)
        q2 = add(p5, p6)
        p7 = mul(sy, cx)
        p8 = mul(ld(szf), cx)
        p9 = mul(cz, cx)
        p10 = mul(cz, sy)
        p11 = mul_m(p1, cyf)
        q3 = add(p10, p11)
        p12 = mul(ld(szf), sy)
        p13 = mul_m(ld(czsxf), cyf)
        q4 = sub(p12, p13)
        p14 = mul(ld(cyf), cx)
        res = {0: q1, 1: q2, 2: chs(p7), 4: chs(p8), 5: p9, 6: sx, 8: q3, 9: q4, 10: p14}
        new[po + 12] = new[po + 13] = new[po + 14] = 0
        for k, r in res.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 7:
        # Invert(in, out): in = pa, out = po; todo sobre la memoria viva, en el orden del EXE (con solapamiento se ven las escrituras anteriores)
        i, o = pa, po
        ins = [words[i + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)]
        new[o + 12] = new[o + 13] = new[o + 14] = 0
        for do, di in ((0, 0), (1, 4), (2, 8), (4, 1), (5, 5), (6, 9), (8, 2), (9, 6), (10, 10)):
            new[o + do] = new[i + di]
        # (destino, registro (i/o), indice, memoria (i/o), indice, suma con el destino)
        seq = (
            (12, 'i', 12, 'o', 0, False), (13, 'o', 1, 'i', 12, False), (14, 'o', 2, 'i', 12, False),
            (12, 'i', 13, 'o', 4, True), (13, 'o', 5, 'i', 13, True), (14, 'o', 6, 'i', 13, True),
            (12, 'o', 8, 'i', 14, True), (13, 'i', 14, 'o', 9, True), (14, 'o', 10, 'i', 14, True),
        )
        for dst, rs, ri, ms, mi, acc in seq:
            reg = new[(i if rs == 'i' else o) + ri]
            mem = new[(i if ms == 'i' else o) + mi]
            r = mul_m(ld(reg), mem)
            if acc:
                r = add_m(r, new[o + dst])
            new[o + dst] = store(r)
        for k in (12, 13, 14):
            r = mul_m(ld(new[o + k]), 0xBF800000)
            new[o + k] = store(r)
            mark_out(r, new[o + k], MARKS)
        eax_out = True
    elif fn == 8:
        # MatrixMultiplyExe(out, a, b): a = pa, b = pb; todas las lecturas antes de las escrituras; +0x40 y +0x44 a cero; el relleno de la salida no se modela (el banco no lo compara)
        P = words[pa:pa + 16]
        Q = words[pb:pb + 16]
        ins = [P[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)] + [Q[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)]
        T = {
            0: [('P', 4, 'Q', 1), ('P', 0, 'Q', 0), ('Q', 2, 'P', 8)],
            1: [('P', 9, 'Q', 2), ('P', 1, 'Q', 0), ('P', 5, 'Q', 1)],
            2: [('P', 6, 'Q', 1), ('P', 10, 'Q', 2), ('Q', 0, 'P', 2)],
            4: [('Q', 6, 'P', 8), ('P', 4, 'Q', 5), ('Q', 4, 'P', 0)],
            5: [('P', 9, 'Q', 6), ('P', 5, 'Q', 5), ('P', 1, 'Q', 4)],
            6: [('Q', 4, 'P', 2), ('P', 10, 'Q', 6), ('P', 6, 'Q', 5)],
            8: [('P', 4, 'Q', 9), ('Q', 10, 'P', 8), ('Q', 8, 'P', 0)],
            9: [('P', 9, 'Q', 10), ('P', 5, 'Q', 9), ('Q', 8, 'P', 1)],
            10: [('Q', 8, 'P', 2), ('P', 10, 'Q', 10), ('P', 6, 'Q', 9)],
            12: [('P', 4, 'Q', 13), ('Q', 14, 'P', 8), ('Q', 12, 'P', 0)],
            13: [('Q', 14, 'P', 9), ('Q', 13, 'P', 5), ('Q', 12, 'P', 1)],
            14: [('Q', 12, 'P', 2), ('P', 6, 'Q', 13), ('P', 10, 'Q', 14)],
        }
        src = {'P': P, 'Q': Q}
        results = {}
        for k, terms in T.items():
            ps = [mul_m(ld(src[rp][ri]), src[mp][mi]) for rp, ri, mp, mi in terms]
            r = add(add(ps[0], ps[1]), ps[2])
            if k >= 12:
                r = add_m(r, P[k])
            results[k] = r
        for k, r in results.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
        new[po + 16] = 0
        new[po + 17] = 0
        # el relleno de la salida (+0x0C, +0x1C, +0x2C, +0x3C) conserva su valor en el modelo
        for k in (3, 7, 11, 15):
            new[po + k] = words[po + k]
        eax_out = True
    else:
        # Multiply3x3VMExe(out, vector, matriz): matriz = pb (derecha 0..2, adelante 4..6, arriba 8..10), vector = pa; todas las lecturas antes de las escrituras
        M = words[pb:pb + 11]
        V = words[pa:pa + 3]
        ins = [M[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10)] + V
        x = add(add(mul_m(ld(M[1]), V[1]), mul_m(ld(M[2]), V[2])), mul_m(ld(V[0]), M[0]))
        y = add(add(mul_m(ld(M[5]), V[1]), mul_m(ld(M[4]), V[0])), mul_m(ld(M[6]), V[2]))
        z = add(add(mul_m(ld(M[9]), V[1]), mul_m(ld(M[8]), V[0])), mul_m(ld(M[10]), V[2]))
        res = [x, y, z]
        eax_out = True
        for k in range(3):
            new[po + k] = store(res[k])
            mark_out(res[k], new[po + k], MARKS)
    classify(ins, MARKS)
    MARKS.append('mode%d' % c['mode'])
    old_b, new_b = bytes_of(words), bytes_of(new)
    changes = [[0, i, new_b[i]] for i in range(len(new_b)) if old_b[i] != new_b[i]]
    return {'marks': list(MARKS), 'changes': changes, 'st0': None, 'eax_out': eax_out, 'fn': fn, 'po': po}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
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
