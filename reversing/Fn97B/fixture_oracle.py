"""Oraculo independiente (Python) de Fn97B: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las seis funciones de CMatrix del lote 97A (segunda parte)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Funciones (desensamblado del EXE; los colaboradores se sustituyen por falsos identicos en ambos lados que registran sus argumentos):
  0 CMatrix::SetRotateZOnly 0x59B020: fcos y fsin del angulo (precision completa de 64 bits de la x87); +0x00 = cos, +0x04 = sin, +0x10 = -sin, +0x14 = cos (guardados como float), ceros en +0x08, +0x18, +0x20
    y +0x24 y 1.0 en +0x28. El modelo es seno/coseno exactos redondeados a float (aritmetica entera de 400 bits); NaN -> el propio NaN (silenciado); infinitos -> el indefinido 0xFFC00000; |x| >= 2^63 -> la x87
    deja el operando sin tocar (C2 = 1) y sin = cos = x. No se usan modulos entre 2^19 y 2^62 (la reduccion del x87 con el pi de 66 bits no se modela).
  1 CMatrix::UpdateRwMatrix 0x59AD70: copia los 12 dwords +0x00..+0x08, +0x10..+0x18, +0x20..+0x28 y +0x30..+0x38 de la matriz a la RwMatrix y llama a RwMatrixUpdate 0x7F18A0 (cdecl, 1 argumento; evento 50).
  2 CMatrix::UpdateRW 0x59BBB0: si el dword +0x40 no es cero llama a UpdateRwMatrix 0x59AD70 (thiscall, matriz en ECX, el puntero en pila; evento 51: matriz, puntero); no escribe nada.
  3 CMatrix::Destructor 0x59ACD0: si el DWORD +0x44 (el relleno cuenta) y el dword +0x40 no son cero llama a RwMatrixDestroy 0x7F2A20 (cdecl; evento 52: puntero); no escribe nada ni toca +0x40.
  4 CMatrix::ConstructCopy 0x59BCF0: pone a cero +0x40 y el dword +0x44 y copia 16 dwords (0x40 bytes) de la matriz origen; EAX = this.
  5 CMatrix::ConstructAttached 0x59C050: pone a cero +0x40 y llama a Attach 0x59BD10 (thiscall, matriz en ECX, RwMatrix y bandera (dword completo) en pila; evento 53: matriz, RwMatrix, bandera); EAX = this.
Un dword de `mem` o `arg` con el valor 0xAAA00000 + desplazamiento es un puntero a ese desplazamiento de la ventana de memoria (la sonda lo resuelve a la direccion real y lo ve como etiqueta de arena).

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
PTR_MARKER = 0xAAA00000
FLT_MAX = Fraction(0xFFFFFF, 1) * Fraction(2) ** 104
FLT_MIN = Fraction(2) ** -126


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def val(w):
    """valor de un puntero de `mem`/`arg` como lo ve la sonda: marcador -> etiqueta de arena; si no, el valor crudo"""
    if (w & 0xFFF00000) == PTR_MARKER:
        return tag(0, w & 0xFFFFF)
    return w


def bytes_of(words):
    out = []
    for w in words:
        out += [(w >> (8 * j)) & 0xFF for j in range(4)]
    return out


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


def classify_copy(words, marks):
    if any((w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) for w in words):
        marks.append('nan_copied')
    if any((w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) and not (w & 0x400000) for w in words):
        marks.append('snan_copied')
    if any(0 < (w & 0x7FFFFFFF) < 0x00800000 for w in words):
        marks.append('denormal_copied')


def ptr_mark(w, marks, prefix):
    if w == 0:
        marks.append(prefix + '_null')
    elif (w & 0xFFF00000) == PTR_MARKER:
        marks.append(prefix + '_arena')
    else:
        marks.append(prefix + '_raw')


def expected(c):
    fn = c['fn']
    words = list(c['mem'])
    po, pa = c['po'], c['pa']
    new = list(words)
    events = []
    eax_out = False
    marks = []
    if fn == 0:
        w = c['arg']
        classify_angle(w, marks)
        a = load(w)
        if isnan(a):
            cs = sn = a
        elif a[0] == 'i':
            cs = sn = INDEF
        elif (w >> 23) & 0xFF >= 127 + 63:
            cs = sn = a
        elif a[1] == 0:
            cs = ('f', Fraction(1), 0)
            sn = a
        else:
            s, c0 = sincos(a[1])
            cs = ('f', c0, 1 if c0 < 0 else 0)
            sn = ('f', s, 1 if s < 0 else 0)
        new[po + 0] = store(cs)
        new[po + 1] = store(sn)
        new[po + 2] = 0
        new[po + 4] = store(chs(sn))
        new[po + 5] = store(cs)
        new[po + 6] = 0
        new[po + 8] = 0
        new[po + 9] = 0
        new[po + 10] = 0x3F800000
    elif fn == 1:
        src = [words[po + k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)]
        classify_copy(src, marks)
        for k, v in zip((0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14), src):
            new[pa + k] = v
        events.append([50, tag(0, 4 * pa)])
        if pa == po:
            marks.append('alias')
    elif fn == 2:
        ptr = words[po + 16]
        ptr_mark(ptr, marks, 'ptr')
        if ptr:
            events.append([51, tag(0, 4 * po), val(ptr)])
    elif fn == 3:
        ptr, flag = words[po + 16], words[po + 17]
        ptr_mark(ptr, marks, 'ptr')
        marks.append('flag_zero' if flag == 0 else ('flag_low' if flag & 0xFF else 'flag_pad_only'))
        if flag and ptr:
            events.append([52, val(ptr)])
            marks.append('destroy_called')
    elif fn == 4:
        src = [words[pa + k] for k in range(16)]
        classify_copy(src, marks)
        if words[po + 16]:
            marks.append('ptr_cleared')
        if words[po + 17]:
            marks.append('flag_cleared')
        new[po + 16] = 0
        new[po + 17] = 0
        for k in range(16):
            new[po + k] = src[k]
        eax_out = True
        if pa == po:
            marks.append('alias')
    else:
        arg, owns = c['arg'], c['owns']
        ptr_mark(arg, marks, 'arg')
        marks.append('owns_0' if owns == 0 else ('owns_1' if owns == 1 else 'owns_other'))
        if words[po + 16]:
            marks.append('ptr_cleared')
        new[po + 16] = 0
        events.append([53, tag(0, 4 * po), val(arg), owns])
        eax_out = True
    marks.append('mode%d' % c['mode'])
    old_b, new_b = bytes_of(words), bytes_of(new)
    changes = [[0, i, new_b[i]] for i in range(len(new_b)) if old_b[i] != new_b[i]]
    return {'marks': marks, 'changes': changes, 'events': events, 'eax_out': eax_out, 'fn': fn, 'po': po}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('events') != e['events']:
        return 'events: %s != %s' % (str(o.get('events'))[:300], str(e['events'])[:300])
    if e['eax_out']:
        if o.get('exit_eax') != tag(0, 4 * e['po']):
            return 'EAX de salida %s != this' % o.get('exit_eax')
    elif 'exit_eax' in o:
        return 'EAX de salida inesperado'
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
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
