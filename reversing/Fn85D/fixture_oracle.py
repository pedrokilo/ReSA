"""Oraculo independiente (Python) de Fn85D: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de los tres metodos de CGarage del lote 85D
(IsAnyOtherCarTouchingGarage 0x449100, ThrowCarsNearDoorOutOfGarage 0x449220, NeatlyLineUpStoredCars 0x448330) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

La aritmetica es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite); los valores que el EXE guarda en la pila (fstp dword) se redondean
a float y los que deja en el registro no. _ftol2 (0x821B40) trunca y da 0x80000000 con NaN, infinito o fuera de rango (el byte bajo es 0).

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
    return mixf((c * 0x9E3779B1 + k * 0x85EBCA6B + 85004) & M32)


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
# fn0 y fn1: 0 garaje, 1 pool, 2 matpool, 5 esferas
G0, P0, MP0, SPH0 = 0, 1, 2, 5
C_002 = 0x3CA3D70A      # 0.02f
K_HALF, K_ZERO, K_FOUR, K_HUNDRED = 0x3F000000, 0, 0x40800000, 0x42C80000


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


def st(a):
    """fstp dword + fld dword: el valor pasa por un float en memoria"""
    return load(f32bits(a))


def ftol_low(a):
    """_ftol2 (0x821B40): EAX; NaN, infinito o fuera de rango dan 0x80000000. Devuelve el byte bajo"""
    if a == NAN or a[0] == 'i':
        return 0
    v = a[1]
    t = v.numerator // v.denominator if v >= 0 else -((-v).numerator // (-v).denominator)
    if t >= 2 ** 31 or t < -(2 ** 31):
        return 0
    return t & 0xFF


def le(bits, n=4):
    return [(bits >> (8 * i)) & 0xFF for i in range(n)]


def bytes_changes(base_off, old_bits, new_bits, out):
    for b in range(4):
        nb, ob = (new_bits >> (8 * b)) & 0xFF, (old_bits >> (8 * b)) & 0xFF
        if nb != ob:
            out.append([base_off + b, nb])


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    ev = []
    out = {'events': ev, 'marks': marks}
    if fn in (0, 1):
        size = c['psize']
        count = [c['nsph'][s] & 0xFFFF for s in range(8)]
        init_count = list(count)
        touch = xform = ipr = 0
        cur = 0
        ret = 0
        throw = fn == 1
        pool_ch = []
        mark('size_zero' if size == 0 else ('size_8' if size == 8 else 'size_mid'))
        done = False
        ign = c['ign']
        if fn == 1:
            marks.extend(float_inputs(c, ['gco', 'ts', 'nrm']))
            marks.extend(float_inputs(c, ['pvel']))
        for i in range(size, 0, -1):
            sl = i - 1
            if c['pmap'][sl] & 0x80:
                mark('free_skipped')
                continue
            veh = tag(P0, sl * SLOT)
            if ign < 8 and ign == sl:
                mark('ignored_skipped')
                continue
            if not throw and (c['pstat'][sl] & 0xF8) == 0x28:
                mark('wrecked_skipped')
                continue
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
                radius = 0 if throw else c['sphr'][sl * 4 + idx]
                ev.append([4, tag(G0, 0), x[0], x[1], x[2], radius])
                kk = ipr
                ipr += 1
                inside = bit(c['rmask'], kk)
                if c['reon'] and kk == c['rek']:
                    count[cur] = c['reval'] & 0xFFFF
                    mark('reenter_applied')
                if throw:
                    if inside:
                        mark('inside_next_sphere')
                    else:
                        mark('pushed_first_sphere' if idx == 0 else 'pushed_later_sphere')
                        posx, posy = c['ppos'][sl * 3], c['ppos'][sl * 3 + 1]
                        gl, gr, gf, gb = (load(c['gco'][j]) for j in range(4))
                        dx = sub(load(posx), mul(add(gr, gl), load(K_HALF)))
                        dy = sub(load(posy), mul(add(gb, gf), load(K_HALF)))
                        ev.append([20, f32bits(dx), f32bits(dy), 0])
                        vx, vy, vz = (load(c['nrm'][j]) for j in range(3))
                        c2 = load(C_002)
                        t = load(c['ts'])
                        a = mul(vx, c2)
                        b = mul(vy, c2)
                        cz = st(mul(vz, c2))
                        as_ = st(mul(a, t))
                        tb = mul(t, b)
                        ct = mul(cz, t)
                        nv = [f32bits(add(as_, load(c['pvel'][sl * 3]))), f32bits(add(tb, load(c['pvel'][sl * 3 + 1]))), f32bits(add(ct, load(c['pvel'][sl * 3 + 2])))]
                        if any(isnan(v) for v in nv):
                            mark('nan_velocity')
                        for j in range(3):
                            bytes_changes(sl * SLOT + 0x44 + 4 * j, c['pvel'][sl * 3 + j], nv[j], pool_ch)
                        if c['nrm'][2] & 0x7FFFFFFF:
                            mark('nrm_z_nonzero')
                        break
                else:
                    if inside:
                        mark('inside_first_sphere' if idx == 0 else 'inside_later_sphere')
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
        if not throw:
            mark('ret_true' if ret else 'ret_false')
        out['ret'] = ret
        for k in ('matpool_changes', 'col_changes', 'sph_changes', 'gar_changes'):
            out[k] = []
        out['pool_changes'] = sorted(pool_ch)
        dch = []
        for s in range(8):
            if count[s] != init_count[s]:
                for b in range(2):
                    nb = (count[s] >> (8 * b)) & 0xFF
                    ob = (init_count[s] >> (8 * b)) & 0xFF
                    if nb != ob:
                        dch.append([s * 0x10 + b, nb])
        out['data_changes'] = dch
    else:
        marks.extend(float_inputs(c, ['gpos', 'gda', 'gdb', 'gw', 'gh', 'nrm']))
        ax, ay, az = (load(c['gpos'][k]) for k in range(3))
        w, h = load(c['gw']), load(c['gh'])
        d1x, d1y = load(c['gda'][0]), load(c['gda'][1])
        d2x, d2y = load(c['gdb'][0]), load(c['gdb'][1])
        half, zero, four, hundred = load(K_HALF), load(K_ZERO), load(K_FOUR), load(K_HUNDRED)
        ev.append([20, c['gda'][0], c['gda'][1], 0])
        base_z = st(add(az, half))
        first_x = st(mul(d1x, w))
        first_y = mul(d1y, w)
        first_z = mul(w, zero)
        half_x = st(mul(first_x, half))
        half_y = st(mul(first_y, half))
        half_z = mul(first_z, half)
        point_x = add(half_x, ax)
        point_y = st(add(half_y, ay))
        point_z = st(add(half_z, base_z))
        second_x = st(mul(h, d2x))
        second_y = mul(d2y, h)
        second_z = mul(h, zero)
        half2_x = st(mul(second_x, half))
        half2_y = st(mul(second_y, half))
        half2_z = st(mul(second_z, half))
        start_x = st(add(half2_x, point_x))
        start_y = st(add(half2_y, point_y))
        start_z = st(add(half2_z, point_z))
        vx, vy, vz = (load(c['nrm'][k]) for k in range(3))
        perp_x = vy
        perp_y = neg(vx)
        step_x = st(mul(vx, four))
        step_y = st(mul(vy, four))
        step_z = st(mul(vz, four))
        origin_x = st(sub(start_x, step_x))
        origin_y = st(sub(start_y, step_y))
        origin_z = st(sub(start_z, step_z))
        models = [m & 0xFFFF for m in c['cmodel']]
        ch = []
        placed = 0
        if models[0] == 0:
            mark('no_cars')
        else:
            for i in range(3):
                fi = ('f', Fraction(i), 0)
                px = f32bits(st(add(mul(step_x, fi), origin_x)))
                py = f32bits(st(add(mul(step_y, fi), origin_y)))
                pz = f32bits(st(add(mul(fi, step_z), origin_z)))
                fx = ftol_low(mul(perp_x, hundred))
                fy = ftol_low(mul(perp_y, hundred))
                base = 0x40 * i
                for off, bits in ((0, px), (4, py), (8, pz)):
                    for b, byte in enumerate(le(bits)):
                        if byte:
                            ch.append([base + off + b, byte])
                for off, byte in ((0x3C, fx), (0x3D, fy)):
                    if byte:
                        ch.append([base + off, byte])
                placed += 1
                if models[i + 1] == 0:
                    mark('stopped_by_empty')
                    break
            mark('cars_%d' % placed)
            if placed == 3 and models[3] != 0:
                mark('fourth_untouched')
        if c['nrm'][2] & 0x7FFFFFFF:
            mark('nrm_z_nonzero')
        if any(x == NAN for x in (px_ for px_ in (start_x, start_y, start_z))):
            mark('nan_start')
        out['ret'] = 0
        out['cars_changes'] = sorted(ch)
        out['gar_changes'] = []
    return out


CHANGE_KEYS = ('ret', 'cars_changes', 'pool_changes', 'matpool_changes', 'col_changes', 'data_changes', 'sph_changes', 'gar_changes')


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
