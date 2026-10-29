"""Oraculo independiente (Python) de Veh81A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6DFC50..0x6E0049 (y de los colaboradores aritmeticos 0x40FDB0, 0x406D70,
0x406DA0, 0x40FE90, 0x40FEC0, 0x4119D0, 0x420800, 0x59C730, 0x59C790, 0x59C910), lo que debe observarse de CVehicle::UpdateTrailerLink (traza de colaboradores EN ORDEN con todos sus
argumentos, cambios en las arenas y en los globales) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado guardado como float (fstp / fst dword,
con desnormales) y vuelto a cargar donde el EXE lo guarda; lo que el EXE deja en el registro (z de la diferencia de posiciones, suma de cuadrados de la masa efectiva, reciproco de 4119D0 para z)
se queda sin guardar. La unica carga de NaN que aparece es el indefinido 0xFFC00000 (las fixtures usan un solo patron): el oraculo exige exactamente ese patron.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 tractor, 1 remolque A, 2 remolque B, 3 matriz del tractor,
4 matriz alternativa del tractor, 5 matriz del remolque A, 6 matriz U (alternativa de A y matriz de B).
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)
QNAN = 0xFFC00000
ONE_BITS, ZERO_BITS = 0x3F800000, 0
STEP_MIN = 0x3F333333      # 0.7f (0x858CB0)
FWD_LIMIT = 0xBE99999A     # -0.3f (0x858EE4)
LIMIT_BITS = 0x3F800000    # 1.0f (0x871DF0)
GAIN_BITS = 0x3E99999A     # 0.3f (0x871DF4)
MINUS_1000 = 0xC47A0000
OBJ_S, OBJ_A, OBJ_B = 0, 1, 2
MAT_S, MAT_S2, MAT_A, MAT_U = 3, 4, 5, 6


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


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
    """redondea al par a 24 bits (exponente ilimitado), o con cuanto minimo 2**quantum_exp_min (desnormales del float)"""
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


def div(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN
    if a[0] == 'i':
        return ('i', a[1] ^ (b[1] if b[0] == 'i' else b[2]))
    if b[0] == 'i':
        return ('f', Fraction(0), a[2] ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', a[2] ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def sqrt24(a):
    """fsqrt a 24 bits (solo recibe sumas de cuadrados: >= 0, infinito o NaN)"""
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


def value(a):
    """valor comparable: Fraction, +-inf o None (NaN)"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return float('inf') if a[1] == 0 else float('-inf')
    return a[1]


def lt(a, b):
    x, y = value(a), value(b)
    return x is not None and y is not None and x < y


def gt(a, b):
    return lt(b, a)


def le(a, b):
    x, y = value(a), value(b)
    return x is not None and y is not None and x <= y


def L(bits):
    return NAN if bits is None else load(bits)


def S(a):
    return store(a)


# ---------------------------------------------------------------- los trozos aritmeticos de la funcion
def too_far(bar, hitch, ts):
    """0x6DFCE0..0x6DFD58: devuelve (diff como bits, true si limite * max(paso, 0.7) < sqrt(distancia^2 guardada))"""
    dxr = sub(L(bar[0]), L(hitch[0]))
    dyr = sub(L(bar[1]), L(hitch[1]))
    dzr = sub(L(bar[2]), L(hitch[2]))
    dxs, dys, dzs = S(dxr), S(dyr), S(dzr)
    dx, dy, dz = reload(dxs), reload(dys), reload(dzs)
    zz = mul(dzr, dz)             # fmul dword [guardado]: el registro sin guardar por el valor guardado
    yy = mul(dy, dy)
    xx = mul(dx, dx)
    sq = reload(S(add(add(zz, yy), xx)))
    tsv = L(ts)
    t = tsv if gt(tsv, L(STEP_MIN)) else L(STEP_MIN)
    ln = sqrt24(sq)
    sc = mul(L(LIMIT_BITS), t)
    LAST.update(sc=sc, ln=ln, tsel=t, sq=sq)
    return [dxs, dys, dzs], lt(sc, ln)


def bent(t, s):
    """0x6DFD5E..0x6DFDAC: ejes +0x10 (suma z, y, x) < -0.3f, o ejes +0x20 < 0.0f. Devuelve 0 (no), 1 (delantero), 2 (superior)"""
    fwd = add(add(mul(L(t[6]), L(s[6])), mul(L(t[5]), L(s[5]))), mul(L(t[4]), L(s[4])))
    LAST['fwd'] = fwd
    if lt(fwd, L(FWD_LIMIT)):
        return 1
    up = add(add(mul(L(t[10]), L(s[10])), mul(L(t[9]), L(s[9]))), mul(L(t[8]), L(s[8])))
    LAST['up'] = up
    if lt(up, L(ZERO_BITS)):
        return 2
    return 0


def damp(diff, ts, gain):
    """0x6DFE92..0x6DFEDF: ((ganancia * diff) / max(1.0f, paso))"""
    tsv = L(ts)
    LAST['r_one'] = gt(L(ONE_BITS), tsv)
    r = reload(S(L(ONE_BITS) if gt(L(ONE_BITS), tsv) else tsv))
    g = L(gain)
    tz = reload(S(mul(g, reload(diff[2]))))
    ty = reload(S(mul(g, reload(diff[1]))))
    tx = reload(S(mul(g, reload(diff[0]))))
    inv = div(L(ONE_BITS), r)
    invs = reload(S(inv))
    oz = S(mul(inv, tz))
    oy = S(mul(invs, ty))
    ox = S(mul(invs, tx))
    return [ox, oy, oz]


def project(e, at):
    """0x6DFEE5..0x6DFF2F: e -= ((e . at) * at), suma z, y, x"""
    d = reload(S(add(add(mul(reload(e[2]), L(at[2])), mul(reload(e[1]), L(at[1]))), mul(reload(e[0]), L(at[0])))))
    p = [reload(S(mul(d, L(at[i])))) for i in range(3)]
    return [S(sub(reload(e[i]), p[i])) for i in range(3)]


def force(mat, centre, e, b, mass, turn):
    """0x6DFF33..0x6DFFE0: devuelve la fuerza (bits) = e * m, m = 1 / ((|(b - R) x n(e)|^2) / turn + 1 / mass)"""
    m = [L(x) for x in mat]
    cv = [L(x) for x in centre]
    rx = reload(S(add(add(mul(m[8], cv[2]), mul(m[4], cv[1])), mul(m[0], cv[0]))))
    ry = reload(S(add(add(mul(m[9], cv[2]), mul(m[1], cv[0])), mul(m[5], cv[1]))))
    rz = reload(S(add(add(mul(m[10], cv[2]), mul(m[2], cv[0])), mul(m[6], cv[1]))))
    f = [reload(x) for x in e]
    s = add(add(mul(f[0], f[0]), mul(f[1], f[1])), mul(f[2], f[2]))
    LAST['norm_zero'] = le(s, L(ZERO_BITS))
    if le(s, L(ZERO_BITS)):
        f[0] = L(ONE_BITS)
    else:
        inv = div(L(ONE_BITS), sqrt24(s))
        f[0] = reload(S(mul(inv, f[0])))
        f[1] = reload(S(mul(inv, f[1])))
        f[2] = reload(S(mul(inv, f[2])))
    cx = reload(S(sub(L(b[0]), rx)))
    cy = reload(S(sub(L(b[1]), ry)))
    cz = reload(S(sub(L(b[2]), rz)))
    kx = reload(S(sub(mul(f[2], cy), mul(cz, f[1]))))
    ky = reload(S(sub(mul(cz, f[0]), mul(f[2], cx))))
    kz = reload(S(sub(mul(cx, f[1]), mul(f[0], cy))))
    sq = add(add(mul(kx, kx), mul(ky, ky)), mul(kz, kz))
    turnv = div(sq, L(turn))
    invmass = div(L(ONE_BITS), L(mass))
    mm = reload(S(div(L(ONE_BITS), add(turnv, invmass))))
    ee = [reload(x) for x in e]
    oz = S(mul(mm, ee[2]))
    oy = S(mul(mm, ee[1]))
    ox = S(mul(mm, ee[0]))
    return [ox, oy, oz]


LAST = {}   # trazas del ultimo calculo (cobertura de run_local.py)


def dword_changes(base, old, new, size=4):
    return [(base + i, (new >> (8 * i)) & 0xFF) for i in range(size) if ((old >> (8 * i)) & 0xFF) != ((new >> (8 * i)) & 0xFF)]


def expected(c):
    """devuelve dict: events (elementos int o None=NaN), path, trailer/matrices, globales y cambios por arena"""
    LAST.clear()
    st = dict(trailer=OBJ_A if c['has_trailer'] else None, matrix=MAT_S, mass=c['mass'], turn=c['turn'], centre=list(c['centre']), subtype=c['subtype'], w9ec=c['w9ec'])
    towed = {OBJ_A: dict(matrix=MAT_A, model=c['model'] & 0xFFFF, angle=c['angle'] & 0xFFFF), OBJ_B: dict(matrix=MAT_U, model=c['model_b'] & 0xFFFF, angle=c['angle_b'] & 0xFFFF)}
    mats = {MAT_S: (c['self_m'], c['self_pos']), MAT_S2: (c['alt_self_m'], c['self_pos']), MAT_A: (c['tow_m'], c['tow_pos']), MAT_U: (c['alt_tow_m'], c['alt_tow_pos'])}
    glob = dict(ts=c['ts'], hoist=c['hoist'] & 0xFFFF)
    ev = []
    count = [0]
    mutated = []
    nums = []     # cambios numericos de la mutacion: (arena, desplazamiento, antiguo, nuevo, tamano)

    def after():
        idx = count[0]
        count[0] += 1
        if idx != c['mut_at']:
            return
        k, v = c['mut_kind'], c['mut_val']
        mutated.append((idx, k))
        if k == 1:
            st['trailer'] = OBJ_B
        elif k == 2:
            st['matrix'] = MAT_S2
        elif k == 3:
            towed[OBJ_A]['matrix'] = MAT_U
        elif k == 4:
            glob['ts'] = v
        elif k == 5:
            nums.append((0, 0x8C, st['mass'], v, 4)); st['mass'] = v
        elif k == 6:
            nums.append((0, 0x90, st['turn'], v, 4)); st['turn'] = v
        elif k == 7:
            nums.append((0, 0xA4, st['centre'][0], v, 4)); st['centre'][0] = v
        elif k == 8:
            nums.append((0, 0x594, st['subtype'], v, 4)); st['subtype'] = v
        elif k == 9:
            nums.append((0, 0x9EC, st['w9ec'], v, 4)); st['w9ec'] = v
        elif k == 10:
            nums.append((1, 0x86C, towed[OBJ_A]['angle'], v & 0xFFFF, 2)); towed[OBJ_A]['angle'] = v & 0xFFFF
        elif k == 11:
            nums.append((1, 0x22, towed[OBJ_A]['model'], v & 0xFFFF, 2)); towed[OBJ_A]['model'] = v & 0xFFFF
        elif k == 12:
            glob['hoist'] = v & 0xFFFF

    out = dict(events=ev, mutated=mutated, path=None, damped=False, projected=False)

    def brk(path):
        ev.append([3, tag(OBJ_S)])
        out['path'] = path
        after()

    def finish():
        out['trailer'] = tag(st['trailer']) if st['trailer'] is not None else 0
        out['self_matrix'] = tag(st['matrix'])
        out['tow_matrix'] = [tag(towed[OBJ_A]['matrix']), tag(towed[OBJ_B]['matrix'])]
        out['step'] = glob['ts']
        out['hoist'] = glob['hoist']
        ch = [[], [], [], [], [], [], []]
        for arena, off, old, new, size in nums:
            ch[arena] += dword_changes(off, old, new, size)
        out['changes'] = [sorted(x) for x in ch]
        return out

    if st['trailer'] is None:
        brk('break_no_trailer')
        return finish()
    status = (c['status_byte'] >> 3) & 0x1F
    if status not in (10, 11):
        brk('break_status')
        return finish()
    first = st['trailer']
    ev.append([1, tag(OBJ_S), 0, 0, 0, 1, tag(first)])
    hitch = list(c['hitch'])
    after()
    if (c['hitch_ret'] & 0xFF) == 0:
        brk('break_hitch')
        return finish()
    second = st['trailer']
    ev.append([2, tag(second), 0, 0, 0, 1, tag(OBJ_S)])
    bar = list(c['bar'])
    after()
    if (c['bar_ret'] & 0xFF) == 0:
        brk('break_bar')
        return finish()
    diff, far = too_far(bar, hitch, glob['ts'])
    LAST['diff'] = diff
    if far:
        brk('break_far')
        return finish()
    tw = st['trailer']
    tmat = mats[towed[tw]['matrix']][0]
    smat, spos = mats[st['matrix']]
    b = bent(tmat, smat)
    if b:
        brk('break_forward' if b == 1 else 'break_up')
        return finish()
    if towed[tw]['model'] in (0x20D, 0x213):
        limit = glob['hoist'] - 100
        if towed[tw]['angle'] > limit:
            out['path'] = 'return_tow'
            return finish()
    hitch = [S(sub(L(hitch[i]), L(spos[i]))) for i in range(3)]
    tpos = mats[towed[tw]['matrix']][1]
    bar = [S(sub(L(bar[i]), L(tpos[i]))) for i in range(3)]
    ev.append([4, tag(OBJ_S)] + hitch)
    spd_self = c['spd0']
    after()
    ev.append([4, tag(st['trailer'])] + bar)
    spd_tow = c['spd1']
    after()
    rel = [S(sub(L(spd_tow[i]), L(spd_self[i]))) for i in range(3)]
    if not (c['arg0'] & 0xFF) and (c['arg1'] & 0xFF):
        rel = damp(diff, glob['ts'], GAIN_BITS)
        out['damped'] = True
    if st['subtype'] == 11 and st['w9ec'] == MINUS_1000:
        out['projected'] = True
        sm = mats[st['matrix']][0]
        rel = project(rel, [sm[8], sm[9], sm[10]])
    sm = mats[st['matrix']][0]
    fo = force(sm, st['centre'], rel, hitch, st['mass'], st['turn'])
    LAST.update(rel=rel, force=fo)
    ev.append([5, tag(OBJ_S)] + fo + hitch + [1])
    after()
    out['path'] = 'full'
    return finish()


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def check_side(c, o, exp):
    """compara un lado (observed) con el modelo; devuelve el motivo del primer desacuerdo o None"""
    got = o['events']
    if len(got) != len(exp['events']):
        return 'numero de eventos %d != %d: %s' % (len(got), len(exp['events']), got)
    for i, (g, w) in enumerate(zip(got, exp['events'])):
        if len(g) != len(w):
            return 'evento %d longitud %s != %s' % (i, g, w)
        for k, (a, b) in enumerate(zip(g, w)):
            if b is None:
                if a != QNAN:
                    return 'evento %d campo %d deberia ser NaN %x: %s' % (i, k, QNAN, g)
            elif a != b:
                return 'evento %d campo %d: %x != %x (%s)' % (i, k, a, b, g)
    for key in ('trailer', 'self_matrix', 'tow_matrix', 'step', 'hoist'):
        if o[key] != exp[key]:
            return '%s %s != %s' % (key, o[key], exp[key])
    ch = [[list(x) for x in a] for a in o['changes']]
    want = [[[off, b] for off, b in a] for a in exp['changes']]
    if ch != want:
        return 'cambios %s != %s' % (ch, want)
    return None


def check_row(row):
    c = row['input']
    exp = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], exp)
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
