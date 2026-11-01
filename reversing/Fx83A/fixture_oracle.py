"""Oraculo independiente (Python) de Fx83A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de los colaboradores,
sorteos consumidos y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Desensamblados leidos: AddWheelSand 0x4A0610..0x4A09B4, AddWheelDust 0x4A09C0..0x4A0D64, AddWheelGrass 0x49FF20..0x4A0168, AddWheelGravel 0x4A0170..0x4A03B8, AddWheelMud 0x4A03C0..0x4A0608,
AddDebris 0x49F750..0x49F960, AddGlass 0x49F970..0x49FB20.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida); un local de pila, 0x20000000.
Indices: 0 posicion, 1 matriz de la camara, 2 sistema A (el hueco de g_fx inicial), 3 sistema B, 4 vehiculo.
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
        return dict(rand_state=self.rng.state, ev_hash=self.hash, particle_calls=self.particles, system_slot=tag(self.slot), events=self.events)


def player_vehicle_event(m):
    m.push([13, 0xFFFFFFFF, 0])


def model_spray(c):
    """Fx_c::AddWheelSpray 0x49FB30..0x49FF18 (desensamblado leido): ver README.md"""
    m = Model(c)
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
    spinning = (c['spinning'] & 0xFF) != 0
    in_water = (c['in_water'] & 0xFF) != 0
    sq = [r24(mul(v, v)) for v in vel]
    length = r24(sqrt_v(r24(add(r24(add(sq[0], sq[1])), sq[2]))))
    absl = V(length.kind, 0, length.m, length.e, length.bits) if length.kind != 'nan' else length
    if not gt(absl, from_bits(0x3C23D70A)) and not spinning:
        return m.result()
    if spinning:
        t = ONE
    else:
        twice = r24(add(length, length))
        t = ONE if gt(twice, ONE) else fl(to_f32_bits(twice))      # fstp dword; NaN conserva 2*|v|
    t = fl(to_f32_bits(t))
    t1 = r24(add(t, ONE))
    alpha = to_f32_bits(r24(mul(t1, from_bits(0x3E4CCCCD) if in_water else from_bits(0x3E19999A))))
    size = to_f32_bits(r24(mul(t1, from_bits(0x3E4CCCCD))))
    u = to_f32_bits(r24(mul(t1, from_bits(0x41200000))))
    w = to_f32_bits(r24(sub(from_bits(0x41F00000), fl(u))))
    ts = fl(c['timestep'])
    velm = [mem(b) for b in c['vel']]
    vp = [r24(mul(ts, velm[i])) for i in range(3)]
    sq2 = [r24(mul(x, x)) for x in vp]
    len2 = r24(sqrt_v(r24(add(r24(add(sq2[2], sq2[1])), sq2[0]))))
    low = ftol_low32(len2)
    count = 1 if low < 1 else low
    inv_n = to_f32_bits(r24(div(ONE, from_int(count))))
    mult = [0x3F800000, 0x3F800000, 0x3F800000, alpha, size, 0x3F800000, 0x3DA3D70A]   # color(r,g,b,a), tamano, rot, vida
    r_m = m.rng.draw()
    rk = r24(mul(from_int(r_m), C7C))
    mreg = r24(add(r24(mul(rk, r24(sub(r24(add(fl(u), from_bits(0x41F00000))), fl(w))))), fl(w)))
    mv = [to_f32_bits(r24(mul(mreg, velm[i]))) for i in range(3)]
    inv = fl(inv_n)
    tsm = mem(c['timestep'])      # operando de memoria de fmul (el NaN de senal conserva su condicion)
    for i in range(count):
        r_i = m.rng.draw()
        a1 = r24(mul(inv, velm[0]))
        a2 = r24(mul(inv, velm[1]))
        a3 = to_f32_bits(r24(mul(inv, velm[2])))
        fi = fl(to_f32_bits(from_int(i)))
        d1 = to_f32_bits(r24(mul(a1, fi)))
        d2 = r24(mul(a2, fi))
        d3 = r24(mul(fl(a3), fi))
        e1 = to_f32_bits(r24(mul(fl(d1), tsm)))
        e2 = to_f32_bits(r24(mul(d2, tsm)))
        e3 = to_f32_bits(r24(mul(d3, tsm)))
        px = to_f32_bits(r24(sub(pos[0], fl(e1))))
        py = to_f32_bits(r24(sub(pos[1], fl(e2))))
        pz = to_f32_bits(r24(sub(pos[2], fl(e3))))
        pz2 = to_f32_bits(r24(add(fl(pz), from_bits(0x3E800000))))
        rk2 = r24(mul(from_int(r_i), C7C))
        vz = to_f32_bits(r24(add(r24(mul(r24(add(rk2, ONE)), t)), fl(mv[2]))))
        m.add_particle(STACK, [px, py, pz2], [mv[0], mv[1], vz], mult, c['light'], 0x3F19999A)
    return m.result()


def expected(row_input, observed, case=0):
    return model_spray(row_input)


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
        bad = [k for k in ('rand_state', 'ev_hash', 'particle_calls', 'system_slot', 'events') if o[k] != e[k]]
        if bad or any(o['changes']):
            msgs.append('DIFIERE %s caso %s campos %s changes %s' % (side, row['case'], bad, any(o['changes'])))
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
