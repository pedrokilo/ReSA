"""Oraculo independiente (Python) de Fn92B: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de la funcion del lote 92B y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Funcion 0, CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents (0x606890..0x606B96; thiscall, ECX = escaner de 0xC bytes (CTaskTimer), ped en pila, `ret 4`; void):
  Temporizador en linea (CTaskTimer: +0 inicio, +4 intervalo, +8 arrancado, +9 detenido; los booleanos se leen como bytes): sin arrancar = inicio hora, intervalo 500, arrancado 1;
  detenido = inicio hora, detenido 0; vence si hora >= inicio + intervalo (32 bits sin signo); al vencer: inicio = hora, intervalo 500, arrancado 1.
  GetSimplestActiveTask(inteligencia + 4); si hay tarea e IsGoToTask (tipo 900 o 926; el tipo se pide una vez por comparacion): estado = [tarea + 8], si no 1.
  CPed::IsPlayer (+0x598 == 0 o 1) sale; estado 1 sale; GetSimplestActiveTask otra vez: sin tarea o sin IsGoToTask sale (su punto de destino es tarea + 0xC).
  Escaner de objetos temporal (0x606010 / 0x5FFF30(escaner, ped) / 0x5FF9D0 al salir); objeto = [escaner + 0x4C]; sin objeto sale. Se descartan las banderas +0x140 con el bit 0 o el 10 y los que no tienen el bit 0 de +0x1C.
  d = ped - objeto (0x40FE00: cada resta guardada como float), cuadrado (0x406DA0) = (x*x + y*y) + z*z, debe ser < 7.5 * 7.5 (0x86C8FC, registro, comparacion ordenada).
  GetBoundCentre (0x534250): z del centro; radio de la esfera = [[0xA9B0C8 + 4 * (int16)[objeto + 0x22]] + 0x14] + 0x24. Se descarta si (z - 1.0) > (radio + centro) (registro) o (z + 1.0, guardado) < (centro - radio, guardado).
  GetIsLineOfSightClear 0x5F5A30(ped, destino, objeto, &distancia = 0) (cdecl): AL != 0 sale; la distancia debe ser > 0.5 (0x86C900). GetIsLineOfSightClear 0x5F2F00(ped - (0, 0, 0.75), destino, objeto): AL != 0 sale.
  CEventPotentialWalkIntoObject(objeto, estado) (0x4AE5D0, RegisterReference), CEventGroup::Add(inteligencia + 0x68, evento, 0) (0x4AB420), destructor (0x4AE670, CleanUpOldReference); el escaner se limpia (0x5FF9D0).

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



WALK_INTO_OBJECT = 44
ONE = load(0x3F800000)
TWO = load(0x40000000)
ZERO_F = load(0)
K75 = load(0x40F00000)
K05 = load(0x3F000000)
K075 = load(0x3F400000)
M32 = 0xFFFFFFFF
STACK = 0x7F000000
GUARD = 0x40
A_SCAN, A_PED, A_PMAT, A_INTEL, A_T1, A_T2, A_OBJ, A_OMAT, A_MINFO, A_COL = range(10)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def le(words):
    out = []
    for w in words:
        out += [(w >> (8 * j)) & 0xFF for j in range(4)]
    return out


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def lt(a, b):
    return gt(b, a)


def fabs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def timer(c, first_interval, interval_after, marks):
    """Temporizador en linea: devuelve (imagen inicial, imagen final, vencido)"""
    img = le(c['scan'])
    now = c['now'] & M32
    start = int.from_bytes(img[0:4], 'little')
    interval = int.from_bytes(img[4:8], 'little')
    started, stopped = img[8], img[9]
    if started == 0:
        marks.append('not_started')
        interval, start, started = first_interval, now, 1
    else:
        marks.append('started')
    if (c['scan'][2] & 0xFF) > 1:
        marks.append('garbage_started')
    if stopped:
        marks.append('stopped')
        if stopped > 1:
            marks.append('garbage_stopped')
        start, stopped = now, 0
    if interval & 0x80000000:
        marks.append('negative_interval')
    total = (start + interval) & M32
    if start + interval > M32:
        marks.append('wrapped')
    if now == total:
        marks.append('boundary')
    expired = now >= total
    marks.append('expired' if expired else 'not_expired')
    if expired:
        start, interval, started = now, interval_after, 1
    final = b4(start) + b4(interval) + [started, stopped] + img[10:16]
    return img, final, expired


def changes_of(img, final):
    return [[A_SCAN, i, final[i]] for i in range(16) if final[i] != img[i]]


def add_event(ev, etype, payload):
    ev.append([5, tag(A_INTEL, 0x68), etype, 0, 0, 1, 0xC8, 0xFFFF] + payload + [0])


def expected0(c):
    marks, ev = [], []
    img, final, expired = timer(c, 500, 500, marks)
    res = {'changes': changes_of(img, final), 'events': ev, 'marks': marks}
    if not expired:
        return res
    intel_tm = tag(A_INTEL, 4)

    def is_goto(arena, typ):
        ev.append([4, tag(arena, 0)])
        if (typ & M32) == 900:
            return True
        ev.append([4, tag(arena, 0)])
        return (typ & M32) == 926

    def clear():
        ev.append([11, STACK])
        return res

    ev.append([1, intel_tm])
    move = 1
    if c['t1']:
        marks.append('t1')
        if is_goto(A_T1, c['t1type']):
            marks.append('t1_goto')
            move = c['t1move'] & M32
    if (c['ptype'] & M32) in (0, 1):
        marks.append('player')
        return res
    if move == 1:
        marks.append('still')
        return res
    ev.append([1, intel_tm])
    if not c['t2']:
        marks.append('t2_none')
        return res
    if not is_goto(A_T2, c['t2type']):
        marks.append('t2_not_goto')
        return res
    marks.append('t2_goto')
    ev.append([10, STACK, tag(A_PED, 0)])
    if not c['opresent']:
        marks.append('no_object')
        return clear()
    if (c['oflags'] & 0x401) or not (c['o1c'] & 1):
        marks.append('flags_skip')
        return clear()
    marks.append('object')
    ppos = [v & M32 for v in c['ppos']]
    opos = [v & M32 for v in c['opos']]
    dx = reload(bits(sub(load(ppos[0]), load(opos[0]))))
    dy = reload(bits(sub(load(ppos[1]), load(opos[1]))))
    dz = reload(bits(sub(load(ppos[2]), load(opos[2]))))
    sq = add(add(mul(dx, dx), mul(dy, dy)), mul(dz, dz))
    if not gt(mul(K75, K75), sq):
        marks.append('far')
        return clear()
    marks.append('near')
    ev.append([12, tag(A_OBJ, 0), STACK])
    zc = load(c['cz'][2] & M32)
    radius = load(c['radius'] & M32)
    pz = load(ppos[2])
    below = sub(pz, ONE)
    above = reload(bits(add(pz, ONE)))
    bottom = reload(bits(sub(zc, radius)))
    top = add(radius, zc)
    if gt(below, top):
        marks.append('z_below')
        return clear()
    if gt(bottom, above):
        marks.append('z_above')
        return clear()
    marks.append('z_ok')
    ttgt = [v & M32 for v in c['ttgt']]
    ev.append([13, tag(A_PED, 0), tag(A_T2, 0xC)] + ttgt + [tag(A_OBJ, 0), STACK, 0])
    if c['los1'] & 1:
        marks.append('los1_clear')
        return clear()
    if not gt(load(c['dist'] & M32), K05):
        marks.append('dist_small')
        return clear()
    marks.append('dist_ok')
    low = [bits(sub(load(ppos[0]), ZERO_F)), bits(sub(load(ppos[1]), ZERO_F)), bits(sub(load(ppos[2]), K075))]
    ev.append([14, STACK, tag(A_T2, 0xC)] + low + ttgt + [tag(A_OBJ, 0)])
    if c['los2'] & 1:
        marks.append('los2_clear')
        return clear()
    marks.append('event')
    ev.append([8, tag(A_OBJ, 0), STACK])
    add_event(ev, WALK_INTO_OBJECT, [tag(A_OBJ, 0), move])
    ev.append([9, tag(A_OBJ, 0), STACK])
    return clear()


def expected(c):
    return {0: expected0}[c['fn']](c)


def check_side(c, o, e):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
