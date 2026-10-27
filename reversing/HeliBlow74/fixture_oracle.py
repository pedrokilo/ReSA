"""Oraculo independiente (Python) de HeliBlow74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C6D30..0x7048 (EXE 1.0 US), lo que debe observarse
(compuertas, traza de colaboradores EN ORDEN, bytes cambiados de las arenas y globales: contadores del jugador, bandera de noticias, ambulancias y camiones) y lo compara con las filas de comparisons.jsonl
(lado original y candidato).

La aritmetica float es la del x87 del EXE: 24 bits de mantisa (redondeo al par, exponente sin limite) y guardado como float para el contador de persecucion y la estadistica. Los NaN solo se comparan como «es NaN».
La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp), asi que los bytes cambiados se comparan exactamente.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 helicoptero, 1 matriz, 2 ped del jugador, 3 vehiculo del jugador, 4 otra entidad, 5 atomo.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 74276  # lo fija cada oraculo


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

# ================================================================================================ BlowUpCar (0x6C6D30)
K_ARENA = {'heli': 0, 'mat': 1, 'ped': 2, 'veh': 3, 'other': 4, 'atom': 5}
LAST = {}
CAMERA, FIRE_MANAGER = 0xB6F028, 0xB71F80
PLAYERS, STEP = 0xB7CD98, 0x190
B_10, B_04, B_08 = 0x41200000, 0x3ECCCCCD, 0x3F4CCCCD   # 10.0f, 0.4f, 0.8f


def ptr_of(kind, raw, own):
    """valor y etiqueta de un puntero segun el tipo del caso (own = clave de la arena propia)"""
    if kind == 0:
        return tag(K_ARENA[own], 0)
    if kind == 1:
        return 0
    if kind == 2:
        return tag(K_ARENA['other'], 0)
    return Tag32(raw)


def Tag32(v):
    return v & M32   # un puntero crudo no esta en ninguna arena (las direcciones del banco no coinciden con valores aleatorios salvo por azar despreciable)


def expected(c):
    """(eventos, cambios [helicoptero, matriz, ped, vehiculo, otra, atomo], globales)"""
    seed = c['seed']
    A = lambda k, win: Arena(win, (seed * 13 + k) & M32)
    h, mat, ped, veh, oth, atom = A(1, 0xA18), A(2, 0x48), A(3, 0x40), A(4, 0x40), A(5, 0x40), A(6, 0x20)
    h.put32(0x14, 0)   # el puntero a la matriz lo pone el banco (no cambia)
    h.put32(0x1C, c['ent1c']); h.m[h.guard + 0x22:h.guard + 0x24] = (c['model'] & 0xFFFF).to_bytes(2, 'little'); h.put8(0x36, c['status']); h.put32(0x40, c['phys']); h.put8(0x3BA, c['mission'])
    h.put32(0x428, c['flags428']); h.put8(0x42A, c['flags42a']); h.put8(0x42D, c['flags42d']); h.put8(0x4A8, c['b4a8']); h.put8(0x868, c['b868'])
    for a in (h, mat, ped, veh, oth, atom):
        a.snap()
    g = dict(time=c['time'], news=c['news'] & 0xFF, ambulances=c['ambulances'], trucks=c['trucks'], havoc=[c['havoc0'], c['havoc1']], chase=[c['chase0'], c['chase1']])
    ev = []
    heli_t = tag(0, 0)
    model = c['model'] & 0xFFFF
    rc = model in (0x1D1, 0x1F5)
    done = lambda: (ev, [h.changes(), mat.changes(), ped.changes(), veh.changes(), oth.changes(), atom.changes()], g)
    if not (h.get8(0x42A) & 0x20):
        return done()
    status = h.get8(0x36) & 0xF8
    if status != 0 and not (h.get8(0x3BA) == 0x3A or rc):
        h.put8(0x3BA, 0x3A)
        h.put32(0x4C0, 0)
        return done()
    # causante
    damager = {0: 0, 1: tag(K_ARENA['ped'], 0), 2: tag(K_ARENA['veh'], 0), 3: tag(K_ARENA['other'], 0), 4: c['dmg_raw'] & M32, 5: heli_t}[c['dmg_kind']]
    ped_ret = ptr_of(c['fpp_kind'], c['fpp_raw'], 'ped')
    ev.append([1, 0xFFFFFFFF])
    is_player = damager == ped_ret
    if not is_player:
        ev.append([2, 0xFFFFFFFF, 0])
        is_player = damager == ptr_of(c['fpv_kind'], c['fpv_raw'], 'veh')
    if is_player:
        i = c['focus']
        g['havoc'][i] = (g['havoc'][i] + 20) & M32
        g['chase'][i] = store(add(load(g['chase'][i]), load(B_10)))
        ev.append([3])
        r = c['rnd'] & M32
        r = r - (1 << 32) if r >= 1 << 31 else r
        rem = abs(r) % 6000 * (1 if r >= 0 else -1)   # idiv: el resto toma el signo del dividendo
        ev.append([4, 0x7D, store(load_int(rem + 4000))])
    if model == 0x1E8:
        g['news'] = 0
    if status == 0:
        h.put32(0x1C, h.get32(0x1C) & 0xFFFFFF7E)
        for off in (0x44, 0x48, 0x4C, 0x50, 0x54, 0x58):
            h.put32(off, 0)
    h.put8(0x36, (h.get8(0x36) & 7) | 0x28)
    h.put32(0x40, h.get32(0x40) | 0x20000000)
    h.put32(0x4D8, c['time'])
    ev.append([5, h.get32(0x18), 0x4000])
    ev.append([6, tag(0, 0x5A0), 0])
    if not rc:
        ev.append([7, heli_t, 5, 0]); ev.append([7, heli_t, 6, 0])
        for d in range(6):
            ev.append([8, heli_t, d, 0])
        ev.append([9, heli_t, 5, 1])
        ev.append([10, h.get32(0x65C), 0xCB000001, 0xF00D0001])
        if c['atomic']:
            atom.put8(2, 0)
    h.put8(0x4A8, h.get8(0x4A8) & 0xF8)
    h.put32(0x4C0, 0)
    h.m[h.guard + 0x4DE:h.guard + 0x4E0] = b'\x00\x00'
    pos = [c['pos_x'], c['pos_y'], c['pos_z']]
    ev.append([11, CAMERA, B_04] + pos)
    ev.append([12, heli_t])
    h.put8(0x4A8, h.get8(0x4A8) & 0xE7)
    h.put8(0x428, h.get8(0x428) & 0xAF)
    h.put8(0x42D, h.get8(0x42D) & 0x7F)
    h.put8(0x868, h.get8(0x868) & 0xFE)
    if h.get8(0x428) & 2:
        h.put8(0x428, h.get8(0x428) & 0xFD)
        g['ambulances'] = (g['ambulances'] - 1) & M32
    if h.get8(0x428) & 4:
        h.put8(0x428, h.get8(0x428) & 0xFB)
        g['trucks'] = (g['trucks'] - 1) & M32
    ev.append([13, heli_t, 0])
    ev.append([14, FIRE_MANAGER, heli_t, damager, B_08, 1, 7000, 0])
    ev.append([15, heli_t, 0])
    ev.append([16, heli_t, damager, 12 if rc else 7] + pos + [0, 1, 0xBF800000, 0])
    LAST.clear()
    LAST.update(is_player=is_player, rc=rc, status=status, damager=damager, ped_ret=ped_ret)
    return done()


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, changes, g = expected(c)
    bad = []
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        got = o['events']
        if len(got) != len(ev):
            bad.append((side, 'numero de eventos %d != %d' % (len(got), len(ev))))
        else:
            for x, w in zip(got, ev):
                if len(x) != len(w) or x[0] != w[0] or any(not same_float(gv, wv) if wv is None else gv != wv for gv, wv in zip(x[1:], w[1:])):
                    bad.append((side, 'evento %s != %s' % (x, w)))
                    break
        if o['changes'] != changes:
            bad.append((side, 'cambios %s != %s' % (str(o['changes'])[:300], str(changes)[:300])))
        og = o['globals']
        for key in ('time', 'news', 'ambulances', 'trucks', 'havoc'):
            if og[key] != g[key]:
                bad.append((side, 'global %s %s != %s' % (key, og[key], g[key])))
        for i in range(2):
            if not same_float(og['chase'][i], g['chase'][i]):
                bad.append((side, 'global chase[%d] %s != %s' % (i, og['chase'][i], g['chase'][i])))
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
