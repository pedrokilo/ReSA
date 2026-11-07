"""Oraculo independiente (Python) de Fn114A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las catorce funciones del lote 114A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas
y el seno/coseno con series de Taylor de 100 digitos (|h| <= 8).

Funcion 0, CPlaceable::GetRightVector (0x41CC70..0x41CCA8; thiscall, ECX = placeable, destino en pila, `ret 4`; EAX = destino): con matriz (+0x14) copia en crudo los tres dwords de la fila derecha (matriz + 0);
  sin ella fsin(h), fcos(h) (h = float en +0x10), z = 0, x = cos y y = sin guardados como float.
Funcion 1, CPlaceable::GetUpVector (0x50E420..0x50E45F; igual ABI): con matriz copia en crudo los tres dwords de matriz + 0x20; sin ella (0, 0, 1.0f). EAX = destino.
Funcion 2, CPhysical::ApplyMoveForce(x, y, z) (0x4ABBA0..0x4ABBDC; thiscall, `ret 0Ch`): llama a 0x5429F0(this, x, y, z) con los tres dwords en crudo (el colaborador es un falso identico).
Funcion 3, CPedIntelligence::GetPedFOVRange (0x4ABEA0..0x4ABEC0; thiscall; float en ST0): devuelve el alcance de oido (+0xBC) solo si es estrictamente mayor (ordenado) que el de vista (+0xC0); si no, el de vista.
Funcion 4, CPedGroups::RegisterKillByPlayer (0x5F7E30..0x5F7E3F): si el byte 0xC098E8 es distinto de cero incrementa el dword 0xC098EC.
Funcion 5, CMissionCleanup::AddEntityToList (0x4637E0..0x463809; thiscall, `ret 8`): primer hueco (byte de tipo +0 de cada entrada de 8 bytes, 75 entradas, contador de 16 bits) con tipo 0: guarda el byte bajo del tipo
  y el handle (+4) e incrementa el byte +0x258.
Funcion 6, CEventDamage::IsSameEventForAI (0x4ADBF0..0x4ADC0B; thiscall, `ret 4`; AL): fuente (+0x14) y arma (+0x1C) iguales.
Funcion 7, CEntryExitManager::WeAreInInteriorTransition (0x43E400..0x43E40A; AL): el dword 0x96A7CC es distinto de cero.
Funcion 8, CPointRoute::AddPoint (0x48E180..0x48E1AD; thiscall, `ret 4`; AL): contador (dword +0, con signo) < 8: copia el punto en la entrada +4 + 12 * contador, incrementa el contador y devuelve 1; si no devuelve 0.
Funcion 9, CPointRoute::RemovePoint (0x5F13A0..0x5F13DA; thiscall, `ret 4`): desde i = indice (con signo) mientras i < contador - 1 copia la entrada i + 1 sobre la i; despues decrementa el contador.
Funcion 10, CPedGroupIntelligence::GetTask (0x5F7660..0x5F768B; thiscall, `ret 8`, ECX sin usar; EAX): primer par (8 pares de 0x14 bytes) cuyo ped (+0) coincide: devuelve su tarea (+4); si no, 0.
Funcion 11, CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent (0x4AE100..0x4AE192; thiscall, `ret 4`; AL): GetEventType (ranura 1) del evento propio y despues del otro (+0x10 de cada evento de grupo); tipos
  0x36, 0x37, 0x39, 0x47, 0x4C, 7, 0x4D son prioritarios: si el propio lo es devuelve 1; si el del otro lo es devuelve 0; si no salta a la ranura 11 (TakesPriorityOver) del propio con el evento del otro.
Funcion 12, CEvent::TakesPriorityOverBase (0x4ABC30..0x4ABC51; thiscall, `ret 4`; AL): GetEventPriority (ranura 2) del otro y despues del propio; devuelve propio >= otro (con signo).
Funcion 13, IsPedPointerValid (0x5E8A30..0x5E8AAC; cdecl; AL): indice = (ped - almacenamiento) / 0x7C4 (division con signo del pool 0xB74490), 0 <= indice < capacidad (con signo) y estado del hueco con el bit 7 a 0;
  con el bit 8 del dword +0x46C y vehiculo (+0x58C) no nulo devuelve IsEntityPointerValid(vehiculo) (0x533310); si no, nodo de colision (+0xB0) no nulo o ped == FindPlayerPed(-1) (0x56E210).

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


# Arenas de Fn114A: 0 objeto, 1 destino/punto, 2 matriz, 3 segundo objeto, 4 evento 1, 5 evento 2, 6 pares de tareas, 7 pool, 8 estados del pool, 9 almacenamiento del pool
A_OBJ, A_OUT, A_MAT, A_O2, A_EV, A_EV2, A_PAIRS, A_POOL, A_FLAGS, A_STORE = range(10)
RAW_VEH = 0x00C0FFEE
RAW_PLAYER = 0x00ABCD00
PED_SIZE = 0x7C4
STORE_BASE = 0x800          # desplazamiento del primer hueco dentro de la arena de almacenamiento
GLOBAL_FLAG, GLOBAL_COUNT, GLOBAL_STATE = 0xC098E8, 0xC098EC, 0x96A7CC


def rmix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rrand(c, k):
    """misma mezcla que Rand() de Fixtures.hpp (sal 114001)"""
    return rmix(((c & M32) * 0x9E3779B1 + (k & M32) * 0x85EBCA6B + 114001) & M32)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


class Mem:
    """memoria dispersa de una arena (desplazamientos respecto al objeto, negativos = guarda anterior): imagen inicial y final para calcular los bytes cambiados"""
    def __init__(self):
        self.init = {}
        self.cur = {}

    def put32(self, off, v):
        for j in range(4):
            self.init[off + j] = (v >> (8 * j)) & 0xFF
            self.cur[off + j] = (v >> (8 * j)) & 0xFF

    def rd32(self, off):
        return sum(self.cur[off + j] << (8 * j) for j in range(4))

    def wr32(self, off, v):
        for j in range(4):
            assert off + j in self.init
            self.cur[off + j] = (v >> (8 * j)) & 0xFF

    def changes(self, arena):
        return [[arena, o, self.cur[o]] for o in sorted(self.init) if self.cur[o] != self.init[o]]


def cls_float(bits_):
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0:
        return 'zero' if m == 0 else 'sub'
    return 'norm'


# ------------------------------------------------------------------------------------------------ funcion 0: CPlaceable::GetRightVector (0x41CC70)
def quiet_bits(bits_):
    return bits_ | 0x00400000


def expected0(c):
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
            new = [q, q, 0]
        elif e == 0xFF:
            marks.append('heading_inf')
            new = [0xFFC00000, 0xFFC00000, 0]
        else:
            lh = load(h)
            v = lh[1]
            if v == 0:
                marks.append('heading_zero')
                new = [0x3F800000, (0x80000000 if lh[2] else 0), 0]
            else:
                if abs(v) > 8:
                    raise ValueError('rumbo fuera del dominio del modelo')
                d = Decimal(v.numerator) / Decimal(v.denominator)
                s_, c_ = sin_cos_dec(d)
                new = [store_dec(c_), store_dec(s_), 0]
                marks.append('heading_finite')
                if abs(v) < Fraction(1, 1 << 20):
                    marks.append('heading_tiny')
    changes = byte_changes(A_OUT, 0, old, new)
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': tag(A_OUT, 0), 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 1: CPlaceable::GetUpVector (0x50E420)
def expected1(c):
    a = c['a']
    marks = []
    old = [a[4] & M32, a[5] & M32, a[6] & M32]
    if a[0]:
        marks.append('matrix')
        new = [a[1] & M32, a[2] & M32, a[3] & M32]
        if any(cls_float(v) == 'nan' for v in new):
            marks.append('row_nan')
        if any(cls_float(v) == 'nan' and not (v & 0x00400000) for v in new):
            marks.append('row_snan')
    else:
        marks.append('no_matrix')
        new = [0, 0, 0x3F800000]
    changes = byte_changes(A_OUT, 0, old, new)
    if not changes:
        marks.append('unchanged')
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': tag(A_OUT, 0), 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 2: CPhysical::ApplyMoveForce(x, y, z) (0x4ABBA0)
def expected2(c):
    a = c['a']
    marks = []
    v = [a[0] & M32, a[1] & M32, a[2] & M32]
    if any(cls_float(x) == 'nan' for x in v):
        marks.append('nan_component')
    if any(cls_float(x) == 'nan' and not (x & 0x00400000) for x in v):
        marks.append('snan_component')
    if any(cls_float(x) == 'inf' for x in v):
        marks.append('inf_component')
    if all(cls_float(x) in ('norm', 'zero') for x in v):
        marks.append('finite')
    return {'events': [[20, tag(A_OBJ, 0)] + v], 'marks': marks, 'changes': [], 'ret': None, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 3: CPedIntelligence::GetPedFOVRange (0x4ABEA0)
def expected3(c):
    a = c['a']
    marks = []
    ab, bb = a[0] & M32, a[1] & M32
    la, lb = load(ab), load(bb)
    if la == NAN:
        marks.append('nan_hearing')
    if lb == NAN:
        marks.append('nan_seeing')
    if la != NAN and lb != NAN:
        marks.append('hearing_greater' if gt(la, lb) else ('equal' if num(la) == num(lb) else 'seeing_greater'))
        if la[0] == 'f' and lb[0] == 'f' and la[1] == 0 and lb[1] == 0:
            marks.append('zeros_%d%d' % (la[2], lb[2]))
        if la[0] == 'i' or lb[0] == 'i':
            marks.append('infinite')
    pick = ab if gt(la, lb) else bb
    return {'events': [], 'marks': marks, 'changes': [], 'ret': None, 'st0': ext_of_bits(pick), 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 4: CPedGroups::RegisterKillByPlayer (0x5F7E30)
def expected4(c):
    a = c['a']
    marks = []
    flag = a[0] & M32
    count = a[1] & M32
    if flag & 0xFF:
        marks.append('flag_set')
        new = (count + 1) & M32
        if count == M32:
            marks.append('counter_wrap')
    else:
        marks.append('flag_clear')
        new = count
        if flag:
            marks.append('garbage_above_flag')
    return {'events': [], 'marks': marks, 'changes': [], 'ret': None, 'st0': None, 'globals': [flag, new, None]}


# ------------------------------------------------------------------------------------------------ funcion 5: CMissionCleanup::AddEntityToList (0x4637E0)
def expected5(c):
    a = c['a']
    marks = []
    k = a[2]
    handle = a[0] & M32
    typ = a[1] & 0xFF
    count_old = a[3] & 0xFF
    changes = []
    if k >= 75:
        marks.append('full')
        return {'events': [], 'marks': marks, 'changes': changes, 'ret': None, 'st0': None, 'globals': None}
    marks.append('slot_first' if k == 0 else ('slot_last' if k == 74 else 'slot_middle'))
    if typ == 0:
        marks.append('type_zero')
    else:
        changes.append([A_OBJ, 8 * k, typ])
    if (a[1] & 0xFFFFFF00):
        marks.append('type_garbage')
    old_handle = a[8] & M32
    for j in range(4):
        ob, nb = (old_handle >> (8 * j)) & 0xFF, (handle >> (8 * j)) & 0xFF
        if ob != nb:
            changes.append([A_OBJ, 8 * k + 4 + j, nb])
    if count_old == 0xFF:
        marks.append('count_wrap')
    if count_old >= 0x80:
        marks.append('count_negative')
    changes.append([A_OBJ, 0x258, (count_old + 1) & 0xFF])
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': None, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 6: CEventDamage::IsSameEventForAI (0x4ADBF0)
def expected6(c):
    a = c['a']
    marks = []
    if a[4]:
        marks.append('alias')
        same = True
    else:
        same_src = (a[0] & M32) == (a[2] & M32)
        same_wpn = (a[1] & M32) == (a[3] & M32)
        same = same_src and same_wpn
        marks.append('same' if same else ('diff_weapon' if same_src else ('diff_source' if same_wpn else 'diff_both')))
    return {'events': [], 'marks': marks, 'changes': [], 'ret': 1 if same else 0, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 7: CEntryExitManager::WeAreInInteriorTransition (0x43E400)
def expected7(c):
    v = c['a'][0] & M32
    marks = ['state_zero'] if v == 0 else ['state_nonzero']
    if v and not (v & 0xFF):
        marks.append('state_zero_low_byte')
    return {'events': [], 'marks': marks, 'changes': [], 'ret': 1 if v else 0, 'st0': None, 'globals': [None, None, v]}


# ------------------------------------------------------------------------------------------------ funcion 8: CPointRoute::AddPoint (0x48E180)
def expected8(c):
    a = c['a']
    marks = []
    count = s32(a[0])
    if count >= 8:
        marks.append('full')
        return {'events': [], 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    mem = Mem()
    off = 4 + 12 * count
    for k in range(3):
        mem.put32(off + 4 * k, a[4 + k] & M32)
    mem.put32(0, a[0] & M32)          # el contador se escribe despues que la entrada (pisa el solape de la entrada -1)
    for k in range(3):
        mem.wr32(off + 4 * k, a[1 + k] & M32)
    mem.wr32(0, (mem.rd32(0) + 1) & M32)
    if count < 0:
        marks.append('negative_count')
    if count == -1:
        marks.append('overlap_count')
    marks.append('added_%s' % ('last' if count == 7 else ('first' if count == 0 else 'middle')))
    if any(cls_float(a[1 + k] & M32) == 'nan' for k in range(3)):
        marks.append('nan_point')
    return {'events': [], 'marks': marks, 'changes': mem.changes(A_OBJ), 'ret': 1, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 9: CPointRoute::RemovePoint (0x5F13A0)
def expected9(c):
    a = c['a']
    marks = []
    count = s32(a[0])
    idx = s32(a[1])
    mem = Mem()
    for j in range(-5, 11):
        for k in range(3):
            mem.put32(4 + 12 * j + 4 * k, rrand(a[2], 50 + (j + 5) * 3 + k))
    mem.put32(0, a[0] & M32)
    last = count - 1
    i = idx
    passes = 0
    while i < last:
        for k in range(3):
            mem.wr32(4 + 12 * i + 4 * k, mem.rd32(4 + 12 * (i + 1) + 4 * k))
        i += 1
        passes += 1
    mem.wr32(0, (mem.rd32(0) - 1) & M32)
    marks.append('passes_%d' % min(passes, 3) if passes < 3 else 'passes_3plus')
    if idx < 0:
        marks.append('negative_index')
    if count <= 0:
        marks.append('count_nonpositive')
    if idx >= count:
        marks.append('index_beyond')
    elif idx == count - 1:
        marks.append('remove_last')
    elif idx == 0:
        marks.append('remove_first')
    else:
        marks.append('remove_middle')
    if count > 8:
        marks.append('count_over_8')
    return {'events': [], 'marks': marks, 'changes': mem.changes(A_OBJ), 'ret': None, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 10: CPedGroupIntelligence::GetTask (0x5F7660)
def pair_ped(a, j, target):
    """valor del ped del par j (el mismo que genera Live: ver CaseRun)"""
    if a[0] == 1:
        if (a[4] >> j) & 1:
            return 0
        v = rrand(a[3], 2 * j) | 1
        return v
    if j == a[1] or j == a[2]:
        return target
    v = rrand(a[3], 2 * j)
    return v ^ 0x00010000 if v == target else v


def expected10(c):
    a = c['a']
    marks = []
    target = {0: 'arena', 1: 0, 2: RAW_VEH}[a[0]]
    tnum = tag(A_O2, 0) if target == 'arena' else target
    first = None
    for j in range(8):
        if pair_ped(a, j, tnum) == tnum:
            first = j
            break
    # pair_ped(.., tnum) con tnum nulo solo devuelve 0 para los pares de la mascara
    if a[0] == 1:
        marks.append('null_search')
    elif a[0] == 2:
        marks.append('raw_ped')
    if first is None:
        marks.append('no_match')
        ret = 0
    else:
        marks.append('match_first' if first == 0 else ('match_last' if first == 7 else 'match_middle'))
        if a[0] != 1 and a[1] < 8 and a[2] < 8 and a[1] != a[2]:
            marks.append('duplicate_first_wins')
        ret = rrand(a[3], 2 * first + 1)
    return {'events': [], 'marks': marks, 'changes': [], 'ret': ret, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 11: CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent (0x4AE100)
PRIORITY_TYPES = {0x36, 0x37, 0x39, 0x47, 0x4C, 0x07, 0x4D}


def expected11(c):
    a = c['a']
    marks = []
    this_ev = tag(A_EV, 0)
    other_ev = tag(A_EV, 0) if a[3] else tag(A_EV2, 0)
    t1 = a[0] & M32
    t2 = (a[0] if a[3] else a[1]) & M32
    ev = [[11, this_ev], [11, other_ev]]
    this_p = t1 in PRIORITY_TYPES
    other_p = t2 in PRIORITY_TYPES
    marks.append('this_priority' if this_p else 'this_plain')
    marks.append('other_priority' if other_p else 'other_plain')
    if a[3]:
        marks.append('same_event')
    if this_p:
        ret = 1
    elif other_p:
        ret = 0
    else:
        ev.append([13, this_ev, other_ev])
        ret = a[2] & 0xFF
        marks.append('virtual_true' if ret else 'virtual_false')
        if a[2] & 0xFFFFFF00:
            marks.append('virtual_garbage')
    if t1 > 0x60 or t2 > 0x60:
        marks.append('type_outside')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': ret, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 12: CEvent::TakesPriorityOverBase (0x4ABC30)
def expected12(c):
    a = c['a']
    marks = []
    this_ev = tag(A_EV, 0)
    other_ev = tag(A_EV, 0) if a[2] else tag(A_EV2, 0)
    p_this = s32(a[0])
    p_other = p_this if a[2] else s32(a[1])
    ev = [[12, other_ev], [12, this_ev]]
    ret = 1 if p_this >= p_other else 0
    marks.append('alias' if a[2] else 'distinct')
    marks.append('greater' if p_this > p_other else ('equal' if p_this == p_other else 'less'))
    if p_this < 0 or p_other < 0:
        marks.append('negative_priority')
    if (p_this < 0) != (p_other < 0):
        marks.append('sign_differs')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': ret, 'st0': None, 'globals': None}


# ------------------------------------------------------------------------------------------------ funcion 13: IsPedPointerValid (0x5E8A30)
def expected13(c):
    a = c['a']
    marks = []
    size = a[0]
    kind = a[1]
    ev = []
    if kind == 1:
        marks.append('null_ped')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    if kind == 2:
        marks.append('below_storage')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    if kind == 3:
        marks.append('above_storage')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    slot_off = a[2] * PED_SIZE + s32(a[3])      # desplazamiento respecto al inicio del almacenamiento
    q = abs(slot_off) // PED_SIZE
    idx = q if slot_off >= 0 else -q          # division con signo, hacia cero
    if idx < 0:
        marks.append('index_negative')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    if idx >= size:
        marks.append('index_beyond_capacity')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    if idx == 0 and slot_off < 0:
        marks.append('index_truncated_to_zero')
    state = ((a[4] >> (8 * idx)) & 0xFF) if idx < 4 else (a[10] & 0xFF)
    if state & 0x80:
        marks.append('slot_free')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 0, 'st0': None, 'globals': None}
    ped_rel = STORE_BASE + slot_off
    in_vehicle = bool((a[5] >> 8) & 1)
    veh = {0: tag(A_O2, 0), 1: 0, 2: RAW_VEH}[a[6]]
    if in_vehicle and veh:
        marks.append('in_vehicle')
        ev.append([33, veh])
        ret = a[9] & 0xFF
        marks.append('vehicle_true' if ret else 'vehicle_false')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': ret, 'st0': None, 'globals': None}
    if in_vehicle:
        marks.append('in_vehicle_without_vehicle')
    if a[7]:
        marks.append('collision_node')
        return {'events': ev, 'marks': marks, 'changes': [], 'ret': 1, 'st0': None, 'globals': None}
    ev.append([30, M32])
    player = {0: ped_rel, 1: 0, 2: STORE_BASE + 0x10, 3: RAW_PLAYER}[a[8]]
    match = player == ped_rel
    marks.append('player_match' if match else 'player_mismatch')
    return {'events': ev, 'marks': marks, 'changes': [], 'ret': 1 if match else 0, 'st0': None, 'globals': None}


def expected(c):
    return [expected0, expected1, expected2, expected3, expected4, expected5, expected6, expected7, expected8, expected9, expected10, expected11, expected12, expected13][c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    if e['globals'] is not None:
        for k, want in enumerate(e['globals']):
            if want is not None and o['globals'][k] != want:
                return 'global %d: %s != %s' % (k, o['globals'][k], want)
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
