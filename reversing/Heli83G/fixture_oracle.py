"""Oraculo independiente (Python) de Heli83G: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6C58E0..0x6C6510), lo que debe observarse de
CHeli::SearchLightCone (traza EN ORDEN de los colaboradores con todos sus argumentos -solo los primeros 24 se guardan, el FNV-1a de TODOS y su numero se comparan-, contadores del bufer temporal,
los 82 vertices (x, y, z, color), los indices, las nueve palabras de salida y las arenas sin otros cambios) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa. El seno y el coseno del
angulo (i * pi/20 en float) se calculan con series de Taylor exactas (fracciones) y se redondean a 24 bits.

Flujo leido (cdecl, 19 dwords; void): d = objetivo - origen, Normalise (0x59C910); objetivo += 3 * d (z guardada) y CWorld::ProcessLineOfSight(origen, objetivo, solo edificios) lo sustituye por el punto de
choque; punto lejano = origen + 100 * d; v = Normalise(camara - origen); corona con c = max(0, v . d)^6 (tamano 20 c, intensidad ftol(255 c)); anillo de 41 puntos (CrossProduct y Normalise por vuelta):
vertice cercano = origen + (lado * sen + arriba * cos) * baseRadius y lejano = proyeccion en el plano z del objetivo del punto punto-lejano + (lado * sen + arriba * cos) * targetRadius (acortado a 100 con
Normalise); indices (dos triangulos si baseRadius > 0), puntos 20 y 30 relativos al objetivo en useless1/2, color por vertice = gris ftol(200 s), azul ftol(255 s) con s = luz * brillo / cuadrado maximo,
RwIm3DTransform/RenderIndexedPrimitive/End, y con drawShadow una sombra (StoreShadowToBeRendered) si los dos vectores escalados por 1.2 miden menos de 100 y la camara esta a 25 o menos.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000.
Indices: 0 salidas (useless0 +0, useless1 +0x10, useless2 +0x20), 1 matriz de la camara.
"""
import json
import struct
import sys
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
STACK = 0x20000000
SYS_A, SYS_B = 2, 3


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





ZERO = from_bits(0)
ONE = from_bits(0x3F800000)
OUT, CAM = range(2)
TRACE_KEEP = 24

# constantes del EXE leidas del desensamblado (0x6C58E0..0x6C6510): patrones de 32 bits
K_EXTEND = 0x40400000    # 0x858B3C  3.0
K_FAR = 0x42C80000       # 0x858628  100.0
K_CORONA = 0x41A00000    # 0x858BA4  20.0
K_BYTE = 0x437F0000      # 0x859AAC  255.0
K_STEP = 0x3E20D97C      # 0x8717B4  pi / 20
K_TEXSCALE = 0x3E19999A  # 0x858FCC  0.15
K_TEXBIAS = 0x3DCCCCCD   # 0x858B1C  0.1
K_GRAY = 0x43480000      # 0x858A48  200.0
K_RISE = 0x40A00000      # 0x858C80  5.0
K_SPOT = 0x3F99999A      # 0x858F08  1.2
K_RANGE = 0x41C80000     # 0x858FE8  25.0
K_FALLOFF = 0x3D23D70A   # 0x858CEC  0.04
K_HALF = 0x3F000000      # 0x858B8C  0.5
K_INTENSITY = 0x43000000  # 0x858BF4  128.0
VERTICES_ADDR = 0xC4D958
INDICES_ADDR = 0xC4B958


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.events = []
        self.count = 0
        self.hash = 2166136261

    def push(self, e):
        if len(self.events) < TRACE_KEEP:
            self.events.append(e)
        self.count += 1
        feed = [e[0], len(e) - 1] + list(e[1:])
        for v in feed:
            for b in range(4):
                self.hash ^= (v >> (8 * b)) & 0xFF
                self.hash = (self.hash * 16777619) & M32


def bits(v):
    return to_f32_bits(v)


def mulr(a, b):
    return r24(mul(a, b))


def addr(a, b):
    return r24(add(a, b))


def subr(a, b):
    return r24(sub(a, b))


def divr(a, b):
    return r24(div(a, b))


def sqrtr(a):
    return r24(sqrt_v(a))


def lt_ordered(a, b):
    """fcomp a, b; test ah,5; jp: a < b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and gt(b, a)


def chs(x):
    """fchs: invierte el signo tambien de un NaN"""
    if x.kind == 'nan':
        return V('nan', bits=x.bits ^ 0x80000000, snan=x.snan)
    return neg_v(x)


# ---- seno y coseno exactos (series de Taylor con fracciones) del angulo de la x87, redondeados a 24 bits y guardados como float ----
from fractions import Fraction


def _series(x, cosine):
    term = Fraction(1) if cosine else x
    total = term
    k = 0 if cosine else 1
    for i in range(1, 90):
        term = -term * x * x / ((k + 2 * i - 1) * (k + 2 * i)) if cosine else -term * x * x / ((2 * i) * (2 * i + 1))
        total += term
    return total


def _from_fraction(q):
    if q == 0:
        return V('f', 0, 0, 0)
    s = 1 if q < 0 else 0
    q = abs(q)
    k = 120
    scaled = q * (1 << k)
    m = scaled.numerator // scaled.denominator
    sticky = scaled.numerator % scaled.denominator != 0
    return rnd(s, m, -k, sticky=sticky)


_TRIG = {}


def trig_bits(ang):
    """(sin, cos) como patrones de float del angulo `ang` (V finito)"""
    key = (ang.s, ang.m, ang.e)
    if key not in _TRIG:
        q = Fraction(ang.m) * (Fraction(2) ** ang.e)
        q = -q if ang.s else q
        _TRIG[key] = (bits(_from_fraction(_series(q, False))), bits(_from_fraction(_series(q, True))))
    return _TRIG[key]


def model_light(c):
    """CHeli::SearchLightCone 0x6C58E0..0x6C6510 (desensamblado leido): ver README.md"""
    m = Model(c)
    path = []
    o, t = list(c['o']), list(c['t'])
    radius, power, base = c['radius'], c['power'], c['base']

    def normalise(v):
        """CVector::Normalise (0x59C910) falseado con Veh83CNormalise: evento con la entrada y normalizacion en el sitio"""
        m.push([1, STACK] + list(v))
        x2, y2, z2 = mulr(fl(v[0]), mem(v[0])), mulr(fl(v[1]), mem(v[1])), mulr(fl(v[2]), mem(v[2]))
        total = addr(addr(x2, y2), z2)
        if total.kind != 'nan' and not gt(total, ZERO):
            return [0x3F800000, v[1], v[2]]
        inv = divr(ONE, sqrtr(total))
        return [bits(mulr(fl(v[i]), inv)) for i in range(3)]

    def cross(a, b):
        """CrossProduct (0x59C730) falseado: a x b con cada producto a 24 bits"""
        m.push([2, STACK, STACK, STACK] + list(a) + list(b))
        x = bits(subr(mulr(fl(a[1]), mem(b[2])), mulr(fl(a[2]), mem(b[1]))))
        y = bits(subr(mulr(fl(a[2]), mem(b[0])), mulr(fl(a[0]), mem(b[2]))))
        z = bits(subr(mulr(fl(a[0]), mem(b[1])), mulr(fl(a[1]), mem(b[0]))))
        return [x, y, z]

    # direccion origen -> objetivo
    n = normalise([bits(subr(fl(t[i]), mem(o[i]))) for i in range(3)])
    # el objetivo se prolonga 3.0 (x e y sin guardar, z guardada) y la linea de vision lo recorta
    ext_x, ext_y = mulr(fl(n[0]), mem(K_EXTEND)), mulr(fl(n[1]), mem(K_EXTEND))
    ext_z = bits(mulr(fl(n[2]), mem(K_EXTEND)))
    t = [bits(addr(ext_x, mem(t[0]))), bits(addr(ext_y, mem(t[1]))), bits(addr(fl(ext_z), mem(t[2])))]
    m.push([3, STACK, STACK, STACK, STACK] + o + t + [1, 0, 0, 0, 0, 0, 0, 0])
    if c['los']:
        t = list(c['col'])
        path.append('los_hit')
    else:
        path.append('los_miss')
    # punto lejano y vector origen -> camara
    far_x, far_y = mulr(fl(n[0]), mem(K_FAR)), mulr(fl(n[1]), mem(K_FAR))
    far_z = bits(mulr(fl(n[2]), mem(K_FAR)))
    farp = [bits(addr(far_x, mem(o[0]))), bits(addr(far_y, mem(o[1]))), bits(addr(fl(far_z), mem(o[2])))]
    cam = list(c['cam_pos'] if c['cam_null'] else c['cam_mat'])
    cv = normalise([bits(subr(fl(cam[i]), mem(o[i]))) for i in range(3)])
    # corona: coseno acotado a 0 elevado a 6
    facing = addr(addr(mulr(fl(cv[0]), mem(n[0])), mulr(fl(cv[1]), mem(n[1]))), mulr(fl(cv[2]), mem(n[2])))
    if lt_ordered(facing, ZERO):
        facing = ZERO
        path.append('facing_clamped')
    f6 = mulr(facing, facing)
    for _ in range(4):
        f6 = mulr(f6, facing)
    size = bits(mulr(fl(K_CORONA), f6))
    inten = ftol_low32(mulr(f6, mem(K_BYTE))) & 0xFF
    m.push([4, c['corona'], 0, 200, 200, 255, inten, STACK] + o + [size, K_FAR, 0, 0, 1, 0, 0, 0, 0, 0x3FC00000, 0, 0x41700000, 0, 0])
    # anillo
    vcount = icount = 0
    glow, light = [], []
    maxsq = 0
    tex_v = bits(addr(mulr(ONE, mem(K_TEXSCALE)), mem(K_TEXBIAS)))
    verts, idx = [], []
    spot20 = spot30 = None
    clamped = flipped = 0
    for i in range(41):
        up = [0, 0, 0x3F800000]
        side = normalise(cross(n, up))
        top = normalise(cross(side, n))
        s_bits, c_bits = trig_bits(mulr(from_int(i), mem(K_STEP)))
        side_sx, side_sy, side_sz = mulr(fl(side[0]), mem(s_bits)), mulr(fl(side[1]), mem(s_bits)), mulr(fl(side[2]), mem(s_bits))
        side_sxs, side_szs = bits(side_sx), bits(side_sz)
        near_sxs = bits(mulr(fl(side_sxs), mem(base)))
        near_sys = bits(mulr(side_sy, mem(base)))
        near_sz = mulr(side_sz, mem(base))
        near_base_x = addr(fl(near_sxs), mem(o[0]))
        near_base_y = bits(addr(fl(near_sys), mem(o[1])))
        near_base_z = bits(addr(near_sz, mem(o[2])))
        top_cx, top_cy = mulr(fl(top[0]), mem(c_bits)), mulr(fl(top[1]), mem(c_bits))
        top_cxs, top_cys, top_czs = bits(top_cx), bits(top_cy), bits(mulr(fl(top[2]), mem(c_bits)))
        top_bxs, top_bys = bits(mulr(top_cx, mem(base))), bits(mulr(top_cy, mem(base)))
        top_bzs = bits(mulr(fl(top_czs), mem(base)))
        near = [bits(addr(fl(top_bxs), near_base_x)), bits(addr(fl(top_bys), mem(near_base_y))), bits(addr(fl(top_bzs), mem(near_base_z)))]
        far_sxs = bits(mulr(fl(side_sxs), mem(radius)))
        far_sys = bits(mulr(side_sy, mem(radius)))
        far_sz = mulr(fl(side_szs), mem(radius))
        far_base_x = addr(fl(far_sxs), mem(farp[0]))
        far_base_y = bits(addr(fl(far_sys), mem(farp[1])))
        far_base_z = bits(addr(far_sz, mem(farp[2])))
        top_txs, top_tys, top_tzs = bits(mulr(fl(top_cxs), mem(radius))), bits(mulr(fl(top_cys), mem(radius))), bits(mulr(fl(top_czs), mem(radius)))
        far_fx = addr(far_base_x, fl(top_txs))
        far_fy = addr(fl(top_tys), fl(far_base_y))
        far_fzs = bits(addr(fl(top_tzs), fl(far_base_z)))
        ratio = bits(divr(subr(fl(near[2]), mem(t[2])), subr(fl(near[2]), mem(far_fzs))))
        delta_x = bits(subr(far_fx, mem(near[0])))
        delta_y = subr(far_fy, mem(near[1]))
        delta_z = subr(fl(far_fzs), mem(near[2]))
        scaled_x = bits(mulr(fl(delta_x), mem(ratio)))
        scaled_y = bits(mulr(delta_y, mem(ratio)))
        scaled_z = mulr(delta_z, mem(ratio))
        spot = [bits(addr(fl(scaled_x), mem(near[0]))), bits(addr(fl(scaled_y), mem(near[1]))), bits(addr(scaled_z, mem(near[2])))]
        if i == 20:
            spot20 = list(spot)
        elif i == 30:
            spot30 = list(spot)
        ex, ey, ez = subr(fl(spot[0]), mem(near[0])), subr(fl(spot[1]), mem(near[1])), subr(fl(spot[2]), mem(near[2]))
        length = sqrtr(addr(addr(mulr(ez, ez), mulr(ey, ey)), mulr(ex, ex)))
        if gt(length, fl(K_FAR)):
            clamped += 1
            d = normalise([bits(ex), bits(ey), bits(ez)])
            cx, cy = mulr(fl(d[0]), mem(K_FAR)), mulr(fl(d[1]), mem(K_FAR))
            czs = bits(mulr(fl(d[2]), mem(K_FAR)))
            spot = [bits(addr(cx, mem(near[0]))), bits(addr(cy, mem(near[1]))), bits(addr(fl(czs), mem(near[2])))]
        verts.append(near)
        verts.append(spot)
        to_near = normalise([bits(subr(fl(near[k]), mem(o[k]))) for k in range(3)])
        cosine = addr(addr(mulr(fl(to_near[0]), mem(cv[0])), mulr(fl(to_near[2]), mem(cv[2]))), mulr(fl(to_near[1]), mem(cv[1])))
        if lt_ordered(cosine, ZERO):
            cosine = chs(cosine)
            flipped += 1
        square = mulr(cosine, cosine)
        if gt(square, fl(maxsq)):
            maxsq = bits(square)
        glow += [tex_v, 0]
        light += [bits(square), bits(square)]
        vi = vcount
        if i != 40:
            idx += [vi, vi + 3, vi + 1]
            if gt(fl(base), ZERO):
                idx += [vi, vi + 2, vi + 3]
        vcount += 2
    icount = len(idx)
    if clamped:
        path.append('clamp')
    if flipped:
        path.append('cos_flip')
    path.append('base_pos' if gt(fl(base), ZERO) else 'base_nonpos')
    # colores
    inv = divr(ONE, fl(maxsq))
    colors = []
    for v in range(vcount):
        lit = mulr(mulr(fl(light[v]), mem(glow[v])), inv)
        gray = ftol_low32(mulr(fl(K_GRAY), lit)) & 0xFF
        blue = ftol_low32(mulr(lit, mem(K_BYTE))) & 0xFF
        colors.append((gray << 16) | (gray << 8) | blue)
    if icount > 0:
        m.push([6, VERTICES_ADDR, vcount, 0, 0x18])
        if c['transform_ok']:
            m.push([7, 3, INDICES_ADDR, icount])
            m.push([8])
            path.append('transform_ok')
        else:
            path.append('transform_fail')
    u1 = [bits(subr(fl(spot20[k]), mem(t[k]))) for k in range(3)]
    u2 = [bits(subr(fl(spot30[k]), mem(t[k]))) for k in range(3)]
    out = list(t) + u1 + u2
    if (c['shadow'] & 0xFF) == 0:
        path.append('shadow_off')
    else:
        pos = [t[0], t[1], bits(addr(fl(t[2]), mem(K_RISE)))]
        top_x, top_y = bits(mulr(fl(u1[0]), mem(K_SPOT))), bits(mulr(fl(u1[1]), mem(K_SPOT)))
        right_x, right_y = bits(mulr(fl(u2[0]), mem(K_SPOT))), bits(mulr(fl(u2[1]), mem(K_SPOT)))
        if not lt_ordered(sqrtr(addr(mulr(fl(top_x), mem(top_x)), mulr(fl(top_y), mem(top_y)))), fl(K_FAR)):
            path.append('shadow_len_top')
        elif not lt_ordered(sqrtr(addr(mulr(fl(right_x), mem(right_x)), mulr(fl(right_y), mem(right_y)))), fl(K_FAR)):
            path.append('shadow_len_right')
        else:
            dx, dy = subr(fl(t[0]), mem(cam[0])), subr(fl(t[1]), mem(cam[1]))
            distance = sqrtr(addr(mulr(dy, dy), mulr(dx, dx)))
            if gt(distance, fl(K_RANGE)):
                path.append('shadow_far')
            else:
                shade = mulr(mulr(subr(ONE, mulr(distance, mem(K_FALLOFF))), mem(power)), mem(K_HALF))
                gray = ftol_low32(mulr(fl(K_GRAY), shade)) & 0xFF
                blue = ftol_low32(mulr(fl(K_BYTE), shade)) & 0xFF
                shade_i = ftol_low32(mulr(shade, mem(K_INTENSITY))) & 0xFFFF
                m.push([5, 2, c['tex'], STACK] + pos + [top_x, top_y, right_x, right_y, shade_i, gray, gray, blue, 0x41700000, 1, 0x3F800000, 0, 0])
                path.append('shadow_draw')
    vhex = ''.join(struct.pack('<I', w).hex() for v in range(vcount) for w in verts[v] + [colors[v]])
    ihex = ''.join(struct.pack('<H', w & 0xFFFF).hex() for w in idx)
    return dict(counters=[vcount, icount], vertices=vhex, indices=ihex, out=out, ev_hash=m.hash, ev_count=m.count, events=m.events, path='+'.join(path))


def expected(row_input, observed=None, case=0):
    return model_light(row_input)


def check_self():
    assert to_f32_bits(r24(mul(from_int(5000), from_bits(0x38D1B717)))) == 0x3F000000
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(sqrt_v(from_int(2))) == 0x3FB504F3
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert ftol_low32(from_int(-7)) == -7
    assert is_zero_ok()
    s, c = trig_bits(from_int(0))
    assert s == 0 and c == 0x3F800000
    s, c = trig_bits(mulr(from_int(10), from_bits(K_STEP)))     # pi / 2 aproximado en float: sin = 1.0
    assert s == 0x3F800000, hex(s)
    print('ok')


def is_zero_ok():
    x = from_bits(0x80000000)
    return x.kind == 'f' and x.m == 0 and from_bits(0xFFC00000).kind == 'nan'


def check_row_obj(row):
    """(mensajes de desacuerdo, ruta del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('counters', 'vertices', 'indices', 'out', 'ev_hash', 'ev_count', 'events') if o[k] != e[k]]
        if o['unexpected_virtuals'] != 0:
            bad.append('unexpected_virtuals')
        if bad or any(o['changes']):
            msgs.append('DIFIERE %s caso %s campos %s changes %s %s' % (side, row['case'], bad, any(o['changes']), _first_diff(o, e) if bad else ''))
    return msgs, e['path']


def check_row(line):
    row = json.loads(line)
    if row.get('event') != 'comparison':
        return None
    msgs, path = check_row_obj(row)
    return row['case'], msgs, path


def _first_diff(o, e):
    for i, (g, w) in enumerate(zip(o['events'], e['events'])):
        if g != w:
            return 'evento %d: %s != %s' % (i, g, w)
    if len(o['events']) != len(e['events']):
        return 'numero de eventos %d != %d' % (len(o['events']), len(e['events']))
    for k in ('counters', 'out', 'ev_count', 'ev_hash'):
        if o[k] != e[k]:
            return '%s %s != %s' % (k, o[k], e[k])
    for k in ('vertices', 'indices'):
        if o[k] != e[k]:
            at = next((i for i in range(min(len(o[k]), len(e[k]))) if o[k][i] != e[k][i]), min(len(o[k]), len(e[k])))
            return '%s difiere en el caracter %d (%s != %s)' % (k, at, o[k][at:at + 16], e[k][at:at + 16])
    return ''


def main(path):
    from concurrent.futures import ProcessPoolExecutor
    lines = [l for l in open(path, encoding='utf-8') if '"event":"comparison"' in l.replace(' ', '')]
    bad = checked = 0
    with ProcessPoolExecutor() as ex:
        for res in ex.map(check_row, lines, chunksize=16):
            if res is None:
                continue
            checked += 1
            for msg in res[1]:
                bad += 1
                if bad <= 8:
                    print(msg)
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    if len(sys.argv) == 1:
        check_self()
        sys.exit(0)
    sys.exit(main(sys.argv[1]))
