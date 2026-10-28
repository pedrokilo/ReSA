"""Oraculo independiente (Python) de FxImpact78: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de los colaboradores,
sorteos consumidos y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Desensamblados leidos: Fx_c::AddSparks 0x49F040..0x49F2FD y Fx_c::AddBulletImpact 0x49F3D0..0x49F66B.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida); un local de pila, 0x20000000.
Indices: 0 posicion/origen, 1 direccion, 2 matriz de la camara, 3 sistema A, 4 sistema B, 5 objeto this.
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


def dist_zyx(cam, pos):
    """(dz*dz + dy*dy) + dx*dx con 24 bits en cada operacion"""
    c = pos_of(cam)
    p = [mem(b) for b in pos]
    d = [r24(sub(c[i], p[i])) for i in range(3)]
    s = r24(add(r24(mul(d[2], d[2])), r24(mul(d[1], d[1]))))
    return r24(add(s, r24(mul(d[0], d[0]))))


K22500, K225 = from_bits(0x46AFC800), from_bits(0x43610000)
ONE = from_bits(0x3F800000)
C7C = from_bits(0x38000100)
C98, C28, C24 = from_bits(0x3F4CCCCD), from_bits(0x3D4CCCCD), from_bits(0x3E99999A)
HALF = from_bits(0x3F000000)
FX_BASE = 0xA9AE00
SURF = 0xB79538
SYS_A, SYS_B = 3, 4


def fchs_bits(bits):
    """fld dword; fchs; fstp dword: el NaN de senal sale silenciado y con el signo invertido"""
    v = fl(bits)
    if v.kind == 'nan':
        return v.bits ^ 0x80000000
    return to_f32_bits(neg_v(v))


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.hash = 2166136261
        self.particles = 0
        # huecos de g_fx: arena, humo, chispa, chispa2 -> tags de los sistemas
        self.slots = [tag(SYS_A), tag(SYS_A), tag(SYS_A), tag(SYS_B)]

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

    def add_particle(self, slot, pos_tag, pos, vel, time, mult, light, limit):
        self.push([31, self.slots[slot], pos_tag] + list(pos) + list(vel) + [time] + list(mult) + [0xBF800000, light, limit, 0])
        self.particles += 1
        if self.c['swap_slot']:
            self.slots = [tag(SYS_B) if t == tag(SYS_A) else tag(SYS_A) for t in self.slots]

    def result(self):
        return dict(rand_state=self.rng.state, ev_hash=self.hash, particle_calls=self.particles, slots=self.slots, events=self.events)


def model_sparks(c):
    m = Model(c)
    s = dist_zyx(c['cam'], c['pos'])     # en registro: sin redondear a float
    if gt(s, K22500):
        return m.result()
    if gt(s, K225) and (c['frame'] & 1):
        return m.result()
    # --- preparacion (0x49F0AD..0x49F160)
    amount = signed32(c['amount'])
    if amount <= 0:
        return m.result()
    ts = mem(c['timestep'])
    life_mul = to_f32_bits(r24(mul(fl(c['life']), C98)))
    across = [to_f32_bits(r24(mul(fl(c['across'][i]), ts))) for i in range(3)]
    amp = to_f32_bits(from_int(amount))
    neg = fchs_bits(c['spread'])
    range_f = to_f32_bits(r24(sub(fl(c['spread']), mem(neg))))
    primary = (c['stype'] & 0xFF) != 0
    slot = 2 if primary else 3
    mult = [0x3F800000] * 5 + [0, life_mul]
    force = mem(c['force'])
    for i in range(amount):
        r1, r2, r3 = m.rng.draw(), m.rng.draw(), m.rng.draw()
        w = to_f32_bits(r24(sub(ONE, r24(div(from_int(i), mem(amp))))))
        range_reg = r24(sub(fl(c['spread']), mem(neg)))
        vx = to_f32_bits(r24(add(r24(add(r24(mul(r24(mul(from_int(r1), C7C)), range_reg)), mem(neg))), mem(c['dir'][0]))))
        vy = to_f32_bits(r24(add(r24(add(r24(mul(r24(mul(from_int(r2), C7C)), range_reg)), mem(neg))), mem(c['dir'][1]))))
        vz_reg = r24(add(r24(add(r24(mul(r24(mul(from_int(r3), C7C)), mem(range_f))), mem(neg))), mem(c['dir'][2])))
        vel = [to_f32_bits(r24(mul(fl(vx), force))), to_f32_bits(r24(mul(fl(vy), force))), to_f32_bits(r24(mul(vz_reg, force)))]
        axw = r24(mul(fl(across[0]), mem(w)))
        ayw = r24(mul(fl(across[1]), mem(w)))
        azw = to_f32_bits(r24(mul(fl(across[2]), mem(w))))
        pos = [to_f32_bits(r24(sub(fl(c['pos'][0]), axw))), to_f32_bits(r24(sub(fl(c['pos'][1]), ayw))), to_f32_bits(r24(sub(fl(c['pos'][2]), mem(azw))))]
        t = to_f32_bits(r24(mul(fl(w), C28)))
        m.add_particle(slot, STACK, pos, vel, t, mult, 0x3F99999A, 0x3F19999A)
    return m.result()


def model_bullet(c):
    m = Model(c)
    m.push([41, SURF, c['surface']])
    kind = c['kind']
    if kind == 0:
        return m.result()
    s = dist_zyx(c['cam'], c['pos'])
    if gt(s, K22500):
        return m.result()
    amount = signed32(c['amount'])
    light = c['light']
    if kind == 1:
        m.push([42, FX_BASE, tag(0), tag(1)] + list(c['pos']) + list(c['dir']) + [0x40400000, c['amount'], 0, 0, 0, 1, 0x3ECCCCCD, 0x3F800000])
        alpha, count = 0x3E19999A, 2
        if amount >= 8:
            alpha, count = 0x3E99999A, 1
        mult = [0x3F800000, 0x3F800000, 0x3F800000, alpha, 0x3ECCCCCD, 0, 0x3D99999A]
        for j in range(count):
            t = to_f32_bits(r24(mul(from_int(j), C28)))
            m.add_particle(1, tag(0), c['pos'], c['dir'], t, mult, light, 0x3F19999A)
    elif kind in (2, 4):
        rgb = [0x3F19999A] * 3 if kind == 4 else [0x3F4F5C29, 0x3F2B851F, 0x3F11EB85]
        alpha, count = 0x3E19999A, 4
        if amount >= 8:
            alpha, count = 0x3E99999A, 2
        mult = rgb + [alpha, 0x3ECCCCCD, 0, 0x3E99999A]
        for j in range(count):
            vec = [to_f32_bits(r24(mul(fl(c['dir'][k]), C24))) for k in range(3)]
            t = to_f32_bits(r24(mul(from_int(j), C28)))
            m.add_particle(0, tag(0), c['pos'], vec, t, mult, light, 0x3F19999A)
    elif kind == 3:
        half = ftol_low32(r24(mul(from_int(amount), HALF))) & M32
        m.push([43, FX_BASE, tag(0), tag(1)] + list(c['pos']) + list(c['dir']) + [half, 0x3F800000])
    return m.result()


def expected(c):
    return model_sparks(c) if c['fn'] == 0 else model_bullet(c)


def check_self():
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert fchs_bits(0x7F800001) == 0xFFC00001 and fchs_bits(0x3F800000) == 0xBF800000
    assert ftol_low32(from_int(-7)) == -7
    assert ((((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF) == 41
    assert to_f32_bits(mul(from_int(5), from_bits(0x3F000000))) == 0x40200000
    print('ok')


def check_row(line):
    row = json.loads(line)
    if row.get('event') != 'comparison':
        return None
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('rand_state', 'ev_hash', 'particle_calls', 'slots', 'events') if o[k] != e[k]]
        if bad or any(o['changes']):
            msgs.append('DIFIERE %s caso %s fn %s campos %s changes %s' % (side, row['case'], c['fn'], bad, any(o['changes'])))
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
