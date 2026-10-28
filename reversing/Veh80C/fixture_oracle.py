"""Oraculo independiente (Python) de Veh80C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x73DF00..0x73E235, lo que debe observarse de
CVehicle::FireFixedMachineGuns (traza de colaboradores EN ORDEN con todos sus argumentos, escrituras en el vehiculo y hora final) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio guardado como float
(fstp / fst dword, con desnormales) y vuelto a cargar donde el EXE lo guarda (direccion, nx, ny, sumas del extremo, ruido en x e y); la longitud, su inversa y el ruido en z se quedan
en el registro (24 bits, sin guardar). La unica carga de NaN que aparece es el indefinido 0xFFC00000 (las fixtures usan un solo patron de NaN): el oraculo exige exactamente ese patron.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 vehiculo, 1 matriz A, 2 matriz B, 3 peaton A, 4 peaton B.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)
QNAN = 0xFFC00000
GARBAGE = 0xA0000010
AUDIO_ENGINE = 0xB6BC90
EVENT_PLANE_FIRE = 0x95
WEAPON_M4 = 0x1F
K15, K02, K60, KMIN, ONE = 0x3C75C28F, 0x3CA3D70A, 0x42700000, 0x3DCCCCCD, 0x3F800000


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


VEH, MATA, MATB, PEDA, PEDB = (tag(i, 0) for i in range(5))


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


def div(a, b):
    """a / b (solo se usa 1.0 / longitud)"""
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


def fromint(j):
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


LAST = {}   # intermedios del ultimo calculo (cobertura de run_local.py)


def direction(c):
    """0x73DF22..0x73DF9D: devuelve (reg, stored): (nx*60, ny*60) tal como quedan en el registro x87 (24 bits, sin guardar; los usa el primer canon) y
    guardados como float y vueltos a cargar (los usa el segundo canon)"""
    fx, fy = load(c['top_x']), load(c['top_y'])
    sq = add(mul(fy, fy), mul(fx, fx))
    ln = sqrt24(sq)
    mn = load(KMIN)
    clamped = False
    if ln[0] == 'f' and mn[1] > ln[1]:
        ln = mn
        clamped = True
    inv = div(load(ONE), ln)
    nx = reload(store(mul(inv, fx)))
    ny = reload(store(mul(inv, fy)))
    k60 = load(K60)
    rx = mul(nx, k60)
    ry = mul(ny, k60)
    stored = (reload(store(rx)), reload(store(ry)))
    LAST.clear()
    LAST.update(dict(sq=sq, len=ln, clamped=clamped, inv=inv, rx=rx, ry=ry))
    return (rx, ry), stored


def end_point(dx, dy, res, jx, jy, jz):
    """0x73DFB6..0x73E1DB: extremo del disparo (bits, None = NaN); dx y dy son valores x87 (registro o float recargado)"""
    ex = store(add(dx, load(res[0])))
    ey = store(add(load(res[1]), dy))
    ez = store(load(res[2]))
    jxs = store(mul(fromint(jx), load(K15)))
    jys = store(mul(fromint(jy), load(K15)))
    jzs = mul(fromint(jz), load(K02))
    ox = store(add(reload(jxs), reload(ex)))
    oy = store(add(reload(ey), reload(jys)))
    oz = store(add(reload(ez), jzs))
    return [ox, oy, oz], dict(ex=ex, ey=ey, ez=ez, jxs=jxs, jys=jys)


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def is_tiny(v):
    return v != NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < Fraction(2) ** -126


# ---------------------------------------------------------------- el modelo de la funcion
def code_ptr(code):
    if code == 0:
        return 0
    if code == 1:
        return PEDA
    if code == 2:
        return PEDB
    if code == 3:
        return VEH
    return GARBAGE + 0x10 * (code - 4)


def dword_changes(base, old, new):
    return [(base + i, (new >> (8 * i)) & 0xFF) for i in range(4) if ((old >> (8 * i)) & 0xFF) != ((new >> (8 * i)) & 0xFF)]


def expected(c):
    """devuelve dict: events (elementos int o None=NaN), gun, ammo, matrix, driver, changes, time, fired"""
    gun, ammo = c['gun'], c['ammo'] & 0xFF
    matrix = MATA if c['has_mat'] else 0
    driver = code_ptr(c['driver'])
    timer = c['now']
    ev = []
    out = dict(events=ev, fired=False, mutated=[], inter=[])
    if not (c['now'] > ((c['gun'] + 150) & M32)):
        out.update(gun=gun, ammo=ammo, matrix=matrix, driver=driver, time=timer, changes=[])
        return out
    out['fired'] = True
    dirs = direction(c)
    gun = c['now']
    count = [0]

    def after():
        nonlocal matrix, driver, ammo, gun, timer
        idx = count[0]
        count[0] += 1
        if idx != c['mut_at']:
            return
        k, v = c['mut_kind'], c['mut_val']
        out['mutated'].append((idx, k))
        if k == 1:
            matrix = MATB
        elif k == 2:
            driver = code_ptr(v % 7)
        elif k == 3:
            ammo = v & 0xFF
        elif k == 4:
            gun = v
        elif k == 5:
            timer = v

    for g in range(2):
        res = [c['res%d%s' % (g, a)] for a in 'xyz']
        vec = [0x40000000 if g == 0 else 0xC0000000, 0x40200000, 0x3F800000]
        ev.append([1, matrix] + vec + [gun])
        after()
        raw = [c['rnd%d' % (3 * g + i)] for i in range(3)]
        jy, jx, jz = ((raw[0] & 0xFF) - 0x80), ((raw[1] & 0xFF) - 0x80), ((raw[2] & 0xFF) - 0x80)
        for _ in range(3):
            ev.append([2, gun])
            after()
        dx, dy = dirs[g]
        end, inter = end_point(dx, dy, res, jx, jy, jz)
        out['inter'].append(inter)
        start = list(res)
        ev.append([3, VEH, driver] + start + end + [gun])
        if c['doom_mode'] & 2:
            start = [c['dt%d' % i] for i in range(3)]
        if c['doom_mode'] & 1:
            end = [c['dt%d' % (3 + i)] for i in range(3)]
        after()
        ev.append([4] + start + end + [15, gun])
        after()
    ev.append([5, AUDIO_ENGINE, EVENT_PLANE_FIRE, WEAPON_M4, VEH, gun])
    after()
    ammo = (ammo - 1) & 0xFF
    reloaded = ammo == 0
    if reloaded:
        ammo = 20
        gun = (timer + 1400) & M32
    out['reloaded'] = reloaded
    changes = []
    if gun != c['gun']:
        changes += dword_changes(0x4D4, c['gun'], gun)
    if ammo != (c['ammo'] & 0xFF):
        changes.append((0x4AB, ammo))
    out.update(gun=gun, ammo=ammo, matrix=matrix, driver=driver, time=timer, changes=sorted(changes))
    return out


def check_side(c, o, exp):
    """compara un lado (observed) con el modelo; devuelve el motivo del primer desacuerdo o None"""
    got = o['events']
    if len(got) != len(exp['events']):
        return 'numero de eventos %d != %d' % (len(got), len(exp['events']))
    for i, (g, w) in enumerate(zip(got, exp['events'])):
        if len(g) != len(w):
            return 'evento %d longitud %s != %s' % (i, g, w)
        for k, (a, b) in enumerate(zip(g, w)):
            if b is None:
                if a != QNAN:
                    return 'evento %d campo %d deberia ser NaN %x: %s' % (i, k, QNAN, g)
            elif a != b:
                return 'evento %d campo %d: %x != %x (%s)' % (i, k, a, b, g)
    for key in ('gun', 'ammo', 'matrix', 'driver'):
        if o[key] != exp[key]:
            return '%s %x != %x' % (key, o[key], exp[key])
    ch = [list(x) for x in o['changes'][0]]
    if ch != [[off, b] for off, b in exp['changes']]:
        return 'cambios del vehiculo %s != %s' % (ch, exp['changes'])
    if any(o['changes'][i] for i in range(1, 5)):
        return 'cambios en matrices o peatones: %s' % (o['changes'][1:],)
    if o['time'] != exp['time']:
        return 'reloj %x != %x' % (o['time'], exp['time'])
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
