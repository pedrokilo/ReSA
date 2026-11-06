"""Oraculo independiente (Python) de Fn105A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote 105A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. Los movimientos de memoria se modelan instruccion a instruccion sobre una copia de la
ventana (la lectura y la escritura de cada palabra en el orden del EXE, de modo que los solapamientos se resuelven igual); la aritmetica x87 (24 bits de mantisa por la palabra de control del juego,
exponente ampliado, NaN con carga util) con fracciones exactas.

Funciones (desensamblado del EXE canonico):
  0 TList_c::AddItem 0x4A8DF0 (ECX = lista {primero, ultimo, recuento}, nodo {anterior, siguiente}): old = [lista]; [lista] = nodo; [nodo] = 0; [nodo + 4] = old; si old != 0 { [old] = nodo } si no { [lista + 4] = nodo };
    cnt = [lista + 8]; [lista + 8] = cnt + 1; EAX = cnt (lectura viva: si el nodo solapa la lista se ven las escrituras anteriores).
  1 TList_c::RemoveAll 0x4A8EB0: [lista], [lista + 4], [lista + 8] = 0; EAX = 0.        2 TList_c::GetNumItems 0x4A8EC0: EAX = [lista + 8].
  3 CMatrix::operator*= 0x411A80: 0x59BE30(tmp, this, arg) (cdecl); 0x59BBC0(this, tmp) (thiscall); 0x59ACD0(tmp) (thiscall); tmp es una matriz de pila. Los tres colaboradores son falsos que registran sus argumentos.
  4 CVector / float 0x4119D0 (cdecl, out, vector, divisor; EAX = out): r = 1.0 / divisor (fdiv de memoria; 24 bits, exponente ampliado), rf = float(r) (fst); z = r * v.z con el registro r SIN redondear, y = rf * v.y y x = rf * v.x
    (primer factor en registro por fld; el segundo operando de memoria: un NaN senalizante de memoria pierde frente a un NaN del registro); se lee todo antes de escribir.
  5 CompressedVector a CVector 0x411590 (cdecl, out, int16*): para x, y, z en ese orden: v = movsx16 [src + 2k]; [out + 4k] = float(v * 0.0078125); cada lectura ve las escrituras anteriores.
  6 CColPoint::operator= 0x40FC80 (ECX = this, origen; EAX = this): dwords +0, +4, +8, +0x10, +0x14, +0x18; word +0x20; byte +0x22; word +0x23; byte +0x25; dword +0x28, cada uno leido y escrito antes del siguiente.
  7 iterador de constructores 0x403D60 (stdcall base, tamano, recuento, constructor): con recuento <= 0 (con signo) no hace nada; si no, llama `recuento` veces al constructor con ECX = base + i * tamano (32 bits).
  8 CVector::operator-= 0x406D70 (ECX = this, otro): por componente x, y, z: fld [this + 4k]; fsub dword [otro + 4k]; fstp [this + 4k] (memoria viva).
  9 CStreaming::SetModelTxdIsDeletable 0x409C70 (cdecl, id): movsx16 [ModelInfo[id] + 0xA] + 20000 como argumento de 0x409C10 (falso, registra el argumento).
Reglas de NaN de la x87 medidas en el hardware (ver Fn103A): con un NaN senalizante en el operando de memoria gana el NaN del registro y, si el registro no es NaN, el senalizante silenciado; entre dos NaN
silenciosos gana el de mayor significando; las operaciones invalidas dan el indefinido 0xFFC00000.

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



import struct


def rd(mem, off, size):
    return int.from_bytes(mem[off:off + size], 'little')


def wr(mem, off, size, v):
    mem[off:off + size] = (v & ((1 << (8 * size)) - 1)).to_bytes(size, 'little')


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 99001) & M32)


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


NAMES_EAX_RAW = (0, 1, 2)      # EAX de salida como valor
NAMES_EAX_PTR = (4, 6)         # EAX de salida = puntero de salida (etiquetado)
WIN = 96 * 4
ARENA = GUARD + WIN + GUARD


def expected(c, base=0x10000000):
    """base = direccion de la ventana (Obj(0)) en el proceso del juego: los punteros guardados en la ventana y las etiquetas de los eventos dependen de ella"""
    global MARKS
    MARKS = []
    fn = c['fn']
    words = list(c['mem'])
    for at, to in zip(c['fix'][0::2], c['fix'][1::2]):
        words[at] = (base + 4 * to) & M32
    pa, pb, po = c['pa'], c['pb'], c['po']
    mem = bytearray()
    for w in words:
        mem += w.to_bytes(4, 'little')
    orig = bytes(mem)
    events = []
    eax = None
    ins = []
    lo = base - GUARD

    def tg(v):
        v &= M32
        if v == 0:
            return 0
        if lo <= v < lo + ARENA:
            return 0x10000000 + (v - lo)
        return v

    if fn == 0:                                  # TList_c::AddItem
        L, node = 4 * po, 4 * pa
        old = rd(mem, L, 4)
        wr(mem, L, 4, base + node)
        wr(mem, node, 4, 0)
        wr(mem, node + 4, 4, old)
        if old:
            wr(mem, old - base, 4, base + node)
            MARKS.append('nonempty_list')
        else:
            wr(mem, L + 4, 4, base + node)
            MARKS.append('empty_list')
        cnt = rd(mem, L + 8, 4)
        wr(mem, L + 8, 4, cnt + 1)
        eax = cnt
        if cnt == 0xFFFFFFFF:
            MARKS.append('count_wrap')
        if old == base + node:
            MARKS.append('double_insert')
        if c['mode'] in (3, 4):
            MARKS.append('node_in_list')
    elif fn == 1:                                # RemoveAll
        L = 4 * po
        for k in (0, 4, 8):
            wr(mem, L + k, 4, 0)
        eax = 0
    elif fn == 2:                                # GetNumItems
        eax = rd(mem, 4 * po + 8, 4)
        MARKS.append('count_zero' if eax == 0 else ('count_big' if eax >= 0x80000000 else 'count_small'))
    elif fn == 3:                                # CMatrix::operator*=
        this, arg = base + 4 * po, base + 4 * pb
        ha, hb = fnv(mem[4 * po:4 * po + 0x48]), fnv(mem[4 * pb:4 * pb + 0x48])
        events.append([10, 0x7F000000, tg(this), tg(arg), ha, hb])
        tmp = [rand(ha ^ ((hb * 31) & M32), k) for k in range(16)]
        tmpbytes = b''.join(t.to_bytes(4, 'little') for t in tmp)
        events.append([11, tg(this), 0x7F000000, fnv(tmpbytes)])
        for k in range(16):
            wr(mem, 4 * po + 4 * k, 4, tmp[k])
        events.append([12, 0x7F000000])
        if pb == po:
            MARKS.append('same_matrix')
        elif abs(pb - po) < 18:
            MARKS.append('overlap')
        else:
            MARKS.append('separate')
    elif fn == 4:                                # vector / escalar
        sbits = c['f0']
        vec = [words[pa + k] for k in range(3)]
        classify(vec + [sbits], MARKS)
        r = div(ONE, ld(sbits))
        rf = store(r)
        if isnan(r):
            MARKS.append('recip_nan')
        elif r[0] == 'i':
            MARKS.append('recip_inf')
        elif r[1] != 0:
            if abs(r[1]) > FLT_MAX:
                MARKS.append('rf_overflow')
            elif abs(r[1]) < FLT_MIN:
                MARKS.append('rf_denormal')
        z = mul_m(r, vec[2])
        y = mul_m(ld(rf), vec[1])
        x = mul_m(ld(rf), vec[0])
        zs = store(z)
        if zs != store(mul_m(ld(rf), vec[2])):
            MARKS.append('z_uses_register')
        for k, rr in ((0, x), (1, y), (2, z)):
            b = store(rr)
            wr(mem, 4 * po + 4 * k, 4, b)
            mark_out(None, b, MARKS)
        eax = tg(base + 4 * po)
    elif fn == 5:                                # descomprimir
        S = 4 * pa + c['f1']
        if c['f1']:
            MARKS.append('src_half_offset')
        for k in range(3):
            v = rd(mem, S + 2 * k, 2)
            sv = v - 0x10000 if v >= 0x8000 else v
            if sv < 0:
                MARKS.append('neg_val')
            elif sv == 0:
                MARKS.append('zero_val')
            else:
                MARKS.append('pos_val')
            if v == 0x8000:
                MARKS.append('min16')
            if v == 0x7FFF:
                MARKS.append('max16')
            wr(mem, 4 * po + 4 * k, 4, f32bits(sv / 128.0))
    elif fn == 6:                                # CColPoint::operator=
        D, S = 4 * po + c['f0'], 4 * pa
        for size, off in ((4, 0), (4, 4), (4, 8), (4, 0x10), (4, 0x14), (4, 0x18), (2, 0x20), (1, 0x22), (2, 0x23), (1, 0x25), (4, 0x28)):
            wr(mem, D + off, size, rd(mem, S + off, size))
        eax = tg(base + D)
        MARKS.append('overlap' if abs(D - S) < 0x2C else 'separate')
        if c['f0']:
            MARKS.append('byte_shift')
    elif fn == 7:                                # iterador de constructores
        count = c['f1'] - (1 << 32) if c['f1'] >= 1 << 31 else c['f1']
        if count < 0:
            MARKS.append('count_neg')
        elif count == 0:
            MARKS.append('count_zero')
        else:
            MARKS.append('count_pos')
        if count > 0:
            ecx = (base + 4 * pa) & M32
            for _ in range(count):
                events.append([14, tg(ecx)])
                MARKS.append('ecx_in_arena' if lo <= ecx < lo + ARENA else 'ecx_out')
                ecx = (ecx + c['f0']) & M32
        MARKS.append('stride_zero' if c['f0'] == 0 else ('stride_neg' if c['f0'] >= 1 << 31 else 'stride_pos'))
    elif fn == 8:                                # CVector::operator-=
        P, Q = 4 * po, 4 * pa
        classify([rd(mem, P + 4 * k, 4) for k in range(3)] + [rd(mem, Q + 4 * k, 4) for k in range(3)], MARKS)
        for k in range(3):
            a = ld(rd(mem, P + 4 * k, 4))
            r = sub_m(a, rd(mem, Q + 4 * k, 4))
            b = store(r)
            wr(mem, P + 4 * k, 4, b)
            mark_out(None, b, MARKS)
    else:                                        # 9: SetModelTxdIsDeletable
        txd = (words[pa + 2] >> 16) & 0xFFFF
        stxd = txd - 0x10000 if txd >= 0x8000 else txd
        events.append([13, (20000 + stxd) & M32])
        MARKS.append('txd_neg' if stxd < 0 else ('txd_zero' if stxd == 0 else 'txd_pos'))
        MARKS.append('id_small' if c['f0'] < 300 else 'id_high')
    MARKS.append('mode%d' % c['mode'])
    new = bytes(mem)
    changes = [[0, i, new[i]] for i in range(len(new)) if orig[i] != new[i]]
    return {'marks': list(MARKS), 'changes': changes, 'eax': eax, 'fn': fn, 'events': events, 'base': base}


def check_side(c, o, e, side_row):
    if o.get('base') != e['base']:
        return 'base'
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('events') != e['events']:
        return 'eventos %s != %s' % (str(o.get('events'))[:300], str(e['events'])[:300])
    if e['eax'] is None:
        if 'exit_eax' in o:
            return 'EAX de salida inesperado'
    elif o.get('exit_eax') != e['eax']:
        return 'EAX de salida %s != %s' % (o.get('exit_eax'), e['eax'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    return None


def check_row(row):
    c = row['input']
    base = row['original']['observed']['base']
    e = expected(c, base)
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
