"""Oraculo independiente (Python) de HeliUpdate78: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C79A0..0x6C7C45 (EXE 1.0 US), lo que debe observarse
(traza de colaboradores EN ORDEN, punteros finales de pHelis, temporizador, contador de focos y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float de la distancia es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits.
Los NaN solo cuentan como «no es mayor». La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0..2 helicopteros (0 y 1 = pHelis, 2 = el nuevo), 3..5 sus matrices;
pHelis (0xC1C964, sin guarda) es el indice 6.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 78175  # lo fija cada oraculo


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + SALT) & M32)


def fill(size, seed):
    out = bytearray(size)
    for i in range(0, size, 4):
        w = rand(seed, i // 4)
        n = min(4, size - i)
        out[i:i + n] = w.to_bytes(4, 'little')[:n]
    return out


class Arena:
    """guarda + ventana + guarda; objeto en `guard`. put32/put8 como el banco; changes() = [[desplazamiento respecto al objeto, byte nuevo]]"""

    def __init__(self, win, seed, guard=0x40):
        self.guard = guard
        self.m = fill(guard + win + guard, seed)
        self.ref = None

    def put32(self, off, v):
        self.m[self.guard + off:self.guard + off + 4] = (v & M32).to_bytes(4, 'little')

    def put8(self, off, v):
        self.m[self.guard + off] = v & 0xFF

    def get32(self, off):
        return int.from_bytes(self.m[self.guard + off:self.guard + off + 4], 'little')

    def get8(self, off):
        return self.m[self.guard + off]

    def snap(self):
        self.ref = bytes(self.m)

    def changes(self):
        return [[i - self.guard, self.m[i]] for i in range(len(self.m)) if self.m[i] != self.ref[i]]


def tag(idx, off, guard=0x40):
    return 0x10000000 + (idx << 20) + guard + off


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
NAN = ('n',)


def load_int(i):
    """fild: un entero exacto como valor de la x87"""
    return ('f', Fraction(i), 1 if i < 0 else 0) if i != 0 else ('f', Fraction(0), 0)


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


def fchs(a):
    if a == NAN:
        return a
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, fchs(b))


def sqrt(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if a[1] else a
    if a[1] == 0:
        return a
    if a[1] < 0:
        return NAN
    s = 80
    n = (a[1] * (1 << (2 * s))).__floor__()
    r = isqrt(n)
    exact = r * r == n and (a[1] * (1 << (2 * s))).denominator == 1
    root = Fraction(r, 1 << s) if exact else Fraction(2 * r + 1, 1 << (s + 1))
    return ('f', rnd(root), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN (cualquier carga)"""
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
    """un valor guardado como float y vuelto a cargar (NaN se mantiene NaN)"""
    return NAN if bits is None else load(bits)


def num(a):
    """valor para comparar: None si es NaN, +-inf como float, o Fraction"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return float('-inf') if a[1] else float('inf')
    return a[1]


def gt(a, b):
    """a > b ordenado (falso con NaN)"""
    x, y = num(a), num(b)
    return x is not None and y is not None and x > y


def lt(a, b):
    x, y = num(a), num(b)
    return x is not None and y is not None and x < y


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def same_float(got, want):
    """want: bits o None (NaN = cualquier carga)"""
    if want is None:
        return is_nan(got)
    return got == want


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_tiny(v):
    return v != NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < Fraction(2) ** -126


def is_signed_zero(v, neg):
    return v != NAN and v[0] == 'f' and v[1] == 0 and v[2] == neg


# ================================================================================================ UpdateHelis (0x6C79A0)
C170 = 0x432A0000       # 0x858F98
C100 = 0x42C80000       # 0x42C80000 (ecx en 0x6C7BD5)
PHELIS_TAG_INDEX = 6
LAST = {}


def sx(v):
    return v - (1 << 32) if v & 0x80000000 else v


def ptag(i):
    """etiqueta de la direccion de pHelis[i] (indice 6, sin guarda)"""
    return 0x10000000 + (PHELIS_TAG_INDEX << 20) + 4 * i


def expected(c):
    """(eventos, cambios [heli0, heli1, heli2, mat0, mat1, mat2], pHelis etiquetado, temporizador, focos)"""
    seed = c['seed']
    heli = [Arena(0xA18, (seed * 13 + 1 + i) & M32) for i in range(3)]
    mat = [Arena(0x48, (seed * 13 + 4 + i) & M32) for i in range(3)]
    s = [{k: c['s%d_%s' % (i, k)] for k in ('present', 'model', 'flags', 'b42b', 'mission', 'has_mat', 'x', 'y', 'z')} for i in range(3)]
    for i in range(3):
        heli[i].put8(0x3BA, s[i]['mission'])   # el banco lo escribe antes de la referencia; +0x9A8 y +0x9B0 conservan el relleno
    for a in heli + mat:
        a.snap()
    mission = [s[i]['mission'] & 0xFF for i in range(3)]
    ev = [[1, 0xFFFFFFFF], [2, c['wanted']], [3]]
    ptrs = [0, 0]          # 0 = vacio, 1 + indice de arena
    if s[0]['present']:
        ptrs[0] = 1
    if s[1]['present']:
        ptrs[1] = 2
    L = {'flag_police': False, 'news_clears': False}
    count = sum(1 for p in ptrs if p)
    flag = False
    for p in ptrs:
        if p:
            k = p - 1
            if (s[k]['model'] & 0xFFFF) == 0x1F1 and not (s[k]['flags'] & 0x20000000) and not (s[k]['b42b'] & 0x40):
                flag = True
    L['flag_police'] = flag
    mx = sx(c['max_helis'])
    L['zero_norain'] = bool(c['no_rain'] & 0xFF)
    L['zero_area'] = c['area'] != 0
    L['zero_allowed'] = (c['allowed'] & 0xFF) == 0
    L['zero_weather_old'] = (c['w_old'] & 0xFFFF) == 0x13
    L['zero_weather_new'] = (c['w_new'] & 0xFFFF) == 0x13
    if L['zero_norain'] or L['zero_area'] or L['zero_allowed'] or L['zero_weather_old'] or L['zero_weather_new']:
        mx = 0
    L['max'] = mx
    for p in ptrs:
        if p and (s[p - 1]['model'] & 0xFFFF) == 0x1E8:
            if flag:
                L['news_clears'] = True
            flag = False
    use_news = (c['use_news'] & 0xFF) != 0
    L['use_news'] = use_news
    L['use_news_flag'] = use_news and flag
    if use_news and flag:
        ok = (c['b72f0'] & 0xFF) == 1
    else:
        flag = False
        ok = (c['b73a4'] & 0xFF) == 1
    L['ok'] = ok
    now, timer = c['now'], c['test']
    L['elapsed'] = now > timer
    L['spawn_try'] = ok and now > timer
    L['spawned'] = False
    L['spawn_slot'] = None
    L['count'] = count
    L['spawn_cap'] = False
    if ok and now > timer:
        timer = (now + 15000) & M32
        if count < mx:
            L['spawn_cap'] = True
            ev.append([4, 0xFFFFFFFF])
            ev.append([5, c['ped'], 1 if flag else 0])
            L['spawned'] = True
            L['flag_passed'] = flag
            if not ptrs[0]:
                ptrs[0] = 3
                ev.append([6, tag(2, 0), ptag(0)])
                L['spawn_slot'] = 0
            elif not ptrs[1]:
                ptrs[1] = 3
                ev.append([6, tag(2, 0), ptag(1)])
                L['spawn_slot'] = 1
            else:
                L['spawn_slot'] = 2   # las dos ocupadas: no se registra
    L['loop'] = []
    dist_info = []
    for i in range(2):
        p = ptrs[i]
        if not p:
            continue
        k = p - 1
        if (s[k]['flags'] & 0x20000000) or (s[k]['b42b'] & 0x40):
            L['loop'].append(('flagged', bool(s[k]['flags'] & 0x20000000), bool(s[k]['b42b'] & 0x40)))
            mission[k] = 0x2A
            ptrs[i] = 0
            continue
        if mission[k] != 0x2A:
            L['loop'].append(('keep_mission',))
            continue
        ev.append([7, 0xFFFFFFFF])
        dx = sub(load(c['player_x']), load(s[k]['x']))
        dy = sub(load(c['player_y']), load(s[k]['y']))
        dz = sub(load(c['player_z']), load(s[k]['z']))
        total = add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx))
        dist = sqrt(total)
        far = gt(dist, load(C170))
        dist_info.append(dist)
        L['loop'].append(('far' if far else 'near', dist == NAN, num(dist), s[k]['has_mat']))
        if far:
            ev.append([8, tag(k, 0)])
            ev.append([9, tag(k, 0), 1])
            ptrs[i] = 0
    L['dists'] = dist_info
    rem = mx
    L['tail'] = []
    if ptrs[0]:
        k = ptrs[0] - 1
        if mission[k] != 0x2A:
            if mx > 0:
                rem = mx - 1
                L['tail'].append('dec')
            else:
                mission[k] = 0x2A
                heli[k].put32(0x9B0, C100)
                heli[k].put32(0x9A8, C100)
                L['tail'].append('flee0')
    if ptrs[1]:
        k = ptrs[1] - 1
        if mission[k] != 0x2A and rem <= 0:
            mission[k] = 0x2A
            heli[k].put32(0x9B0, C100)
            heli[k].put32(0x9A8, C100)
            L['tail'].append('flee1')
    for k in range(3):
        if mission[k] != (s[k]['mission'] & 0xFF):
            heli[k].put8(0x3BA, mission[k])
    L['ptrs'] = list(ptrs)
    helis_tag = [0 if not p else tag(p - 1, 0) for p in ptrs]
    changes = [a.changes() for a in heli] + [a.changes() for a in mat]
    LAST.clear()
    LAST.update(L)
    return ev, changes, helis_tag, timer, 0


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, changes, helis_tag, timer, lights = expected(c)
    bad = []
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['helis'] != helis_tag:
            bad.append((side, 'pHelis %s != %s' % (o['helis'], helis_tag)))
        if o['timer'] != timer:
            bad.append((side, 'temporizador'))
        if o['lights'] != lights:
            bad.append((side, 'focos'))
        if o['now'] != c['now']:
            bad.append((side, 'reloj'))
        got = o['events']
        if got != ev:
            bad.append((side, 'eventos %s != %s' % (str(got)[:200], str(ev)[:200])))
        if o['changes'] != changes:
            bad.append((side, 'cambios %s != %s' % (str(o['changes'])[:300], str(changes)[:300])))
    return bad, None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        why, _ = check_row(row)
        checked += 1
        if why:
            bad += 1
            if bad <= 8:
                print('DIFIERE', row['case'], why[:3])
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
