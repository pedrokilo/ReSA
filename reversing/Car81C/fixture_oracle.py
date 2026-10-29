"""Oraculo independiente (Python) de Car81C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x424F80..0x425433, lo que debe observarse de
CCarCtrl::PossiblyRemoveVehicle (traza de colaboradores EN ORDEN con sus argumentos, escrituras en el vehiculo, estado final de matrices, centro, reloj y camara) y lo compara con las filas de
comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio guardado como float donde el EXE
hace fstp dword (la distancia, el rango de la camara, las restas de 0x40FE60) y en el registro (sin guardar) en el resto (el producto del rango, la z del eje up, las sumas de cuadrados).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 vehiculo, 1 matriz A, 2 matriz B, 3 centro, 4 clump.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)
GARBAGE = 0xA0000010
CAMERA = 0xB6F028
# Constantes de .rdata leidas del desensamblado: 170.0, 0x858F94 (1/170), 0x858F90, 22.0, 42.25, 0x858CAC (-0.9f), 0.0 y las inmediatas 45.0 / 70.0
K170, KINV, K074, K22, K4225, KLOW, KZERO = 0x432A0000, 0x3BC0C0C1, 0x3F3D70A4, 0x41B00000, 0x42290000, 0xBF666666, 0
V45, V70 = 0x42340000, 0x428C0000
VT = (0, 3, 4, 5, 9, 0xFFFFFFFF, 0x80000003, 1)


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


VEH, MATA, MATB, CEN, CLUMP = (tag(i) for i in range(5))


class Uninit(Exception):
    """el EXE usaria el centro del jugador sin inicializar (el generador de fixtures debe evitarlo)"""


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


def fromint(j):
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


# ---------------------------------------------------------------- estado y modelo
def init_state(c):
    cams = [c['cam%d' % i] & 0xFF for i in range(9)]
    v = dict(m14='A' if c['has_mat'] else None, m18=c['clump'], m1c=c['ent_flags'], m22=c['model'] & 0xFFFF, m36=c['s36'] & 0xFF, m04=[c['vp0'], c['vp1'], c['vp2']], mb8=c['fake_phys'] & 0xFF,
             m3b0=c['switched'], m3b9=c['style'] & 0xFF, m3ba=c['mission'] & 0xFF, m3bb=c['tempact'] & 0xFF, m41c=c['target'], m428=c['f428'] & 0xFF, m42a=c['f42A'] & 0xFF, m42b=c['f42B'] & 0xFF,
             m42c=c['f42C'] & 0xFF, m485=c['b485'] & 0xFF, m4a4=c['created'] & 0xFF, m4a6=c['range'] & 0xFFFF, m4d0=c['till'], m4d8=c['blown'], m594=c['vtype'])
    return dict(v=v, ma=[c['mp0'], c['mp1'], c['mp2'], c['upz']], mb=[c['bp0'], c['bp1'], c['bp2'], c['bupz']], cen=[c['cen0'], c['cen1'], c['cen2']], time=c['now'], cam_mult=c['cam_mult'],
                cam_low=c['cam_low'], active=c['active_cam'] % 256, cams=cams, focus=c['focus'] & 0xFF)


def dv(v, i):
    return (v ^ ((i * 0x01234567) & M32)) & M32


def apply_mut(st, kind, val):
    v = st['v']
    if kind == 1:
        v['m14'] = 'B'
    elif kind == 2:
        st['time'] = val
    elif kind == 3:
        v['m428'] = val & 0xFF
    elif kind == 4:
        v['m42a'] = val & 0xFF
    elif kind == 5:
        v['m42b'] = val & 0xFF
    elif kind == 6:
        v['m42c'] = val & 0xFF
    elif kind == 7:
        v['m36'] = val & 0xFF
    elif kind == 8:
        v['m594'] = VT[val % 8]
    elif kind == 9:
        v['m3b9'] = val % 8
    elif kind == 10:
        v['m3bb'] = val & 0xFF
    elif kind == 11:
        v['m3ba'] = val & 0xFF
    elif kind == 12:
        v['m3b0'] = val
    elif kind == 13:
        v['m4d0'] = val
    elif kind == 14:
        v['m4d8'] = val
    elif kind == 15:
        for i in range(3):
            st['ma'][i] = dv(val, i)
            st['mb'][i] = dv(val, i + 3)
    elif kind == 16:
        v['m04'] = [dv(val, i) for i in range(3)]
    elif kind == 17:
        st['cen'] = [dv(val, i) for i in range(3)]
    elif kind == 18:
        st['ma'][3] = val
        st['mb'][3] = dv(val, 1)
    elif kind == 19:
        v['m4a4'] = val & 0xFF
    elif kind == 20:
        v['m4a6'] = val & 0xFFFF
    elif kind == 21:
        v['m22'] = val & 0xFFFF
    elif kind == 22:
        v['m1c'] = val
    elif kind == 23:
        st['cam_mult'] = val
    elif kind == 24:
        st['cam_low'] = val
    elif kind == 25:
        st['active'] = val % 3
    elif kind == 26:
        st['cams'][st['active'] * 3 + ((val >> 8) % 3)] = val & 0xFF
    elif kind == 27:
        v['m18'] = val % 3


LAST = {}   # marcas de ramas del ultimo calculo (cobertura de run_local.py)


def expected(c):
    st = init_state(c)
    v = st['v']
    ev = []
    cnt = {}
    marks = set()
    ctr = [0]
    centre = [None]

    def after():
        idx = ctr[0]
        ctr[0] += 1
        if idx == c['mut_at']:
            apply_mut(st, c['mut_kind'], c['mut_val'])
            marks.add('mut_applied')
            marks.add('mut_event_%d' % idx)
            marks.add('mut_kind_%d' % c['mut_kind'])

    def fake(kind, args, seq=None, key=None):
        ev.append([kind] + list(args) + [st['time']])
        r = None
        if seq is not None:
            i = cnt.get(key, 0)
            r = seq[min(i, len(seq) - 1)]
            cnt[key] = i + 1
        after()
        return r

    def clump_tag():
        return {0: 0, 1: CLUMP}.get(v['m18'], GARBAGE + 0x20)

    def posv():
        m = v['m14']
        if m == 'A':
            return st['ma'][:3]
        if m == 'B':
            return st['mb'][:3]
        return list(v['m04'])

    def remove():
        fake(11, [VEH])
        fake(12, [VEH, 1])
        marks.add('removed')

    def diff3(pos, cen):
        return [reload(store(sub(load(pos[i]), load(cen[i])))) for i in range(3)]

    ret = [None]

    def stage3():
        if (v['m36'] & 0xF8) != 0x28:
            return
        blown = v['m4d8']
        if blown == 0:
            return
        thr = (blown + 0xEA60) & M32
        now = st['time']
        if not now > thr:
            return
        if not now > v['m4d0']:
            return
        marks.add('s3_time_ok')
        if fake(5, [VEH], c_ron, 'on') & 0xFF:
            return
        if centre[0] is None:
            raise Uninit()
        mat = v['m14']
        pos = st['ma'][:3] if mat == 'A' else st['mb'][:3] if mat == 'B' else list(v['m04'])
        d = diff3(pos, centre[0])
        mag = add(add(mul(d[0], d[0]), mul(d[1], d[1])), mul(d[2], d[2]))
        if not gt(mag, load(K4225)):
            marks.add('s3_near')
            return
        pos = st['ma'][:3] if mat == 'A' else st['mb'][:3] if mat == 'B' else list(v['m04'])
        if fake(7, pos, c_rgar, 'gar') & 0xFF:
            marks.add('s3_garage')
            return
        marks.add('s3_remove')
        remove()

    c_ron = [c['ron%d' % i] for i in range(4)]
    c_rgar = [c['rgar%d' % i] for i in range(3)]
    c_rint = [c['rint0'], c['rint1']]
    c_rcbd = [c['rcbd0'], c['rcbd1']]

    def run():
        if v['m485'] > 0:
            marks.add('exit_getting_in')
            return
        if (v['m42b'] & 8) and v['m41c'] != 0:
            marks.add('exit_convoy')
            return
        entered = False
        if not (v['m428'] & 8):
            if fake(1, [VEH], c_rcbd, 'cbd') & 0xFF:
                if not (fake(2, [VEH], [c['rcrane']], 'crane') & 0xFF):
                    entered = True
        if entered:
            marks.add('entered')
            if v['m42a'] & 4:
                marks.add('fade_flag')
                if fake(3, [clump_tag()], [c['ralpha']], 'alpha') == 0:
                    marks.add('fade_remove')
                    remove()
                    return
            fake(4, [c['focus']])
            centre[0] = list(st['cen'])
            pos = posv()
            dx = sub(load(pos[0]), load(centre[0][0]))
            dy = sub(load(pos[1]), load(centre[0][1]))
            dist = store(sqrt24(add(mul(dy, dy), mul(dx, dx))))
            LAST['dist'] = dist
            near = False
            if not (fake(5, [VEH], c_ron, 'on') & 0xFF):
                b = st['active'] * 3
                if st['cams'][b + 1] == 0 and st['cams'][b + 2] == 0 and st['cams'][b] == 0:
                    marks.add('cam_flags_clear')
                    if fake(6, [CAMERA], [c['rlook']], 'look') != 0:
                        marks.add('look_ok')
                        if (v['m4a4'] != 3 and v['m22'] != 0x1A0 and v['m22'] != 0x197 and not (v['m428'] & 1) and not (v['m42b'] & 2) and st['time'] >= v['m4d0']
                                and not (v['m42c'] & 0x40)):
                            near = True
            if near:
                value = V45
                marks.add('range_45')
            else:
                value = store(mul(load(st['cam_mult']), load(K170)))
                marks.add('range_cam')
            if lt(load(st['cam_low']), load(KLOW)):
                value = V70
                marks.add('range_70')
            rng = s16(v['m4a6'])
            a = fromint(rng)
            bb = load(K170)
            m = a if gt(a, bb) else bb
            scaled = mul(mul(m, reload(value)), load(KINV))
            LAST['value'] = value
            far = lt(scaled, reload(dist))
            marks.add('far' if far else 'not_far')
            if far:
                if v['m3ba'] != 0x3B:
                    pt = posv()
                    if not (fake(7, pt, c_rgar, 'gar') & 0xFF):
                        if fake(8, [VEH], c_rint, 'inter') & 0xFF:
                            v['mb8'] = 0x0A
                            marks.add('far_interesting')
                            return
                        if not (fake(5, [VEH], c_ron, 'on') & 0xFF):
                            marks.add('far_remove')
                            remove()
                            return
                        v['m42a'] |= 4
                        marks.add('far_fade')
                        return
                    marks.add('far_garage')
                else:
                    marks.add('far_mission59')
            status = v['m36'] >> 3
            if status == 2:
                marks.add('flat_check')
                mat = v['m14']
                up = st['ma'][3] if mat == 'A' else st['mb'][3]
                x = load(up)
                if lt(x, load(KZERO)):
                    x = neg(x)
                if lt(x, load(K074)):
                    marks.add('flat_remove')
                    remove()
                    return
            elif status == 3 or status == 5:
                t = v['m594']
                if t == 4 or t == 3:
                    marks.add('stuck_check')
                    if (v['m1c'] >> 4) & 1:
                        marks.add('stuck_remove')
                        remove()
                        return
        # etapa 2 (0x425236)
        t = v['m594']
        if not (t == 3 or t == 4 or v['m3bb'] == 0x0C):
            s = v['m36'] >> 3
            ok = s == 2 or (s == 3 and v['m3b9'] in (0, 4))
            if ok:
                marks.add('s2_status')
                now = st['time']
                sw = v['m3b0']
                if ((now - sw) & M32) > 5000 and v['m4d0'] == 0 and now != 0:
                    marks.add('s2_time')
                    if not (fake(5, [VEH], c_ron, 'on') & 0xFF):
                        if centre[0] is None:
                            raise Uninit()
                        pos = posv()
                        d = diff3(pos, centre[0])
                        r = sqrt24(add(mul(d[0], d[0]), mul(d[1], d[1])))
                        if gt(r, load(K22)):
                            marks.add('s2_far22')
                            if not (fake(8, [VEH], c_rint, 'inter') & 0xFF):
                                if not (v['m428'] & 8):
                                    if fake(1, [VEH], c_rcbd, 'cbd') & 0xFF:
                                        if not (fake(9, [VEH, 1], [c['rlight']], 'light') & 0xFF):
                                            if not (fake(10, [VEH], [c['rbridge']], 'bridge') & 0xFF):
                                                marks.add('s2_all')
                                                if not (fake(7, posv(), c_rgar, 'gar') & 0xFF):
                                                    marks.add('s2_remove')
                                                    remove()
                                                    return
        stage3()

    run()
    return dict(st=st, events=ev, marks=marks, entered=centre[0] is not None)


def expect_obs(c):
    e = expected(c)
    st = e['st']
    v = st['v']
    m14 = {'A': MATA, 'B': MATB, None: 0}[v['m14']]
    m18 = {0: 0, 1: CLUMP}.get(v['m18'], GARBAGE + 0x20)
    return e, dict(m14=m14, m18=m18, m1c=v['m1c'], m22=v['m22'], m36=v['m36'], m04=v['m04'], mb8=v['mb8'], m3b0=v['m3b0'], m3b9=v['m3b9'], m3ba=v['m3ba'], m3bb=v['m3bb'], m41c=v['m41c'],
                   m428=v['m428'], m42a=v['m42a'], m42b=v['m42b'], m42c=v['m42c'], m485=v['m485'], m4a4=v['m4a4'], m4a6=v['m4a6'], m4d0=v['m4d0'], m4d8=v['m4d8'], m594=v['m594'])


def check_side(c, o, e, exp_v):
    got = o['events']
    if len(got) != len(e['events']):
        return 'numero de eventos %d != %d: %s vs %s' % (len(got), len(e['events']), got[:12], e['events'][:12])
    for i, (g, w) in enumerate(zip(got, e['events'])):
        if g != w:
            return 'evento %d: %s != %s' % (i, g, w)
    for k, w in exp_v.items():
        if o['v'][k] != w:
            return 'campo %s: %s != %s' % (k, o['v'][k], w)
    st = e['st']
    if o['ma'] != st['ma'] or o['mb'] != st['mb'] or o['cen'] != st['cen']:
        return 'matrices o centro: %s %s %s != %s %s %s' % (o['ma'], o['mb'], o['cen'], st['ma'], st['mb'], st['cen'])
    if o['time'] != st['time'] or o['cam_mult'] != st['cam_mult'] or o['cam_low'] != st['cam_low'] or o['active_cam'] != st['active'] or o['cams'] != st['cams']:
        return 'reloj o camara'
    if o['focus'] != st['focus']:
        return 'foco'
    if any(o['changes'][i] for i in range(5)):
        return 'escrituras no esperadas: %s' % (o['changes'],)
    return None


def check_row(row):
    c = row['input']
    try:
        e, exp_v = expect_obs(c)
    except Uninit:
        return 'entrada', 'el EXE usaria el centro sin inicializar'
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, exp_v)
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
                print('DIFIERE', r[0], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
