"""Oraculo independiente (Python) de Fn85B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de las cuatro funciones del lote 85B
(CGarage::RestoreCarsForThisImpoundingGarage 0x4485C0, CountCarsWithCenterPointWithinGarage 0x4495F0, SlideDoorOpen 0x44A660, SlideDoorClosed 0x44A750)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica de SlideDoorOpen/SlideDoorClosed es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); la posicion de la puerta se guarda
como float (fstp dword: redondeo al par con desnormales y desbordamiento a infinito).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

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


def rnd(v):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
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


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def lt(a, b):
    """a < b ordenado (fcomp + test ah,5 / jp): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) < num(b)


def gt(a, b):
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


# ---------------------------------------------------------------- codificacion del registro de la x87 (80 bits, explicito el bit entero)
def enc80(a):
    """bytes (hex, orden de memoria) del registro ST0 para un valor ('f', v, neg) o ('i', neg)"""
    if a == NAN:
        return 'n'
    if a[0] == 'i':
        mant, exp, sign = 0x8000000000000000, 0x7FFF, a[1]
    else:
        v, sign = a[1], a[2]
        if v == 0:
            mant, exp = 0, 0
        else:
            av = abs(v)
            e = av.numerator.bit_length() - av.denominator.bit_length()
            if Fraction(2) ** e > av:
                e -= 1
            elif Fraction(2) ** (e + 1) <= av:
                e += 1
            m = av / Fraction(2) ** (e - 63)
            assert m.denominator == 1
            mant, exp = int(m), e + 16383
            sign = 1 if v < 0 else 0
    return (mant.to_bytes(8, 'little') + ((sign << 15) | exp).to_bytes(2, 'little')).hex()



NANBITS = 0xFFC00000
INFBITS = 0x7F800000


def f32bits(a):
    """fstp dword de un valor ya redondeado a 24 bits: bits del float"""
    if a == NAN:
        return NANBITS
    if a[0] == 'i':
        return (a[1] << 31) | INFBITS
    v, sign = a[1], a[2]
    if v == 0:
        return (sign << 31)
    sign = 1 if v < 0 else 0
    av = abs(v)
    e = av.numerator.bit_length() - av.denominator.bit_length()
    if Fraction(2) ** e > av:
        e -= 1
    elif Fraction(2) ** (e + 1) <= av:
        e += 1
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
    ee = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** ee > r:
        ee -= 1
    elif Fraction(2) ** (ee + 1) <= r:
        ee += 1
    mant = int(r / Fraction(2) ** (ee - 23)) & 0x7FFFFF
    return (sign << 31) | ((ee + 127) << 23) | mant


def isnan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


T_GAR, T_VEH0, T_CARS, T_POOL, T_DOOR0, T_DOOR1, T_MAT0, T_MAT1, T_MAP, T_PSTRUCT, T_MATPOOL = 0, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13
SLOT = 0xA18
C_OPEN = {'std': 0x3C343958, 'hangar': 0x3A902DE0}      # 0.011f, 0.0011f (floats de 0x8599A8 / 0x8599AC)
C_CLOSE = {'std': 0x3C54FDF4, 'hangar': 0x3AAA64C3}     # 0.013f, 0.0013f (0x8599B0 / 0x8599B4)


def vehtag(k):
    return tag(T_VEH0 + k, 0)


def bits_ge(bits, ref):
    """float(bits) >= float(ref) ordenado (NaN falso); ref es un float finito"""
    a = load(bits)
    if a == NAN:
        return False
    return num(a) >= num(load(ref))


def bits_le(bits, ref):
    a = load(bits)
    if a == NAN:
        return False
    return num(a) <= num(load(ref))


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    ev = []
    out = {'events': ev, 'marks': marks}
    flat = list(c.get('ppos', [])) + [c.get('pos', 0), c.get('ts', 0)] if fn != 0 else []
    for x in flat:
        if isnan(x):
            mark('nan_input')
        elif (x & 0x7F800000) == 0x7F800000:
            mark('inf_input')
        elif (x & 0x7F800000) == 0 and (x & 0x7FFFFF):
            mark('denormal_input')
        elif x == 0x80000000:
            mark('negzero_input')
    if fn == 0:
        models = [m & 0xFFFF for m in c['cmodel']]
        vb = list(c['vb'])
        vch = [[] for _ in range(4)]
        cch = []
        call = 0
        for k in range(3):
            if models[k] == 0:
                mark('empty_slot')
                continue
            ev.append([1, tag(T_CARS, 0x40 * k), call])
            call += 1
            if not c['rret'][k]:
                mark('restore_null')
                continue
            mark('restore_ok')
            nb = vb[k] | 0x20
            if nb != vb[k]:
                vch[k].append([0x42D, nb])
            vb[k] = nb
            ev.append([2, vehtag(k), nb] + [m & 0xFFFF for m in models])
            t = c['vtype'][k] & M32
            if t == 0:
                ev.append([3, vehtag(k)])
                mark('type_auto')
            elif t == 9:
                ev.append([4, vehtag(k)])
                mark('type_bike')
            else:
                mark('type_other')
            models[k] = 0
        for k in range(3):
            if (c['cmodel'][k] & 0xFFFF) != models[k]:
                lo, hi = c['cmodel'][k] & 0xFF, (c['cmodel'][k] >> 8) & 0xFF
                if lo:
                    cch.append([0x40 * k + 0x12, 0])
                if hi:
                    cch.append([0x40 * k + 0x13, 0])
        r = 1 if all(m == 0 for m in models[:3]) else 0
        mark('ret_true' if r else 'ret_false')
        if sum(1 for k in range(3) if c['cmodel'][k] & 0xFFFF) == 3:
            mark('all_full')
        if (c['cmodel'][3] & 0xFFFF) and all((m & 0xFFFF) == 0 for m in c['cmodel'][:3]):
            mark('fourth_ignored')
        out['ret'] = r
        out['cars_changes'] = cch
        out['veh_changes'] = vch
    elif fn == 1:
        size = c['psize']
        ign = c['ign']
        ignored_tag = 0 if ign == 0 else (tag(T_POOL, (ign - 1) * SLOT) if ign <= 8 else (vehtag(0) if ign == 9 else 0x12345678))
        mark('ign_null' if ign == 0 else ('ign_in_pool' if ign <= 8 else 'ign_external'))
        if size == 0:
            mark('size_zero')
        if size == 8:
            mark('size_8')
        calls = 0
        count = 0
        occupied = 0
        for i in range(size, 0, -1):
            sl = i - 1
            if c['pmap'][sl] & 0x80:
                mark('empty_skipped')
                continue
            occupied += 1
            if tag(T_POOL, sl * SLOT) == ignored_tag:
                mark('ignored_hit')
                continue
            mark('with_matrix' if c['pmat'][sl] else 'no_matrix')
            x, y, z = c['ppos'][sl * 3:sl * 3 + 3]
            ev.append([5, tag(T_GAR, 0), x, y, z])
            inside = (c['imask'] >> calls) & 1
            calls += 1
            if inside:
                count += 1
        if occupied == 0:
            mark('all_empty')
        mark('some_inside' if count else 'none_inside')
        if count == 0:
            mark('ret_zero')
        out['ret'] = count
        out['pool_changes'] = []
        out['matpool_changes'] = []
        out['gar_changes'] = []
    else:
        open_ = fn == 2
        gt = c['gtype'] & 0xFF
        hangar = gt in (44, 45)
        mark('type44' if gt == 44 else ('type45' if gt == 45 else 'type_other'))
        step = (C_OPEN if open_ else C_CLOSE)['hangar' if hangar else 'std']
        prod = mul(load(c['ts']), load(step))
        pos = load(c['pos'])
        newv = add(prod, pos) if open_ else sub(pos, prod)
        nb = f32bits(newv)
        if open_:
            finished = bits_ge(nb, 0x3F800000)
            if finished:
                nb = 0x3F800000
        else:
            finished = bits_le(nb, 0)
            if finished:
                nb = 0
        mark('finished' if finished else 'not_finished')
        if isnan(nb):
            mark('nan_result')
        gch = [[0x38 + i, (nb >> (8 * i)) & 0xFF] for i in range(4) if ((nb >> (8 * i)) & 0xFF) != ((c['pos'] >> (8 * i)) & 0xFF)]
        ev.append([6, tag(T_GAR, 0), 0, 0])
        mark(['door_null', 'door_matrix', 'door_nomatrix'][c['door1']])
        if c['door1']:
            if open_:
                event = 0xA0 if finished else 0x9F
            else:
                event = 0xA2 if finished else 0xA1
            posn = tag(T_MAT0, 0x30) if c['door1'] == 1 else tag(T_DOOR0, 4)
            ev.append([7, tag(T_GAR, 0x50), event, posn, 0, 0x3F800000])
        out['ret'] = 1 if finished else 0
        out['gar_changes'] = gch
        out['door_changes'] = [[], []]
    return out


def check_side(c, o, e):
    for k in ('ret', 'cars_changes', 'veh_changes', 'pool_changes', 'matpool_changes', 'gar_changes', 'door_changes'):
        if k in e and o.get(k) != e[k]:
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
