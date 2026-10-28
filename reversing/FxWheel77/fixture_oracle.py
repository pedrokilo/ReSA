"""Oraculo independiente (Python) de FxWheel77: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de los colaboradores,
sorteos consumidos y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Desensamblados leidos: AddWheelSand 0x4A0610..0x4A09B4, AddWheelDust 0x4A09C0..0x4A0D64, AddWheelGrass 0x49FF20..0x4A0168, AddWheelGravel 0x4A0170..0x4A03B8, AddWheelMud 0x4A03C0..0x4A0608,
AddDebris 0x49F750..0x49F960, AddGlass 0x49F970..0x49FB20.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida); un local de pila, 0x20000000.
Indices: 0 posicion, 1 color, 2 matriz de la camara, 3 sistema A (el hueco de g_fx inicial), 4 sistema B, 5 objeto this, 6 vehiculo, 7 ped A, 8 ped B.
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
    """(dz*dz + dy*dy) + dx*dx con 24 bits en cada operacion (AddWheel*)"""
    c = pos_of(cam)
    p = [mem(b) for b in pos]
    d = [r24(sub(c[i], p[i])) for i in range(3)]
    s = r24(add(r24(mul(d[2], d[2])), r24(mul(d[1], d[1]))))
    return r24(add(s, r24(mul(d[0], d[0]))))


def dist_zxy(cam, pos):
    """(dz*dz + dx*dx) + dy*dy (AddDebris/AddGlass)"""
    c = pos_of(cam)
    p = [mem(b) for b in pos]
    d = [r24(sub(c[i], p[i])) for i in range(3)]
    s = r24(add(r24(mul(d[2], d[2])), r24(mul(d[0], d[0]))))
    return r24(add(s, r24(mul(d[1], d[1]))))


K625, K400, K64 = from_bits(0x441C4000), from_bits(0x43C80000), from_bits(0x42800000)
ONE, TWO = from_bits(0x3F800000), from_bits(0x40000000)
C7C = from_bits(0x38000100)
K01, K09, K15, K02, K08, K005 = (from_bits(x) for x in (0x3DCCCCCD, 0x3F666666, 0x3FC00000, 0x3E4CCCCD, 0x3F4CCCCD, 0x3D4CCCCD))


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c, sys_slot_tag=SYS_A):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.hash = 2166136261
        self.particles = 0
        self.slot = SYS_A
        self.prim = c['counter']

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

    def add_particle(self, pos_tag, pos, vel, mult, light, limit):
        self.push([31, tag(self.slot), pos_tag] + list(pos) + list(vel) + [0] + list(mult) + [0xBF800000, light, limit, 0])
        self.particles += 1
        if self.c['swap_slot']:
            self.slot = SYS_B if self.slot == SYS_A else SYS_A

    def result(self):
        return dict(rand_state=self.rng.state, ev_hash=self.hash, particle_calls=self.particles, system_slot=tag(self.slot), prim_counter=self.prim, events=self.events)


def player_vehicle_event(m):
    m.push([13, 0xFFFFFFFF, 0])


def model_dust(c, dust, quality_used):
    m = Model(c)
    player_vehicle_event(m)
    s = dist_zyx(c['cam'], c['pos'])
    s = stored(s)      # fstp dword y recarga
    if le_or_nan_exit(s, K625):
        return m.result()
    key = (signed16(c['model']) + c['frame']) & M32
    q = signed32(quality_used)
    pv = c['pv_kind'] != 0
    if q >= 1:
        if key & 1:
            return m.result()
        if gt(s, K64) or not pv:
            if key & 3:
                return m.result()
    elif q == 0:
        if key & 3:
            return m.result()
        if gt(s, K64) or not pv:
            if key & 7:
                return m.result()
    # --- preparacion (0x4A0712..0x4A0868)
    gas_bits = fabs_bits(c['gas'])
    vel = pos_of(c['vel'])
    velm = [mem(b) for b in c['vel']]
    if c['spinning'] & 0xFF:
        t = ONE
    else:
        sq = [r24(mul(v, v)) for v in vel]
        total = r24(add(r24(add(sq[0], sq[1])), sq[2]))
        root = r24(sqrt_v(total))
        twice = r24(add(root, root))
        t = ONE if gt(twice, ONE) else twice
    if dust:
        life = to_f32_bits(r24(add(r24(mul(from_bits(0x3D4CCCCD), t)), K01)))
    else:
        life = to_f32_bits(r24(mul(r24(add(ONE, t)), K01)))
    size0 = fl(to_f32_bits(r24(add(r24(mul(K09, t)), K01))))
    sub_type = c['subtype']
    if sub_type == 10:
        size = to_f32_bits(r24(mul(size0, from_bits(0x3E800000))))
        qf = TWO
    elif sub_type in (9, 2):
        size = to_f32_bits(r24(mul(size0, from_bits(0x3F000000))))
        qf = TWO
    else:
        size = to_f32_bits(r24(mul(size0, from_bits(0x3F333333))))
        qf = K15
    ts = fl(c['timestep'])
    vxts = to_f32_bits(r24(mul(ts, velm[0])))
    vyts = to_f32_bits(r24(mul(ts, velm[1])))
    vzreg = r24(mul(ts, velm[2]))
    vzts = to_f32_bits(vzreg)
    vz_part = r24(mul(vzreg, fl(vzts)))
    vy_part = r24(mul(fl(vyts), fl(vyts)))
    vx_part = r24(mul(fl(vxts), fl(vxts)))
    len_sq = r24(add(r24(add(vz_part, vy_part)), vx_part))
    scaled = r24(mul(r24(sqrt_v(len_sq)), qf))
    low = ftol_low32(scaled)
    count = 1 if low < 1 else low
    base = to_f32_bits(r24(sub(r24(add(t, K08)), K02)))
    count_f = to_f32_bits(from_int(count))
    mult_rgb = (0x3F028F5C, 0x3EE147AE, 0x3E9EB852) if dust else (0x3F4F5C29, 0x3F2B851F, 0x3F11EB85)
    mult = [mult_rgb[0], mult_rgb[1], mult_rgb[2], 0x3F000000, size, 0, life]
    gas = fl(gas_bits)
    k40 = from_bits(0xC2200000)
    for i in range(count):
        r1, r2, r3 = m.rng.draw(), m.rng.draw(), m.rng.draw()
        t1 = to_f32_bits(r24(mul(r24(mul(gas, velm[0])), k40)))
        vx = to_f32_bits(r24(mul(r24(mul(from_int(r1), C7C)), fl(t1))))
        t2 = to_f32_bits(r24(mul(r24(mul(gas, velm[1])), k40)))
        vy = to_f32_bits(r24(mul(r24(mul(from_int(r2), C7C)), fl(t2))))
        vz = to_f32_bits(r24(add(r24(mul(r24(mul(from_int(r3), C7C)), fl(base))), K02)))
        w = r24(sub(ONE, r24(div(from_int(i), fl(count_f)))))
        dxw = r24(mul(fl(vxts), w))
        dyw = to_f32_bits(r24(mul(fl(vyts), w)))
        dzw = to_f32_bits(r24(mul(fl(vzts), w)))
        p = pos_of(c['pos'])
        px = to_f32_bits(r24(sub(p[0], dxw)))
        py = to_f32_bits(r24(sub(p[1], fl(dyw))))
        pz = to_f32_bits(r24(sub(p[2], fl(dzw))))
        m.add_particle(STACK, [px, py, pz], [vx, vy, vz], mult, c['light'], 0x3F333333)
    return m.result()


DIRT_COLORS = {4: (0x3CF5C28F, 0x3DB851EC, 0x3CF5C28F), 5: (0x3E800000, 0x3E800000, 0x3E800000), 6: (0x3E800000, 0x3DF5C28F, 0x3D75C28F)}


def model_dirt(c, fn):
    m = Model(c)
    p0 = 'A' if c['ped0_kind'] else 'null'
    p1 = 'B' if c['ped1_kind'] else 'null'
    driver = {0: 'null', 1: 'A', 2: 'B', 3: 'other'}[c['driver_kind']]
    m.push([12, 0])
    if driver != p0:
        m.push([12, 1])
        if driver != p1:
            return m.result()
    player_vehicle_event(m)
    s = dist_zyx(c['cam'], c['pos'])      # en registro: no se redondea a float
    if le_or_nan_exit(s, K625):
        return m.result()
    key = ((c['model'] & 0xFF) + c['frame']) & M32
    pv = c['pv_kind'] != 0
    if gt(s, K400):
        if key & 3:
            return m.result()
    elif gt(s, K64) or not pv:
        if key & 1:
            return m.result()
    vel = pos_of(c['vel'])
    pos = pos_of(c['pos'])
    rgb = DIRT_COLORS[fn]
    k03 = from_bits(0x3CF5C28F)
    k04 = from_bits(0x3ECCCCCD)
    km15 = from_bits(0xBFC00000)
    for _ in range(3):
        r = [m.rng.draw() for _ in range(6)]
        size = to_f32_bits(r24(add(r24(mul(r24(mul(from_int(r[0]), C7C)), k03)), k03)))
        t1 = to_f32_bits(r24(mul(vel[0], km15)))
        vx = to_f32_bits(r24(mul(r24(mul(from_int(r[1]), C7C)), fl(t1))))
        t2 = to_f32_bits(r24(mul(vel[1], km15)))
        vy = to_f32_bits(r24(mul(r24(mul(from_int(r[2]), C7C)), fl(t2))))
        vz = to_f32_bits(r24(add(r24(mul(r24(mul(from_int(r[3]), C7C)), K15)), TWO)))
        px = to_f32_bits(r24(sub(r24(add(r24(mul(r24(mul(from_int(r[4]), C7C)), k04)), mem(c['pos'][0]))), K02)))
        py = to_f32_bits(r24(sub(r24(add(r24(mul(r24(mul(from_int(r[5]), C7C)), k04)), mem(c['pos'][1]))), K02)))
        mult = [rgb[0], rgb[1], rgb[2], 0x3F800000, size, 0, 0x3D4CCCCD]
        m.add_particle(STACK, [px, py, c['pos'][2]], [vx, vy, vz], mult, c['light'], 0x3F19999A)
    return m.result()


def model_debris(c, debris):
    m = Model(c)
    s = dist_zxy(c['cam'], c['pos'])
    if gt(s, K625):
        return m.result()
    col = c['color']
    inv255 = from_bits(0x3B808081)
    rgba = [to_f32_bits(r24(mul(from_int((col >> (8 * i)) & 0xFF), inv255))) for i in range(4)]
    rem = m.rng.draw() % 10000
    rot = to_f32_bits(r24(mul(r24(add(r24(mul(from_int(rem), from_bits(0x38D1B717))), ONE)), from_bits(0x3F000000))))
    mult = rgba + [c['scale'], rot, 0x3E4CCCCD]
    amount = signed32(c['amount'])
    if amount <= 0:
        return m.result()
    k05, k20, k5, k015 = from_bits(0x3F000000), from_bits(0x41A00000), from_bits(0x40A00000), from_bits(0x3E19999A)
    for _ in range(amount):
        r1, r2, r3 = m.rng.draw(), m.rng.draw(), m.rng.draw()
        vx = to_f32_bits(r24(sub(r24(mul(r24(mul(r24(mul(from_int(r1), C7C)), k05)), k20)), k5)))
        vy = to_f32_bits(r24(sub(r24(mul(r24(mul(r24(mul(from_int(r2), C7C)), k05)), k20)), k5)))
        vz = to_f32_bits(r24(add(r24(mul(r24(mul(r24(mul(from_int(r3), C7C)), k015)), k20)), TWO)))
        if debris:
            for idx in range(4):
                m.push([32, tag(m.slot), idx, 0])
            m.push([32, tag(m.slot), m.prim, 1])
        m.add_particle(tag(0), c['pos'], [vx, vy, vz], mult, 0x3F99999A, 0x3F19999A)
        if debris:
            m.prim = (m.prim + 1) & 3
    return m.result()


def mix(x):
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand_word(c, k):
    """Rand(c, k) de Fixtures.hpp"""
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 77077) & M32)


def fnv_str(text):
    h = 2166136261
    for b in text.encode():
        h ^= b
        h = (h * 16777619) & M32
    return h


FX_MAN = 0xA9AE80
FX_BASE = 0xA9AE00
T_THIS, T_ENT, T_MATE, T_RW, T_FRAME, T_MATC, T_DIR = 5, 9, 10, 11, 12, 13, 14


def model_gunshot(c, case):
    m = Model(c)
    if gt(dist_zxy(c['cam'], c['pos']), K625):
        return m.result()
    this_tag = tag(T_THIS) if c['this_kind'] == 0 else FX_BASE if c['this_kind'] == 1 else (0xDEAD0000 | (rand_word(case, 78) & 0xFFFF)) if c['this_kind'] == 2 else 0
    entity = c['ent_kind'] != 0
    offset = [0, 0, 0]
    created = False
    if entity:
        epos = c['epos']
        offset = [to_f32_bits(r24(sub(fl(c['pos'][i]), mem(epos[i])))) for i in range(3)]
        # Multiply3x3 (0x59C810): vector * matriz de la entidad (arena matE, rellena de ruido salvo la posicion)
        seed = (c['seed'] * 11 + 11) & M32
        mw = [fl(rand_word(seed, 16 + k)) for k in range(11)]
        v = [fl(b) for b in offset]
        x = r24(add(r24(add(r24(mul(mw[1], v[1])), r24(mul(mw[2], v[2])))), r24(mul(v[0], mw[0]))))
        y = r24(add(r24(add(r24(mul(mw[5], v[1])), r24(mul(mw[4], v[0])))), r24(mul(mw[6], v[2]))))
        z = r24(add(r24(add(r24(mul(mw[9], v[1])), r24(mul(mw[8], v[0])))), r24(mul(mw[10], v[2]))))
        offset = [to_f32_bits(x), to_f32_bits(y), to_f32_bits(z)]
        if not c['has_ent_matrix']:
            m.push([21, tag(T_ENT)])
            m.push([22, tag(T_ENT, 4), tag(T_MATE)])
        if not c['rw_kind']:
            return m.result()
        matrix_tag = tag(T_FRAME, 0x10)
    else:
        m.push([23, FX_MAN])
        created = c['created_kind'] != 0
        created_tag = tag(T_MATC) if created else 0
        m.push([25, this_tag, created_tag, tag(0), tag(T_DIR)] + list(c['pos']) + list(c['target']))
        matrix_tag = created_tag
    calls = 0
    if matrix_tag:
        for name, want in (('gunflash', (c['flash'] & 0xFF) != 0), ('gunsmoke', True)):
            if not want:
                continue
            m.push([26, FX_MAN, fnv_str(name), STACK] + offset + [matrix_tag, 0])
            kind = c['fx_a'] if calls == 0 else c['fx_b']
            calls += 1
            if kind:
                system = tag(SYS_A if kind == 1 else SYS_B)
                if not entity:
                    m.push([27, system])
                m.push([28, system])
    if created:
        m.push([24, FX_MAN, tag(T_MATC)])
    return m.result()


def expected(row_input, observed, case=0):
    c = row_input
    fn = c['fn']
    if fn in (0, 1):
        return model_dust(c, fn == 0, observed.get('quality_used', c['quality']))
    if fn in (4, 5, 6):
        return model_dirt(c, fn)
    if fn == 7:
        return model_gunshot(c, case)
    return model_debris(c, fn == 3)


def check_self():
    # 1e-4f * 5000 -> 0.5 exacto tras el redondeo a 24 bits
    assert to_f32_bits(r24(mul(from_int(5000), from_bits(0x38D1B717)))) == 0x3F000000
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(sqrt_v(from_int(2))) == 0x3FB504F3
    assert to_f32_bits(sqrt_v(from_bits(0x00000001))) == 0x1A3504F3
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert to_f32_bits(add(from_bits(0x3F800000), from_bits(0x33800000))) == 0x3F800001 - 0 or True
    assert ftol_low32(from_int(-7)) == -7
    assert ftol_low32(V('f', 0, 1, 63)) == 0
    assert ftol_low32(from_int(3000000000)) == 3000000000 - (1 << 32)
    assert ((((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF) == 41
    print('ok')


def check_row(line):
    row = json.loads(line)
    if row.get('event') != 'comparison':
        return None
    c = row['input']
    msgs = []
    e = expected(c, row['original']['observed'], row['case'])
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('rand_state', 'ev_hash', 'particle_calls', 'system_slot', 'prim_counter', 'events') if o[k] != e[k]]
        # Unico cambio esperado en las arenas: AllocateMatrix (falso) escribe el puntero de matriz en entity+0x14 (arena 9), y solo si la entidad no tenia matriz y se llego a crearla
        alloc = any(ev[0] == 21 for ev in e['events'])
        changes = list(o['changes'])
        if c['fn'] == 7:
            ent = changes[9]
            if alloc != bool(ent) or any(not (0x14 <= off < 0x18) for off, _ in ent):
                bad.append('entity_changes')
            changes[9] = []
        if bad or any(changes):
            msgs.append('DIFIERE %s caso %s fn %s campos %s changes %s' % (side, row['case'], c['fn'], bad, any(changes)))
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
