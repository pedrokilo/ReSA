"""Oraculo independiente (Python) de Veh83D: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6D73B0..0x6D7BBF), lo que debe observarse de
CVehicle::ProcessBikeWheel (traza EN ORDEN de los colaboradores con todos sus argumentos, sorteos de rand() consumidos, hash de todos los eventos, estado de la rueda, estaticas 0xC1CDAF..0xC1CDB2
y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Flujo leido (thiscall, 14 dwords; ret 0x38; void): velocidad hacia delante = (fy*cy + cz*fz) + fx*cx (guardada); estaticas 0xC1CDB2 (freno: brake != 0), 0xC1CDB1 (empuje: sin freno y thrust != 0), 0xC1CDB0 (thrust < 0 sin freno);
0xC1CDAF (ya patinaba: se pone a 1 si wheelState != 0) -> agarre = dt * adhesion (x perdida de traccion si ya patinaba); lateral = -(derecha . velocidad) / ruedas (special 2 y 3 lo omiten; con wheelStatus == 1 le suma ruido de rand());
con empuje: fuerza = thrust y lateral acotado a +-agarre; sin empuje y con velocidad: fuerza = -v / ruedas, freno pasivo segun tipo (BMX, moto, otros) y acotacion al freno, o rueda fija (3) si el freno supera el agarre;
circulo de agarre: si lateral^2 + fuerza^2 > agarre^2 se reescalan y el estado pasa a 1 (empuje y v < 0.1) o 2; si no y destab < 1 se reduce el lateral. Fuerzas: ApplyMoveForce y 1 o 2 ApplyTurnForce.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000.
Indices: 0 vehiculo, 1 matriz del vehiculo, 2 CHandlingData, 3 vectores de argumentos (fwd +0, right +0x10, cs +0x20, cp +0x30), 4 *wheelState, 5 wheelSpeed.
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
VEH, MATV, HAND, ARGS, STATE, SPEED = range(6)


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.count = 0
        self.hash = 2166136261

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


def is_zero(x):
    """igual a cero ordenado (fcom con 0.0 y `test ah,44h; jnp`): un NaN no es cero"""
    return x.kind == 'f' and x.m == 0


def lt_ordered(a, b):
    """fcomp a, b; test ah,5; jp: a < b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and gt(b, a)


def le_ordered(a, b):
    """a <= b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and not gt(a, b)


def chs(x):
    """fchs: invierte el signo tambien de un NaN (a diferencia de la resta, que no toca el signo del NaN)"""
    if x.kind == 'nan':
        return V('nan', bits=x.bits ^ 0x80000000, snan=x.snan)
    return neg_v(x)


def abs_v(x):
    if x.kind == 'nan':
        return x
    if x.kind == 'inf':
        return V('inf', s=0)
    return V('f', 0, x.m, x.e)


def model_wheel(c):
    """CVehicle::ProcessBikeWheel 0x6D73B0..0x6D7BBF (desensamblado leido): ver README.md"""
    m = Model(c)
    path = []
    fwd, right, cs, cp = c['fwd'], c['right'], c['cs'], c['cp']
    n = signed32(c['wheels'])
    thrust, brake = c['thrust'], c['brake']
    special = c['special'] & M32
    status = c['status'] & 0xFFFF
    wid = c['wheel_id'] & 0xFF
    veh = tag(VEH)
    # velocidad hacia delante: (y*y + z*z) + x*x, guardada como float
    fs_bits = bits(addr(addr(mulr(fl(fwd[1]), mem(cs[1])), mulr(fl(cs[2]), mem(fwd[2]))), mulr(fl(fwd[0]), mem(cs[0]))))
    # estaticas
    if not is_zero(fl(brake)):
        b2, b1, b0 = 1, 0, 0
    else:
        b2 = 0
        b1 = 0 if is_zero(fl(thrust)) else 1
        b0 = 1 if lt_ordered(fl(thrust), ZERO) else 0
    wstate0 = c['wstate'] & M32
    af_init = c['af'] & 0xFF
    if wstate0 != 0:
        af, dl = 1, True
        path.append('prev_state')
    else:
        af, dl = af_init, af_init != 0
    if dl:
        path.append('wasskid')
    state = 0
    adh = bits(mulr(fl(c['ts']), mem(c['adh'])))
    if dl:
        adh = bits(mulr(fl(adh), mem(c['tl'])))

    def result(tag_name):
        path.append(tag_name)
        return dict(rand_state=m.rng.state, ev_hash=m.hash, ev_count=m.count, events=m.events, wheel_state=state, flags=[af, b0, b1, b2], path='+'.join(path))

    # velocidad lateral
    v = ZERO
    if special != 2 and special != 3:
        rs = addr(addr(mulr(fl(right[2]), mem(cs[2])), mulr(fl(right[1]), mem(cs[1]))), mulr(fl(right[0]), mem(cs[0])))
        rs_bits = bits(rs)
        if not is_zero(rs):
            x = chs(divr(mem(rs_bits), from_int(n)))
            lat_bits = bits(x)
            v = x
            path.append('lat')
            if status == 1:
                lim_bits = fs_bits if lt_ordered(fl(fs_bits), mem(0x3DF5C28F)) else 0x3DF5C28F
                c2 = 0x3D4CCCCD
                c3 = bits(mulr(fl(c2), mem(0xBF800000)))
                r = m.rng.draw()
                unit = mulr(from_int(r), mem(0x38000100))
                span = subr(fl(c2), mem(c3))
                v = addr(mulr(addr(mulr(unit, span), mem(c3)), mem(lim_bits)), mem(lat_bits))
                path.append('status1')
        else:
            path.append('lat_zero')
    else:
        path.append('special')
    a_reg = fl(brake)
    force = 0
    if b1:
        path.append('drive')
        force = thrust
        if gt(v, ZERO):
            if gt(v, mem(adh)):
                v = fl(adh)
        else:
            neg = bits(chs(fl(adh)))
            if lt_ordered(v, mem(neg)):
                v = fl(neg)
    elif not is_zero(fl(fs_bits)):
        force = bits(chs(divr(mem(fs_bits), from_int(n))))
        if b2:
            path.append('brake')
        else:
            path.append('coast')
            gas = fl(c['gas'])
            g = chs(gas) if lt_ordered(gas, ZERO) else gas
            if lt_ordered(g, mem(0x3C23D70A)):
                fric, mh = c['fric'], c['hmass']
                if (c['subtype'] & M32) == 10:
                    if gt(fl(force), mem(0xBD4CCCCD)) and lt_ordered(fl(force), mem(0x3D4CCCCD)):
                        a_reg = divr(mulr(fl(fric), mem(0x3F000000)), addr(fl(mh), mem(0x43480000)))
                        path.append('idle_bmx')
                    else:
                        path.append('idle_bmx_skip')
                elif (c['vtype'] & M32) == 9:
                    a_reg = divr(mulr(fl(fric), mem(0x3F19999A)), addr(fl(mh), mem(0x43480000)))
                    path.append('idle_bike')
                else:
                    a_reg = divr(fl(fric), mem(mh))
                    if lt_ordered(fl(mh), mem(0x43FA0000)) or (c['model'] & 0xFFFF) == 0x1B9:
                        a_reg = mulr(a_reg, mem(0x3E4CCCCD))
                        path.append('idle_light')
                    else:
                        path.append('idle_car')
            else:
                path.append('idle_skip')
        if le_ordered(a_reg, mem(adh)):
            ff = fl(force)
            if gt(ff, ZERO):
                if gt(ff, a_reg):
                    force = bits(a_reg)
            else:
                neg_a = chs(a_reg)
                if lt_ordered(ff, neg_a):
                    force = bits(neg_a)
            path.append('clamp')
        elif gt(abs_v(fl(fs_bits)), mem(0x3BA3D70A)):
            state = 3
            path.append('fixed')
        else:
            path.append('open')
    else:
        path.append('nofwd')
    # circulo de agarre
    adhsq = bits(mulr(fl(adh), mem(adh)))
    energy = addr(mulr(v, v), mulr(fl(force), mem(force)))
    mag_bits = bits(energy)
    destab = c['destab']
    if gt(energy, mem(adhsq)):
        path.append('circle_out')
        if state != 3:
            state = 1 if (b1 and lt_ordered(fl(fs_bits), mem(0x3DCCCCCD))) else 2
            path.append('state_spin' if state == 1 else 'state_skid')
        tl_reg = ONE if dl else fl(c['tl'])
        k = mulr(divr(tl_reg, sqrtr(fl(mag_bits))), mem(adh))
        force = bits(mulr(fl(force), k))
        v = mulr(v, k)
        if lt_ordered(fl(destab), ONE):
            v = mulr(v, mem(destab))
            path.append('destab_out')
    else:
        path.append('circle_in')
        if lt_ordered(fl(destab), ONE):
            if not dl:
                destab = bits(mulr(fl(destab), mem(c['tl'])))
            q = mulr(mulr(fl(adhsq), mem(destab)), mem(destab))
            if lt_ordered(q, mem(mag_bits)):
                v = mulr(v, divr(mulr(fl(adh), mem(destab)), sqrtr(fl(mag_bits))))
                path.append('destab_in')
    if is_zero(fl(force)) and is_zero(v):
        return result('noforce')
    # fuerza total = lateral * derecha + fuerza * adelante
    fx = [bits(mulr(fl(force), mem(fwd[i]))) for i in range(3)]
    sx, sy = bits(mulr(v, mem(right[0]))), bits(mulr(v, mem(right[1])))
    sz = mulr(v, mem(right[2]))
    tot = [bits(addr(fl(sx), mem(fx[0]))), bits(addr(fl(sy), mem(fx[1]))), bits(addr(sz, mem(fx[2])))]
    m.push([1, STACK] + tot)                                       # CVector::Normalise (in situ)
    nrm = list(c['nrm'])
    length = bits(sqrtr(addr(addr(mulr(fl(tot[2]), mem(tot[2])), mulr(fl(tot[1]), mem(tot[1]))), mulr(fl(tot[0]), mem(tot[0])))))
    m.push([2, STACK, tag(ARGS, 0x30), STACK] + list(cp) + nrm)    # CrossProduct(&arm, &cp, &dir)
    crs = c['crs']
    big_g = mulr(fl(c['mass']), mem(length))
    q2 = addr(addr(mulr(fl(crs[0]), fl(crs[0])), mulr(fl(crs[1]), fl(crs[1]))), mulr(fl(crs[2]), fl(crs[2])))
    imp = mulr(divr(ONE, addr(divr(q2, mem(c['turn'])), divr(ONE, mem(c['mass'])))), mem(length))
    mf_force = [bits(mulr(fl(nrm[i]), big_g)) for i in range(3)]
    turn = [bits(mulr(fl(nrm[i]), imp)) for i in range(3)]
    m.push([3, veh] + mf_force)                                    # ApplyMoveForce
    mr, mfw = c['mr'], c['mf']
    tr = bits(addr(addr(mulr(fl(turn[2]), mem(mr[2])), mulr(fl(turn[1]), mem(mr[1]))), mulr(fl(turn[0]), mem(mr[0]))))
    p_r = bits(addr(addr(mulr(fl(mr[1]), mem(cp[1])), mulr(fl(mr[2]), mem(cp[2]))), mulr(fl(cp[0]), mem(mr[0]))))
    p_f = bits(addr(addr(mulr(fl(mfw[1]), mem(cp[1])), mulr(fl(mfw[2]), mem(cp[2]))), mulr(fl(cp[0]), mem(mfw[0]))))
    if wid != 1 or (not b2 and not b0):
        ax, ay = mulr(fl(p_r), mem(mr[0])), mulr(fl(p_r), mem(mr[1]))
        az = bits(mulr(fl(p_r), mem(mr[2])))
        point = [bits(subr(fl(cp[0]), ax)), bits(subr(fl(cp[1]), ay)), bits(subr(fl(cp[2]), mem(az)))]
        bx, by = mulr(fl(tr), mem(mr[0])), mulr(fl(tr), mem(mr[1]))
        bz = bits(mulr(fl(tr), mem(mr[2])))
        tx = bits(subr(fl(turn[0]), bx))
        ty, tz = subr(fl(turn[1]), by), subr(fl(turn[2]), mem(bz))
        fo = [bits(mulr(fl(tx), mem(0x40000000))), bits(mulr(ty, mem(0x40000000))), bits(mulr(tz, mem(0x40000000)))]
        m.push([4, veh] + fo + point)                              # ApplyTurnForce (componente sobre el eje derecho quitada)
        path.append('turn_a')
    else:
        path.append('turn_a_skip')
    pt = [bits(mulr(fl(p_f), mem(mfw[i]))) for i in range(3)]
    fr = [bits(mulr(fl(tr), mem(mr[i]))) for i in range(3)]
    m.push([4, veh] + fr + pt)                                     # ApplyTurnForce (fuerza sobre el eje derecho)
    return result('forces')


def expected(row_input, observed=None, case=0):
    return model_wheel(row_input)


def check_self():
    assert to_f32_bits(r24(mul(from_int(5000), from_bits(0x38D1B717)))) == 0x3F000000
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(sqrt_v(from_int(2))) == 0x3FB504F3
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert ftol_low32(from_int(-7)) == -7
    assert ((((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF) == 41
    assert is_zero(from_bits(0x80000000)) and not is_zero(from_bits(0xFFC00000))
    print('ok')


def check_row_obj(row):
    """(mensajes de desacuerdo, ruta del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('rand_state', 'ev_hash', 'ev_count', 'events', 'wheel_state', 'flags') if o[k] != e[k]]
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
    return 'rand %x/%x count %d/%d hash %x/%x estado %s/%s flags %s/%s' % (o['rand_state'], e['rand_state'], o['ev_count'], e['ev_count'], o['ev_hash'], e['ev_hash'], o['wheel_state'], e['wheel_state'], o['flags'], e['flags'])


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
