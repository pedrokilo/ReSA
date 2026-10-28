"""Oraculo independiente (Python) de CarPolice75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42C2B0..0x42C735 (EXE 1.0 US), lo que debe observarse de
CCarCtrl::CreatePoliceChase (traza de colaboradores EN ORDEN, resultado en AL, y la memoria final de las arenas: plantilla, coche nuevo, matrices, conductor y pasajeros de la plantilla, conductor nuevo,
inteligencia, tarea, nodos y modelo) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los guardados
`fstp dword` redondeados al float (con desnormales). fsin/fcos trabajan con 64 bits de mantisa (serie de Taylor de 200 digitos tras reducir por 2 pi, redondeada a 64 bits); mas alla de 2**16 la reduccion de
argumento del equipo (66 bits de pi) no se reproduce y el resultado se da por «desconocido» (solo se exige que EXE y DLL coincidan). Los NaN solo se comparan como «es NaN» (la carga depende de la forma de la
instruccion); todo lo demas, bit a bit. La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 plantilla, 1 coche nuevo, 2 matriz de la plantilla, 3 matriz del coche nuevo,
4 conductor de la plantilla, 5..12 pasajeros de la plantilla, 13 conductor del coche nuevo, 14 inteligencia, 15 tarea, 16 nodos, 17 informacion de modelo, 18 modelo de colision.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 75001  # lo fija cada oraculo


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

# ================================================================================================ CreatePoliceChase (0x42C2B0)
from decimal import Decimal, getcontext

getcontext().prec = 220
PI_DIGITS = Decimal('3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196')
TWO_PI = 2 * PI_DIGITS
ZERO = ('f', Fraction(0), 0)
ONE_E9 = 0x4E6E6B28
C002 = 0x3CA3D70A
C_TWO_E63 = Fraction(2) ** 63
TRIG_EXACT_LIMIT = Fraction(1 << 16)   # mas alla, la reduccion de argumento de 66 bits de pi del equipo no se puede reproducir: el resultado queda "desconocido" (solo se comprueba que sea el mismo en ambos lados)
LAST = {}
UNKNOWN = 'unknown'


def fsincos_exact(a):
    """(seno, coseno) extendidos de un valor cargado: NaN/inf dan NaN; |x| >= 2**63 deja el operando; UNKNOWN si |x| supera TRIG_EXACT_LIMIT"""
    if a == NAN or a[0] == 'i':
        return NAN, NAN
    x = a[1]
    if x == 0:
        return a, ('f', Fraction(1), 0)   # sen(+-0) = +-0, cos = 1
    if abs(x) >= C_TWO_E63:
        return a, a
    if abs(x) >= TRIG_EXACT_LIMIT:
        return UNKNOWN, UNKNOWN
    xd = Decimal(x.numerator) / Decimal(x.denominator)
    k = (xd / TWO_PI).to_integral_value()
    r = xd - k * TWO_PI
    s = Decimal(0)
    c = Decimal(0)
    term = Decimal(1)
    n = 0
    sign = 1
    while True:
        if n % 2 == 0:
            c += sign * term
        else:
            s += sign * term
            sign = -sign
        n += 1
        term = term * r / n
        if abs(term) < Decimal(10) ** -200 and n > 10:
            break
    return ext(s), ext(c)


def rndp(v, p):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (e - (p - 1))
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def ext(d):
    f = Fraction(d)
    return ('f', rndp(f, 64), 1 if f < 0 else 0) if f != 0 else ('f', Fraction(0), 0)


def sgn32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def tag(idx, off, guard=0x40):
    return 0x10000000 + (idx << 20) + guard + off


T_TPL, T_NEW, T_TMAT, T_NMAT, T_TDRV, T_PASS0, T_NDRV, T_INTEL, T_TASK, T_NODES, T_INFO, T_COL = 0, 1, 2, 3, 4, 5, 13, 14, 15, 16, 17, 18
SEEDS = {'tpl': 1, 'new': 2, 'tmat': 3, 'nmat': 4, 'tdrv': 5, 'ndrv': 6, 'intel': 20, 'task': 21, 'nodes': 22, 'info': 23, 'col': 24}
WIN = {'tpl': 0x5A0, 'new': 0x5A0, 'tmat': 0x48, 'nmat': 0x48, 'tdrv': 0x4A0, 'ndrv': 0x4A0, 'intel': 0x100, 'task': 0x40, 'nodes': 0x1C * 20, 'info': 0x40, 'col': 0x40}
ORDER = ['tpl', 'new', 'tmat', 'nmat', 'tdrv', 'ndrv', 'intel', 'task', 'nodes', 'info', 'col']


class Arena2(Arena):
    """Arena con un registro de dwords de valor desconocido (solo se exige que sean NaN)"""

    def __init__(self, win, seed, guard=0x40):
        super().__init__(win, seed, guard)
        self.unknown = set()   # dwords que deben ser NaN (carga cualquiera)
        self.wild = set()      # dwords de valor desconocido (sin restriccion)
        self.win = win

    def put16(self, off, v):
        self.m[self.guard + off:self.guard + off + 2] = (v & 0xFFFF).to_bytes(2, 'little')

    def get16(self, off):
        return int.from_bytes(self.m[self.guard + off:self.guard + off + 2], 'little')


def make_arenas(c):
    seed = c['seed']
    ar = {k: Arena2(WIN[k], (seed * 13 + SEEDS[k]) & M32) for k in ('tpl', 'new', 'tmat', 'nmat', 'tdrv', 'ndrv', 'intel', 'task', 'nodes', 'info', 'col')}
    ar['pass'] = [Arena2(0x4A0, (seed * 13 + 7 + i) & M32) for i in range(8)]
    # los mismos campos que escribe el banco (los punteros no se modelan: no cambian)
    ar['tpl'].put32(0x10, c['head']); ar['tpl'].put8(0x3D0, c['cruise']); ar['tpl'].put8(0x488, c['pcount'])
    for i in range(3):
        ar['tmat'].put32(0x10 + 4 * i, c['fwd_' + 'xyz'[i]])
    ar['new'].put16(0x22, c['pmodel']); ar['new'].put32(0x590, c['ptype'])
    ar['col'].put32(0x24, c['radius'])
    return ar


def fwd_of(c):
    """adelante de la plantilla (bits o UNKNOWN o None = NaN, por componente)"""
    if c['tmat']:
        return [c['fwd_x'], c['fwd_y'], c['fwd_z']]
    sn, cs = fsincos_exact(load(c['head']))
    if sn == UNKNOWN:
        return [UNKNOWN, UNKNOWN, 0]
    return [store(fchs(sn)), store(cs), 0]


def move_speed(c, scale):
    """velocidad de la plantilla: float(0.02 * float(escala * adelante)) por componente"""
    out = []
    for f in fwd_of(c):
        if f == UNKNOWN:
            out.append(UNKNOWN)
            continue
        v2 = store(mul(load_int(scale), load(f))) if f is not None else None
        v2 = reload(v2) if f is not None else NAN
        out.append(store(mul(load(C002), v2)))
    return out


def put_word(arena, off, value):
    """value: bits, None (NaN) o UNKNOWN"""
    if value == UNKNOWN:
        arena.wild.add(arena.guard + off)
        arena.put32(off, 0x7FC00000)
    elif value is None:
        arena.unknown.add(arena.guard + off)
        arena.put32(off, 0x7FC00000)
    else:
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


def expected(c):
    """(eventos, resultado, arenas modelo) del caso"""
    ar = make_arenas(c)
    for a in list(ar.values())[:11] + ar['pass']:
        a.snap()
    ev = []
    result = {'ret': 0}
    L = {'path': 'start'}
    LAST.clear()
    LAST.update(L=L, c=c)

    def done(ret, why):
        L['path'] = why
        result['ret'] = ret
        return ev, result, ar

    gangs_zero = (c['gangs'] & 0x7FFFFFFF) == 0
    ped = sgn32(c['ped_type'])
    L['roll'] = (not gangs_zero) and (ped < 14 or ped > 23)
    if L['roll']:
        ev.append([1])
        if (c['rnd'] & 0xFFFF) >= 0x2000:
            return done(0, 'roll_fails')
    ev.append([2, 1])
    model = sgn32(c['model'])
    if model <= -1:
        return done(0, 'model_negative')
    if (c['loaded'] & 0xFF) != 1:
        return done(0, 'not_loaded')
    ev.append([3, model & M32, 1])
    if c['veh_null']:
        return done(0, 'vehicle_null')
    ev.append([4, tag(T_NODES, c['node'] * 0x1C)])
    px, py, pz = c['node_x'], c['node_y'], c['node_z']
    flags = [1, 0, 0, 0, 1, 0, 0]
    ev.append([5, px, py, pz, 0x447A0000, 1, 1] + flags)
    ground = ONE_E9
    L['hit1'] = bool(c['hit1'] & 0xFF)
    if L['hit1']:
        ground = c['hit1z']
    ev.append([5, px, py, pz, 0xC47A0000, 1, 1] + flags)
    L['hit2'] = bool(c['hit2'] & 0xFF)
    L['second_nearer'] = False
    if L['hit2']:
        L['second_nearer'] = nearer(c['hit2z'], ground, pz)
        if L['second_nearer']:
            ground = c['hit2z']
    L['ground'] = ground
    if gt(load(ground), load(ONE_E9)):
        ev.append([10, tag(T_NEW, 0), 1])
        return done(0, 'no_ground')
    ev.append([6, tag(T_NEW, 0)])
    zp = store(add(load(c['height']), load(ground)))    # None = NaN
    L['zp'] = zp
    ev.append([7, tag(T_TPL, 0)])
    L['onscreen'] = bool(c['on_screen'] & 0xFF)
    if not L['onscreen']:
        ev.append([8, 0xB6F028, px, py, zp, c['radius']])
        L['visible'] = bool(c['visible'] & 0xFF)
        if L['visible']:
            ev.append([10, tag(T_NEW, 0), 1])
            return done(0, 'visible')
    ev.append([9, px, py, zp, c['radius'], 1, 1, 2, 0, 0, 1, 1, 0, 0])
    if c['colliding'] & 0xFFFF:
        ev.append([10, tag(T_NEW, 0), 1])
        return done(0, 'colliding')
    tpl, new = ar['tpl'], ar['new']
    tpl.put8(0x36, (tpl.get8(0x36) & 7) | 0x18)
    tpl.put8(0x3B9, 2)
    tpl.put8(0x42E, tpl.get8(0x42E) | 8)
    cruise = (c['cruise'] + 10) & 0xFF
    tpl.put8(0x3D0, cruise)
    L['cruise_wrap'] = c['cruise'] + 10 > 255
    speed = move_speed(c, cruise)
    L['speed'] = speed
    for i in range(3):
        put_word(tpl, 0x44 + 4 * i, speed[i])
    ev.append([11, tag(T_TPL, 0), c['ped_type'], 2, 1, 1, 0x63])
    ev.append([12, tag(T_NMAT, 0), tag(T_TMAT, 0) if c['tmat'] else 0])
    ev.append([13, tag(T_NEW, 0), px, py, zp])
    for i in range(3):
        put_word(new, 0x44 + 4 * i, speed[i])
    new.put8(0x36, (new.get8(0x36) & 7) | 0x18)
    new.put8(0x42C, new.get8(0x42C) | 1)
    ev.append([14, tag(T_NEW, 0), 1])
    ev.append([15, tag(T_NEW, 0)])
    ev.append([16 if sgn32(c['ptype']) == 9 else 17, tag(T_NEW, 0)])
    ev.append([18, tag(T_NEW, 0), 1])
    ar['tdrv'].put32(0x478, ar['tdrv'].get32(0x478) | 0x800)
    for i in range(min(c['pcount'], 8) if c['pcount'] <= 8 else 8):
        if (c['pmask'] >> i) & 1:
            p = ar['pass'][i]
            p.put32(0x478, p.get32(0x478) | 0x800)
    ev.append([19, 0x24])
    task = 0
    if not c['task_null']:
        ev.append([20, tag(T_TASK, 0), tag(T_NEW, 0), 0, 0x41200000])
        task = tag(T_TASK, 0)
    ev.append([21, tag(T_INTEL, 4), task, 4, 0])
    ev.append([22, tag(T_TDRV, 0)])
    ev.append([24, tag(T_INTEL, 0x68), 0, 0, 1, 0x451, 0xFFFF, tag(T_TDRV, 0), 0])
    new.put32(0x42C, new.get32(0x42C) | 0x40)
    new.put16(0x4A6, 0xFF)
    ev.append([23, tag(T_TDRV, 0)])
    return done(1, 'created')


def final_bytes(arena, changes):
    m = bytearray(arena.ref)
    for off, b in changes:
        m[arena.guard + off] = b
    return m


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, result, ar = expected(c)
    bad = []
    names = ORDER
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['ret'] != result['ret']:
            bad.append((side, 'resultado %x != %x' % (o['ret'], result['ret'])))
        got = o['events']
        if len(got) != len(ev):
            bad.append((side, 'numero de eventos %d != %d: %s' % (len(got), len(ev), str(got)[:200])))
        else:
            for g, w in zip(got, ev):
                if len(g) != len(w) or g[0] != w[0] or any(not same_float(gv, wv) if wv is None else gv != wv for gv, wv in zip(g[1:], w[1:])):
                    bad.append((side, 'evento %s != %s' % (g, w)))
                    break
        arenas = [ar[k] for k in names]
        for a, ch, nm in zip(arenas, o['changes'], names):
            obs = final_bytes(a, ch)
            for i in range(len(obs)):
                if obs[i] != a.m[i]:
                    word = i & ~3
                    if word in a.unknown or word in a.wild:
                        continue
                    bad.append((side, 'bytes de %s +%x: %02x != %02x' % (nm, i - a.guard, obs[i], a.m[i])))
                    break
            for word in a.unknown:
                if not is_nan(int.from_bytes(obs[word:word + 4], 'little')):
                    bad.append((side, 'palabra de %s +%x no es NaN' % (nm, word - a.guard)))
        for i, (a, ch) in enumerate(zip(ar['pass'], o['passengers'])):
            obs = final_bytes(a, ch)
            if bytes(obs) != bytes(a.m):
                bad.append((side, 'bytes del pasajero %d' % i))
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
