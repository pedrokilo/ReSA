"""Oraculo independiente (Python) de Fn100A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las nueve funciones del lote 100A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado, NaN con carga) se modela con fracciones exactas.

Funcion 0, CTask::IsGoToTask (0x61A360..0x61A388; cdecl, (tarea); AL): llama a la ranura +0x10 de la vtabla (GetTaskType) con ECX = tarea; si es 0x384 (900) devuelve 1; si no, la llama de nuevo y devuelve 1 si es 0x39E (926), si no 0.
Funcion 1, CMemoryMgr::Free (0x72F430; cdecl, (puntero)): `jmp 0x82413F` (free del CRT del EXE) con el puntero tal cual (sin comprobar nulo).
Funcion 2, CEvent::operator new (0x4B5620; cdecl, tamano ignorado): ECX = [0xB744AC] y `jmp 0x4B5570` (CEventPool::New): ++ultima; si == capacidad la pone a 0 (una segunda vez devuelve nulo); la primera ranura con el bit 7 del estado
  a 1 se ocupa (bit 7 a 0, contador de 7 bits + 1) y se devuelve almacenamiento + ranura * 0x44. Funcion 3, CObject::operator new (0x5A1EE0): igual con [0xB7449C], 0x5A1C20 y ranuras de 0x19C bytes.
Funcion 4, CVehicle::operator new(tamano, ref) (0x6E2D60; cdecl): idx = ref >> 8 (con signo); estado[idx] = (estado[idx] & 0x7F) con el contador sustituido por ref & 0x7F; ultima = 0 y avanza mientras la ranura este ocupada (0x6D5DD0);
  devuelve almacenamiento + idx * 0xA18 de [0xB74494].
Funcion 5, lerp de CRenPar (0x6E5280; cdecl, (destino oculto, rp1, rp2, t); EAX = destino): omt = 1.0 - t (registro, 24 bits), z/bigWaves/smallWaves = a * omt + b * t sin guardar los productos, guardados como float; flowX y flowY = 0.
Funcion 6, CEventGroupEvent::CEventGroupEvent (0x4ADFD0; thiscall (ped, evento), `ret 8`; EAX = this): +4 = 0, byte +8 = 0, vtabla 0x85AE48 (GetEventType 0x2F, prioridad 41, vida 0), +0xC = ped, +0x10 = evento;
  si el ped no es nulo llama a 0x571B70 (RegisterReference) con ECX = ped y (&this+0xC).
Funcion 7, CEventGroupEvent::~CEventGroupEvent (0x4AE070; thiscall): vtabla 0x85AE48; si el ped (+0xC) no es nulo llama a 0x571A00 (CleanUpOldReference) con ECX = ped y (&this+0xC); si el evento (+0x10) no es nulo
  llama a la ranura 0 de su vtabla con 1 (destructor con borrado); deja la vtabla de CEvent (0x85AB38).
Funcion 8, CEventDamage::~CEventDamage (0x4AD960; thiscall): vtabla 0x85AD78; si la entidad de origen (+0x14) no es nula llama a 0x571A00 con ECX = entidad y (&this+0x14); deja la vtabla de CEvent.

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



STACK = 0x7F000000
GUARD = 0x40
A_OBJ, A_OUT, A_POOL, A_ST, A_STO, A_PED, A_EV = range(7)
M32 = 0xFFFFFFFF
ELEM = {2: 0x44, 3: 0x19C, 4: 0xA18}      # tamano de ranura de los pools de eventos, objetos y vehiculos (desensamblado: imul 0x44 en 0x4B55C9, 0x19C en 0x5A1F02, 0xA18 en 0x6E2D72)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def ptr(kind, raw, arena, off=0):
    """valor que registran los falsos para un puntero: etiqueta de arena, 0 o el valor crudo"""
    return tag(arena, off) if kind == 0 else (0 if kind == 1 else raw & M32)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def byte_changes(arena, base_off, old_words, new_words):
    """cambios de bytes [arena, desplazamiento, byte nuevo] entre dos listas de dwords consecutivos (desde base_off)"""
    out = []
    for k, (o, n) in enumerate(zip(old_words, new_words)):
        for j in range(4):
            ob, nb = (o >> (8 * j)) & 0xFF, (n >> (8 * j)) & 0xFF
            if ob != nb:
                out.append([arena, base_off + 4 * k + j, nb])
    return out


# ------------------------------------------------------------------------------------------------ aritmetica x87 con NaN (24 bits, exponente ampliado)
INDEF = ('n', 0x400000, 1)      # indefinido real: NaN silencioso negativo (0xFFC00000)


def ldq(bits_):
    """fld dword: los NaN conservan signo y carga y quedan silenciados"""
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF and m:
        return ('n', m | 0x400000, bits_ >> 31)
    return load(bits_)


def isn(x):
    return x[0] == 'n'


def pick_nan(a, b):
    """resultado de una operacion con un NaN: con uno solo, ese (silenciado); con dos, el SNaN pierde frente al QNaN y, a igual clase, gana el mayor significando"""
    if isn(a) and isn(b):
        sa, sb = not (a[1] & 0x400000), not (b[1] & 0x400000)
        if sa != sb:
            r = b if sa else a
        else:
            r = a if a[1] >= b[1] else b
    else:
        r = a if isn(a) else b
    return ('n', r[1] | 0x400000, r[2])


def mq(bits_):
    """operando float en memoria de una instruccion aritmetica (fmul/fsub m32): un NaN senalizante conserva su clase (el fld lo silencia)"""
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF and m:
        return ('n', m, bits_ >> 31)
    return load(bits_)


def fix(r):
    return INDEF if r == NAN else r


def qmul(a, b):
    if isn(a) or isn(b):
        return pick_nan(a, b)
    return fix(mul(a, b))


def qadd(a, b):
    if isn(a) or isn(b):
        return pick_nan(a, b)
    return fix(add(a, b))


def qsub(a, b):
    if isn(a) or isn(b):
        return pick_nan(a, b)
    return fix(sub(a, b))


def qbits(x):
    """fstp dword"""
    if isn(x):
        return (x[2] << 31) | 0x7F800000 | (x[1] & 0x7FFFFF)
    b = store(x)
    return NAN_BITS if b is None else b


# ------------------------------------------------------------------------------------------------ funcion 0: CTask::IsGoToTask (0x61A360)
def expected0(c):
    t1, t2 = c['a'][0] & M32, c['a'][1] & M32
    events = [[10, tag(A_OBJ, 0)]]
    marks = []
    if t1 == 900:
        ret = 1
        marks.append('first_hit')
    else:
        events.append([10, tag(A_OBJ, 0)])
        marks.append('second_called')
        if t2 == 926:
            ret = 1
            marks.append('second_hit')
        else:
            ret = 0
            marks.append('neither')
        if t1 == 926:
            marks.append('first_is_926')
        if t2 == 900:
            marks.append('second_is_900')
    if (t1 >> 31) or (t2 >> 31):
        marks.append('negative')
    return {'events': events, 'marks': marks, 'changes': [], 'ret': ret, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 1: CMemoryMgr::Free (0x72F430)
def expected1(c):
    a = c['a']
    p = ptr(a[0], a[1], A_OBJ, a[2])
    marks = [('arena', 'null', 'raw')[a[0]]]
    return {'events': [[40, p]], 'marks': marks, 'changes': [], 'ret': None, 'st0': None}


# ------------------------------------------------------------------------------------------------ funciones 2, 3 y 4: operator new de pool
def pool_new(c, fn):
    a = c['a']
    cap = a[1] & M32
    last = s32(a[2])
    st = [a[3 + i] & 0xFF for i in range(8)]
    old_st = list(st)
    marks = []
    ret = 0
    if fn == 4:
        ref = a[11] & M32
        idx = ref >> 8
        st[idx] = ref & 0x7F
        last = 0
        while not (st[last] & 0x80):
            last += 1
        ret = tag(A_STO, idx * ELEM[4])
        marks.append('idx%d' % min(idx, 7))
        marks.append('ref_low_max' if (ref & 0x7F) == 0x7F else 'ref_low_other')
        if old_st[idx] & 0x7F:
            marks.append('prev_counter')
    else:
        wrapped = False
        while True:
            last += 1
            if last == cap:
                last = 0
                if wrapped:
                    break
                wrapped = True
                marks.append('wrap')
            if st[last] & 0x80:
                new = ((st[last] & 0x7F) + 1) & 0x7F
                if (st[last] & 0x7F) == 0x7F:
                    marks.append('counter_wrap')
                st[last] = new
                ret = tag(A_STO, last * ELEM[fn])
                break
        marks.append('alloc' if ret else 'full')
        if s32(a[2]) == -1:
            marks.append('last_neg1')
        if ret and last == 0:
            marks.append('slot0')
    changes = byte_changes(A_POOL, 0xC, [a[2] & M32], [last & M32])
    for i in range(8):
        if st[i] != old_st[i]:
            changes.append([A_ST, i, st[i]])
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': ret, 'st0': None}


def expected2(c):
    return pool_new(c, 2)


def expected3(c):
    return pool_new(c, 3)


def expected4(c):
    return pool_new(c, 4)


# ------------------------------------------------------------------------------------------------ funcion 5: CWaterLevel LerpRenPar (0x6E5280)
def expected5(c):
    a = [x & M32 for x in c['a']]
    t = mq(a[8])                                   # fsub / fmul con t en memoria
    omt = qsub(ldq(0x3F800000), t)                 # fld 1.0; fsub dword [t]: queda en el registro
    out = []
    marks = []
    for i in range(3):
        # componentes 0 y 1: fld a; fmul st, st(1) (a cargado, omt en registro); componente 2: fmul dword [a] sobre omt (a en memoria)
        x = qmul(ldq(a[i]), omt) if i < 2 else qmul(omt, mq(a[i]))
        y = qmul(ldq(a[4 + i]), t)                 # fld b; fmul dword [t]
        r = qadd(x, y)
        out.append(qbits(r))
        if isn(r):
            marks.append('nan_result')
        elif r[0] == 'i':
            marks.append('inf_result')
        elif r[1] == 0:
            marks.append('zero_result')
    if isn(t):
        marks.append('t_nan')
    elif t[0] == 'f' and t[1] == 0:
        marks.append('t_zero')
    elif t[0] == 'f' and 0 < t[1] < 1:
        marks.append('t_inside')
    elif t[0] == 'f' and t[1] == 1:
        marks.append('t_one')
    elif t[0] == 'f':
        marks.append('t_outside')
    else:
        marks.append('t_inf')
    if any(isn(ldq(a[i])) for i in (0, 1, 2, 4, 5, 6)):
        marks.append('nan_input')
    old = a[9:13]
    new = out + [old[3] & 0xFFFF0000]
    return {'events': [], 'marks': marks, 'changes': byte_changes(A_OUT, 0, old, new), 'ret': tag(A_OUT, 0), 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 6: CEventGroupEvent::CEventGroupEvent (0x4ADFD0)
def expected6(c):
    a = [x & M32 for x in c['a']]
    off = a[4]
    ped = ptr(a[0], a[1], A_PED)
    ev = ptr(a[2], a[3], A_EV)
    events = [] if a[0] == 1 else [[31, ped, tag(A_OBJ, off + 0xC)]]
    old = a[6:11]
    new = [0, old[2] & ~0xFF]
    changes = byte_changes(A_OBJ, off + 4, [old[1], old[2]], new)
    marks = ['ped_' + ('arena', 'null', 'raw')[a[0]], 'ev_' + ('arena', 'null', 'raw')[a[2]], 'off%d' % off, 'fill%d' % a[5]]
    return {'events': events, 'marks': marks, 'changes': changes, 'ret': tag(A_OBJ, off), 'st0': None, 'ptrs': [ped, ev], 'vt': [0x2F, 41, 0], 'vptr_changed': True}


# ------------------------------------------------------------------------------------------------ funcion 7: CEventGroupEvent::~CEventGroupEvent (0x4AE070)
def expected7(c):
    a = [x & M32 for x in c['a']]
    off = a[3]
    ped = ptr(a[0], a[1], A_PED)
    ev = tag(A_EV, 0) if a[2] == 0 else 0
    events = []
    if a[0] != 1:
        events.append([32, ped, tag(A_OBJ, off + 0xC)])
    if a[2] == 0:
        events.append([33, ev, 1])
    marks = ['ped_' + ('arena', 'null', 'raw')[a[0]], 'ev_' + ('arena', 'null')[a[2]], 'off%d' % off, 'fill%d' % a[4]]
    return {'events': events, 'marks': marks, 'changes': [], 'ret': None, 'st0': None, 'ptrs': [ped, ev], 'vptr_changed': True}


# ------------------------------------------------------------------------------------------------ funcion 8: CEventDamage::~CEventDamage (0x4AD960)
def expected8(c):
    a = [x & M32 for x in c['a']]
    off = a[2]
    ent = ptr(a[0], a[1], A_PED)
    events = [] if a[0] == 1 else [[32, ent, tag(A_OBJ, off + 0x14)]]
    marks = ['ent_' + ('arena', 'null', 'raw')[a[0]], 'off%d' % off, 'fill%d' % a[3]]
    return {'events': events, 'marks': marks, 'changes': [], 'ret': None, 'st0': None, 'ptrs': [ent], 'vptr_changed': True}


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected5, 6: expected6, 7: expected7, 8: expected8}[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    for k in ('ptrs', 'vt', 'vptr_changed'):
        if k in e and o.get(k) != e[k]:
            return '%s %s != %s' % (k, o.get(k), e[k])
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
