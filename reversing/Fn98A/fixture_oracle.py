"""Oraculo independiente (Python) de Fn98A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las seis funciones del lote 98A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.

Funcion 0, FloatMax (0x420800..0x420818; cdecl, (a, b); float en ST0): fld a; fcomp b; fnstsw ax; test ah,41h; jne -> fld b; si no fld a. Devuelve a solo si a > b (ordenado); con NaN o a <= b devuelve b.
  El valor de ST0 es la carga de 80 bits del float elegido (los NaN senalizantes se silencian y conservan la carga).
Funcion 1, CCarPathLink::GetNodeCoors (0x420A60..0x420A95; thiscall, ECX = enlace, destino en pila, `ret 4`): lee x = int16 [ecx], y = int16 [ecx+2]; escribe z = 0, x * 0.125 (0x858C48) en +0, y * 0.125 en +4;
  devuelve el destino en EAX. Los productos son exactos.
Funcion 2, CVehicleModelInfo::IncrementTimesUsed (0x421120..0x421132; thiscall): byte con signo en +0x50: eax = int8 + 1; si eax >= 0x78 -> 0x78; guarda AL.
Funcion 3, CPlaceable::GetForwardVector (0x41CCB0..0x41CCF3; thiscall, ECX = placeable, destino en pila, `ret 4`): si la matriz (+0x14) no es nula copia los tres dwords de matriz + 0x10 al destino; si no:
  fcos(h), fsin(h) con h = float en +0x10; escribe (-sin h, cos h, 0) guardados como float (z = 0 primero). Devuelve el destino en EAX.
Funcion 4, CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName (0x46CED0..0x46CEFA; thiscall (nombre, entidad, tipo), `ret 0Ch`): ax = 0x46AA30(nombre, tipo); si ax >= 0 (con signo)
  llama a 0x46CD80(ax en EAX completo, entidad, tipo, 1) con el mismo `this`.
Funcion 5, CTheScripts::IsPointWithinSearchLight (0x493280..0x493358; cdecl (&posicion, indice); AL): copia los ejes A (+0x64) y B (+0x70) del registro (0x7C * indice + 0xA94D68), llama a 0x59C970 con cada uno
  (magA se guarda como float, magB queda en el registro); d = posicion - (+0x58); proyecciones ((z * dz + y * dy) + x * dx) con los ejes normalizados; qA = float(proyA / magA), qB = proyB / magB;
  devuelve (qB * qB + qA * qA) <= 1.0 (ordenado: con NaN devuelve 0).

Funcion 6, WinInput::diPadEnumerate (0x7477C0..0x747814; cdecl, sin argumentos; EAX): hr = IDirectInput8::EnumDevices(psGlobal[+0x18], 4, 0x747020, 0, 1) (ranura 4 de la vtabla); si hr != 0 devuelve hr;
  sin primer mando (psGlobal[+0x20]) devuelve 1; con el: 0x746D80(mando1, 0) y, si hay segundo (+0x24), 0x746D80(mando2, 1); devuelve 0.
Funcion 7, CCarCtrl::WeaveForPed (0x426970..0x426BB2; cdecl (entidad, vehiculo, &limite1, &limite2); void): mision (byte +0x3BA del vehiculo) 3 -> sale si FindPlayerPed(-1) es el peaton; mision 0x37 -> sale si
  el objetivo (+0x41C) es el peaton; dx = x peaton - x vehiculo y dy (posicion de la matriz +0x30 o del placement +4) guardados como float; angulo = 0x53CC70(dx, dy) guardado como float; dist = sqrt(dy * dy + dx * dx)
  sin guardar; con dist < 1.0 sale; ratio = float((maxx * 2.4 + 0.8) / dist) (maxx = caja de colision del modelo del vehiculo, +0xC de +0x14 de la informacion de modelo [indice int16 +0x22]),
  margen = float(ratio * 0.5); para cada limite: d = angulo - limite llevado a [-pi, pi] por bucles de +-2pi, en valor absoluto; si d < margen: limite1 = float(angulo - margen) y, si el valor sin redondear
  < -pi, se relee y se le suma 2pi mientras < -pi; limite2 = float(margen + angulo) y, si > pi, se le resta 2pi mientras > pi.

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


# ------------------------------------------------------------------------------------------------ ampliaciones del modelo (lote 98A)
def div(a, b):
    """a / b a 24 bits con exponente ampliado; con divisor cero: infinito con signo (o NaN si a tambien es cero o NaN)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else ('i', a[1] ^ (b[2] if b[0] == 'f' else 0))
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', (1 if a[1] < 0 else 0) ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def le(a, b):
    """a <= b ordenado (fcomp + test ah,41h; jnp...): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) <= num(b)


def ext_of_bits(bits):
    """codificacion de 80 bits (10 bytes) de `fld dword` de un patron de float (los NaN se silencian y conservan la carga)"""
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        if m == 0:
            mant = 1 << 63
        else:
            mant = (1 << 63) | (1 << 62) | (m << 40)
        return mant.to_bytes(8, 'little') + (((s << 15) | 0x7FFF)).to_bytes(2, 'little')
    if e == 0:
        if m == 0:
            return (0).to_bytes(8, 'little') + (s << 15).to_bytes(2, 'little')
        top = m.bit_length() - 1
        mant = m << (63 - top)
        return mant.to_bytes(8, 'little') + ((s << 15) | (top - 149 + 16383)).to_bytes(2, 'little')
    mant = (m | 0x800000) << 40
    return mant.to_bytes(8, 'little') + ((s << 15) | (e - 127 + 16383)).to_bytes(2, 'little')


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


# seno y coseno de la x87 (series de Taylor en 100 digitos; |a| <= 8)
from decimal import Decimal, getcontext
getcontext().prec = 100


def sin_cos_dec(a):
    a2 = a * a
    eps = Decimal(10) ** -95
    s, c = a, Decimal(1)
    ts, tc = a, Decimal(1)
    n = 1
    while True:
        tc = -tc * a2 / (n * (n + 1))
        ts = -ts * a2 / ((n + 1) * (n + 2))
        n += 2
        c += tc
        s += ts
        if abs(tc) < eps and abs(ts) < eps:
            return s, c


def store_dec(d):
    """fstp dword de un valor decimal exacto: bits del float"""
    if d == 0:
        return 0x80000000 if d.is_signed() else 0
    num_, den_ = abs(d).as_integer_ratio()
    return store(('f', Fraction(num_, den_) * (-1 if d < 0 else 1), 1 if d < 0 else 0))


STACK = 0x7F000000
GUARD = 0x40
A_OBJ, A_OUT, A_MAT, A_STR, A_ENT, A_POS, A_VEH, A_VMAT, A_MI, A_COL = range(10)
M32 = 0xFFFFFFFF
RAW = 0x00C0FFEE


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def fl(x):
    """valor tras guardarlo como float (fstp dword) y volver a cargarlo"""
    return reload(store(x))


def byte_changes(arena, base_off, old_words, new_words):
    """cambios de bytes [arena, desplazamiento, byte nuevo] entre dos listas de dwords consecutivos (desde base_off)"""
    out = []
    for k, (o, n) in enumerate(zip(old_words, new_words)):
        for j in range(4):
            ob, nb = (o >> (8 * j)) & 0xFF, (n >> (8 * j)) & 0xFF
            if ob != nb:
                out.append([arena, base_off + 4 * k + j, nb])
    return out


# ------------------------------------------------------------------------------------------------ funcion 0: FloatMax (0x420800)
def expected0(c):
    a = c['a']
    marks = []
    ab, bb = a[0] & M32, a[1] & M32
    la, lb = load(ab), load(bb)
    if la == NAN:
        marks.append('nan_a')
    if lb == NAN:
        marks.append('nan_b')
    if la != NAN and lb != NAN:
        marks.append('a_greater' if gt(la, lb) else ('equal' if num(la) == num(lb) else 'a_less'))
        if la[0] == 'f' and lb[0] == 'f' and la[1] == 0 and lb[1] == 0:
            marks.append('zeros_%d%d' % (la[2], lb[2]))
        if la[0] == 'i' or lb[0] == 'i':
            marks.append('infinite')
    pick = ab if gt(la, lb) else bb
    return {'events': [], 'marks': marks, 'changes': [], 'ret': None, 'st0': ext_of_bits(pick)}


# ------------------------------------------------------------------------------------------------ funcion 1: CCarPathLink::GetNodeCoors (0x420A60)
def expected1(c):
    a = c['a']
    marks = []
    x, y = s16(a[0]), s16(a[1])
    marks.append('x_neg' if x < 0 else ('x_zero' if x == 0 else 'x_pos'))
    marks.append('y_neg' if y < 0 else ('y_zero' if y == 0 else 'y_pos'))
    if x in (0x7FFF, -0x8000) or y in (0x7FFF, -0x8000):
        marks.append('extreme')
    new = [store(('f', Fraction(x, 8), 0)), store(('f', Fraction(y, 8), 0)), 0]
    old = [a[2] & M32, a[3] & M32, a[4] & M32]
    changes = byte_changes(A_OUT, 0, old, new)
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': tag(A_OUT, 0), 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 2: CVehicleModelInfo::IncrementTimesUsed (0x421120)
def expected2(c):
    b = c['a'][0] & 0xFF
    used = s8(b) + 1
    marks = []
    if used >= 0x78:
        used = 0x78
        marks.append('clamped')
    else:
        marks.append('below_limit')
    if b >= 0x80:
        marks.append('negative_byte')
    if b == 0xFF:
        marks.append('wrap_zero')
    if b == 0x77:
        marks.append('exact_limit')
    new = used & 0xFF
    changes = [] if new == b else [[A_OBJ, 0x50, new]]
    if not changes:
        marks.append('unchanged')
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': None, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 3: CPlaceable::GetForwardVector (0x41CCB0)
def quiet_bits(bits):
    return bits | 0x00400000


def expected3(c):
    a = c['a']
    marks = []
    old = [a[5] & M32, a[6] & M32, a[7] & M32]
    if a[0]:
        marks.append('matrix')
        new = [a[2] & M32, a[3] & M32, a[4] & M32]
    else:
        marks.append('heading')
        h = a[1] & M32
        e, m = (h >> 23) & 0xFF, h & 0x7FFFFF
        if e == 0xFF and m:
            marks.append('heading_nan')
            q = quiet_bits(h)
            new = [q ^ 0x80000000, q, 0]
        elif e == 0xFF:
            marks.append('heading_inf')
            new = [0x7FC00000, 0xFFC00000, 0]
        else:
            lh = load(h)
            v = lh[1]
            if v == 0:
                marks.append('heading_zero')
                new = [(0x80000000 if lh[2] == 0 else 0), 0x3F800000, 0]
            else:
                if abs(v) > 8:
                    raise ValueError('rumbo fuera del dominio del modelo')
                d = Decimal(v.numerator) / Decimal(v.denominator)
                s_, c_ = sin_cos_dec(d)
                new = [store_dec(-s_), store_dec(c_), 0]
                marks.append('heading_finite')
                if abs(v) < Fraction(1, 1 << 20):
                    marks.append('heading_tiny')
    changes = byte_changes(A_OUT, 0, old, new)
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': tag(A_OUT, 0), 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 4: CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName (0x46CED0)
def kind_ptr(kind, arena):
    return tag(arena, 0) if kind == 0 else (0 if kind == 1 else RAW)


def expected4(c):
    a = c['a']
    marks = []
    this = tag(A_OBJ, 0)
    name = kind_ptr(a[2], A_STR)
    ent = kind_ptr(a[3], A_ENT)
    typ = a[0] & M32
    ev = [[50, this, name, typ]]
    idx = s16(a[1])
    if idx >= 0:
        marks.append('idx_found')
        ev.append([51, this, idx, ent, typ, 1])
        if idx >= 70:
            marks.append('idx_high')
        if idx == 0:
            marks.append('idx_zero')
    else:
        marks.append('idx_missing')
    marks.append(['name_arena', 'name_null', 'name_raw'][a[2]])
    marks.append(['ent_arena', 'ent_null', 'ent_raw'][a[3]])
    if typ & 0x80:
        marks.append('type_neg')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': None, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 5: CTheScripts::IsPointWithinSearchLight (0x493280)
def expected5(c):
    a = c['a']
    marks = []
    ev = [[60, STACK, a[14] & M32, a[15] & M32, a[16] & M32], [60, STACK, a[17] & M32, a[18] & M32, a[19] & M32]]
    nA = [load(a[6]), load(a[7]), load(a[8])]
    nB = [load(a[10]), load(a[11]), load(a[12])]
    magA = fl(load(a[9]))          # se guarda como float (fstp) antes de dividir
    magB = load(a[13])             # queda en el registro
    S = [load(a[0]), load(a[1]), load(a[2])]
    P = [load(a[3]), load(a[4]), load(a[5])]
    d = [sub(P[0], S[0]), sub(P[1], S[1]), sub(P[2], S[2])]
    dotA = add(add(mul(nA[2], d[2]), mul(nA[1], d[1])), mul(nA[0], d[0]))
    qA = fl(div(dotA, magA))
    dotB = add(add(mul(nB[2], d[2]), mul(nB[1], d[1])), mul(nB[0], d[0]))
    qB = div(dotB, magB)
    total = add(mul(qB, qB), mul(qA, qA))
    inside = le(total, ('f', Fraction(1), 0))
    marks.append('inside' if inside else 'outside')
    marks.append('idx%d' % a[20])
    if total == NAN:
        marks.append('nan')
    elif total[0] == 'i':
        marks.append('infinite')
    elif total[1] == 1:
        marks.append('boundary_equal')
    elif total[1] == 0:
        marks.append('zero_sum')
    if qA != NAN and qA[0] == 'f' and qA[1] == 0:
        marks.append('qa_zero')
    if dotA == NAN or dotB == NAN:
        marks.append('dot_nan')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': 1 if inside else 0, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 6: WinInput::diPadEnumerate (0x7477C0)
def expected6(c):
    a = c['a']
    marks = []
    ev = [[70, tag(A_STR, 0), 4, 0x747020, 0, 1]]
    hr = a[0] & M32
    dev1 = [0, tag(A_ENT, 0), 0x00C0FFEE][a[1]]
    dev2 = [0, tag(A_POS, 0), 0x00C0FFEF][a[2]]
    if hr != 0:
        marks.append('enum_fails')
        ret = hr
    elif dev1 == 0:
        marks.append('no_pad1')
        ret = 1
    else:
        ev.append([71, dev1, 0])
        marks.append('pad1')
        if dev2 != 0:
            ev.append([71, dev2, 1])
            marks.append('pad2')
        else:
            marks.append('no_pad2')
        ret = 0
    if hr == 0 and dev1 == 0 and dev2 != 0:
        marks.append('pad2_only')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': ret, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 7: CCarCtrl::WeaveForPed (0x426970)
PI = load(0x40490FDB)
MPI = load(0xC0490FDB)
TWO_PI = load(0x40C90FDB)
C24 = load(0x4019999A)    # 2.4f
C08 = load(0x3F4CCCCD)    # 0.8f
HALF_C = load(0x3F000000)
ZERO = ('f', Fraction(0), 0)
LIMIT_ITERS = 200000


def lt(a, b):
    return gt(b, a)


def fold_abs(angle, bound):
    diff = sub(angle, bound)
    n = 0
    while lt(diff, MPI):
        diff = add(diff, TWO_PI)
        n += 1
        if n > LIMIT_ITERS:
            raise ValueError('bucle sin fin')
    while gt(diff, PI):
        diff = sub(diff, TWO_PI)
        n += 1
        if n > LIMIT_ITERS:
            raise ValueError('bucle sin fin')
    if lt(diff, ZERO):
        diff = neg(diff)
    return diff


def expected7(c):
    a = c['a']
    marks = []
    ev = []
    mission = s8(a[0])
    ped = tag(A_OBJ, 0)
    if mission == 3:
        ev.append([30, 0xFFFFFFFF])
        player = [ped, 0, 0x00ABCD00][a[15]]
        if player == ped:
            marks.append('mission3_player')
            return {'events': ev, 'marks': marks, 'changes': [], 'ret': None, 'st0': None}
        marks.append('mission3_other')
    if mission == 0x37:
        target = [ped, 0, 0x00ABCD01][a[16]]
        if target == ped:
            marks.append('mission37_target')
            return {'events': ev, 'marks': marks, 'changes': [], 'ret': None, 'st0': None}
        marks.append('mission37_other')
    marks.append('ped_matrix' if a[1] else 'ped_placement')
    marks.append('veh_matrix' if a[8] else 'veh_placement')
    pp = [load(a[2]), load(a[3]), load(a[4])]
    vp = [load(a[9]), load(a[10]), load(a[11])]
    dx = fl(sub(pp[0], vp[0]))
    dy = fl(sub(pp[1], vp[1]))
    ev.append([80, bits(dx), bits(dy)])
    angle = fl(load(a[17]))
    dist = sqrt24(add(mul(dy, dy), mul(dx, dx)))
    changes = []
    res = {'events': ev, 'marks': marks, 'changes': changes, 'ret': None, 'st0': None}
    if lt(dist, ('f', Fraction(1), 0)):
        marks.append('too_close')
        return res
    marks.append('far')
    maxx = load(a[18])
    ratio = fl(div(add(mul(maxx, C24), C08), dist))
    margin = fl(mul(ratio, HALF_C))
    old1, old2 = a[19] & M32, a[20] & M32
    new1, new2 = old1, old2
    if lt(fold_abs(angle, load(old1)), margin):
        marks.append('block1')
        v = sub(angle, margin)
        new1 = bits(v)
        if lt(v, MPI):
            marks.append('wrap1')
            w = reload(store(v))
            n = 0
            while lt(w, MPI):
                w = add(w, TWO_PI)
                n += 1
                if n > LIMIT_ITERS:
                    raise ValueError('bucle sin fin')
            new1 = bits(w)
    if lt(fold_abs(angle, load(old2)), margin):
        marks.append('block2')
        v = add(margin, angle)
        new2 = bits(v)
        if gt(v, PI):
            marks.append('wrap2')
            w = reload(store(v))
            n = 0
            while gt(w, PI):
                w = sub(w, TWO_PI)
                n += 1
                if n > LIMIT_ITERS:
                    raise ValueError('bucle sin fin')
            new2 = bits(w)
    changes.extend(byte_changes(A_OUT, 0, [old1, old2], [new1, new2]))
    if not changes:
        marks.append('no_change')
    return res


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected5, 6: expected6, 7: expected7}[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != e['st0']:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], e['st0'].hex())
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
