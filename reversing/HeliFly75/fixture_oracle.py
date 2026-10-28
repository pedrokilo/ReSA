"""Oraculo independiente (Python) de HeliFly75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C4E60..0x6C541A (EXE 1.0 US), lo que debe observarse
(traza de colaboradores EN ORDEN y bytes cambiados de las arenas: rotor, byte +0xB8, motor y el giro de referencia +0x958) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: 24 bits de mantisa (redondeo al par, exponente sin limite) y guardado como float. Los NaN solo se comparan como «es NaN».
La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp), asi que los bytes cambiados se comparan exactamente.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 helicoptero, 1 matriz, 2 matriz de la camara, 3 marco del nodo, 4 atomo, 5 manejo.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 75276  # lo fija cada oraculo


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


# ================================================================================================ ProcessFlyingCarStuff (0x6C4E60)
LAST = {}
K_ARENA = {'heli': 0, 'mat': 1, 'cam': 2, 'frame': 3, 'atom': 4, 'hand': 5}
STEP_ADDR = 0xB7CB5C
C = {  # constantes del EXE (patrones de float)
    'k8717B0': 0x3A102DE0, 'k8717AC': 0x3E6147AE, 'k858FCC': 0x3E19999A, 'k8717A8': 0x3C75C290, 'k8717A4': 0x3BF5C290, 'k866FBC': 0x40D55555, 'one': 0x3F800000,
    'k8717A0': 0x3E1DB22D, 'k87179C': 0x3B902DE0, 'k85A700': 0x43C80000, 'k858F20': 0x3F060A92, 'k858C58': 0x3C23D70A, 'k858EF0': 0x3F733333, 'k858B38': 0x3CA3D70A,
    'k858B1C': 0x3DCCCCCD, 'k858CDC': 0x3A83126F, 'k859CD8': 0x3B449BA6, 'half': 0x3F000000, 'k_f666': 0x3F666666, 'k_f4ccc': 0x3F4CCCCD,
}


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


def fabs(a):
    if a == NAN:
        return a
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def f_lt(bits_a, bits_b):
    return lt(load(bits_a), load(bits_b))


def f_gt(bits_a, bits_b):
    return gt(load(bits_a), load(bits_b))


def expected(c):
    """(eventos, cambios [helicoptero, matriz, camara, marco, atomico, manejo], globales)"""
    seed = c['seed']
    LAST.clear()
    A = lambda k, win: Arena(win, (seed * 13 + k) & M32)
    h, mat, cam, frame, atom, hand = A(1, 0xA18), A(2, 0x48), A(3, 0x48), A(4, 0x60), A(5, 0x60), A(6, 0x100)
    h.put32(0x14, 0)   # el puntero a la matriz lo pone el banco (no cambia respecto a la imagen de referencia)
    h.put32(0x384, 0); h.put32(0x678, 0)
    if c['mat_present']:
        for i in range(3):
            mat.put32(0x30 + 4 * i, c['pos%d' % i])
    else:
        for i in range(3):
            h.put32(4 + 4 * i, c['pos%d' % i])
    for i in range(12):
        if i % 4 != 3:
            mat.put32(4 * i, c['mat%d' % i])
    h.put8(0x36, c['status']); h.m[h.guard + 0x22:h.guard + 0x24] = (c['model'] & 0xFFFF).to_bytes(2, 'little'); h.put32(0x40, c['flags40'])
    h.put8(0x428, c['b428']); h.put8(0x42C, c['b42c']); h.put8(0x960, c['b960']); h.put8(0xB8, c['bb8'])
    hand.put32(0xCC, c['hand_flags'])
    h.put32(0x84C, c['rotor'])
    for i in range(4):
        h.put32(0x98C + 4 * i, c['ctl%d' % i])
    for i in range(3):
        h.put32(0x44 + 4 * i, c['vel%d' % i])
    h.put32(0x958, c['prop']); h.put32(0x82C, c['wheel_rot'])
    for i in range(16):
        frame.put32(0x10 + 4 * i, c['frame%d' % i])
    atom.put8(0x4C, c['atom_flags']); atom.put32(0x28, c['radius'])
    if c['cam_mat']:
        for i in range(3):
            cam.put32(0x30 + 4 * i, c['cam%d' % i])
    # El banco puntea los punteros en el helicoptero DESPUES de snap? No: Put32 de punteros se hace antes de Snap, asi que no son cambios. Aqui se ponen igual que el banco (antes de la imagen de referencia).
    h.put32(0x14, 0xAAAAAAAA); h.put32(0x384, 0xAAAAAAAA); h.put32(0x678, 0xAAAAAAAA)
    for a in (h, mat, cam, frame, atom, hand):
        a.snap()
    ev = []
    done = lambda: (ev, [h.changes(), mat.changes(), cam.changes(), frame.changes(), atom.changes(), hand.changes()],
                    dict(step=c['step'], cam_ptr=(tag(K_ARENA['cam'], 0) if c['cam_mat'] else 0)))
    heli_t = tag(0, 0)
    status = c['status'] >> 3
    model = c['model'] & 0xFFFF
    rotor = h.get32(0x84C)
    step = c['step']
    rc = model in (0x1D1, 0x1F5)

    def set_rotor(bits):
        nonlocal rotor
        rotor = bits
        h.put32(0x84C, bits)

    if status in (0, 8, 3):
        if f_lt(rotor, C['k8717AC']):
            if not (c['flags40'] & 0x100):
                set_rotor(store(add(load(rotor), load(C['k859CD8'] if rc else C['k858CDC']))))
        if f_gt(rotor, C['k858FCC']):
            special = bool(c['flags40'] & 0x8000000) and model in (0x1BF, 0x1A1)
            ctl = [c['ctl%d' % i] for i in range(4)]
            if c['b42c'] & 0x80:
                ev.append([1, heli_t, 2] + ctl)
            else:
                vx = [c['vel%d' % i] for i in range(3)]
                call = (c['b960'] < 4 and not special) or f_gt(c['ctl3'], 0)
                for v in vx:
                    call = call or gt(fabs(load(v)), load(C['k858B38']))
                if call:
                    ev.append([1, heli_t, 6] + ctl)
    else:
        if not (c['hand_flags'] & 0x2000000):
            return done()
        h.put8(0x428, h.get8(0x428) & 0xEF)
        f = mul(load(step), load(C['k8717B0']))
        if lt(f, load(rotor)):
            h.put8(0xB8, 0)
            set_rotor(store(sub(load(rotor), f)))
        else:
            set_rotor(0)

    frame_t = tag(K_ARENA['frame'], 0)
    mat_t = tag(K_ARENA['mat'], 0) if c['mat_present'] else 0
    pos = [c['pos0'], c['pos1'], c['pos2']]
    if status in (0, 8, 3) and f_gt(rotor, C['k8717A8']) and c['node_present']:
        ev.append([2, 0xF00D0001, tag(K_ARENA['frame'], 0x10), 0])
        ev.append([3, frame_t, 0xCB000001, 0xF00D0001])
        radius = c['radius']
        if c['atomic']:
            if c['atom_flags'] & 2:
                ev.append([4, tag(K_ARENA['atom'], 0)])
                radius = c['radius_after']
                atom.put32(0x28, c['radius_after'])
            if f_gt(radius, C['k858B1C']):
                scale = {0x1D1: C['k_f666'], 0x1F5: C['k_f666'], 0x1D5: C['k_f4ccc'], 0x1BF: C['k_f4ccc'], 0x1A9: C['half']}.get(model, C['one'])
                if status in (0, 8):
                    ev.append([5, heli_t, c['frame12'], c['frame13'], c['frame14'], mat_t, 0xFFFD, radius, scale])
        if status in (0, 3) and f_gt(rotor, C['k8717A4']):
            t = mul(load(rotor), load(C['k866FBC']))
            power = C['one'] if lt(load(C['one']), t) else store(t)
            ev.append([6] + pos + [1, power])

    # 0x6C5200
    if rc:
        return done()
    if not (f_lt(rotor, C['k8717A0']) and f_gt(rotor, C['k87179C'])):
        return done()
    if not c['node_present']:
        return done()
    cp = [c['cam0'], c['cam1'], c['cam2']]
    dz = sub(load(cp[2]), load(pos[2])); dzs = store(dz)
    dy = sub(load(cp[1]), load(pos[1])); dys = store(dy)
    dx = sub(load(cp[0]), load(pos[0])); dxs = store(dx)
    xx = mul(dx, reload(dxs)); yy = mul(reload(dys), reload(dys)); zz = mul(reload(dzs), reload(dzs))
    d2 = add(add(xx, yy), zz)
    d2s = store(d2)
    LAST.update(status=status, tail=True, reached=False, near=lt(d2, load(C['k85A700'])), diff=False, fire=False)
    if not LAST['near']:
        return done()
    LAST['diff'] = gt(fabs(sub(load(c['prop']), load(c['wheel_rot']))), load(C['k858F20']))
    if not LAST['diff']:
        return done()
    if not c['mat_present']:
        raise RuntimeError('caso invalido: llega a Multiply3x3 sin matriz del helicoptero')
    ev.append([2, 0xF00D0001, tag(K_ARENA['frame'], 0x10), 0])
    m = [load(c['mat%d' % i]) if i % 4 != 3 else None for i in range(12)]
    ax = [load(c['frame0']), load(c['frame1']), load(c['frame2'])]   # vector derecho del nodo
    up, fw, rt = (m[8], m[9], m[10]), (m[4], m[5], m[6]), (m[0], m[1], m[2])
    vx, vy, vz = ax
    rx = store(add(add(mul(up[0], vz), mul(fw[0], vy)), mul(rt[0], vx)))
    ry = store(add(add(mul(up[1], vz), mul(rt[1], vx)), mul(fw[1], vy)))
    rz = store(add(add(mul(up[2], vz), mul(rt[2], vx)), mul(fw[2], vy)))
    root = sqrt(reload(d2s))
    dd = load(C['k858C58']) if gt(load(C['k858C58']), root) else root
    inv = div(load(C['one']), dd)
    X = mul(reload(dxs), inv)
    Y = mul(inv, reload(dys))
    Z = mul(inv, reload(dzs))
    dot = add(add(mul(reload(rz), Z), mul(reload(ry), Y)), mul(reload(rx), X))
    fire = gt(fabs(dot), load(C['k858EF0']))
    LAST.update(reached=True, fire=fire, dot=dot)
    if fire:
        ev.append([7, tag(0, 0x138), 0x5E, 0])
        h.put32(0x958, h.get32(0x82C))
    return done()


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    try:
        ev, changes, g = expected(c)
    except RuntimeError as e:
        return [('modelo', str(e))], None
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
        if og['step'] != g['step']:
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
