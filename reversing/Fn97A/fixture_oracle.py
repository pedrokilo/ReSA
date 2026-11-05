"""Oraculo independiente (Python) de Fn97A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las doce funciones del lote 97A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente
ampliado, NaN con carga util) se modela con fracciones exactas.

Funciones (desensamblado del EXE; en las que combinan operandos, el primer factor o sumando va en el registro de la x87 (fld: un NaN senalizante se silencia al cargarlo) y el segundo en memoria
(fmul/fadd/fsub con operando float de memoria: un NaN senalizante de memoria pierde frente a un NaN del registro; entre dos registros, o con un silencioso de memoria, gana el NaN de mayor significando)):
  0 DotProduct 0x40FDB0: ST0 = ((a.z*b.z + a.y*b.y) + a.x*b.x), sin guardar como float.
  1 CrossProduct 0x59C730: x = b.z*a.y - a.z*b.y; y = a.z*b.x - b.z*a.x; z = a.x*b.y - b.x*a.y (productos registro * memoria); lee todo antes de escribir; EAX = out.
  2 Multiply3x3Exe 0x59C790: x = (m8*v.z + m4*v.y) + m0*v.x; y = (m9*v.z + m1*v.x) + m5*v.y; z = (m10*v.z + m2*v.x) + m6*v.y (m = dwords de la matriz: elemento en el registro, vector en memoria); EAX = out.
  3 VectorAdd 0x40FE30 / 4 VectorSubtract 0x40FE60: out.k = a.k +/- b.k (a en el registro, b en memoria), de z a x; lee todo antes de escribir; EAX = out.
  5 FloatTimesVector 0x40FE90 / 6 VectorTimesFloat 0x40FEC0: out.k = f * v.k (f en el registro, v.k en memoria); lee todo antes de escribir; EAX = out.
  7 CVector::operator*= 0x40FEF0: this.k = f * this.k, componente a componente (lee y escribe cada una antes de pasar a la siguiente).
  8 CVector::operator+= 0x411A00: this.k = v.k + this.k (v.k en el registro), componente a componente en orden x, y, z (un solapamiento ve las escrituras anteriores).
  9 RectMin 0x404330: ST0 = a solo si a < b (estricto); si no (mayor, igual o algun NaN: FCOM deja C0, C2 y C3 a 1, paridad par) b; cargado con fld (los NaN senalizantes salen silenciados).
 10 CColLine::Constructor 0x40EF50: copia los 3 dwords de `start` en +0x00 y los 3 de `end` en +0x10 (como enteros); EAX = this; no toca +0xC ni +0x1C.
 11 CMatrix::SetTranslate 0x59AF40: pone 1.0 en +0x00, +0x14 y +0x28; ceros en +0x04, +0x08, +0x10, +0x18, +0x20 y +0x24; la traslacion (3 dwords de pila) en +0x30, +0x34 y +0x38.

Reglas de NaN de la x87 medidas en el hardware (micro-prueba con fld/fadd/fmul de memoria y faddp de registros): con un NaN senalizante en el operando de memoria gana el NaN del registro (aunque tenga menor
significando) y, si el registro no es NaN, el senalizante silenciado; entre dos NaN silenciosos gana el de mayor significando; fld convierte los senalizantes en silenciosos; las operaciones invalidas
(inf*0, inf-inf) dan el indefinido 0xFFC00000. NO se modela el empate de significando con signo distinto (los fixtures lo evitan: ningun par de NaN de los fixtures comparte significando con signo distinto).

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


def expected(c):
    global MARKS
    MARKS = []
    fn = c['fn']
    words = list(c['mem'])
    pa, pb, po = c['pa'], c['pb'], c['po']
    A = words[pa:pa + 3]
    Bv = words[pb:pb + 3]
    st0 = None
    eax_out = False
    new = list(words)
    ins = []
    if fn == 0:
        ins = A + Bv
        p = [mul_m(ld(A[k]), Bv[k]) for k in (2, 1, 0)]
        st0 = add(add(p[0], p[1]), p[2])
        MARKS.append(res_mark(st0))
    elif fn == 1:
        ins = A + Bv
        x = sub(mul_m(ld(Bv[2]), A[1]), mul_m(ld(A[2]), Bv[1]))
        y = sub(mul_m(ld(A[2]), Bv[0]), mul_m(ld(Bv[2]), A[0]))
        z = sub(mul_m(ld(A[0]), Bv[1]), mul_m(ld(Bv[0]), A[1]))
        res = [x, y, z]
        eax_out = True
        for k in range(3):
            new[po + k] = store(res[k])
            mark_out(res[k], new[po + k], MARKS)
            if res[k][0] == 'f' and res[k][1] == 0:
                MARKS.append('cancel_zero')
    elif fn == 2:
        M = words[pb:pb + 11]
        V = words[pa:pa + 3]
        ins = [M[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10)] + V
        x = add(add(mul_m(ld(M[8]), V[2]), mul_m(ld(M[4]), V[1])), mul_m(ld(M[0]), V[0]))
        y = add(add(mul_m(ld(M[9]), V[2]), mul_m(ld(M[1]), V[0])), mul_m(ld(M[5]), V[1]))
        z = add(add(mul_m(ld(M[10]), V[2]), mul_m(ld(M[2]), V[0])), mul_m(ld(M[6]), V[1]))
        res = [x, y, z]
        eax_out = True
        for k in range(3):
            new[po + k] = store(res[k])
            mark_out(res[k], new[po + k], MARKS)
    elif fn in (3, 4):
        ins = A + Bv
        res = [None] * 3
        for k in (2, 1, 0):
            res[k] = add_m(ld(A[k]), Bv[k]) if fn == 3 else sub_m(ld(A[k]), Bv[k])
        eax_out = True
        for k in range(3):
            new[po + k] = store(res[k])
            mark_out(res[k], new[po + k], MARKS)
            if res[k][0] == 'f' and res[k][1] == 0 and (A[k] & 0x7FFFFFFF) and (Bv[k] & 0x7FFFFFFF):
                MARKS.append('cancel_zero')
    elif fn in (5, 6):
        ins = A + [c['f0']]
        res = [None] * 3
        for k in (2, 1, 0):
            res[k] = mul_m(ld(c['f0']), A[k])
        eax_out = True
        for k in range(3):
            new[po + k] = store(res[k])
            mark_out(res[k], new[po + k], MARKS)
    elif fn == 7:
        ins = words[po:po + 3] + [c['f0']]
        for k in range(3):
            r = mul_m(ld(c['f0']), new[po + k])
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 8:
        ins = words[po:po + 3] + words[pa:pa + 3]
        for k in range(3):
            r = add_m(ld(new[pa + k]), new[po + k])
            new[po + k] = store(r)
            mark_out(r, new[po + k], MARKS)
    elif fn == 9:
        a, b = c['f0'], c['f1']
        ins = [a, b]
        unordered = any((w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) for w in (a, b))
        less = (not unordered) and rawfloat(a) < rawfloat(b)
        st0 = ld(a) if less else ld(b)
        MARKS.append('min_unordered' if unordered else ('min_first_less' if less else 'min_second'))
        MARKS.append(res_mark(st0))
    elif fn == 10:
        ins = A + Bv
        eax_out = True
        for k in range(3):
            new[po + k] = A[k]
            new[po + 4 + k] = Bv[k]
    elif fn == 11:
        ins = words[pa:pa + 3]
        for k in (1, 2, 4, 6, 8, 9):
            new[po + k] = 0
        for k in (0, 5, 10):
            new[po + k] = 0x3F800000
        for k in range(3):
            new[po + 12 + k] = words[pa + k]
    classify(ins, MARKS)
    MARKS.append('mode%d' % c['mode'])
    old_b, new_b = bytes_of(words), bytes_of(new)
    changes = [[0, i, new_b[i]] for i in range(len(new_b)) if old_b[i] != new_b[i]]
    return {'marks': list(MARKS), 'changes': changes, 'st0': st0, 'eax_out': eax_out, 'fn': fn, 'po': po}


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
