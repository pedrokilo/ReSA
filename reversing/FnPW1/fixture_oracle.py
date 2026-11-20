"""Oraculo independiente (Python) de FnPW1: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6D6C00..0x6D73AF), lo que debe observarse de
CVehicle::ProcessWheel (traza EN ORDEN de los colaboradores con todos sus argumentos, sorteos de rand() consumidos, hash de todos los eventos, estado de la rueda, estaticas 0xC1CDAC..0xC1CDAF
y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Las cargas de los NaN no se comparan (cualquier NaN en una palabra de float de un
evento se canoniza a 0x7FC00000 en el falso y aqui): "NaN con NaN".

Flujo leido (thiscall, 12 dwords; ret 0x30; void):
 - fs = (fy*cy + fx*cx) + cz*fz (guardada); freno = (brake != 0, NaN cuenta); empuje = no freno y thrust != 0 (NaN cuenta); estaticas 0xC1CDAC (ya patinaba), 0xC1CDAD (empuje), 0xC1CDAE (freno).
 - agarre = dt * adhesion (guardado); con estado de rueda != 0: estatica AC = 1, agarre *= perdida de traccion y, con estado 1 y vehiculo STATUS_PLAYER (0) o STATUS_REMOTE_CONTROLLED (8),
   agarre *= 1 - |gas| * 0.2.
 - lateral v = -(derecha . velocidad) / ruedas (el cero se decide con el registro x87, no con el float guardado); con wheelStatus == 1 le suma ruido de rand() (tope 0.3 sobre fs).
 - con empuje: fuerza = thrust y v acotado a +-agarre (comparacion estricta y ordenada); sin empuje y fs != 0: fuerza = -fs / ruedas, freno pasivo (moto, avion, coche con masa < 500 o RCBANDIT) y acotacion
   al freno; si freno > agarre o es NaN la rueda queda fija (3) cuando |fs| > 0.005.
 - circulo de agarre: si v^2 + fuerza^2 > agarre^2 el estado pasa a 1 (empuje y tl*agarre < |fuerza|) o 2 (salvo fija) y se reescalan fuerza y v con (perdida / raiz(m2)) * agarre.
 - sin fuerza ni v sale; si no: total = fuerza * adelante + v * derecha; con multiplicador antihundimiento > 0 y freno o empuje, direccion de giro separada; modulos (z+y)+x, normalizacion con 1/modulo
   (con modulo NaN tambien normaliza; con modulo <= 0 solo x = 1), producto vectorial (punto, giro; real en ambos lados), ApplyMoveForce(masa*velocidad*direccion), ApplyTurnForce(giro * (1 / (|cruz|^2/giro + 1/masa)) * velocidad_giro, punto).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Eventos: 3 ApplyMoveForce [this, fuerza(3)], 4 ApplyTurnForce [this, fuerza(3), punto(3)]. Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000.
Indices: 0 vehiculo, 1 CHandlingData, 2 vectores de argumentos (fwd +0, right +0x10, cs +0x20, cp +0x30), 3 *wheelState, 4 wheelSpeed.
"""
import json
import struct
import sys
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
STACK = 0x20000000


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



ZERO = from_bits(0)
ONE = from_bits(0x3F800000)
VEH, HAND, ARGS, STATE, SPEED = range(5)
# constantes de .rdata del EXE leidas del binario (direccion -> patron)
K_LIM = 0x3E99999A      # 0x8D34B8 (0.3)
K_MOD = 0x3E051EB8      # 0x8D34B4 (0.13)
K_NEG = 0xBF800000      # 0x858C1C (-1.0)
K_RAND = 0x38000100     # 0x858C7C (1/32767)
K_SPIN = 0x3E4CCCCD     # 0x8D34C4 (0.2)
K_TRAC = 0x3E99999A     # 0x8D34C0 (0.3)
K_FS = 0x3E19999A       # 0x858FCC (0.15)
K_FIX = 0x3BA3D70A      # 0x858B4C (0.005)
K_GAS = 0x3C23D70A      # 0x858C58 (0.01)
K_BIKE = 0x3F19999A     # 0x858CC8 (0.6)
K_200 = 0x43480000      # 0x858A48
K_500 = 0x43FA0000      # 0x858B58
K_TENTH = 0x3DCCCCCD    # 0x858B1C (0.1)
K_RC = 0x3E4CCCCD       # 0x858CC4 (0.2)
K_HALF = 0x3F000000     # 0x858B8C
K_ONE = 0x3F800000      # 0x858624
CANON = 0x7FC00000


def canon(w):
    return CANON if is_nan(w) else w


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.rng = Rng(c['rand_seed'])
        self.events = []
        self.count = 0
        self.hash = 2166136261

    def push(self, e, float_from, float_to):
        e = list(e)
        for i in range(float_from, float_to + 1):
            e[i] = canon(e[i])
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
    """a < b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and gt(b, a)


def le_ordered(a, b):
    """a <= b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and not gt(a, b)


def chs(x):
    """fchs: invierte el signo tambien de un NaN"""
    if x.kind == 'nan':
        return V('nan', bits=x.bits ^ 0x80000000, snan=x.snan)
    return neg_v(x)


def abs_v(x):
    if x.kind == 'nan':
        return V('nan', bits=x.bits & 0x7FFFFFFF, snan=x.snan)
    if x.kind == 'inf':
        return V('inf', s=0)
    return V('f', 0, x.m, x.e)


def model_wheel(c):
    """CVehicle::ProcessWheel 0x6D6C00..0x6D73AF (desensamblado leido): ver el docstring"""
    m = Model(c)
    path = []
    fwd, right, cs, cp = c['fwd'], c['right'], c['cs'], c['cp']
    n = signed32(c['wheels'])
    thrust, brake = c['thrust'], c['brake']
    status = c['status'] & 0xFFFF
    wid = c['wheel_id'] & 0xFF
    veh = tag(VEH)
    player = ((c['vstat'] & 0xFF) >> 3) in (0, 8)
    ts, gas, tl, mass, turn_mass, hmass = c['ts'], c['gas'], c['tl'], c['mass'], c['turn'], c['hmass']
    # velocidad hacia delante: (fy*cy + fx*cx) + cz*fz, guardada como float
    fs_bits = bits(addr(addr(mulr(fl(fwd[1]), mem(cs[1])), mulr(fl(fwd[0]), mem(cs[0]))), mulr(fl(cs[2]), mem(fwd[2]))))
    # estaticas
    braking = 0 if is_zero(fl(brake)) else 1
    if braking:
        driving = 0
    else:
        driving = 0 if is_zero(fl(thrust)) else 1
    af = c['af'] & 0xFF
    adh = bits(mulr(fl(ts), mem(c['adh'])))
    wstate0 = c['wstate'] & M32
    if wstate0 != 0:
        af = 1
        path.append('prev_state')
        adh = bits(mulr(fl(adh), mem(tl)))
        if wstate0 == 1 and player:
            path.append('prev_spin_loss')
            loss = subr(fl(K_ONE), mulr(abs_v(fl(gas)), mem(K_SPIN)))
            adh = bits(mulr(loss, mem(adh)))
    state = 0

    def result(tag_name):
        path.append(tag_name)
        return dict(rand_state=m.rng.state, ev_hash=m.hash, ev_count=m.count, events=m.events, wheel_state=state, flags=[af, driving, braking, 0], path='+'.join(path))

    # velocidad lateral: (ry*cy + rz*cz) + rx*cx; el cero se decide con el registro de la x87
    rs = addr(addr(mulr(fl(right[1]), mem(cs[1])), mulr(fl(right[2]), mem(cs[2]))), mulr(fl(right[0]), mem(cs[0])))
    rs_bits = bits(rs)
    v = ZERO
    if not is_zero(rs):
        x = chs(divr(mem(rs_bits), from_int(n)))
        lat_bits = bits(x)
        v = x
        path.append('lat')
        if status == 1:
            lim_bits = fs_bits if lt_ordered(fl(fs_bits), mem(K_LIM)) else K_LIM
            c3 = bits(mulr(fl(K_MOD), mem(K_NEG)))
            r = m.rng.draw()
            unit = mulr(from_int(r), mem(K_RAND))
            span = subr(fl(K_MOD), mem(c3))
            v = addr(mulr(addr(mulr(unit, span), mem(c3)), mem(lim_bits)), mem(lat_bits))
            path.append('burst')
    else:
        path.append('lat_zero')
    force = 0
    if driving:
        path.append('drive')
        force = thrust
        if gt(v, ZERO):
            if gt(v, mem(adh)):
                v = fl(adh)
                path.append('clamp_pos')
        else:
            neg = bits(chs(fl(adh)))
            if lt_ordered(v, mem(neg)):
                v = fl(neg)
                path.append('clamp_neg')
    elif not is_zero(fl(fs_bits)):
        force = bits(chs(divr(mem(fs_bits), from_int(n))))
        a_reg = fl(brake)
        if braking:
            path.append('brake')
            if is_nan(brake):
                path.append('brake_nan')
        else:
            g = abs_v(fl(gas))
            if lt_ordered(g, mem(K_GAS)):
                fric = c['fric']
                if (c['vtype'] & M32) == 9:
                    a_reg = divr(mulr(fl(fric), mem(K_BIKE)), addr(fl(hmass), mem(K_200)))
                    path.append('idle_bike')
                elif (c['subtype'] & M32) == 4:
                    a_reg = ZERO
                    path.append('idle_plane')
                else:
                    a_reg = divr(fl(fric), mem(hmass))
                    if lt_ordered(fl(hmass), mem(K_500)):
                        a_reg = mulr(a_reg, mem(K_TENTH))
                        path.append('idle_light')
                    elif (c['model'] & 0xFFFF) == 0x1B9:
                        a_reg = mulr(a_reg, mem(K_RC))
                        path.append('idle_rc')
                    else:
                        path.append('idle_car')
            else:
                path.append('coast_gas')
        if le_ordered(a_reg, mem(adh)):
            ff = fl(force)
            if gt(ff, ZERO):
                if gt(ff, a_reg):
                    force = bits(a_reg)
                    path.append('limit_pos')
            else:
                neg_a = chs(a_reg)
                if lt_ordered(ff, neg_a):
                    force = bits(neg_a)
                    path.append('limit_neg')
            path.append('limit')
        else:
            path.append('over_adh')
            if gt(abs_v(fl(fs_bits)), mem(K_FIX)):
                state = 3
                path.append('fixed')
    else:
        path.append('nofwd')
    # circulo de agarre
    adhsq = mulr(fl(adh), mem(adh))
    energy = addr(mulr(v, v), mulr(fl(force), mem(force)))
    e_bits = bits(energy)
    if gt(energy, adhsq):
        path.append('circle_out')
        if state != 3:
            tlim = fl(K_TRAC)
            if gt(fl(fs_bits), mem(K_FS)):
                if wid == 0 or wid == 2:
                    tlim = addr(tlim, tlim)
                    path.append('tlim_double')
            if driving:
                state = 1 if lt_ordered(mulr(tlim, mem(adh)), abs_v(fl(force))) else 2
            else:
                state = 2
            path.append('state_spin' if state == 1 else 'state_skid')
        tlr = fl(tl)
        if af:
            tlr = fl(K_ONE)
            path.append('tl_one')
        elif state == 1 and player:
            tlr = mulr(tlr, subr(fl(K_ONE), mulr(abs_v(fl(gas)), mem(K_SPIN))))
            path.append('tl_gas')
        k = mulr(divr(tlr, sqrtr(fl(e_bits))), mem(adh))
        force = bits(mulr(fl(force), k))
        v = mulr(v, k)
    else:
        path.append('circle_in')
    if is_zero(fl(force)) and is_zero(v):
        return result('noforce')
    # fuerza total = fuerza * adelante + v * derecha
    fx = [bits(mulr(fl(force), mem(fwd[i]))) for i in range(3)]
    sx, sy = bits(mulr(v, mem(right[0]))), bits(mulr(v, mem(right[1])))
    sz = mulr(v, mem(right[2]))
    tot = [bits(addr(fl(sx), mem(fx[0]))), bits(addr(fl(sy), mem(fx[1]))), bits(addr(sz, mem(fx[2])))]
    # direccion de giro separada con el multiplicador antihundimiento
    sep = False
    T = list(tot)
    sadm = fl(c['sadm'])
    if gt(sadm, ZERO):
        mreg = None
        if braking:
            mreg = sadm
        elif driving:
            mreg = mulr(sadm, mem(K_HALF))
        if mreg is not None:
            sep = True
            mx = mulr(mreg, mem(fwd[0]))
            my = bits(mulr(mreg, mem(fwd[1])))
            mz = bits(mulr(mreg, mem(fwd[2])))
            mxf = mulr(mx, mem(force))
            myf = mulr(fl(my), mem(force))
            mzf = bits(mulr(fl(mz), mem(force)))
            T = [bits(subr(fl(tot[0]), mxf)), bits(subr(fl(tot[1]), myf)), bits(subr(fl(tot[2]), mem(mzf)))]
            path.append('antidive')
    speed_bits = bits(sqrtr(addr(addr(mulr(fl(tot[2]), mem(tot[2])), mulr(fl(tot[1]), mem(tot[1]))), mulr(fl(tot[0]), mem(tot[0])))))
    tspeed_bits = speed_bits
    if sep:
        tspeed_bits = bits(sqrtr(addr(addr(mulr(fl(T[2]), mem(T[2])), mulr(fl(T[1]), mem(T[1]))), mulr(fl(T[0]), mem(T[0])))))
    # normalizacion: con modulo > 0 o NaN se divide (jp tras test ah,41h); con modulo <= 0 solo x = 1
    if not le_ordered(fl(speed_bits), ZERO):
        recip = divr(fl(K_ONE), mem(speed_bits))
        dirv = [bits(mulr(fl(tot[i]), recip)) for i in range(3)]
        path.append('norm')
    else:
        dirv = [K_ONE, tot[1], tot[2]]
        path.append('norm_skip')
    if sep:
        if not le_ordered(fl(tspeed_bits), ZERO):
            recip2 = divr(fl(K_ONE), mem(tspeed_bits))
            turnv = [bits(mulr(fl(T[i]), recip2)) for i in range(3)]
            path.append('tnorm')
        else:
            turnv = [K_ONE, T[1], T[2]]
            path.append('tnorm_skip')
    else:
        turnv = list(dirv)
    # producto vectorial 0x59C730 (a = punto de contacto, b = direccion de giro): cada componente = (p1 - p2) con los dos productos en la x87 y guardado como float
    a_, b_ = [fl(x) for x in cp], [fl(x) for x in turnv]
    crs = [bits(subr(mulr(b_[2], mem(cp[1])), mulr(a_[2], mem(turnv[1])))),
           bits(subr(mulr(a_[2], mem(turnv[0])), mulr(b_[2], mem(cp[0])))),
           bits(subr(mulr(a_[0], mem(turnv[1])), mulr(b_[0], mem(cp[1]))))]
    big_g = mulr(fl(mass), mem(speed_bits))
    q2 = addr(addr(mulr(fl(crs[0]), fl(crs[0])), mulr(fl(crs[1]), fl(crs[1]))), mulr(fl(crs[2]), fl(crs[2])))
    imp = mulr(divr(fl(K_ONE), addr(divr(q2, mem(turn_mass)), divr(fl(K_ONE), mem(mass)))), mem(tspeed_bits))
    mf_force = [bits(mulr(big_g, mem(dirv[i]))) for i in range(3)]
    turn_force = [bits(mulr(fl(turnv[i]), imp)) for i in range(3)]
    m.push([3, veh] + mf_force, 2, 4)                                             # ApplyMoveForce
    m.push([4, veh] + turn_force + list(cp), 2, 7)                                # ApplyTurnForce
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
