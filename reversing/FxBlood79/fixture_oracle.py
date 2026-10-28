"""Oraculo independiente (Python) de FxBlood79: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de los colaboradores,
sorteos consumidos, contador de manchas y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Desensamblado leido: Fx_c::AddBlood 0x49EB00..0x49EE02.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida); un local de pila, 0x20000000.
Indices: 0 posicion, 1 direccion, 2 matriz de la camara, 3 sistema A, 4 sistema B, 5 objeto this.
"""
import json
import struct
import sys
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
STACK = 0x20000000
SYS_A, SYS_B = 3, 4


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


# ---- rand() REAL del CRT del EXE (0x821B1E): seed = seed * 0x343FD + 0x269EC3 (mod 2^32), devuelve (seed >> 16) & 0x7FFF; la semilla es el estado inicial `rand_seed` ----
LCG_A, LCG_C = 0x343FD, 0x269EC3


class Rng:
    def __init__(self, seed):
        self.state = seed

    def draw(self):
        self.state = (self.state * LCG_A + LCG_C) & M32
        return (self.state >> 16) & 0x7FFF


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


class V:
    """valor de la x87: kind 'f' (finito: (-1)^s * m * 2^e con m entero; m == 0 es un cero con signo s), 'inf' (s) o 'nan' (bits ya silenciado)"""
    __slots__ = ('kind', 's', 'm', 'e', 'bits', 'snan')

    def __init__(self, kind, s=0, m=0, e=0, bits=0, snan=False):
        self.kind, self.s, self.m, self.e, self.bits, self.snan = kind, s, m, e, bits, snan


INDEFINITE = 0xFFC00000


def from_bits(b):
    if is_nan(b):
        return V('nan', bits=b | 0x400000)       # fld dword silencia el NaN senalizante
    if is_inf(b):
        return V('inf', s=b >> 31)
    ex = (b >> 23) & 0xFF
    man = b & 0x7FFFFF
    if ex == 0:
        return V('f', b >> 31, man, -149)
    return V('f', b >> 31, man | 0x800000, ex - 150)


def mem(b):
    """operando de memoria de una instruccion aritmetica (fadd m32...): un NaN de senal conserva su condicion hasta que la operacion lo silencia"""
    if is_nan(b):
        return V('nan', bits=b, snan=(b & 0x400000) == 0)
    return from_bits(b)


def rnd(s, m, e, sticky=False, bits=24):
    """redondea m * 2^e (mas algo positivo si sticky) a `bits` bits de mantisa, mitad al par; el exponente no tiene limite"""
    if m == 0:
        return V('f', s, 0, 0)
    bl = m.bit_length()
    if bl <= bits:
        return V('f', s, m, e)
    sh = bl - bits
    q = m >> sh
    rem = m & ((1 << sh) - 1)
    half = 1 << (sh - 1)
    if rem > half or (rem == half and (sticky or (q & 1))):
        q += 1
        if q == 1 << bits:
            q >>= 1
            sh += 1
    return V('f', s, q, e + sh)


def r24(x):
    """redondeo a 24 bits con exponente extendido"""
    if x.kind != 'f' or x.m == 0:
        return x
    return rnd(x.s, x.m, x.e)


def to_f32_bits(x):
    if x.kind == 'nan':
        return x.bits
    if x.kind == 'inf':
        return 0xFF800000 if x.s else 0x7F800000
    sign = 0x80000000 if x.s else 0
    if x.m == 0:
        return sign
    bl = x.m.bit_length()
    top = bl - 1 + x.e          # exponente sin sesgo del bit mas alto
    if top < -126:
        # desnormal: multiplo de 2^-149
        sh = -(x.e + 149)
        if sh <= 0:
            n = x.m << -sh
        else:
            n = x.m >> sh
            rem = x.m & ((1 << sh) - 1)
            half = 1 << (sh - 1)
            if rem > half or (rem == half and (n & 1)):
                n += 1
        return sign | n
    r = rnd(x.s, x.m, x.e)
    q, e2 = r.m, r.e
    if q.bit_length() < 24:
        shift = 24 - q.bit_length()
        q <<= shift
        e2 -= shift
    top2 = 23 + e2
    if top2 > 127:
        return sign | 0x7F800000
    return sign | ((top2 + 127) << 23) | (q - (1 << 23))


def stored(x):
    """fstp dword + fld dword: el valor que queda tras guardarlo como float y volver a cargarlo"""
    return from_bits(to_f32_bits(x))


def fl(b):
    return from_bits(b)


def indefinite():
    return V('nan', bits=INDEFINITE)


def quiet(x):
    return V('nan', bits=x.bits | 0x400000)


def nan_pick(a, b):
    """dos NaN (Intel SDM, tabla 4-7): NaN de senal y silencioso -> el silencioso; dos del mismo tipo -> el de mayor mantisa; con mantisa igual, el de signo positivo (o el primero si el signo coincide). El resultado sale silenciado"""
    if a.snan != b.snan:
        return quiet(b if a.snan else a)
    if (b.bits & 0x7FFFFF) > (a.bits & 0x7FFFFF):
        return quiet(b)
    if (b.bits & 0x7FFFFF) == (a.bits & 0x7FFFFF) and (a.bits >> 31) != (b.bits >> 31):
        # misma mantisa y distinto signo (medido en el hardware, microtests de `begin`): gana el de signo positivo
        return quiet(a if not (a.bits >> 31) else b)
    return quiet(a)


def nan_of(a, b):
    if a.kind == 'nan' and b.kind == 'nan':
        return nan_pick(a, b)
    return quiet(a) if a.kind == 'nan' else quiet(b) if b.kind == 'nan' else None


def add(a, b):
    n = nan_of(a, b)
    if n:
        return n
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf' and a.s != b.s:
            return indefinite()
        return a if a.kind == 'inf' else b
    if a.m == 0 and b.m == 0:
        return V('f', a.s & b.s, 0, 0)
    e = min(a.e, b.e) if a.m and b.m else (a.e if a.m else b.e)
    ta = (a.m << (a.e - e)) * (-1 if a.s else 1) if a.m else 0
    tb = (b.m << (b.e - e)) * (-1 if b.s else 1) if b.m else 0
    t = ta + tb
    if t == 0:
        return V('f', 0, 0, 0)
    return rnd(1 if t < 0 else 0, abs(t), e)


def neg_v(a):
    if a.kind == 'nan':
        return a
    if a.kind == 'inf':
        return V('inf', s=1 - a.s)
    return V('f', 1 - a.s, a.m, a.e)


def sub(a, b):
    return add(a, neg_v(b))


def mul(a, b):
    n = nan_of(a, b)
    if n:
        return n
    sign = a.s ^ b.s
    if a.kind == 'inf' or b.kind == 'inf':
        other = b if a.kind == 'inf' else a
        if other.kind == 'f' and other.m == 0:
            return indefinite()
        return V('inf', s=sign)
    if a.m == 0 or b.m == 0:
        return V('f', sign, 0, 0)
    return rnd(sign, a.m * b.m, a.e + b.e)


def div(a, b):
    n = nan_of(a, b)
    if n:
        return n
    sign = a.s ^ b.s
    if a.kind == 'inf':
        return indefinite() if b.kind == 'inf' else V('inf', s=sign)
    if b.kind == 'inf':
        return V('f', sign, 0, 0)
    if b.m == 0:
        return indefinite() if a.m == 0 else V('inf', s=sign)
    if a.m == 0:
        return V('f', sign, 0, 0)
    k = max(0, 27 + b.m.bit_length() - a.m.bit_length())
    q, r = divmod(a.m << k, b.m)
    return rnd(sign, q, a.e - b.e - k, sticky=r != 0)


def sqrt_v(a):
    if a.kind == 'nan':
        return quiet(a)
    if a.kind == 'inf':
        return indefinite() if a.s else a
    if a.m == 0:
        return a
    if a.s:
        return indefinite()
    m, e = a.m, a.e
    if e & 1:
        m <<= 1
        e -= 1
    k = max(0, (56 - m.bit_length() + 1) // 2)
    n = m << (2 * k)
    root = isqrt(n)
    return rnd(0, root, (e - 2 * k) // 2, sticky=root * root != n)


def cmp_gt(a, b):
    """a > b para finitos/infinitos ya descartados los NaN"""
    ka = float('inf') if a.kind == 'inf' and not a.s else float('-inf') if a.kind == 'inf' else None
    kb = float('inf') if b.kind == 'inf' and not b.s else float('-inf') if b.kind == 'inf' else None
    if ka is not None or kb is not None:
        va = ka if ka is not None else 0.0
        vb = kb if kb is not None else 0.0
        if ka is not None and kb is not None:
            return va > vb
        return va > 0 if ka is not None else vb < 0
    ex = [x.e for x in (a, b) if x.m]
    e = min(ex) if ex else 0
    ta = ((a.m << (a.e - e)) * (-1 if a.s else 1)) if a.m else 0
    tb = ((b.m << (b.e - e)) * (-1 if b.s else 1)) if b.m else 0
    return ta > tb


def gt(a, b):
    """comparacion ordenada a > b: con NaN es falsa"""
    if a.kind == 'nan' or b.kind == 'nan':
        return False
    return cmp_gt(a, b)


def le_or_nan_exit(a, limit):
    """True si a > limit o a es NaN (la salida por `jp` tras fcomp)"""
    return a.kind == 'nan' or gt(a, limit)


def from_int(i):
    return V('f', 1 if i < 0 else 0, abs(i), 0)


def fabs_bits(bits):
    """fld dword; fabs; fstp dword"""
    return to_f32_bits(from_bits(bits)) & 0x7FFFFFFF


def ftol_low32(x):
    """_ftol2: trunca a 64 bits con signo (NaN, infinito o fuera de rango dan 0x8000000000000000); el EXE usa EAX (con signo)"""
    if x.kind != 'f':
        return 0
    t = (x.m << x.e) if x.e >= 0 else (x.m >> -x.e)
    t = -t if x.s else t
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    low = t & M32
    return low - (1 << 32) if low & 0x80000000 else low


def signed32(u):
    return u - (1 << 32) if u & 0x80000000 else u


def signed16(u):
    u &= 0xFFFF
    return u - 0x10000 if u & 0x8000 else u


def pos_of(bits):
    return [fl(b) for b in bits]


def dist_xzy(cam, pos):
    """(dx*dx + dz*dz) + dy*dy con 24 bits en cada operacion (0x49EB3F..0x49EB4D)"""
    c = pos_of(cam)
    p = [mem(b) for b in pos]
    d = [r24(sub(c[i], p[i])) for i in range(3)]
    s = r24(add(r24(mul(d[0], d[0])), r24(mul(d[2], d[2]))))
    return r24(add(s, r24(mul(d[1], d[1]))))


K625 = from_bits(0x441C4000)
ONE = from_bits(0x3F800000)
C1E4 = from_bits(0x38D1B717)
C03, C07 = from_bits(0x3E99999A), from_bits(0x3F333333)
C15, HALF = from_bits(0x3FC00000), from_bits(0x3F000000)
C02, C01 = from_bits(0x3E4CCCCD), from_bits(0x3DCCCCCD)


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.hash = 2166136261
        self.particles = 0
        self.slot = tag(SYS_A)

    def push(self, e):
        # el banco guarda 256 eventos; los siguientes sobrescriben la ultima ranura
        if len(self.events) < 256:
            self.events.append(e)
        else:
            self.events[255] = e
        feed = [e[0], len(e) - 1] + list(e[1:])
        for v in feed:
            for b in range(4):
                self.hash ^= (v >> (8 * b)) & 0xFF
                self.hash = (self.hash * 16777619) & M32

    def add_particle(self, pos_tag, pos, vel, time, mult, light, limit):
        self.push([31, self.slot, pos_tag] + list(pos) + list(vel) + [time] + list(mult) + [0xBF800000, light, limit, 0])
        self.particles += 1
        if self.c['swap_slot']:
            self.slot = tag(SYS_B) if self.slot == tag(SYS_A) else tag(SYS_A)

    def draw_rem(self):
        return self.rng.draw() % 10000      # cdq; idiv 10000 (rand() >= 0)

    def result(self, counter):
        return dict(rand_state=self.rng.state, ev_hash=self.hash, particle_calls=self.particles, slots=[self.slot], events=self.events, counter=counter)


def model_blood(c):
    m = Model(c)
    counter = c['counter0']
    m.push([51])
    if not c['blood_on']:
        return m.result(counter)
    s = dist_xzy(c['cam'], c['pos'])     # en registro: sin redondear a float
    if gt(s, K625):
        return m.result(counter)
    amount = signed32(c['amount'])
    mult_c = [0x3F000000, 0, 0, 0x3F800000]
    for _ in range(max(amount, 0)):
        size = to_f32_bits(r24(add(r24(mul(r24(mul(from_int(m.draw_rem()), C1E4)), C03)), C07)))
        v0 = [to_f32_bits(r24(mul(fl(c['dir'][k]), C15))) for k in range(3)]
        vel = []
        for k in range(3):
            t = r24(mul(from_int(m.draw_rem()), C1E4))
            t = r24(add(t, t))
            t = r24(sub(t, ONE))
            t = r24(add(t, mem(v0[k])))
            vel.append(to_f32_bits(t))
        m.add_particle(tag(0), c['pos'], vel, 0, mult_c + [size, 0, 0x3F4CCCCD], c['light'], 0x3F19999A)
    # posicion de la mancha: pos + direccion * 0.5 (x e y sin guardar el producto), mas un desplazamiento aleatorio en x e y y +1.0 en z
    px_reg = r24(mul(fl(c['dir'][0]), HALF))
    py_reg = r24(mul(fl(c['dir'][1]), HALF))
    zs = to_f32_bits(r24(mul(fl(c['dir'][2]), HALF)))
    ox = to_f32_bits(r24(add(px_reg, mem(c['pos'][0]))))
    oy = to_f32_bits(r24(add(py_reg, mem(c['pos'][1]))))
    oz = to_f32_bits(r24(add(fl(zs), mem(c['pos'][2]))))

    def jitter(rem, base):
        t = r24(mul(from_int(rem), C1E4))
        t = r24(mul(t, C02))
        t = r24(sub(t, C01))
        return to_f32_bits(r24(add(t, mem(base))))
    sx = jitter(m.draw_rem(), ox)
    ry = m.draw_rem()
    counter = (counter + 1) & M32
    sy = jitter(ry, oy)
    sz = to_f32_bits(r24(add(fl(oz), ONE)))
    spot = [sx, sy, sz]
    low = counter & 7
    if low == 5:
        life = (m.rng.draw() & 0xFFF) + 0x7D0
        m.push([61, 1, c['tex'], STACK] + spot + [0x3DCCCCCD, 0, 0, 0xBDCCCCCD, 0xFF, 0xC8, 0, 0, 0x40800000, life, 0x3F800000])
    elif low == 2:
        life = (m.rng.draw() & 0xFFF) + 0x1F40
        m.push([61, 1, c['tex'], STACK] + spot + [0x3E4CCCCD, 0, 0, 0xBE4CCCCD, 0xFF, 0xC8, 0, 0, 0x40800000, life, 0x3F800000])
    return m.result(counter)


def expected(c):
    return model_blood(c)


def check_self():
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert ((((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF) == 41
    assert to_f32_bits(mul(from_int(5), from_bits(0x3F000000))) == 0x40200000
    print('ok')


def counter_changes(c, counter):
    """bytes que cambian en el objeto this de la arena (desplazamiento respecto al objeto, byte nuevo)"""
    if c['this_kind'] != 0:
        return []
    out = []
    for b in range(4):
        old = (c['counter0'] >> (8 * b)) & 0xFF
        new = (counter >> (8 * b)) & 0xFF
        if old != new:
            out.append([0x50 + b, new])
    return out


def check_row(line):
    row = json.loads(line)
    if row.get('event') != 'comparison':
        return None
    c = row['input']
    msgs = []
    e = expected(c)
    want_changes = [[], [], [], [], [], counter_changes(c, e['counter'])]
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('rand_state', 'ev_hash', 'particle_calls', 'slots', 'events', 'counter') if o[k] != e[k]]
        if bad or o['changes'] != want_changes:
            msgs.append('DIFIERE %s caso %s fn %s campos %s changes %s' % (side, row['case'], c['fn'], bad, o['changes'] != want_changes))
    return row['case'], msgs


def main(path):
    from concurrent.futures import ProcessPoolExecutor
    lines = [l for l in open(path, encoding='utf-8') if '"event":"comparison"' in l.replace(' ', '')]
    bad = checked = 0
    with ProcessPoolExecutor() as ex:
        for res in ex.map(check_row, lines, chunksize=16):
            if res is None:
                continue
            checked += 1
            for m in res[1]:
                bad += 1
                if bad <= 8:
                    print(m)
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    if len(sys.argv) == 1:
        check_self()
        sys.exit(0)
    sys.exit(main(sys.argv[1]))
