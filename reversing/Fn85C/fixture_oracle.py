"""Oraculo independiente (Python) de Fn85C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de las cuatro funciones del lote 85C
(CGarage::IsAnyCarBlockingDoor 0x4494F0, CGarage::StoreAndRemoveCarsForThisImpoundingGarage 0x449A50, CTaskSimpleFight::BeHitWhileBlocking 0x61C650, CTaskSimpleFight::ChooseAttackAI 0x624A40)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica de BeHitWhileBlocking y de la tirada de ChooseAttackAI es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); la tirada se guarda
como float (fst dword) pero la primera comparacion usa el registro.

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


def mixf(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand32(c, k):
    return mixf((c * 0x9E3779B1 + k * 0x85EBCA6B + 85003) & M32)


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def bit(mask, k):
    return (mask >> (k if k < 31 else 31)) & 1


SLOT = 0xA18
# fn0: 0 garaje, 1 pool, 2 matpool, 3 col, 4 data, 5 esferas, 6 map, 7 pool struct
G0, P0, MP0, SPH0 = 0, 1, 2, 5
# fn1: 0 garaje, 1 pool, 2 coches, 3 vtable
G1, P1, CARS1 = 0, 1, 2
# fn3: 0 tarea, 1 ped, 2 objetivo
TASK3, PED3, TGT3 = 0, 1, 2
K_ROLL = 0x38000100
K_08, K_20, K_05, K_03 = 0x3F4CCCCD, 0x41A00000, 0x3F000000, 0x3E99999A


def float_inputs(c, names):
    marks = []
    for nm in names:
        v = c.get(nm, 0)
        for x in (v if isinstance(v, list) else [v]):
            if isnan(x):
                marks.append('nan_input')
            elif (x & 0x7F800000) == 0x7F800000:
                marks.append('inf_input')
            elif (x & 0x7F800000) == 0 and (x & 0x7FFFFF):
                marks.append('denormal_input')
    return marks


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    ev = []
    out = {'events': ev, 'marks': marks}
    if fn == 0:
        size = c['psize']
        count = [c['nsph'][s] & 0xFFFF for s in range(8)]
        init_count = list(count)
        touch = xform = ipr = 0
        cur = 0
        ret = 0
        mark('size_zero' if size == 0 else ('size_8' if size == 8 else 'size_mid'))
        done = False
        for i in range(size, 0, -1):
            sl = i - 1
            if c['pmap'][sl] & 0x80:
                mark('free_skipped')
                continue
            veh = tag(P0, sl * SLOT)
            ev.append([1, tag(G0, 0), veh])
            touching = bit(c['tmask'], touch)
            touch += 1
            if not touching:
                mark('touch_false')
                continue
            mark('touch_true')
            ev.append([2, veh])
            cur = sl
            if s16(count[sl]) <= 0:
                mark('neg_count' if count[sl] & 0x8000 else 'no_spheres')
                continue
            idx = 0
            while True:
                ev.append([3, tag(MP0, sl * 0x48) if c['pmat'][sl] else 0, tag(SPH0, (sl * 4 + idx) * 0x14)])
                mark('with_matrix_veh' if c['pmat'][sl] else 'no_matrix_veh')
                k = xform
                xform += 1
                x = [rand32(c['seed'], k * 4 + j + 1) for j in range(3)]
                ev.append([4, tag(G0, 0), x[0], x[1], x[2], c['sphr'][sl * 4 + idx]])
                kk = ipr
                ipr += 1
                inside = bit(c['rmask'], kk)
                if c['reon'] and kk == c['rek']:
                    count[cur] = c['reval'] & 0xFFFF
                    mark('reenter_applied')
                if not inside:
                    mark('blocked_first_sphere' if idx == 0 else 'blocked_later_sphere')
                    ret = 1
                    done = True
                    break
                idx += 1
                mark('multi_sphere')
                if idx == 3:
                    mark('last_idx3')
                if not idx < s16(count[sl]):
                    break
            if done:
                break
        mark('ret_true' if ret else 'ret_false')
        out['ret'] = ret
        for k in ('pool_changes', 'matpool_changes', 'col_changes', 'sph_changes', 'gar_changes'):
            out[k] = []
        dch = []
        for s in range(8):
            if count[s] != init_count[s]:
                for b in range(2):
                    nb = (count[s] >> (8 * b)) & 0xFF
                    ob = (init_count[s] >> (8 * b)) & 0xFF
                    if nb != ob:
                        dch.append([s * 0x10 + b, nb])
        out['data_changes'] = dch
    elif fn == 1:
        size = c['psize']
        ms = s32(c['maxslot'])
        lim = min(ms, 4)
        mark('size_zero' if size == 0 else ('size_8' if size == 8 else 'size_mid'))
        mark('lim_neg' if ms < 0 else ('lim_zero' if ms == 0 else ('lim_over' if ms >= 4 else 'lim_mid')))
        n = 0
        ipp = esph = 0
        for i in range(size, 0, -1):
            sl = i - 1
            if c['pmap'][sl] & 0x80:
                mark('free_skipped')
                continue
            veh = tag(P1, sl * SLOT)
            x, y, z = c['ppos'][sl * 3:sl * 3 + 3]
            ev.append([5, tag(G1, 0), x, y, z])
            inside = bit(c['imask'], ipp)
            ipp += 1
            if not inside:
                mark('outside_skipped')
                continue
            if c['createdby'][sl] & 0xFF == 2:
                mark('mission_skipped')
                continue
            mark('with_matrix' if c['pmat'][sl] else 'no_matrix')
            if n < lim:
                ev.append([8, tag(G1, 0), veh, 0x3F800000])
                r = bit(c['esmask'], esph)
                esph += 1
                if r:
                    mark('sphere_outside')
                else:
                    ev.append([9, tag(CARS1, 0x40 * n), veh])
                    mark('stored')
                    n += 1
            else:
                mark('store_blocked_by_lim')
            ev.append([10, c['pinfo'], veh])
            ev.append([11, veh])
            ev.append([12, veh, 1])
            mark('removed')
        if n == 4:
            mark('full_four')
        final = [0x4000 + k if k < n else 0 for k in range(4)]
        cch = []
        for k in range(4):
            init = c['cmodel'][k] & 0xFFFF
            for b in range(2):
                nb = (final[k] >> (8 * b)) & 0xFF
                ob = (init >> (8 * b)) & 0xFF
                if nb != ob:
                    cch.append([0x40 * k + 0x12 + b, nb])
        out['ret'] = 0
        out['cars_changes'] = cch
        for k in ('pool_changes', 'matpool_changes', 'gar_changes'):
            out[k] = []
    elif fn == 2:
        marks.extend(float_inputs(c, ['total', 'cur', 'vpos', 'cpos', 'fwd']))
        combo, move, cset = c['combo'] & 0xFF, c['move'] & 0xFF, s8(c['cset'])
        mark('creator_mat' if c['chasmat'] else 'creator_nomat')
        ret = 0
        ach = []
        if (c['lastcmd'] & 0xFF) != 2:
            mark('bad_lastcmd')
        elif c['animnull']:
            mark('anim_null')
        elif c['animflags'] & 1:
            mark('anim_flag')
        else:
            total, cur = load(c['total']), load(c['cur'])
            if total == NAN or cur == NAN:
                mark('total_nan')
            if not gt(total, cur):
                mark('total_not_greater')
            else:
                d = [sub(load(c['cpos'][k]), load(c['vpos'][k])) for k in range(3)]
                f = [load(c['fwd'][k]) for k in range(3)]
                dot = add(add(mul(d[2], f[2]), mul(d[1], f[1])), mul(d[0], f[0]))
                if dot == NAN:
                    mark('dot_nan')
                behind = lt(dot, load(K_03))
                mark('behind' if behind else 'not_behind')
                stop = False
                if combo in (8, 10, 11, 12):
                    mark('set_group1')
                    if cset < 8 or cset > 12 or cset == 9:
                        stop = True
                        mark('set_bad')
                    else:
                        mark('set_ok')
                elif combo == 9:
                    mark('set_group9')
                    if cset < 8 or cset > 12:
                        stop = True
                        mark('set_bad')
                    else:
                        mark('set_ok')
                elif combo == 7:
                    mark('set_group7')
                    if move == 1:
                        stop = True
                        mark('move1_blocked')
                if not stop and not behind:
                    ret = 1
                    ach = [[0x2E, (c['animflags'] | 1) & 0xFF]]
        mark('ret_true' if ret else 'ret_false')
        out['ret'] = ret
        out['anim_changes'] = ach
        for k in ('task_changes', 'hier_changes', 'vic_changes', 'cre_changes', 'vmat_changes', 'cmat_changes'):
            out[k] = []
    else:
        cset = s8(c['cset'])
        flags = c['tflags'] & 0xFF
        if 4 < cset <= 7:
            flags &= c['pbyte'] & 0xFF
            mark('mask_applied')
        if cset < 0:
            mark('cset_neg')
        if cset >= 16:
            mark('cset_ge16')
        rv = s32(c['randv'])
        if rv < 0 or rv >= 32768:
            mark('roll_big')
        ev.append([17])
        exact = ('f', Fraction(rv), 1 if rv < 0 else 0)
        roll_reg = mul(exact, load(K_ROLL))
        roll_bits = f32bits(roll_reg)
        ret = None
        gt08 = gt(roll_reg, load(K_08))
        if gt08 and flags & 2:
            mark('path_a')
            if not flags & 0x40:
                mark('a_no40')
                ret = 2
            elif c['tkind'] == 0:
                mark('a_notarget')
                ret = 2
            elif (c['ttype'] & 7) != 3:
                mark('a_nonped')
                ret = 2
            elif not c['tpd']:
                mark('a_nopd')
                ret = 2
            else:
                ev.append([18, tag(TGT3, 0)])
                if s32(c['wlevel']) <= 0:
                    mark('a_wanted_le0')
                    ret = 2
                elif not gt(load(c['health']), load(K_20)):
                    mark('a_health_low')
                    ret = 2
                else:
                    mark('a_ok')
        else:
            mark('path_b')
            ev.append([19, tag(TASK3, 0), tag(PED3, 0)])
            if (c['reach'] & 1) and flags & 4:
                mark('b_reach_flag4_ret3')
                ret = 3
            elif not (c['reach'] & 1):
                mark('b_reach_false')
        if ret is None:
            if gt(load(roll_bits), load(K_05)) and flags & 1:
                ret = 1
            else:
                ret = 0
            if gt(roll_reg, load(K_05)) and not gt08:
                mark('roll_gt05_only')
        if gt08:
            mark('roll_gt08')
        mark('ret%d' % ret)
        marks.extend(float_inputs(c, ['health']))
        out['ret'] = ret
        for k in ('task_changes', 'ped_changes', 'target_changes'):
            out[k] = []
        out['melee_flag_unchanged'] = True
    return out


CHANGE_KEYS = ('ret', 'cars_changes', 'pool_changes', 'matpool_changes', 'col_changes', 'data_changes', 'sph_changes', 'gar_changes', 'task_changes', 'anim_changes', 'hier_changes',
               'vic_changes', 'cre_changes', 'vmat_changes', 'cmat_changes', 'ped_changes', 'target_changes', 'melee_flag_unchanged')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
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
