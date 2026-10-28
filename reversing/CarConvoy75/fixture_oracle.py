"""Oraculo independiente (Python) de CarConvoy75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42C740..0x42CBCF (EXE 1.0 US), lo que debe observarse de
CCarCtrl::CreateConvoy (traza de colaboradores EN ORDEN, resultado en AL, punteros al coche anterior y la memoria final de las arenas: plantilla, tres coches, matrices, conductores, inteligencias, tareas y
modelo) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los guardados
`fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga depende de la forma de la instruccion); todo lo demas, bit a bit. La memoria de las arenas se
reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 plantilla, 1..3 coches, 4 matriz de la plantilla, 5..7 matrices de los coches,
8..10 conductores, 11..13 inteligencias, 14..16 tareas, 17 informacion de modelo, 18 modelo de colision.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 75002  # lo fija cada oraculo


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

# ================================================================================================ CreateConvoy (0x42C740)
ZERO = ('f', Fraction(0), 0)
ONE_E9 = 0x4E6E6B28
C002 = 0x3CA3D70A
C25 = 0x40200000
LAST = {}
UNKNOWN = 'unknown'


def sgn32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def sext16(v):
    v &= 0xFFFF
    return (v - 0x10000 if v & 0x8000 else v) & M32


def tag(idx, off, guard=0x40):
    return 0x10000000 + (idx << 20) + guard + off


T_TPL, T_CAR0, T_TMAT, T_CMAT0, T_CDRV0, T_CINT0, T_TASK0, T_INFO, T_COL = 0, 1, 4, 5, 8, 11, 14, 17, 18
SEEDS = {'tpl': 1, 'tmat': 2, 'info': 3, 'col': 4}
WIN = {'tpl': 0x5A0, 'tmat': 0x48, 'info': 0x40, 'col': 0x40}


class Arena2(Arena):
    """Arena con un registro de dwords de valor desconocido (solo se exige que sean NaN)"""

    def __init__(self, win, seed, guard=0x40):
        super().__init__(win, seed, guard)
        self.unknown = set()

    def put16(self, off, v):
        self.m[self.guard + off:self.guard + off + 2] = (v & 0xFFFF).to_bytes(2, 'little')

    def get16(self, off):
        return int.from_bytes(self.m[self.guard + off:self.guard + off + 2], 'little')


def make_arenas(c):
    seed = c['seed']
    ar = {}
    for k in ('tpl', 'tmat', 'info', 'col'):
        ar[k] = Arena2(WIN[k], (seed * 13 + SEEDS[k]) & M32)
    for i in range(3):
        ar['car%d' % i] = Arena2(0x5A0, (seed * 13 + 10 + i) & M32)
        ar['cmat%d' % i] = Arena2(0x48, (seed * 13 + 20 + i) & M32)
        ar['cdrv%d' % i] = Arena2(0x4A0, (seed * 13 + 30 + i) & M32)
        ar['cint%d' % i] = Arena2(0x100, (seed * 13 + 40 + i) & M32)
        ar['task%d' % i] = Arena2(0x40, (seed * 13 + 50 + i) & M32)
    # los mismos campos que escribe el banco (los punteros no se modelan: no cambian)
    ar['tpl'].put16(0x22, c['tmodel']); ar['tpl'].put8(0x3D0, c['cruise']); ar['tpl'].put32(0x3C8, c['speed'])
    for i in range(3):
        ar['tmat'].put32(0x10 + 4 * i, c['fwd_' + 'xyz'[i]])
        ar['tmat'].put32(0x30 + 4 * i, c['pos_' + 'xyz'[i]])
        ar['car%d' % i].put32(0x590, c['ctype%d' % i])
    ar['col'].put32(0x24, c['radius'])
    return ar


ORDER = ['tpl', 'car0', 'car1', 'car2', 'tmat', 'cmat0', 'cmat1', 'cmat2', 'cdrv0', 'cdrv1', 'cdrv2', 'cint0', 'cint1', 'cint2', 'task0', 'task1', 'task2', 'info', 'col']


def put_word(arena, off, value):
    """value: bits, None (NaN) o UNKNOWN"""
    if value is None or value == UNKNOWN:
        arena.unknown.add(arena.guard + off)
        arena.put32(off, 0x7FC00000)
    else:
        arena.unknown.discard(arena.guard + off)
        arena.put32(off, value)


def nearer(hit_bits, ground_bits, z_bits):
    z = load(z_bits)
    d1 = sub(load(hit_bits), z)
    if lt(d1, ZERO):
        d1 = fchs(d1)
    d2 = sub(load(ground_bits), z)
    if lt(d2, ZERO):
        d2 = fchs(d2)
    return lt(d1, d2)


def convoy_point(c, k):
    """(x, y, z) de la vuelta con k = indice + 1: bits o None (NaN)"""
    kd = load_int(k)
    m10, m14, m18 = c['fwd_x'], c['fwd_y'], c['fwd_z']
    a = mul(kd, load(m18))
    b = store(mul(kd, load(m14)))
    cc = store(mul(kd, load(m10)))
    r = load(c['radius'])
    t = store(add(add(r, r), load(C25)))
    at = mul(a, reload(t))
    zp = store(sub(load(c['pos_z']), at))
    bt = store(mul(reload(b), reload(t)))
    yp = store(sub(load(c['pos_y']), reload(bt)))
    ct = store(mul(reload(cc), reload(t)))
    xp = store(sub(load(c['pos_x']), reload(ct)))
    return xp, yp, zp


def convoy_move(c):
    """velocidad de la plantilla: adelante * (float)crucero * 0.02f (el producto en z se queda en el registro)"""
    cr = load_int(c['cruise'])
    k = load(C002)
    z = store(mul(mul(cr, load(c['fwd_z'])), k))
    y = store(mul(reload(store(mul(cr, load(c['fwd_y'])))), k))
    x = store(mul(reload(store(mul(cr, load(c['fwd_x'])))), k))
    return [x, y, z]


def expected(c):
    """(eventos, resultado, arenas modelo, destinos)"""
    ar = make_arenas(c)
    for a in ar.values():
        a.snap()
    L = {'created': 0, 'aborts': 0}
    LAST.clear()
    LAST.update(L=L, c=c)
    ev = [[1]]
    roll = ((c['rnd'] & 0xFFFF) * 100) // 32768
    count = 2 + (1 if roll > 50 else 0)
    L['count'] = count
    tpl = ar['tpl']
    tpl.put16(0x45E, tpl.get16(0x20))
    tmodel = sext16(c['tmodel'])
    flags = [1, 0, 0, 0, 1, 0, 0]
    prev = tag(T_TPL, 0)
    created = False
    pv = hh = ff = tk = 0
    targets = [ar['car%d' % i].get32(0x41C) for i in range(3)]
    tpl_move = None
    for i in range(count):
        ev.append([2, tmodel, 1])
        if (c['null_mask'] >> i) & 1:
            L['aborts'] += 1
            L['null_car'] = True
            continue
        car = ar['car%d' % i]
        x, y, z = convoy_point(c, i + 1)
        ev.append([3, x, y, z, 0x447A0000, 1, 1] + flags)
        ground = ONE_E9
        if c['hit%d' % pv] & 0xFF:
            ground = c['hitz%d' % pv]
        pv += 1
        ev.append([3, x, y, z, 0xC47A0000, 1, 1] + flags)
        if c['hit%d' % pv] & 0xFF:
            zload = z  # el punto sigue siendo (x, y, z) con la z del punto (puede ser NaN)
            hz = c['hitz%d' % pv]
            if z is None:
                nr = False
            else:
                nr = nearer(hz, ground, z)
            L['second_nearer'] = L.get('second_nearer', 0) + (1 if nr else 0)
            if nr:
                ground = hz
        pv += 1
        if gt(load(ground), load(ONE_E9)):
            ev.append([6, tag(T_CAR0 + i, 0), 1])
            L['aborts'] += 1
            L['no_ground'] = True
            continue
        ev.append([4, tag(T_CAR0 + i, 0)])
        zp = store(add(load(c['height%d' % hh]), load(ground)))
        hh += 1
        ev.append([5, x, y, zp, c['radius'], 1, 1, 2, 0, 0, 1, 1, 0, 0])
        cv = c['col%d' % ff]
        ff += 1
        if cv & 0xFFFF:
            ev.append([6, tag(T_CAR0 + i, 0), 1])
            L['aborts'] += 1
            L['colliding'] = True
            continue
        if not created:
            tpl.put8(0x36, (tpl.get8(0x36) & 7) | 0x18)
            tpl.put8(0x3B9, 2)
            tpl_move = convoy_move(c)
            for j in range(3):
                put_word(tpl, 0x44 + 4 * j, tpl_move[j])
        cmat = ar['cmat%d' % i]
        has_mat = bool(c['cmat%d' % i])
        ev.append([7, tag(T_CMAT0 + i, 0) if has_mat else 0, tag(T_TMAT, 0)])
        dest, base = (cmat, 0x30) if has_mat else (car, 4)
        for j, v in enumerate((x, y, zp)):
            put_word(dest, base + 4 * j, v)
        for j in range(3):
            put_word(car, 0x44 + 4 * j, tpl_move[j])
        car.put8(0x36, (car.get8(0x36) & 7) | (tpl.get8(0x36) & 0xF8))
        car.put8(0x42B, car.get8(0x42B) | 8)
        ev.append([8, tag(T_CAR0 + i, 0)])
        ct = sgn32(c['ctype%d' % i])
        if ct == 9:
            ev.append([9, tag(T_CAR0 + i, 0)])
        elif ct == 0:
            ev.append([10, tag(T_CAR0 + i, 0)])
        ev.append([11, tag(T_CAR0 + i, 0), c['ped_type'], 0, 1, 0, 0x63])
        ev.append([12, 0x24])
        task = 0
        if not c['ctask_null%d' % tk]:
            ev.append([13, tag(T_TASK0 + tk, 0), tag(T_CAR0 + i, 0), 0, 0x41200000])
            task = tag(T_TASK0 + tk, 0)
        tk += 1
        ev.append([14, tag(T_CINT0 + i, 4), task, 4, 0])
        car.put8(0x3D0, tpl.get8(0x3D0))
        val = sub(load(c['speed']), load_int(i))
        put_word(car, 0x3C8, 0 if lt(val, ZERO) else store(val))
        car.put8(0x3B9, tpl.get8(0x3B9))
        car.put8(0x3BA, tpl.get8(0x3BA))
        car.put16(0x20, tpl.get16(0x20))
        car.put16(0x45E, tpl.get16(0x45E))
        targets[i] = prev
        ev.append([15, prev])
        prev = tag(T_CAR0 + i, 0)
        created = True
        L['created'] += 1
    return ev, {'ret': 1 if created else 0}, ar, targets


def final_bytes(arena, changes):
    m = bytearray(arena.ref)
    for off, b in changes:
        m[arena.guard + off] = b
    return m


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, result, ar, targets = expected(c)
    bad = []
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['ret'] != result['ret']:
            bad.append((side, 'resultado %x != %x' % (o['ret'], result['ret'])))
        if o['targets'] != targets:
            bad.append((side, 'objetivos %s != %s' % (o['targets'], targets)))
        got = o['events']
        if len(got) != len(ev):
            bad.append((side, 'numero de eventos %d != %d: %s' % (len(got), len(ev), str(got)[:200])))
        else:
            for g, w in zip(got, ev):
                if len(g) != len(w) or g[0] != w[0] or any(not same_float(gv, wv) if wv is None else gv != wv for gv, wv in zip(g[1:], w[1:])):
                    bad.append((side, 'evento %s != %s' % (g, w)))
                    break
        for nm, ch in zip(ORDER, o['changes']):
            a = ar[nm]
            obs = final_bytes(a, ch)
            for i in range(len(obs)):
                if obs[i] != a.m[i]:
                    if (i & ~3) in a.unknown:
                        continue
                    bad.append((side, 'bytes de %s +%x: %02x != %02x' % (nm, i - a.guard, obs[i], a.m[i])))
                    break
            for word in a.unknown:
                if not is_nan(int.from_bytes(obs[word:word + 4], 'little')):
                    bad.append((side, 'palabra de %s +%x no es NaN' % (nm, word - a.guard)))
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
