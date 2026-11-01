"""Oraculo independiente (Python) de Veh83C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6DE880..0x6DF3B9), lo que debe observarse de
CVehicle::AddSingleWheelParticles (traza EN ORDEN de los colaboradores con todos sus argumentos, sorteos de rand() consumidos, hash de todos los eventos, sistema de particulas del hueco de g_fx
y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Flujo leido (thiscall, 11 dwords; ret 0x2C; devuelve siempre false):
  FindPlayerVehicle(-1, false); desactivado (+0x1C bit 17) -> sale. (dz^2 + dy^2) + dx^2 de camara - posicion: > 625 y sin bAlwaysSkidMarks (+0x42D bit 0) -> sale; cadencia: > 400 una de cada 4, > 64 o sin
  vehiculo del jugador una de cada 2 (suma del byte bajo del modelo y el contador). arg2 >= 1 -> sale. IsWater(superficie B) o, tocando el agua (+0x40 bit 27), GetWaterLevel >= z -> en el agua.
  Chispas (wheelStatus == 1, a3 > 0.1 o estado 1, friccion 1): AddSparks y ReportCollision. MakeDirty. Estado 1: AddWheelDirtAndWater(.., 1, ..) + particula de giro (4 sorteos);
  estados 2 y 3: AddWheelDirtAndWater(.., 0, ..) + rastro de particulas; resto: sin particulas. Huella (CSkidmarks::RegisterOne con FindWheelWidth) salvo wheelStatus == 1 o en el agua.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000.
Indices: 0 vehiculo, 1 matriz del vehiculo, 2 matriz de la camara, 3 sistema A (el hueco de g_fx inicial), 4 sistema B, 5 CColPoint, 6 posicion de la rueda, 7 byte de sangre, 8 jugador, 9 vehiculo del jugador.
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



K625, K400, K64 = from_bits(0x441C4000), from_bits(0x43C80000), from_bits(0x42800000)
ONE = from_bits(0x3F800000)
K01, K02, K003, K004, K015 = (from_bits(x) for x in (0x3DCCCCCD, 0x3E4CCCCD, 0x3CF5C28F, 0x3D23D70A, 0x3E19999A))
M50, K25, K32, M12, M30 = (from_bits(x) for x in (0xC2480000, 0x40200000, 0x42000000, 0xC1400000, 0xC1F00000))
K0001, K08, K09, K05, K03, C7C = (from_bits(x) for x in (0x38D1B717, 0x3F4CCCCD, 0x3F666666, 0x3F000000, 0x3E99999A, 0x38000100))
ZERO = from_bits(0)
VEH, MATV, CAMM, SYSA, SYSB, COL, POS, BLOOD, PL, PV = range(10)
SYS_A, SYS_B = SYSA, SYSB


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.count = 0
        self.hash = 2166136261
        self.slot = SYS_A

    def push(self, e):
        # el banco guarda 256 eventos; los siguientes sobrescriben la ultima ranura
        if len(self.events) < 256:
            self.events.append(e)
        else:
            self.events[255] = e
        self.count += 1
        feed = [e[0], len(e) - 1] + list(e[1:])
        for v in feed:
            for b in range(4):
                self.hash ^= (v >> (8 * b)) & 0xFF
                self.hash = (self.hash * 16777619) & M32

    def result(self, path):
        return dict(rand_state=self.rng.state, ev_hash=self.hash, ev_count=self.count, system_slot=tag(self.slot), events=self.events, path=path)


def bits(v):
    return to_f32_bits(v)


def rand_in_range(m, lo, hi):
    """0x41BD90: rand() (entero del CRT) * 1/32767 aproximado (0x38000100), por (max - min), mas min; resultado en el registro (24 bits)"""
    r = m.rng.draw()
    t = r24(mul(from_int(r), C7C))
    d = r24(sub(fl(hi), mem(lo)))
    return r24(add(r24(mul(t, d)), mem(lo)))


def magnitude(v3):
    """CVector::Magnitude (0x4082C0): sqrt((x*x + y*y) + z*z), resultado en el registro"""
    x, y, z = (fl(b) for b in v3)
    s = r24(add(r24(add(r24(mul(x, x)), r24(mul(y, y)))), r24(mul(z, z))))
    return r24(sqrt_v(s))


def normalise_and_mag(v3):
    """CVector::NormaliseAndMag (0x59C970): devuelve (vector nuevo en bits, fuerza en bits)"""
    x, y, z = (fl(b) for b in v3)
    s = r24(add(r24(add(r24(mul(x, x)), r24(mul(y, y)))), r24(mul(z, z))))
    if s.kind != 'nan' and not gt(s, ZERO):
        return [0x3F800000, v3[1], v3[2]], 0x3F800000
    inv = r24(div(ONE, r24(sqrt_v(s))))
    out = [bits(r24(mul(inv, mem(b)))) for b in v3]
    return out, bits(r24(div(ONE, inv)))


def lt_ordered(a, b):
    """fcomp a, b; test ah,5; jp: a < b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and gt(b, a)


def ge_ordered(a, b):
    """fcomp a, b; test ah,1; jne -> no salta: a >= b ordenado"""
    return a.kind != 'nan' and b.kind != 'nan' and not gt(b, a)


def model_wheel(c):
    """CVehicle::AddSingleWheelParticles 0x6DE880..0x6DF3B9 (desensamblado leido): ver README.md"""
    m = Model(c)
    veh = tag(VEH)
    colp, posp = tag(COL), tag(POS)
    m.push([1, 0xFFFFFFFF, 0])                                    # FindPlayerVehicle(-1, false)
    pv = c['pv_kind'] != 0
    if c['offscreen']:
        return m.result('offscreen')
    cam = c['cam']
    s = dist_zyx(cam, [c['mat'][9], c['mat'][10], c['mat'][11]])
    if gt(s, K625) and not c['always_skid']:
        return m.result('far')
    key = ((c['model'] & 0xFF) + c['frame']) & M32
    path = []
    if gt(s, K400):
        can = (key & 3) == 0
        path.append('gate400')
    elif gt(s, K64) or not pv:
        can = (key & 1) == 0
        path.append('gate64')
    else:
        can = True
    a2 = fl(c['a2'])
    if not (lt_ordered(a2, ONE) or a2.kind == 'nan'):
        return m.result('arg2_exit')
    in_water = False
    surf_b = c['surf_b'] & 0xFF
    m.push([2, surf_b])
    if c['is_water'] & 0xFF:
        in_water = True
        path.append('water_surface')
    elif c['touch_water']:
        m.push([3, c['pos'][0], c['pos'][1], c['pos'][2], STACK, 1, 0])
        if c['water_ret'] & 0xFF:
            if ge_ordered(fl(c['water_level']), mem(c['pos'][2])):
                in_water = True
                path.append('water_level')
    a3 = fl(c['a3'])
    produce = 1 if lt_ordered(a3, ONE) else 0
    ws = c['ws'] & M32
    wstatus = c['wstatus'] & M32
    vel = c['vel']
    if wstatus == 1 and (gt(a3, K01) or ws == 1):
        m.push([4, surf_b])
        if (c['friction'] & M32) == 1:
            sp = [bits(r24(mul(fl(vel[0]), M50))), bits(r24(mul(fl(vel[1]), M50))), bits(r24(add(r24(mul(fl(vel[2]), M50)), K25)))]
            cnt = bits(r24(mul(a3, K32)))
            if ws == 1 and lt_ordered(a3, K02):
                st = c['gas']
                fwd = [c['mat'][3], c['mat'][4], c['mat'][5]]
                t1 = [bits(r24(mul(fl(st), mem(fwd[i])))) for i in range(3)]
                t2 = [bits(r24(mul(fl(0xC1400000), mem(t1[i])))) for i in range(3)]
                sp = [t2[0], t2[1], bits(r24(add(fl(t2[2]), K25)))]
                cnt = 0x41200000
                path.append('sparks_gas')
            vec, force = normalise_and_mag(sp)
            count = ftol_low32(fl(cnt)) & M32
            m.push([5, posp] + list(c['pos']) + vec + [force, count] + list(vel) + [1, 0x3DCCCCCD, 0x3E99999A])
            m.push([6, 0])
            pp = tag(PL) if c['pp_kind'] else 0
            m.push([7, veh, pp, c['surf_a'] & 0xFF, surf_b, posp, 0, 0x3F800000, 0x3F800000, 0, 1])
            path.append('sparks')
    m.push([8, veh, colp])                                        # MakeDirty
    sub_type = c['subtype'] & M32

    def adjust(mult):
        if sub_type in (9, 2):
            mult[4] = bits(r24(mul(fl(mult[4]), K05)))
            path.append('subtype_half')
            return True
        if sub_type == 10:
            mult[4] = bits(r24(mul(fl(mult[4]), K02)))
            mult[6] = bits(r24(mul(fl(mult[6]), K03)))
            path.append('subtype_10')
            return True
        return False

    def dirt(spin):
        m.push([9, veh, colp, produce, spin, 1 if in_water else 0])
        return c['dirt_ret'] != 0

    def trail():
        mult = [0x3F666666, 0x3F666666, 0x3F800000, 0x3F000000, 0x3F333333, 0x3F800000, 0x3E99999A]
        scale = 0x40400000 if adjust(mult) else 0x40000000
        d = [bits(r24(mul(fl(c['timestep']), mem(vel[i])))) for i in range(3)]
        prod = r24(mul(magnitude(d), fl(scale)))
        low = ftol_low32(prod)
        n = 1 if low < 1 else low
        nf = fl(bits(from_int(n)))
        for i in range(n):
            res = rand_in_range(m, 0x3F000000, 0x3F800000)
            blue = bits(res)
            gr = bits(r24(mul(res, K09)))
            mult[0], mult[1], mult[2] = gr, gr, blue
            tt = bits(r24(sub(ONE, r24(div(from_int(i), nf)))))
            tmp = [bits(r24(mul(fl(tt), mem(d[k])))) for k in range(3)]
            pos = [bits(r24(sub(fl(c['pos'][k]), mem(tmp[k])))) for k in range(3)]
            m.push([10, tag(m.slot), STACK] + pos + [0, 0, 0x3F000000, 0x3E99999A] + mult + [0xBF800000, c['bright'], 0x3F19999A, 0])
            if c['swap_slot']:
                m.slot = SYS_B if m.slot == SYS_A else SYS_A
        path.append('trail')

    def spin_particle():
        mult = [0x3F666666, 0x3F666666, 0x3F800000, 0x3F000000, 0x3F800000, 0x3F800000, 0x3F000000]
        if gt(magnitude(vel), K015):
            mult[3] = 0x3E99999A
            mult[4] = 0x3F000000
        adjust(mult)
        sab = fabs_bits(c['gas'])
        mx = bits(r24(mul(r24(mul(fl(sab), mem(vel[0]))), M30)))
        vx = bits(rand_in_range(m, 0, mx))
        my = bits(r24(mul(r24(mul(fl(sab), mem(vel[1]))), M30)))
        vy = bits(rand_in_range(m, 0, my))
        rem = m.rng.draw() % 10000
        vz = bits(r24(mul(r24(mul(from_int(rem), K0001)), K08)))
        res = rand_in_range(m, 0x3F000000, 0x3F800000)
        gr = bits(r24(mul(res, K09)))
        mult[0], mult[1], mult[2] = gr, gr, bits(res)
        m.push([10, tag(m.slot), posp] + list(c['pos']) + [vx, vy, vz, 0] + mult + [0xBF800000, c['bright'], 0x3F19999A, 0])
        if c['swap_slot']:
            m.slot = SYS_B if m.slot == SYS_A else SYS_A
        path.append('spin_particle')

    def skid():
        if wstatus == 1:
            path.append('wstatus1_exit')
            return
        pos = list(c['pos'])
        idx = c['idx'] & M32
        sidx = signed32(idx)
        if (c['model'] & 0xFFFF) == 0x1D7:
            right = [c['mat'][0], c['mat'][1], c['mat'][2]]
            tmp = [bits(r24(mul(fl(0x3E19999A), mem(right[k])))) for k in range(3)]
            if sidx < 2:
                pos = [bits(r24(add(fl(tmp[k]), mem(pos[k])))) for k in range(3)]
                path.append('quad_front')
            else:
                pos = [bits(r24(sub(fl(pos[k]), mem(tmp[k])))) for k in range(3)]
                path.append('quad_back')
        if in_water:
            path.append('skid_water_exit')
            return
        m.push([12, veh, idx & 1])
        width = bits(fl(c['width']))
        m.push([11, idx, STACK] + pos + [c['mat'][3], c['mat'][4], c['skid'] & M32, tag(BLOOD), width])
        path.append('skid')

    if ws == 1:
        path.append('ws1')
        if dirt(1) and can:
            spin_particle()
        skid()
    elif ws in (2, 3):
        path.append('ws%d' % ws)
        if ws == 2 and (c['flags'] & 4):
            return m.result('+'.join(path + ['ws2_flag4_exit']))
        if gt(a3, K003):
            if dirt(0) and can:
                trail()
        skid()
    else:
        path.append('ws_default')
        if gt(a3, K003):
            dirt(0)
        idx = c['idx'] & M32
        moving = False
        if c['always_skid'] and (idx & 1):
            x, y = fl(vel[0]), fl(vel[1])
            moving = gt(r24(sqrt_v(r24(add(r24(mul(x, x)), r24(mul(y, y)))))), K004)
        if not (moving or (c['blood'] & 0xFF) or (c['flags'] & 2)):
            return m.result('+'.join(path + ['default_exit']))
        skid()
    return m.result('+'.join(path))


def expected(row_input, observed=None, case=0):
    return model_wheel(row_input)


def check_self():
    assert to_f32_bits(r24(mul(from_int(5000), from_bits(0x38D1B717)))) == 0x3F000000
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(sqrt_v(from_int(2))) == 0x3FB504F3
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert ftol_low32(from_int(-7)) == -7
    assert ftol_low32(V('f', 0, 1, 63)) == 0
    assert ((((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF) == 41
    print('ok')


def check_row_obj(row):
    """(mensajes de desacuerdo, ruta del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('rand_state', 'ev_hash', 'ev_count', 'system_slot', 'events') if o[k] != e[k]]
        if o['ret_al'] != 0:
            bad.append('ret_al')
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
    return 'rand %x/%x slot %x/%x count %d/%d hash %x/%x' % (o['rand_state'], e['rand_state'], o['system_slot'], e['system_slot'], o['ev_count'], e['ev_count'], o['ev_hash'], e['ev_hash'])


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
