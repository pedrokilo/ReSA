"""Oraculo independiente (Python) de Fn85F: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de
CTaskSimpleFight::FindTargetOnGround (0x61C9B0, thiscall, ret 4) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Modelo (desensamblado 0x61C9B0..0x61CB93):
  Se piden las asociaciones de las animaciones 0x29, 0x2A, 0x2B y 0x2C con RpAnimBlendClumpGetAssociation(clump del ped +0x18, id) (cdecl; el clump se relee en cada llamada).
  Despues se leen el byte +0x26 (siguiente) y el +0x24 (combo) de la tarea: si ambos son cero va a "parada".
  Si no se leen x (+0xC) e y (+0x10) de los datos del jugador (+0x480 del ped) y se calcula con la x87 a 24 bits: longitud = raiz(x*x + y*y); si longitud < 0.1f (comparacion ordenada: con NaN
  no se cumple; con exactamente 0.1f tampoco: `fcomp; test ah,5; jp` salta a movimiento con ah&5 = 0, que es longitud >= 0.1) va a "parada".
  Parada: cada asociacion existente (en el orden 0x29, 0x2A, 0x2B, 0x2C) recibe +0x1C = -8.0f; combo (+0x24), ultimo (+0x27) y siguiente (+0x26) = 0.
  Movimiento: k = 1 / (|y| + |x|) (24 bits); fx = float(k * x), fy = float(k * y) (guardados en la pila). Eje x: fx > 0: la asociacion 0x2A (si existe) pasa a peso 0, la 0x2C (creada con
  AddAnimation(clump, 0, 0x2C) si falta) recibe peso fx; fx < 0: la 0x2C (si existe) a 0, la 0x2A (creada si falta) recibe -fx; fx cero o NaN: nada. Eje y: fy < 0: la 0x2B (si existe) a 0, la 0x29
  (creada si falta) recibe -fy; fy > 0: la 0x29 (si existe) a 0, la 0x2B (creada si falta) recibe fy; fy cero o NaN: nada. Al final combo = 1, ultimo = siguiente (releido) y siguiente = 0.
  Los pesos (+0x18) y retardos (+0x1C) son floats; las asociaciones nuevas son las que devuelve AddAnimation (1.a, 2.a... llamada).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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


def ilog2(v):
    """floor(log2(v)) para un racional positivo"""
    e = v.numerator.bit_length() - v.denominator.bit_length()
    if Fraction(2) ** e > v:
        e -= 1
    elif Fraction(2) ** (e + 1) <= v:
        e += 1
    return e


def rnd(v):
    if v == 0:
        return v
    a = abs(v)
    e = ilog2(a)
    quantum = Fraction(2) ** (e - 23)
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


def div(a, b):
    """a / b (a finito y distinto de cero en este modelo)"""
    if a == NAN or b == NAN:
        return NAN
    if b[0] == 'i':
        return ('f', Fraction(0), 0 if a[2] == b[1] else 1) if a[0] == 'f' else NAN
    if b[1] == 0:
        return ('i', a[2] ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def sqrt(a):
    """raiz cuadrada con el resultado redondeado a 24 bits (al par)"""
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    e = ilog2(v) // 2                      # floor(log2(sqrt(v)))
    t = v * Fraction(4) ** (23 - e)        # sqrt(t) en [2^23, 2^24)
    n = isqrt(t.numerator // t.denominator)
    while (n + 1) * (n + 1) <= t:
        n += 1
    while n * n > t:
        n -= 1
    # compara t con (n + 1/2)^2 = n^2 + n + 1/4
    half = n * n + n + Fraction(1, 4)
    if t > half or (t == half and n % 2 == 1):
        n += 1
    r = Fraction(n) * Fraction(2) ** (e - 23)
    return ('f', r, 0)


def fabs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def lt(a, b):
    """a < b ordenado (fcomp + test ah): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) < num(b)


def gt(a, b):
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def le(a, b):
    if a == NAN or b == NAN:
        return False
    return num(a) <= num(b)


NANBITS = 0xFFC00000
INFBITS = 0x7F800000


def f32bits(a):
    """fstp dword de un valor ya redondeado a 24 bits: bits del float (con desnormales)"""
    if a == NAN:
        return NANBITS
    if a[0] == 'i':
        return (a[1] << 31) | INFBITS
    v, sign = a[1], a[2]
    if v == 0:
        return (sign << 31)
    sign = 1 if v < 0 else 0
    av = abs(v)
    e = ilog2(av)
    q = max(e - 23, -149)
    quantum = Fraction(2) ** q
    k = av / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    if r >= Fraction(2) ** 128:
        return (sign << 31) | INFBITS
    if r == 0:
        return sign << 31
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    ee = ilog2(r)
    mant = int(r / Fraction(2) ** (ee - 23)) & 0x7FFFFF
    return (sign << 31) | ((ee + 127) << 23) | mant


def isnan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def st(a):
    """fstp dword + fld dword: el valor pasa por un float en memoria"""
    return load(f32bits(a))


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def byte_changes(base, old, new):
    return [[base + j, (new >> (8 * j)) & 0xFF] for j in range(4) if ((old >> (8 * j)) & 0xFF) != ((new >> (8 * j)) & 0xFF)]


C_01 = load(0x3DCCCCCD)      # 0.1f
C_ONE = load(0x3F800000)
ZERO = load(0)
DELTA = 0xC1000000           # -8.0f


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    clump = c['clump'] & M32
    nxt, combo, last = c['next'] & 0xFF, c['combo'] & 0xFF, c['last'] & 0xFF
    px, py = c['x'] & M32, c['y'] & M32          # floats de los datos pdA
    pdptr_alt = False
    site = c['mutsite']
    ma, mb = c['muta'] & M32, c['mutb'] & M32
    amt = [v & M32 for v in c['amt']]
    dlt = [v & M32 for v in c['dlt']]
    newamt = [v & M32 for v in c['newamt']]
    newdlt = [v & M32 for v in c['newdlt']]
    pres = [bool(v) for v in c['pres']]
    n_add = 0
    new_changed = {}

    # --- las cuatro llamadas a RpAnimBlendClumpGetAssociation
    for i, anim in enumerate((0x29, 0x2A, 0x2B, 0x2C)):
        ev.append([1, clump, anim])
        if anim == 0x29 and site == 1:
            nxt, combo = ma & 0xFF, mb & 0xFF
            mark('mut1_applied')
        if anim == 0x2C and site == 2:
            pdptr_alt = True
            mark('mut2_applied')
        if anim == 0x2C and site == 5:
            px, py = ma, mb
            mark('mut5_applied')

    pda_x0, pda_y0 = c['x'] & M32, c['y'] & M32
    new_amount = {}      # indice de asociacion existente -> bits nuevos del peso
    new_delta = {}
    moving = False
    if nxt == 0 and combo == 0:
        mark('stop_idle')
    else:
        xbits, ybits = (c['xb'] & M32, c['yb'] & M32) if pdptr_alt else (px, py)
        xv, yv = load(xbits), load(ybits)
        length = sqrt(add(mul(xv, xv), mul(yv, yv)))
        if length == NAN:
            mark('len_nan')
        elif length[0] == 'i':
            mark('len_inf')
        elif num(length) == num(C_01):
            mark('len_equal')
        elif abs(num(length) - num(C_01)) <= num(C_01) * Fraction(1, 1 << 22):
            mark('len_near_above' if num(length) > num(C_01) else 'len_near_below')
        moving = not lt(length, C_01)
        if not moving:
            mark('stop_len')
    if not moving:
        mark('stop')
        for i in range(4):
            if pres[i]:
                new_delta[i] = DELTA
                mark('delta_assoc_%d' % i)
        nxt, combo, last = 0, 0, 0
    else:
        mark('blend')
        k = div(C_ONE, add(fabs(yv), fabs(xv)))
        fxr, fyr = mul(k, xv), mul(k, yv)
        fx, fy = f32bits(fxr), f32bits(fyr)
        fxv, fyv = load(fx), load(fy)
        for nm, bits in (('fx', fx), ('fy', fy)):
            if (bits & 0x7F800000) == 0 and (bits & 0x7FFFFF):
                mark('denorm_' + nm)

        def weight(index, bits, other):
            nonlocal n_add, clump, px, py
            if pres[other]:
                new_amount[other] = 0
                mark('zero_other_%d' % other)
            if not pres[index]:
                ev.append([2, clump, 0, 0x29 + index])
                mark('create_%X' % (0x29 + index))
                j = n_add
                n_add += 1
                new_amount[('n', j)] = bits
                pres_new[index] = j
                if site == 3 and not mut_done[3]:
                    mut_done[3] = True
                    clump = ma
                    mark('mut3_applied')
                if site == 4 and not mut_done[4]:
                    mut_done[4] = True
                    px, py = ma, mb
                    mark('mut4_applied')
            elif index in pres_new:
                new_amount[('n', pres_new[index])] = bits
            else:
                new_amount[index] = bits
        mut_done = {3: False, 4: False}
        pres_new = {}
        if gt(fxv, ZERO):
            mark('fx_pos')
            weight(3, fx, 1)
        elif lt(fxv, ZERO):
            mark('fx_neg')
            weight(1, fx ^ 0x80000000, 3)
        else:
            mark('fx_nan' if fxv == NAN else 'fx_zero')
        if lt(fyv, ZERO):
            mark('fy_neg')
            weight(0, fy ^ 0x80000000, 2)
        elif gt(fyv, ZERO):
            mark('fy_pos')
            weight(2, fy, 0)
        else:
            mark('fy_nan' if fyv == NAN else 'fy_zero')
        if site == 3 and mut_done[3] and n_add >= 2:
            mark('mut3_second_add')
        if (gt(fxv, ZERO) or lt(fxv, ZERO)) and (gt(fyv, ZERO) or lt(fyv, ZERO)):
            mark('both_axes')
        last = nxt
        combo = 1
        nxt = 0
        mark('last_copy')
    # el peso de una asociacion existente que se anula y despues se vuelve a escribir no ocurre (cada una pertenece a un solo eje)
    # --- cambios de bytes
    out = {}
    out['task_changes'] = ([[0x24, combo]] if combo != (c['combo'] & 0xFF) else []) + ([[0x26, nxt]] if nxt != (c['next'] & 0xFF) else []) + ([[0x27, last]] if last != (c['last'] & 0xFF) else [])
    out['task_changes'].sort()
    out['ped_changes'] = byte_changes(0x18, c['clump'] & M32, clump)
    out['player_data'] = tag(3, 0) if pdptr_alt else tag(2, 0)
    out['pda_changes'] = byte_changes(0xC, pda_x0, px) + byte_changes(0x10, pda_y0, py)
    out['pdb_changes'] = []
    ac, nc = [], []
    for i in range(4):
        ch = []
        if i in new_amount:
            ch += byte_changes(0x18, amt[i], new_amount[i])
        if i in new_delta:
            ch += byte_changes(0x1C, dlt[i], new_delta[i])
        ac.append(ch)
    for j in range(4):
        nc.append(byte_changes(0x18, newamt[j], new_amount[('n', j)]) if ('n', j) in new_amount else [])
    out['assoc_changes'] = ac
    out['new_changes'] = nc
    out['events'] = ev
    out['marks'] = marks
    return out


CHANGE_KEYS = ('task_changes', 'ped_changes', 'player_data', 'pda_changes', 'pdb_changes', 'assoc_changes', 'new_changes')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
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
