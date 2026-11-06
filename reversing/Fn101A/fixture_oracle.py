"""Oraculo independiente (Python) de Fn101A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote 101A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente
ampliado, NaN con carga util) se modela con fracciones exactas; fsin/fcos con enteros de punto fijo de 400 bits redondeados a 64 bits de mantisa; fsqrt y la division 1/x a 24 bits exactos.

Funciones (desensamblado del EXE; en las que combinan operandos, el primer factor o sumando va en el registro de la x87 (fld: un NaN senalizante se silencia al cargarlo) y el segundo en memoria
(fmul/fadd/fsub con operando float de memoria: un NaN senalizante de memoria pierde frente a un NaN del registro; entre dos registros, o con un silencioso de memoria, gana el NaN de mayor significando).
Se escribe S = fsin(a) y C = fcos(a) (64 bits de mantisa, sin redondear a float mientras estan en la pila) y Sf = float(S) (fstp/fst a la pila):
  0 SetRotateX 0x59B060: m0 = 1.0; m1, m2, m4, m8 y la posicion a cero; m5 = float(C), m6 = float(S), m9 = float(-S), m10 = float(C). 1 SetRotateY 0x59B0A0: m5 = 1.0; m1, m4, m6, m9 y pos a cero;
    m0 = float(C), m2 = float(-S), m8 = float(S), m10 = float(C). 2 SetRotateZ 0x59B0E0: m10 = 1.0; m2, m6, m8, m9 y pos a cero; m0 = float(C), m1 = float(S), m4 = float(-S), m5 = float(C).
  3 RotateX 0x59B1E0: por fila (1,2), (5,6), (9,10), (13,14): y' = C*y - Sf*z, z' = Sf*y + C*z (todo se lee antes de escribir; C sin redondear a float); 4 RotateY 0x59B2C0: por fila (0,2), (4,6), (8,10),
    (12,14): x' = Sf*z + C*x, z' = C*z - Sf*x, y en la fila de right x' = S*z + C*x con S SIN redondear a float; 5 RotateZ 0x59B390: por fila (0,1), (4,5), (8,9), (12,13): x' = C*x - Sf*y, y' = Sf*x + C*y.
  6 Reorthogonalise 0x59B6A0: producto vectorial de right y forward con 1/sqrt a 24 bits, tres pasadas (ver expected); escribe m0, m1, m2, m4, m5, m6, m8, m9 y m10.
  7 CopyToRwMatrix 0x59B8B0: copia los 12 dwords de la matriz (pa) a la RwMatrix (pb) de dword en dword (leyendo la memoria viva) y llama a RwMatrixUpdate (evento 50 con la RwMatrix).
  8 Invert(in) 0x59BDD0: Invert(in, tmp) 0x59B920 (evento 54 con in y el temporal de pila 0x7F000000; el falso escribe tmp[k] = in[k] ^ 0x9E3779B1*(k+1) para k < 16), copia los 16 dwords de tmp
    al resultado (po) y pone a cero +0x40 y +0x44; RwMatrixDestroy no se llama; EAX = po.
  9 CVector::FromMultiply 0x59C670: x = ((m8*v.z + m4*v.y) + m0*v.x) + m12, y = ((m9*v.z + m1*v.x) + m5*v.y) + m13, z = ((m10*v.z + m2*v.x) + m6*v.y) + m14; cada componente se escribe antes de calcular
    la siguiente (la memoria viva: un destino solapado con el vector o la matriz ve las escrituras anteriores).

Reglas de NaN de la x87 medidas en el hardware (micro-prueba con fld/fadd/fmul de memoria y faddp de registros): con un NaN senalizante en el operando de memoria gana el NaN del registro (aunque tenga
menor significando) y, si el registro no es NaN, el senalizante silenciado; entre dos NaN silenciosos gana el de mayor significando; fld convierte los senalizantes en silenciosos; las operaciones invalidas
(inf*0, inf-inf) dan el indefinido 0xFFC00000. NO se modela el empate de significando con signo distinto (los fixtures lo evitan). Angulos con modulo entre 2^19 y 2^62: no se generan.

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


def sinf(bits):
    """(S, C, Sf) de un angulo: fsin y fcos (64 bits) y el seno guardado como float (fstp a la pila)"""
    sn, cs = trig(bits)
    return sn, cs, store(sn)


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
    m = words[po:po + 16]      # matriz de las funciones 0..6 (una sola; se lee entera antes de escribir)
    if fn in (0, 1, 2, 3, 4, 5):
        ab = c['f0']
        classify_angle(ab, MARKS)
        S, C, Sf = sinf(ab)
        ins = [ab] + (m[0:3] + m[4:7] + m[8:11] + m[12:15] if fn >= 3 else [])
    if fn == 0:
        res = {0: 0x3F800000, 1: 0, 2: 0, 4: 0, 8: 0, 12: 0, 13: 0, 14: 0}
        for k, r in ((5, C), (6, S), (9, chs(S)), (10, C)):
            res[k] = store(r)
            mark_out(r, res[k], MARKS)
        for k, v in res.items():
            new[po + k] = v
    elif fn == 1:
        res = {5: 0x3F800000, 1: 0, 4: 0, 6: 0, 9: 0, 12: 0, 13: 0, 14: 0}
        for k, r in ((0, C), (2, chs(S)), (8, S), (10, C)):
            res[k] = store(r)
            mark_out(r, res[k], MARKS)
        for k, v in res.items():
            new[po + k] = v
    elif fn == 2:
        res = {10: 0x3F800000, 2: 0, 6: 0, 8: 0, 9: 0, 12: 0, 13: 0, 14: 0}
        for k, r in ((0, C), (1, S), (4, chs(S)), (5, C)):
            res[k] = store(r)
            mark_out(r, res[k], MARKS)
        for k, v in res.items():
            new[po + k] = v
    elif fn == 3:
        # RotateX: (y', z') = (C*y - Sf*z, Sf*y + C*z) por fila; el primer factor va en el registro y el segundo en memoria
        out = {}
        for y, z in ((1, 2), (5, 6), (9, 10), (13, 14)):
            r1 = sub(mul_m(C, m[y]), mul_m(ld(Sf), m[z]))
            r2 = add(mul_m(ld(Sf), m[y]), mul_m(C, m[z]))
            out[y], out[z] = r1, r2
        for k, r in out.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 4:
        # RotateY: (x', z') = (Sf*z + C*x, C*z - Sf*x) por fila; en la fila de right x' usa S sin redondear a float
        out = {}
        for x, z in ((0, 2), (4, 6), (8, 10), (12, 14)):
            sreg = S if x == 0 else ld(Sf)
            r1 = add(mul_m(sreg, m[z]), mul_m(C, m[x]))
            r2 = sub(mul_m(C, m[z]), mul_m(ld(Sf), m[x]))
            out[x], out[z] = r1, r2
        for k, r in out.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 5:
        # RotateZ: (x', y') = (C*x - Sf*y, Sf*x + C*y) por fila
        out = {}
        for x, y in ((0, 1), (4, 5), (8, 9), (12, 13)):
            r1 = sub(mul_m(C, m[x]), mul_m(ld(Sf), m[y]))
            r2 = add(mul_m(ld(Sf), m[x]), mul_m(C, m[y]))
            out[x], out[y] = r1, r2
        for k, r in out.items():
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 6:
        ins = m[0:3] + m[4:7]
        ONEF = 0x3F800000
        X1 = sub(mul_m(ld(m[1]), m[6]), mul_m(ld(m[5]), m[2]))
        Y1 = sub(mul_m(ld(m[4]), m[2]), mul_m(ld(m[0]), m[6]))
        yf = store(Y1)
        Z1 = sub(mul_m(ld(m[5]), m[0]), mul_m(ld(m[4]), m[1]))
        zf = store(Z1)
        G = mul_m(Z1, zf)
        H = mul_m(ld(yf), yf)
        K = add(add(G, H), mul(X1, X1))
        R = sqrt24(K)
        res = div(ld(ONEF), R)
        invf = store(res)
        nx = mul(X1, res)
        ny = mul_m(ld(invf), yf)
        nz = mul_m(ld(invf), zf)
        nzf = store(nz)
        P1 = mul_m(nz, m[5])
        P2 = mul_m(ny, m[6])
        Q1 = sub(P1, P2)
        P3 = mul_m(nx, m[6])
        P4 = mul_m(ld(nzf), m[4])
        Q2 = sub(P3, P4)
        q2f = store(Q2)
        Q3 = sub(mul_m(ny, m[4]), mul_m(nx, m[5]))
        q3f = store(Q3)
        S1 = mul_m(Q3, q3f)
        S2 = mul_m(ld(q2f), q2f)
        S5 = add(add(S1, S2), mul(Q1, Q1))
        R2 = sqrt24(S5)
        r2v = div(ld(ONEF), R2)
        inv2f = store(r2v)
        T1 = mul_m(Q1, inv2f)
        T2 = mul_m(ld(q2f), inv2f)
        T3 = mul_m(ld(q3f), inv2f)
        t3f = store(T3)
        U1 = mul(T3, ny)
        U2 = mul_m(T2, nzf)
        V1 = sub(U1, U2)
        v1f = store(V1)
        W1 = mul_m(T1, nzf)
        W2 = mul(ld(t3f), nx)
        V2 = sub(W1, W2)
        v2f = store(V2)
        X2 = mul(T2, nx)
        X3 = mul(T1, ny)
        Y2 = sub(X2, X3)
        y2f = store(Y2)
        new[po + 2] = t3f
        new[po + 4] = v1f
        new[po + 5] = v2f
        new[po + 10] = nzf
        new[po + 6] = y2f
        new[po + 0] = store(T1)
        new[po + 1] = store(T2)
        new[po + 8] = store(nx)
        new[po + 9] = store(ny)
        for k in (0, 1, 2, 4, 5, 6, 8, 9, 10):
            mark_out(None, new[po + k], MARKS)
        MARKS.append(res_mark(K))
        MARKS.append('sum2_' + res_mark(S5)[4:])
    elif fn == 7:
        # CopyToRwMatrix(matriz = pa, RwMatrix = pb): dword a dword con la memoria viva
        ins = [words[pa + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)]
        for row in (0, 4, 8, 12):
            for k in (0, 1, 2):
                new[pb + row + k] = new[pa + row + k]
        events = [[50, tag(0, 4 * pb)]]
        if pb == pa:
            MARKS.append('alias_same')
        elif pb != pa and (abs(pb - pa) < 16):
            MARKS.append('alias_shift')
    elif fn == 8:
        # Invert(in): in = pa; resultado oculto = po; Invert(in, tmp) falso; copia de los 16 dwords
        ins = [words[pa + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)]
        tmp = [(words[pa + k] ^ ((0x9E3779B1 * (k + 1)) & M32)) & M32 for k in range(16)]
        for k in range(16):
            new[po + k] = tmp[k]
        new[po + 16] = 0
        new[po + 17] = 0
        events = [[54, tag(0, 4 * pa), 0x7F000000]]
        eax_out = True
    else:
        if fn == 9:
            # FromMultiply: this = po; matriz = pa; vector = pb; memoria viva
            ins = [words[pa + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)] + words[pb:pb + 3]
            terms = (((8, 2), (4, 1), (0, 0), 12), ((9, 2), (1, 0), (5, 1), 13), ((10, 2), (2, 0), (6, 1), 14))
            for comp, (t1, t2, t3, pos) in enumerate(terms):
                p1 = mul_m(ld(new[pa + t1[0]]), new[pb + t1[1]])
                p2 = mul_m(ld(new[pa + t2[0]]), new[pb + t2[1]])
                p3 = mul_m(ld(new[pa + t3[0]]), new[pb + t3[1]])
                r = add_m(add(add(p1, p2), p3), new[pa + pos])
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
