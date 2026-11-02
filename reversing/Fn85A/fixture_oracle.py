"""Oraculo independiente (Python) de Fn85A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de las cinco funciones del lote 85A
(JoinGroupFlags 0x609380, CGarage::RightModTypeForThisGarage 0x447720, CalcDistToGarageRectangleSquared 0x447D80, RestoreCarsForThisHideOut 0x448550, IsPlayerOutsideGarage 0x448E50)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float de CalcDistToGarageRectangleSquared es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); el valor que queda en ST0 se
compara byte a byte (formato extendido de 80 bits).

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


def isnan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


T_GAR, T_VEH0, T_PED, T_CARS = 0, 1, 5, 6


def vehtag(k):
    return tag(T_VEH0 + k, 0)


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    ev = []
    out = {'events': ev, 'marks': marks}
    for x in list(c.get('rect', [])) + [c.get('cx', 0), c.get('cy', 0), c.get('radius', 0)]:
        if isnan(x):
            mark('nan_input')
        elif (x & 0x7F800000) == 0x7F800000:
            mark('inf_input')
        elif (x & 0x7F800000) == 0 and (x & 0x7FFFFF):
            mark('denormal_input')
        elif x == 0x80000000:
            mark('negzero_input')
    if fn == 0:
        b = c['jb']
        r = 1 if (b[0] | b[1] | b[2]) else 0
        mark('any_set' if r else 'none_set')
        for i in range(3):
            if b[i] and not any(b[j] for j in range(3) if j != i):
                mark('only_b%d' % i)
        if b[3]:
            mark('b3_nonzero')
        if sum(1 for i in range(3) if b[i]) == 3:
            mark('all_three')
        out['ret'] = r
    elif fn == 1:
        if not c['has_veh']:
            mark('null_veh')
            out['ret'] = 0
            return out
        flags = c['hflags'][c['mhid'][c['model_slot']]] & M32
        t = c['gtype'] & 0xFF
        if t == 36:
            r = 1 if flags & 0x2000000 else 0
            mark('type36')
        elif t == 37:
            r = 1 if flags & 0x4000000 else 0
            mark('type37')
        elif t == 38:
            r = 0 if flags & 0x6000000 else 1
            mark('type38')
        else:
            r = 0
            mark('other_type')
        mark('true' if r else 'false')
        out['ret'] = r
    elif fn == 2:
        x, y = load(c['cx']), load(c['cy'])
        l, rr, f, b = (load(v) for v in c['rect'])

        def axis(v, lo, hi, name):
            if v == NAN:
                mark(name + '_nan')
            if lt(v, lo):
                mark(name + '_below')
                return sub(v, lo)
            if gt(v, hi):
                mark(name + '_above')
                return sub(v, hi)
            mark(name + '_inside')
            return ('f', Fraction(0), 0)
        dx = axis(x, l, rr, 'x')
        dy = axis(y, f, b, 'y')
        res = add(mul(dy, dy), mul(dx, dx))
        if res != NAN and res[0] == 'i':
            mark('inf_result')
        out['st0'] = enc80(res)
    elif fn == 3:
        w = 0x12
        models = list(c['cmodel'])
        vb = list(c['vb'])
        vch = [[] for _ in range(4)]
        cch = []
        call = 0
        for k in range(4):
            if models[k] & 0xFFFF == 0:
                mark('empty_slot')
                continue
            ev.append([4, tag(T_CARS, 0x40 * k), call])
            call += 1
            if not c['rret'][k]:
                mark('restore_null')
                continue
            mark('restore_ok')
            nb = vb[k] & 0xDF
            if nb != vb[k]:
                vch[k].append([0x42D, nb])
            vb[k] = nb
            ev.append([5, vehtag(k), nb] + [m & 0xFFFF for m in models])
            models[k] = 0
        for k in range(4):
            if (c['cmodel'][k] & 0xFFFF) != (models[k] & 0xFFFF):
                lo, hi = c['cmodel'][k] & 0xFF, (c['cmodel'][k] >> 8) & 0xFF
                if lo:
                    cch.append([0x40 * k + w, 0])
                if hi:
                    cch.append([0x40 * k + w + 1, 0])
        r = 1 if all((m & 0xFFFF) == 0 for m in models) else 0
        mark('ret_true' if r else 'ret_false')
        if sum(1 for k in range(4) if c['cmodel'][k] & 0xFFFF) == 4:
            mark('all_full')
        out['ret'] = r
        out['cars_changes'] = cch
        out['veh_changes'] = vch
    else:
        ev.append([1, 0, 0xFFFFFFFF, 0])
        if c['fv1'] & 1:
            mark('veh_first')
            ev.append([1, 1, 0xFFFFFFFF, 0])
            ent = vehtag(0) if (c['fv2'] & 1) else 0
            mark('veh_second_ok' if ent else 'veh_second_null')
        else:
            mark('ped_branch')
            ev.append([2, 0xFFFFFFFF])
            ent = tag(T_PED, 0) if (c['fp'] & 1) else 0
            mark('ped_ok' if ent else 'ped_null')
        ev.append([3, tag(T_GAR, 0), ent, c['radius'] & M32])
        r = c['out_ret'] & 1
        mark('out_true' if r else 'out_false')
        out['ret'] = r
    return out


def check_side(c, o, e):
    for k in ('ret', 'st0', 'cars_changes', 'veh_changes'):
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
