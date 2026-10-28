"""Oraculo independiente (Python) de HeliPre75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C5420..0x6C58D3 (EXE 1.0 US), lo que debe observarse
(traza de colaboradores EN ORDEN y bytes cambiados de las arenas: posiciones de las ruedas, giros de los rotores y las matrices de las cuatro paletas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: 24 bits de mantisa (redondeo al par, exponente sin limite) y guardado como float. Los NaN solo se comparan como «es NaN» (la carga la fijan el EXE y la DLL, que se comparan entre si).
La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp), asi que los bytes cambiados se comparan exactamente.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 helicoptero, 1 matriz, 2 manejo, 3..6 marcos de los nodos 12..15.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 75277  # lo fija cada oraculo


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


# ================================================================================================ PreRender (0x6C5420)
LAST = {}
K_ARENA = {'heli': 0, 'mat': 1, 'hand': 2, 'frame0': 3, 'frame1': 4, 'frame2': 5, 'frame3': 6}
STEP_ADDR = 0xB7CB5C
DUST = 0x3FD47AE1  # 1.66f en 0x8D33A0
K = {'one': 0x3F800000, 'zero': 0, 'k75': 0x3F400000, 'neg2pi': 0xC0C90FDB, 'pos2pi': 0x40C90FDB, 'k23': 0x40133333}
NODES = (0x678, 0x67C, 0x680, 0x684)
MARKER = 0xA0D00001


def div(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else ('i', a[1] ^ (b[2] if b[0] == 'f' else 0))
    if b[0] == 'i':
        return ('f', Fraction(0), a[2] ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', a[2] ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def quiet(bits):
    return bits | 0x400000 if is_nan(bits) else bits


def f_gt(a, b):
    return gt(load(a), load(b))


def f_lt(a, b):
    return lt(load(a), load(b))


def expected(c):
    """(eventos, cambios [helicoptero, matriz, manejo, marco0..3], globales, rangos de float con NaN por arena)"""
    seed = c['seed']
    LAST.clear()
    A = lambda k, win: Arena(win, (seed * 13 + k) & M32)
    h, mat, hand = A(1, 0xA18), A(2, 0x48), A(3, 0x100)
    frames = [A(4 + k, 0x60) for k in range(4)]
    h.put32(0, 0xAAAAAAAA); h.put32(0x14, 0xAAAAAAAA); h.put32(0x384, 0xAAAAAAAA)   # vtable, matriz y manejo: punteros del banco (no cambian)
    h.m[h.guard + 0x22:h.guard + 0x24] = (c['model'] & 0xFFFF).to_bytes(2, 'little'); h.put32(0x40, c['flags40']); h.put32(0x38C, c['flags38c']); h.put8(0x42B, c['b42b'])
    hand.put32(0xB8, c['hand_b8'])
    h.put8(0xA11, c['a11']); h.put32(0xA00, c['intens'])
    for i in range(3):
        h.put32(0x9F4 + 4 * i, c['tgt%d' % i])
    for i in range(4):
        h.put32(0x7D4 + 4 * i, c['comp%d' % i]); h.put32(0x878 + 4 * i, c['spr%d' % i]); h.put32(0x888 + 4 * i, c['line%d' % i]); h.put32(0x838 + 4 * i, c['wp%d' % i])
    h.put32(0x84C, c['rotor']); h.put32(0x9A0, c['a0']); h.put32(0x9A4, c['a1'])
    for k in range(4):
        h.put32(NODES[k], 0xAAAAAAAA)
        for i in range(16):
            frames[k].put32(0x10 + 4 * i, c['frame%d_%d' % (k, i)])
    for a in [h, mat, hand] + frames:
        a.snap()
    wild = {i: [] for i in range(7)}   # arena -> desplazamientos (respecto al objeto) de floats cuyo resultado es NaN (se comparan como «es NaN»)
    ev = []
    arenas = [h, mat, hand] + frames
    done = lambda: (ev, [a.changes() for a in arenas], dict(step=c['step']), wild)
    heli_t = tag(0, 0)
    model = c['model'] & 0xFFFF
    mat_t = tag(K_ARENA['mat'], 0) if c['mat_present'] else 0
    ev.append([1, heli_t])

    def store_f(arena_idx, arena, off, value):
        """guarda un valor x87 como float en la arena; si es NaN deja una marca y anota el rango"""
        bits = store(value)
        if bits is None:
            arena.put32(off, 0x7FC00000)
            wild[arena_idx].append(off)
        else:
            arena.put32(off, bits)
        return bits

    # foco de busqueda
    if (c['a11'] & 0xFF) != 0 and f_gt(c['intens'], 0):
        ev.append([2, 19, 6])
        if (c['time_ret'] & 0xFF) != 0:
            ev.append([3, mat_t, 0, 0x40600000, 0xBE99999A])
            ev.append([4, c['origin0'], c['origin1'], c['origin2'], c['tgt0'], quiet(c['tgt1']), quiet(c['tgt2']), 0x41A00000, c['intens'], tag(0, 0xB), 1, 1])
    ev.append([5, heli_t])
    if c['b42b'] & 1:
        ev.append([6, heli_t])
        direct = bool(c['flags40'] & 0x4000000) and bool(c['flags38c'] & 0x20000)
        for i in range(4):
            r = div(load(c['spr%d' % i]), load(c['line%d' % i]))
            q = sub(load(K['one']), r)
            nn = sub(load(c['comp%d' % i]), q)
            dd = sub(load(K['one']), q)
            ratio_bits = store(div(nn, dd))
            ev.append([7, MARKER, i, 1])
            ratio = reload(ratio_bits)
            t = add(load(c['wheel%d_2' % i]), load(c['hand_b8']))
            if gt(ratio, load(K['zero'])):
                t = sub(t, mul(ratio, load(c['spr%d' % i])))
            wp = load(c['wp%d' % i])
            if gt(t, wp) or direct:
                store_f(0, h, 0x838 + 4 * i, t)
            else:
                store_f(0, h, 0x838 + 4 * i, add(mul(sub(t, wp), load(K['k75'])), wp))
    for node in (4, 7, 2, 5):
        ev.append([8, heli_t, node, 1])
    if model not in (0x1D1, 0x1F5):
        ev.append([9, heli_t, 0x3F800000, 0x3F800000])
    heavy = model in (0x1D5, 0x1BF, 0x1E7, 0x1E8, 0x1F1)
    step = load(c['step'])
    rotor = load(c['rotor'])
    tmp = mul(step, rotor)
    if heavy:
        tmp = mul(tmp, load(DUST))
    a0 = store(sub(load(c['a0']), tmp))
    a0_nan = a0 is None
    if a0_nan:
        h.put32(0x9A0, 0x7FC00000); wild[0].append(0x9A0)
    else:
        h.put32(0x9A0, a0)
        if lt(load(a0), load(K['neg2pi'])):
            v = load(a0)
            while True:
                v = add(v, load(K['pos2pi']))
                if not lt(v, load(K['neg2pi'])):
                    break
            a0 = store(v)
            h.put32(0x9A0, a0)
    tmp = mul(step, rotor)
    tmp = add(tmp, tmp) if model == 0x1A1 else mul(tmp, load(K['k23']))
    a1 = store(sub(load(c['a1']), tmp))
    a1_nan = a1 is None
    if a1_nan:
        h.put32(0x9A4, 0x7FC00000); wild[0].append(0x9A4)
    else:
        h.put32(0x9A4, a1)
        if gt(load(a1), load(K['pos2pi'])):
            v = load(a1)
            while True:
                v = sub(v, load(K['pos2pi']))
                if not gt(v, load(K['pos2pi'])):
                    break
            a1 = store(v)
            h.put32(0x9A4, a1)
    for k in range(4):
        if not c['node%d' % k]:
            continue
        fr = frames[k]
        fi = 3 + k
        ev.append([10, 0xF00D0001, tag(fi, 0x10), 0])
        saved = [c['frame%d_%d' % (k, 12 + j)] for j in range(3)]
        z_type = k < 2
        ang, ang_nan = (a0, a0_nan) if z_type else (a1, a1_nan)
        angle = None if ang_nan else ang
        ev.append([11 if z_type else 12, 0xF00D0001, angle])
        pos = []
        for j in range(3):
            if is_nan(saved[j]):
                pos.append(quiet(saved[j]))
            else:
                pos.append(store(add(load(0), load(saved[j]))))
        ev.append([13, 0xF00D0001, tag(fi, 0x10)])
        rot = {0: angle, 1: 0, 2: 0, 4: 0, 5: angle, 6: 0, 8: 0, 9: 0, 10: angle} if z_type else {0: 0x3F800000, 1: 0, 2: 0, 4: 0, 5: angle, 6: angle, 8: 0, 9: angle, 10: angle}
        for w, val in rot.items():
            if val is None:
                fr.put32(0x10 + 4 * w, 0x7FC00000); wild[fi].append(0x10 + 4 * w)
            else:
                fr.put32(0x10 + 4 * w, val)
        for j in range(3):
            fr.put32(0x10 + 4 * (12 + j), pos[j])
    ev.append([14, heli_t, 2])
    return done()


def strip(changes, ranges):
    """quita de la lista de cambios [[desplazamiento, byte]] los bytes de los rangos de 4 de `ranges`"""
    bad = set()
    for r in ranges:
        for b in range(4):
            bad.add(r + b)
    return [x for x in changes if x[0] not in bad]


def observed_word(arena_ref, changes, off, guard=0x40):
    """valor de la palabra en `off` tras aplicar los cambios observados sobre la imagen de referencia"""
    m = bytearray(arena_ref)
    for o, b in changes:
        m[guard + o] = b
    return int.from_bytes(m[guard + off:guard + off + 4], 'little')


def reference_images(c):
    """imagenes de referencia de las arenas (antes de la llamada) para reconstruir lo observado"""
    seed = c['seed']
    A = lambda k, win: Arena(win, (seed * 13 + k) & M32)
    h, mat, hand = A(1, 0xA18), A(2, 0x48), A(3, 0x100)
    frames = [A(4 + k, 0x60) for k in range(4)]
    h.put32(0, 0xAAAAAAAA); h.put32(0x14, 0xAAAAAAAA); h.put32(0x384, 0xAAAAAAAA)
    h.m[h.guard + 0x22:h.guard + 0x24] = (c['model'] & 0xFFFF).to_bytes(2, 'little'); h.put32(0x40, c['flags40']); h.put32(0x38C, c['flags38c']); h.put8(0x42B, c['b42b'])
    hand.put32(0xB8, c['hand_b8'])
    h.put8(0xA11, c['a11']); h.put32(0xA00, c['intens'])
    for i in range(3):
        h.put32(0x9F4 + 4 * i, c['tgt%d' % i])
    for i in range(4):
        h.put32(0x7D4 + 4 * i, c['comp%d' % i]); h.put32(0x878 + 4 * i, c['spr%d' % i]); h.put32(0x888 + 4 * i, c['line%d' % i]); h.put32(0x838 + 4 * i, c['wp%d' % i])
    h.put32(0x84C, c['rotor']); h.put32(0x9A0, c['a0']); h.put32(0x9A4, c['a1'])
    for k in range(4):
        h.put32(NODES[k], 0xAAAAAAAA)
        for i in range(16):
            frames[k].put32(0x10 + 4 * i, c['frame%d_%d' % (k, i)])
    return [bytes(a.m) for a in [h, mat, hand] + frames]


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, changes, g, wild = expected(c)
    refs = reference_images(c)
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
        obs = o['changes']
        if len(obs) != len(changes):
            bad.append((side, 'numero de arenas'))
            continue
        for idx in range(len(changes)):
            if strip(obs[idx], wild[idx]) != strip(changes[idx], wild[idx]):
                bad.append((side, 'cambios de la arena %d %s != %s' % (idx, str(obs[idx])[:300], str(changes[idx])[:300])))
            for off in wild[idx]:
                if not is_nan(observed_word(refs[idx], obs[idx], off)):
                    bad.append((side, 'se esperaba NaN en arena %d +0x%X' % (idx, off)))
        if o['globals']['step'] != g['step']:
            bad.append((side, 'global step'))
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
