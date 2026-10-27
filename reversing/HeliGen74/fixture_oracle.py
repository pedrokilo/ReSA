"""Oraculo independiente (Python) de HeliGen74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6C6520..0x6C6882 (EXE 1.0 US), lo que debe observarse
(traza de colaboradores EN ORDEN, posicion de salida del helicoptero, matriz y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). fsin/fcos trabajan con 64 bits de mantisa (serie de Taylor de 90 digitos redondeada a 64 bits). Los NaN solo se comparan como «es NaN»
(la carga depende de la forma de la instruccion); todo lo demas, bit a bit. La memoria de las arenas se reconstruye con el mismo relleno que el banco (Mix/Rand de Fixtures.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 helicoptero, 1 objetivo, 2 matriz del helicoptero, 3 matriz del objetivo.
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF

# ---------------------------------------------------------------- arenas: mismo relleno que el banco (Mix/Rand/FillWords de Fixtures.hpp)
SALT = 74175  # lo fija cada oraculo


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

# ================================================================================================ GenerateHeli (0x6C6520)
from decimal import Decimal, getcontext

getcontext().prec = 90
K, R250, A90, A94, PI = 0x3CC8F5C3, 0x437A0000, 0xC53B8000, 0x453B8000, 0x40490FDB
C50, CHX, CHY, CR, C200, C560, C20 = 0x42480000, 0xC5112000, 0xC4CEA000, 0x43AF0000, 0x43480000, 0x440C0000, 0x41A00000
BC_FLOAT = 0x3E28F5C3
ONE = 0x3F800000
TAG = {'heli': 0, 'tgt': 1, 'matH': 2, 'matT': 3}
LAST = {}


def rndp(v, p):
    """redondea al par a p bits de mantisa (exponente ilimitado)"""
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


def trig(x):
    """(sin, cos) de un Fraction con 90 digitos (serie de Taylor); fsin/fcos de la x87 trabajan con 64 bits de mantisa"""
    xd = Decimal(x.numerator) / Decimal(x.denominator)
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
        term = term * xd / n
        if abs(term) < Decimal(10) ** -85 and n > 10:
            break
    return s, c


def ext(d):
    """un Decimal como valor extendido de la x87 (64 bits de mantisa)"""
    f = Fraction(d)
    return ('f', rndp(f, 64), 1 if f < 0 else 0) if f != 0 else ('f', Fraction(0), 0)


def fsincos(a):
    """fsin / fcos de un valor cargado (a24 bits): devuelve (seno, coseno) extendidos; NaN/inf dan NaN"""
    if a == NAN or a[0] == 'i':
        return NAN, NAN
    s, c = trig(a[1])
    return ext(s), ext(c)


def pos_xy(base_bits, angle_bits):
    """px = float(cos(angle) * 250 + base.x), py = float(sin(angle) * 250 + base.y): bits (None = NaN)"""
    sn, cs = fsincos(load(angle_bits))
    r = load(R250)
    return store(add(mul(cs, r), load(base_bits[0]))), store(add(mul(sn, r), load(base_bits[1])))


def expected(c):
    """(eventos, cambios [heli, objetivo, matriz heli, matriz objetivo], puntero del objetivo etiquetado)"""
    seed = c['seed']
    heli = Arena(0xA18, (seed * 13 + 1) & M32)
    tgt = Arena(0x60, (seed * 13 + 2) & M32)
    mat = Arena(0x48, (seed * 13 + 3) & M32)
    matT = Arena(0x48, (seed * 13 + 4) & M32)
    heli.put32(0x14, 0)   # el puntero de la matriz lo pone el banco: no cambia
    heli.snap(); tgt.snap(); mat.snap(); matT.snap()
    base = [c['base_x'], c['base_y'], c['base_z']]
    ev = [[1, 0xA18], [2, tag(0, 0), 0x1E8 if c['news'] & 0xFF else 0x1F1, 4], [3]]
    angle = store(mul(load_int(c['rnd'] & 0xFF), load(K)))
    px, py = pos_xy(base, angle)
    out = (lt(reload(px), load(A90)) or gt(reload(px), load(A94)) or lt(reload(py), load(A90)) or gt(reload(py), load(A94)))
    if out:
        angle = store(add(load(angle), load(PI)))
        px, py = pos_xy(base, angle)
    d1 = sub(reload(px), load(CHX))
    d2 = sub(reload(py), load(CHY))
    near = lt(sqrt(add(mul(d2, d2), mul(d1, d1))), load(CR))
    cand_bits = store(add(load(base[2]), load(C50)))
    if near:
        x, y = base[0], base[1]
        lifted = add(load(base[2]), load(C200))
        cand_bits = store(lifted) if gt(lifted, load(C560)) else C560
    else:
        x, y = px, py
    ev.append([4, x, y])
    low = add(load(c['gz']), load(C20))
    zc = reload(cand_bits)
    z = cand_bits if gt(zc, low) else store(low)
    ev.append([5, tag(2, 0), x, y, z])
    if c['news'] & 0xFF:
        ev.append([6, 0xFFFFFFFF])
    ev.append([7, tag(0, 0)])
    ev.append([8, tag(0, 0), 0xFFFFFFFF, 0, 0])
    # memoria del helicoptero: bandera +0x428 |= 8, estado +0x36, mision +0x3BA, crucero +0x3D0, rotor +0x84C, alturas +0x9A8 y +0x9B0
    heli.put8(0x428, heli.get8(0x428) | 8)
    heli.put8(0x36, (heli.get8(0x36) & 7) | 0x18)
    news = bool(c['news'] & 0xFF)
    heli.put8(0x3BA, 0x29 if news else 0x28)
    heli.put8(0x3D0, 0x23 if news else 0x46)
    heli.put32(0x84C, BC_FLOAT)
    heli.put32(0x9A8, 0x41F00000 if news else 0x41A00000)
    heli.put32(0x9B0, 0x41D80000 if news else 0x41400000)
    # orientacion: rumbo = float(angulo + pi); right = (sin, -cos, 0), forward = (cos, sin, 0), up = (0, 0, 1)
    heading = store(add(load(angle), load(PI)))
    sn, cs = fsincos(load(heading))
    sin_b, cos_b, ncos_b = store(sn), store(cs), store(fchs(cs))
    for off, v in ((0, sin_b), (4, ncos_b), (8, 0), (0x10, cos_b), (0x14, sin_b), (0x18, 0), (0x20, 0), (0x24, 0), (0x28, ONE)):
        mat.put32(off, v if v is not None else 0x7FC00000)
    LAST.clear()
    LAST.update(angle=angle, px=px, py=py, out=out, near=near, cand=zc, cand_bits=cand_bits, z=z, x=x, y=y, low=low, d1=d1, d2=d2)
    changes = [heli.changes(), tgt.changes(), mat.changes(), matT.changes()]
    return ev, changes, tag(1, 0), (cos_b, sin_b)


def check_row(row):
    """compara ambos lados de una fila con el modelo; devuelve (lista de motivos, None)"""
    c = row['input']
    ev, changes, target_tag, trig_bits = expected(c)
    bad = []
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        if o['ret'] != tag(0, 0):
            bad.append((side, 'resultado %x' % o['ret']))
        if o['target_entity'] != target_tag:
            bad.append((side, 'objetivo'))
        got = o['events']
        if len(got) != len(ev):
            bad.append((side, 'numero de eventos %d != %d' % (len(got), len(ev))))
        else:
            for g, w in zip(got, ev):
                if len(g) != len(w) or g[0] != w[0] or any(not same_float(gv, wv) if wv is None else gv != wv for gv, wv in zip(g[1:], w[1:])):
                    bad.append((side, 'evento %s != %s' % (g, w)))
                    break
        # la matriz: los componentes NaN no se pueden predecir (nunca ocurre: el rumbo es finito)
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
