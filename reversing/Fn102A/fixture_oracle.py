"""Oraculo independiente (Python) de Fn102A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las nueve funciones del lote 102A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas;
el seno y el coseno de fsin/fcos (precision completa, guardados como float) con mpmath a 300 bits.

Funcion 0, FxPrtMult_c::FxPrtMult_c(7 floats) (0x4AB290; thiscall, 7 floats en pila, `ret 1Ch`): this[+0x00..+0x18] = los siete argumentos en orden; devuelve this.
Funcion 1, CSimpleTransform::UpdateMatrix (0x54F1B0; thiscall, matriz en pila, `ret 4`): CMatrix::SetTranslate(x, y, z) (0x59AF40) y CMatrix::SetRotateZOnly(rumbo) (0x59B020), en ese orden, sobre la matriz.
Funcion 2, CSimpleTransform::UpdateRwMatrix (0x54EF40; thiscall, RwMatrix en pila, `ret 4`): right = (cos, sen, 0), up = (-sen, cos, 0), at = (0, 0, 1), pos = posicion; RwMatrixUpdate (flags &= 0xFFFDFFFC).
Funcion 3, CVector::SquaredMagnitudeExe (0x406DA0; thiscall; ST0): (x*x + y*y) + z*z en la x87; ECX y EDX no se tocan.
Funcion 4, CAESound::Destructor (0x4EF660; thiscall): si la referencia +8 no es nula, CEntity::CleanUpOldReference (0x571A00) con (entidad, &referencia) y despues referencia = 0.
Funcion 5, RpClumpForAllAtomics (0x749B70; cdecl): recorre la lista de atomicos leyendo el siguiente antes del callback; un resultado 0 corta; devuelve el clump.
Funcion 6, RpGeometryForAllMaterials (0x74C790; cdecl): n (int32, una sola lectura) vueltas con el arreglo de materiales releido en cada una; un resultado 0 corta; devuelve la geometria.
Funcion 7, RpAtomicSetFrame (0x74BF20; cdecl): _rwObjectHasFrameSetFrame (0x804EF0) y despues byte +3 |= 1 (releido); devuelve el atomico.
Funcion 8, AtomicDefaultRenderCallBack (0x7491C0; cdecl): RxPipelineExecute (0x805710) con (tuberia propia o la del motor, atomico, 1); devuelve el atomico si el resultado no es 0.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

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


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
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


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def sqrt24(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN"""
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


def reload(bits):
    return NAN if bits is None else load(bits)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / jne): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jp: cero solo si es igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero aqui)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]




def lt(a, b):
    return gt(b, a)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(x):
    """valor tras guardarlo como float (fstp dword) y volver a cargarlo"""
    return reload(store(x))


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo; NaN = indefinido"""
    if a == NAN:
        return (0xC000000000000000).to_bytes(8, 'little') + (0xFFFF).to_bytes(2, 'little')
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




import mpmath
mpmath.mp.prec = 300

STACK = 0x7F000000
GUARD = 0x40
A_OBJ, A_OUT, A_LIST, A_ENT = range(4)
M32 = 0xFFFFFFFF
# tuberia por defecto del motor ([0xC9BC60] + [0xC97B24] + 0x3C): la fija run_local.analyze desde la fila `begin`
DEFAULT_PIPELINE = [None]


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def trig(hb):
    """(seno, coseno) como float tras fsin/fcos y fstp dword de un float de entrada `hb` (bits)"""
    e = (hb >> 23) & 0xFF
    m = hb & 0x7FFFFF
    if e == 0xFF:
        if m == 0:
            return 0xFFC00000, 0xFFC00000          # infinito: operacion invalida, NaN indefinido
        q = hb | 0x400000                          # NaN: se vuelve silencioso y conserva signo y carga
        return q, q
    if e >= 0x7F + 63:
        return hb, hb                              # |x| >= 2^63: la x87 deja el operando intacto (C2 = 1)
    a = load(hb)
    if a[1] == 0:
        return hb, 0x3F800000                      # seno de +-0 = +-0, coseno 1
    v = a[1]
    x = mpmath.mpf(v.numerator) / mpmath.mpf(v.denominator)

    def conv(r):
        sg_, m_, e_, _ = r._mpf_
        fr = Fraction(int(m_)) * Fraction(2) ** int(e_)
        if sg_:
            fr = -fr
        return bits(('f', fr, 0 if fr > 0 else 1))
    return conv(mpmath.sin(x)), conv(mpmath.cos(x))


def word_changes(arena, base, olds, news):
    """cambios por byte [arena, desplazamiento, byte nuevo] de palabras de 32 bits consecutivas desde `base`"""
    out = []
    for k, (o, n) in enumerate(zip(olds, news)):
        for j in range(4):
            if (o >> (8 * j)) & 0xFF != (n >> (8 * j)) & 0xFF:
                out.append([arena, base + 4 * k + j, (n >> (8 * j)) & 0xFF])
    return out


def result(events, marks, changes=None, eax=None, st0=None, extra=None, regs=None):
    return {'events': events, 'marks': marks, 'changes': changes or [], 'eax': eax, 'st0': st0, 'extra': extra or {}, 'regs': regs}


# ------------------------------------------------------------------------------------------------ funcion 0: FxPrtMult_c::FxPrtMult_c(7 floats) (0x4AB290)
# 0x4AB290 (thiscall, ECX = this, siete floats en pila, `ret 1Ch`): this[0..6] = argumentos 1..7 en orden (+0x00..+0x18); devuelve this. Solo copia bits: no pasa por la x87.
def expected0(c):
    a = c['a']
    marks = ['store']
    news = [a[k] & M32 for k in range(7)]
    olds = [a[7 + k] & M32 for k in range(7)]
    if news == olds:
        marks.append('same')
    if any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in news):
        marks.append('nan')
    if any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in news):
        marks.append('inf')
    if any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) and not (v & 0x400000) for v in news):
        marks.append('snan')
    return result([], marks, word_changes(A_OBJ, 0, olds, news), tag(A_OBJ, 0))


# ------------------------------------------------------------------------------------------------ funcion 1: CSimpleTransform::UpdateMatrix (0x54F1B0)
# 0x54F1B0 (thiscall, ECX = transformacion, matriz en pila, `ret 4`): CMatrix::SetTranslate (0x59AF40, thiscall en la matriz) con (x, y, z) = this[0..2] y despues CMatrix::SetRotateZOnly (0x59B020) con el rumbo this[3].
def expected1(c):
    a = c['a']
    m = tag(A_OUT, 0)
    ev = [[20, m, a[0] & M32, a[1] & M32, a[2] & M32], [21, m, a[3] & M32]]
    marks = ['translate_then_rotate']
    if any(((a[k] >> 23) & 0xFF) == 0xFF for k in range(4)):
        marks.append('special')
    return result(ev, marks)


# ------------------------------------------------------------------------------------------------ funcion 2: CSimpleTransform::UpdateRwMatrix (0x54EF40)
# 0x54EF40 (thiscall, ECX = transformacion, RwMatrix en pila, `ret 4`): cos y sen del rumbo (this+0xC) con fcos/fsin de precision completa, guardados como float; right = (cos, sen, 0), up = (-sen, cos, 0), at = (0, 0, 1.0f),
# pos = this[0..2]; luego RwMatrixUpdate (0x7F18A0: flags &= 0xFFFDFFFC). Los rellenos (+0x1C, +0x2C, +0x3C) no se tocan.
def expected2(c):
    a = c['a']
    s, co = trig(a[3] & M32)
    ns = s ^ 0x80000000
    olds = [a[4 + k] & M32 for k in range(16)]
    news = list(olds)
    news[0], news[1], news[2] = co, s, 0
    news[3] = olds[3] & 0xFFFDFFFC
    news[4], news[5], news[6] = ns, co, 0
    news[8], news[9], news[10] = 0, 0, 0x3F800000
    news[12], news[13], news[14] = a[0] & M32, a[1] & M32, a[2] & M32
    # nota: el desplazamiento 6 (+0x18) es up.z y el 7 (+0x1C) el relleno; at empieza en el 8 (+0x20)
    news[6] = 0
    marks = ['matrix']
    hb = a[3] & M32
    e = (hb >> 23) & 0xFF
    if e == 0xFF:
        marks.append('heading_inf' if (hb & 0x7FFFFF) == 0 else 'heading_nan')
    elif e >= 0x7F + 63:
        marks.append('heading_huge')
    elif (hb & 0x7FFFFFFF) == 0:
        marks.append('heading_zero')
    elif e == 0:
        marks.append('heading_denormal')
    else:
        marks.append('heading_normal')
    return result([], marks, word_changes(A_OUT, 0, olds, news))


# ------------------------------------------------------------------------------------------------ funcion 3: CVector::SquaredMagnitudeExe (0x406DA0)
# 0x406DA0 (thiscall, ECX = vector; ST0 = resultado; ECX y EDX intactos): carga z, y, x; (x*x + y*y) + z*z sin guardar como float (24 bits de mantisa, exponente ampliado).
def expected3(c):
    a = c['a']
    x, y, z = load(a[0] & M32), load(a[1] & M32), load(a[2] & M32)
    r = add(add(mul(x, x), mul(y, y)), mul(z, z))
    marks = ['sum']
    if r == NAN:
        marks.append('result_nan')
    elif r[0] == 'i':
        marks.append('result_inf')
    elif r[1] == 0:
        marks.append('result_zero')
    else:
        marks.append('result_finite')
    return result([], marks, st0=r, regs=[tag(A_OBJ, 0), c['edx'] & M32])


# ------------------------------------------------------------------------------------------------ funcion 4: CAESound::Destructor (0x4EF660)
# 0x4EF660 (thiscall, ECX = sonido): si la referencia (+8) no es nula llama a CEntity::CleanUpOldReference (0x571A00, thiscall: ECX = entidad, &referencia en pila) y despues pone la referencia a 0.
def expected4(c):
    a = c['a']
    kind = a[0]
    ref = {0: 0, 1: tag(A_ENT, 0), 2: a[1] & M32, 3: tag(A_OBJ, 0)}[kind]
    ev = []
    marks = ['ref_null' if kind == 0 else ('ref_arena' if kind in (1, 3) else 'ref_raw')]
    if kind:
        ev.append([10, ref, tag(A_OBJ, 8), ref])
    return result(ev, marks, extra={'slot': 0})


# ------------------------------------------------------------------------------------------------ funcion 5: RpClumpForAllAtomics (0x749B70)
# 0x749B70 (cdecl, clump, callback, dato): recorre los enlaces desde clump+8 hasta volver a clump+8; el siguiente se lee ANTES del callback; atomico = enlace - 0x40; el callback se llama con (atomico, dato) y un resultado 0 corta.
LIMIT = 11


def expected5(c):
    a = c['a']
    n, stop, mode, data, ret, cyc, garbage = a[0], a[1], a[2], a[3] & M32, a[4] & M32, a[5], a[6] & M32
    sent = tag(A_LIST, 8)
    L = [tag(A_LIST, 0x60 + 0x80 * k) for k in range(n)]
    nxt = [L[k + 1] if k + 1 < n else (L[0] if cyc and n else sent) for k in range(n)]
    prv = [L[k - 1] if k else sent for k in range(n)]
    snext = L[0] if n else sent
    sprev = L[n - 1] if n else sent
    ev, marks = [], []
    marks.append('empty' if n == 0 else 'list')
    i = 0
    k = 0
    cur = 0 if n else None
    stopped = False
    while cur is not None:
        nk = (cur + 1) if cur + 1 < n else (0 if cyc else None)
        ev.append([30, tag(A_LIST, 0x20 + 0x80 * cur), data])
        if mode == 1:
            nxt[cur] = garbage
        elif mode == 2:
            prv[cur] = garbage
        ret_v = 0 if (i == stop or i >= LIMIT) else ret
        i += 1
        if ret_v == 0:
            stopped = True
            break
        cur = nk
    if stopped:
        marks.append('stopped')
    if mode == 1:
        marks.append('clobber_next')
    if mode == 2:
        marks.append('clobber_prev')
    if cyc and n:
        marks.append('ring')
    if len(ev) >= 2:
        marks.append('multi')
    links = [snext, sprev]
    for k in range(n):
        links += [nxt[k], prv[k]]
    return result(ev, marks, eax=tag(A_LIST, 0), extra={'links': links})


# ------------------------------------------------------------------------------------------------ funcion 6: RpGeometryForAllMaterials (0x74C790)
# 0x74C790 (cdecl, geometria, callback, dato): n = int32 en +0x24 (se lee una vez; si n <= 0 no llama); en cada vuelta se relee el arreglo de punteros (+0x20); callback(material, dato); resultado 0 corta; devuelve la geometria.
def expected6(c):
    a = c['a']
    n, stop, mode, data, ret = s32(a[0]), a[1], a[2], a[3] & M32, a[20] & M32
    arrays = [[a[4 + k] & M32 for k in range(8)], [a[12 + k] & M32 for k in range(8)]]
    cur = 0
    num = a[0] & M32
    ev, marks = [], []
    marks.append('no_loop' if n <= 0 else 'loop')
    i = 0
    stopped = False
    while i < n:
        ev.append([31, arrays[cur][i], data])
        if mode == 1 and i == 0:
            cur = 1
        elif mode == 2:
            num = 0
        elif mode == 3:
            num = 100
        ret_v = 0 if (i == stop or i >= LIMIT) else ret
        i += 1
        if ret_v == 0:
            stopped = True
            break
    if stopped:
        marks.append('stopped')
    if mode == 1 and len(ev) >= 2:
        marks.append('array_swapped')
    if mode in (2, 3) and ev:
        marks.append('count_changed')
    if n < 0:
        marks.append('negative')
    if len(ev) >= 2:
        marks.append('multi')
    return result(ev, marks, eax=tag(A_OBJ, 0), extra={'geom': [tag(A_LIST, 0x200 if cur else 0x100), num]})


# ------------------------------------------------------------------------------------------------ funcion 7: RpAtomicSetFrame (0x74BF20)
# 0x74BF20 (cdecl, atomico, marco): _rwObjectHasFrameSetFrame (0x804EF0, cdecl, (atomico, marco)); luego el byte +3 del atomico (releido tras la llamada) |= 1; devuelve el atomico.
def expected7(c):
    a = c['a']
    kind = a[0]
    frame = {0: tag(A_OUT, 0), 1: 0, 2: a[1] & M32}[kind]
    old, fake = a[2] & 0xFF, a[3] & 0xFF
    new = fake | 1
    ev = [[33, tag(A_OBJ, 0), frame]]
    marks = ['frame_arena' if kind == 0 else ('frame_null' if kind == 1 else 'frame_raw')]
    marks.append('flag_was_set' if fake & 1 else 'flag_added')
    changes = [[A_OBJ, 3, new]] if new != old else []
    return result(ev, marks, changes, tag(A_OBJ, 0))


# ------------------------------------------------------------------------------------------------ funcion 8: AtomicDefaultRenderCallBack (0x7491C0)
# 0x7491C0 (cdecl, atomico): tuberia = atomico+0x6C; si es nula, la del motor ([0xC9BC60] + [0xC97B24] + 0x3C); RxPipelineExecute (0x805710, cdecl) con (tuberia, atomico, 1); devuelve el atomico si el resultado no es 0, si no 0.
def expected8(c):
    a = c['a']
    own = a[0] == 1
    pipe = (a[1] & M32) if own else DEFAULT_PIPELINE[0]
    ev = [[32, pipe, tag(A_OBJ, 0), 1]]
    r = a[2] & M32
    marks = ['own_pipeline' if own else 'default_pipeline', 'executed_nonzero' if r else 'executed_zero']
    return result(ev, marks, eax=tag(A_OBJ, 0) if r else 0)


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected5, 6: expected6, 7: expected7, 8: expected8}[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if e['eax'] is not None and o.get('eax') != e['eax']:
        return 'eax %s != %s' % (o.get('eax'), e['eax'])
    for k, v in e['extra'].items():
        if o.get(k) != v:
            return '%s: %s != %s' % (k, o.get(k), v)
    if e['regs'] is not None and [o.get('ecx'), o.get('edx')] != e['regs']:
        return 'ecx/edx %s != %s' % ([o.get('ecx'), o.get('edx')], e['regs'])
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
        if row.get('event') == 'begin':
            DEFAULT_PIPELINE[0] = row.get('default_pipeline')
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
