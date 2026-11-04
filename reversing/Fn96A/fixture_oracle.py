"""Oraculo independiente (Python) de Fn96A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las cuatro funciones del lote 96A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego,
exponente ampliado, NaN con carga util) se modela con fracciones exactas.

Funcion 0, CVector::Normalise (0x59C910..0x59C960; thiscall, ECX = vector, `ret`; void): dot = ((x*x + y*y) + z*z) en la x87 (fld, fmul, faddp, sin guardar); si dot == 0 (ordenado: con NaN sigue el camino
  normal) solo guarda x = 1.0f; si no r = 1.0 / raiz(dot) (fsqrt, fdivr [1.0f]) y guarda x = r * x, y = r * y, z = r * z (fmul con operando de memoria float, un redondeo a 24 bits y otro al guardar).
Funcion 1, CVector::NormaliseAndMag (0x59C970..0x59C9CE): igual, y devuelve en ST0 1.0 / r (fdivr [1.0f], sin guardar como float); con dot == 0, x = 1.0f y ST0 = 1.0.
Funcion 2, CVector::Magnitude (0x4082C0..0x4082E0; `ret`): ST0 = raiz(((x*x + y*y) + z*z)) sin guardar.
Funcion 3, CVector::Magnitude2D (0x406D50..0x406D65): ST0 = raiz(x*x + y*y) sin guardar.
Ninguna toca ECX ni EDX; Normalise y NormaliseAndMag cambian solo AX (fnstsw ax); Magnitude y Magnitude2D no tocan EAX.

Reglas de NaN de la x87 (Intel SDM vol. 1, tabla 4-7): el operando float de memoria con un NaN senalizante pierde frente a un NaN silencioso del registro; entre dos NaN silenciosos gana el de mayor
significando; fld convierte los senalizantes en silenciosos; las operaciones invalidas (inf*0, inf-inf, inf/inf, 0/0) dan el indefinido 0xFFC00000.

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


GUARD = 0x40
M32 = 0xFFFFFFFF
ONE = ('f', Fraction(1), 0)
FLT_MAX = Fraction(0xFFFFFF, 1) * Fraction(2) ** 104
FLT_MIN = Fraction(2) ** -126


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def bytes_of(words):
    out = []
    for w in words:
        out += [(w >> (8 * j)) & 0xFF for j in range(4)]
    return out


def classify_inputs(a, marks):
    nans = [w for w in a[:3] if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF)]
    infs = [w for w in a[:3] if (w & 0x7FFFFFFF) == 0x7F800000]
    if nans:
        marks.append('nan_in')
    if any(is_snan_bits(w) for w in nans):
        marks.append('snan_in')
    if len(nans) >= 2:
        marks.append('multi_nan')
        if len({load(w)[2] for w in nans}) >= 2:
            marks.append('multi_nan_distinct')
    if any((w & 0x7FFFFF) | 0x400000 != 0x400000 for w in nans):
        marks.append('nan_payload')
    if infs:
        marks.append('inf_in')
    if any(w & 0x7FFFFFFF == 0 for w in a[:3]):
        marks.append('zero_in')
    if any(w == 0x80000000 for w in a[:3]):
        marks.append('neg_zero_in')
    if any(0 < (w & 0x7FFFFFFF) < 0x00800000 for w in a[:3]):
        marks.append('denormal_in')


def mark_value(r, marks, prefix):
    if isnan(r):
        marks.append(prefix + '_nan')
    elif r[0] == 'i':
        marks.append(prefix + '_inf')
    elif r[1] == 0:
        marks.append(prefix + '_zero')
    elif abs(r[1]) > FLT_MAX:
        marks.append(prefix + '_over_float')
    elif abs(r[1]) < FLT_MIN:
        marks.append(prefix + '_under_float')
    else:
        marks.append(prefix + '_finite')


def dot3(a):
    x, y, z = (load(w) for w in a[:3])
    return add(add(mul(x, x), mul(y, y)), mul(z, z))


def expected_norm(c, with_mag):
    a = c['a']
    marks = []
    classify_inputs(a, marks)
    dot = dot3(a)
    mark_value(dot, marks, 'dot')
    new = list(a[:4])
    if not isnan(dot) and dot[0] == 'f' and dot[1] == 0:
        marks.append('zero_path')
        new[0] = 0x3F800000
        st0 = ONE if with_mag else None
    else:
        marks.append('normal_path')
        r = div(ONE, sqrt24(dot))
        for k in range(3):
            new[k] = store(mul_mem(r, a[k]))
        st0 = div(ONE, r) if with_mag else None
        for k in range(3):
            w = new[k]
            if (w & 0x7FFFFFFF) == 0 and (a[k] & 0x7FFFFFFF) != 0:
                marks.append('out_underflow_zero')
            if 0 < (w & 0x7FFFFFFF) < 0x00800000:
                marks.append('out_denormal')
            if (w & 0x7FFFFFFF) == 0x7F800000 and ((a[k] & 0x7FFFFFFF) != 0x7F800000):
                marks.append('out_inf_from_finite')
            if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
                marks.append('out_nan')
    if st0 is not None:
        mark_value(st0, marks, 'res')
    old_b, new_b = bytes_of(a[:4]), bytes_of(new)
    changes = [[0, i, new_b[i]] for i in range(16) if old_b[i] != new_b[i]]
    return {'marks': marks, 'changes': changes, 'st0': st0}


def expected_mag(c, two_d):
    a = c['a']
    marks = []
    classify_inputs(a, marks)
    x, y, z = (load(w) for w in a[:3])
    dot = add(mul(x, x), mul(y, y)) if two_d else add(add(mul(x, x), mul(y, y)), mul(z, z))
    mark_value(dot, marks, 'dot')
    st0 = sqrt24(dot)
    mark_value(st0, marks, 'res')
    return {'marks': marks, 'changes': [], 'st0': st0}


def expected(c):
    fn = c['fn']
    e = {0: lambda: expected_norm(c, False), 1: lambda: expected_norm(c, True), 2: lambda: expected_mag(c, False), 3: lambda: expected_mag(c, True)}[fn]()
    e['fn'] = fn
    return e


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('exit_ecx') != tag(0, 0):
        return 'ECX de salida %s != vector' % o.get('exit_ecx')
    if o.get('exit_edx') != c['edx']:
        return 'EDX de salida %s != %s' % (o.get('exit_edx'), c['edx'])
    if o.get('exit_eax_hi') != (c['eax'] >> 16):
        return 'EAX alto de salida %s != %s' % (o.get('exit_eax_hi'), c['eax'] >> 16)
    if e['fn'] >= 2 and o.get('exit_eax') != c['eax']:
        return 'EAX de salida %s != %s' % (o.get('exit_eax'), c['eax'])
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
