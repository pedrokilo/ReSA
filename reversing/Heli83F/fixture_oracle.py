"""Oraculo independiente (Python) de Heli83F: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6C7050..0x6C797F), lo que debe observarse de
CHeli::ProcessControl (traza EN ORDEN de los colaboradores con todos sus argumentos, hash de todos los eventos, las palabras que escribe en el helicoptero, bytes de los SWAT, byte 0xA11, estado del rand() del
EXE y arenas sin otros cambios) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.

Flujo leido (thiscall virtual, sin argumentos; void): CAutomobile::ProcessControl; sin motor y con particulas: Kill + 0x580 = 0 y 0x984 = 0; HornJustDown conmuta 0xA11. Si la bandera 0x20000000 de +0x40 o PlayerNoRain: potencia 0xA00 = 0.
Si no: mision 0x28 con el jugador fuera de un helicoptero o avion (FindPlayerVehicle, hasta tres veces) -> persecucion, objetivo = FindPlayerEntity; mision 0x27 con objetivo del piloto automatico (+0x41C) y bit 1 de +0x988; o estado 0,
modelo 0x1F1 y 0xA11 -> foco fijo sin objetivo (punto de mira = posicion + 10 * adelante - 30 * arriba). El bit 0x100 de +0x40 lo apaga. Historial de 6 muestras (X en 0x9C0, Y en 0x9D8) con una entrada por cada 1000 ms
transcurridos (0x9F0), interpolacion lineal entre las muestras 1 y 2 -> 0x9F4/0x9F8 (+ 0x9FC = z), potencia por la distancia (>60: 0; <40: 1; si no 1 - (d - 40) * 0.05), reinicio de 0xA04/0x504 con potencia < 0.9 o
punto de mira lejos (cuadrado > 49) y, si no, temporizador 0xA08 con rand(). Persecucion: retardo segun el nivel de busqueda (0..2: 999999; 3: 10000; 4: 5000; 5: 3500; 6: 2000; otro: los bits de dx), SayPedless, mitad si NoPolice, 5000 si el
objetivo no es el jugador, disparo con dispersion si toca (linea de vision, FireOneInstantHitRound, ReportWeaponEvent, 0x504 = hora + 400 o 150 segun una tirada < 0.15). Despues: SendDownSwat y evento de interes 0x1A (mision 0x28 con
SWAT), cuerdas de los cuatro SWAT, UpdateWinch, ProcessWeapons y, con 0xC0B184 bit 0, evento de interes 0x15 si rand() * 1/32767 < paso * 0.02 * 0.1 (doble en persecucion).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un puntero a CPad::Pads, 0x10000000 + (10 << 20) + 0x134 * mando + desplazamiento; un local de pila, 0x20000000.
Indices: 0 helicoptero, 1 matriz, 2 conductor, 3 vehiculo del jugador, 4 entidad, 5 matriz del objetivo, 6 CPlayerPed, 7 datos del jugador, 8 CWanted, 9 particulas, 10 CPad::Pads.
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


class Model:
    """acumula los eventos y el estado como lo hacen los falsos del banco"""

    def __init__(self, c):
        self.c = c
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




VEH, MATV, DRV, PV, ENT, TMAT, PLAYER, DATA, WANTED, FX, PADS = range(11)
AUDIO, INTEV = 0xB6BC90, 0xC0B058
OFFS = [0x504, 0x580, 0x984, 0x9C0, 0x9C4, 0x9C8, 0x9CC, 0x9D0, 0x9D4, 0x9D8, 0x9DC, 0x9E0, 0x9E4, 0x9E8, 0x9EC, 0x9F0, 0x9F4, 0x9F8, 0x9FC, 0xA00, 0xA04, 0xA08]


def ptag(pad, off=0):
    """etiqueta de un puntero dentro de CPad::Pads (region sin guarda): mando `pad` (0x134 bytes cada uno) + desplazamiento"""
    return 0x10000000 + (PADS << 20) + 0x134 * pad + off


# constantes de la lectura del EXE (patrones de float)
KUP, KFWD, K100, K001, K60, K40, K005 = 0xC1F00000, 0x41200000, 0x42C80000, 0x3A83126F, 0x42700000, 0x42200000, 0x3D4CCCCD
K09, K49, KJIT, K3, K015, KTENTH, K7C = 0x3F666666, 0x42440000, 0x3CA3D70A, 0x40400000, 0x3E19999A, 0x3DCCCCCD, 0x38000100
ONE_B = 0x3F800000
ONE = from_bits(ONE_B)
ZERO = from_bits(0)
STRIDE_SLOTS = 6


def signed32(u):
    u &= M32
    return u - (1 << 32) if u & 0x80000000 else u


def entity_tag(kind):
    return {1: tag(ENT), 2: tag(PLAYER), 3: tag(PV)}.get(kind, 0)


def model_heli(c):
    """CHeli::ProcessControl 0x6C7050..0x6C797F (desensamblado leido): ver README.md"""
    m = Model(c)
    path = []
    w = dict(zip(OFFS, c['iw']))
    fire_ptr = tag(FX) if c['fire'] else 0
    w[0x580] = fire_ptr
    sw = [b & 0xFF for b in c['swt']]
    a11 = c['a11'] & 0xFF
    nsw = c['nsw'] & 0xFF
    rng = Rng(c['rseed'])
    now, prev = c['now'], c['prev']
    mission = c['mission'] & 0xFF
    cnt = {'sw': 0, 'mp': 0, 'm3': 0}
    mat_t = 0 if c['mat_null'] else tag(MATV)
    hpos = c['hpos'] if c['mat_null'] else c['mpos']       # posicion del helicoptero (placement o matriz)

    # CAutomobile::ProcessControl
    m.push([6, tag(VEH)])
    if not (c['fl428'] & 0x10):
        if fire_ptr:
            m.push([7, tag(FX)])
            w[0x580] = 0
            w[0x984] = 0
            path.append('fx_kill')
        else:
            path.append('engine_off')
    pad = 1 if (c['drv_kind'] and c['drv_type'] == 1) else 0
    m.push([8, ptag(pad)])
    if c['horn'] & 0xFF:
        a11 = 1 if a11 == 0 else 0
        path.append('horn_toggle')

    def reset_timers():
        w[0xA04] = now
        w[0x504] = now

    def mat_point(vec):
        """operator*(matriz, vector) 0x59C890 y su salida del caso"""
        m.push([13, STACK, mat_t, STACK] + list(vec))
        k = min(cnt['mp'], 4)
        cnt['mp'] += 1
        return list(c['mp'][3 * k:3 * k + 3])

    def find_swat(i):
        m.push([12, tag(VEH), STACK, i])
        k = min(cnt['sw'], 7)
        cnt['sw'] += 1
        return list(c['sw'][3 * k:3 * k + 3])

    bl = 0
    f13 = 0
    ebp = 0
    blocked = False
    if c['fl40'] & 0x20000000:
        blocked = True
        path.append('blocked_flag')
    else:
        m.push([9])
        if c['norain'] & 0xFF:
            blocked = True
            path.append('blocked_rain')
    if blocked:
        w[0xA00] = 0
    else:
        cond = False
        if mission == 0x28:
            m.push([2, M32, 0])
            if c['pv_kind'] == 0:
                cond = True
            else:
                m.push([2, M32, 0])
                if c['pv_sub'] != 3:
                    m.push([2, M32, 0])
                    if c['pv_sub'] != 4:
                        cond = True
                if not cond:
                    path.append('m28_air')
        if cond:
            bl, f13 = 1, 1
            m.push([3, M32])
            ebp = c['pe_kind']
            path.append('m28')
        elif mission == 0x27 and c['ap_kind'] != 0 and (c['hf988'] & 2):
            bl, ebp = 1, c['ap_kind']
            path.append('m27')
        elif (c['status'] & 0xF8) == 0 and (c['model'] & 0xFFFF) == 0x1F1 and a11 != 0:
            bl = 1
            path.append('m1f1')
        if c['fl40'] & 0x100:
            bl = 0
            f13 = 0
            path.append('ctl_clear')
        a11 = bl
        if not bl:
            path.append('light_off')
        else:
            path.append('light')
            if f13:
                path.append('chase')
            # ---- punto de mira y velocidad
            if ebp:
                path.append('target')
                aim = list(c['tpos'] if c['tmat_null'] else c['tmpos'])
                spd = list(c['tspd'])
            else:
                path.append('notarget')
                if c['mat_null']:
                    path.append('nomat')
                    up = [0, 0, ONE_B]
                    fwd = [(c['heading'] ^ 0x80000000) & M32, ONE_B, 0]         # (-sin h, cos h, 0) con h = +-0
                else:
                    up = list(c['mu'])
                    fwd = list(c['mf'])
                upv = [bits(mulr(fl(up[i]), mem(KUP))) for i in range(3)]       # [F+4C], [F+50], [F+54]
                fx10 = mulr(fl(fwd[0]), mem(KFWD))
                fy10 = bits(mulr(fl(fwd[1]), mem(KFWD)))
                fz10 = bits(mulr(fl(fwd[2]), mem(KFWD)))
                t1 = addr(fx10, mem(hpos[0]))
                t2 = addr(fl(fy10), mem(hpos[1]))
                zs = bits(addr(fl(fz10), mem(hpos[2])))
                aim = [bits(addr(t1, mem(upv[0]))), bits(addr(t2, mem(upv[1]))), bits(addr(fl(zs), mem(upv[2])))]
                spd = list(c['ms'])
            # ---- historial de 1 s
            delta = signed32((now - w[0x9F0]) & M32)
            rem = delta
            X = [w[0x9C0 + 4 * i] for i in range(6)]
            Y = [w[0x9D8 + 4 * i] for i in range(6)]
            if delta > 1000:
                path.append('loop')
                count = ((((delta - 1001) & M32) * 0x10624DD3) >> 38) + 1
                if count >= 6:
                    path.append('loop_many')
                rem = signed32((delta - count * 1000) & M32)
                vx = mulr(fl(spd[0]), mem(K100))
                vy = mulr(fl(spd[1]), mem(K100))
                nx = bits(addr(vx, mem(aim[0])))
                ny = bits(addr(fl(aim[1]), vy))
                for _ in range(min(count, 6)):
                    X = [nx] + X[:5]
                    Y = [ny] + Y[:5]
                w[0x9F0] = (w[0x9F0] + 1000 * count) & M32
                for i in range(6):
                    w[0x9C0 + 4 * i] = X[i]
                    w[0x9D8 + 4 * i] = Y[i]
            t = mulr(from_int(rem), mem(K001))
            omt = subr(ONE, t)
            w[0x9FC] = aim[2]
            xi = bits(addr(mulr(omt, mem(X[2])), mulr(t, mem(X[1]))))
            w[0x9F4] = xi
            yi = addr(mulr(omt, mem(Y[2])), mulr(t, mem(Y[1])))
            w[0x9F8] = bits(yi)
            # ---- potencia segun la distancia
            dy = subr(yi, mem(hpos[1]))
            dx = subr(fl(xi), mem(hpos[0]))
            dist = sqrtr(addr(mulr(dy, dy), mulr(dx, dx)))
            dfb = bits(dist)
            if not gt(dist, from_bits(K60)):
                if lt_ordered(fl(dfb), from_bits(K40)):
                    w[0xA00] = ONE_B
                    path.append('near')
                else:
                    w[0xA00] = bits(subr(ONE, mulr(subr(fl(dfb), mem(K40)), mem(K005))))
                    path.append('mid')
            else:
                w[0xA00] = 0
                path.append('far')
            dxb = bits(subr(fl(aim[0]), mem(xi)))
            dy2 = subr(mem(aim[1]), yi)
            if lt_ordered(fl(w[0xA00]), mem(K09)):
                reset_timers()
                path.append('pw_low')
            else:
                path.append('pw_hi')
                sq = addr(mulr(dy2, dy2), mulr(fl(dxb), mem(dxb)))
                if gt(sq, from_bits(K49)):
                    reset_timers()
                    path.append('sq_hi')
                else:
                    path.append('shout')
                    if now > w[0xA08]:
                        w[0xA08] = ((rng.draw() & 0xFFF) + now + 4500) & M32
                        path.append('shout_timer')
            # ---- persecucion del jugador
            if f13:
                lvl = c['wlevel'] & M32
                m.push([4, M32])
                if lvl <= 6:
                    delay = [999999, 999999, 999999, 10000, 5000, 3500, 2000][lvl]
                    path.append('wl%d' % lvl)
                else:
                    delay = signed32(dxb)
                    path.append('wl_big')
                m.push([4, M32])
                if lvl != 0:
                    m.push([11, AUDIO, 0x34, 0xAE, tag(VEH), 0, ONE_B, 0, 0, 0])
                    path.append('say')
                m.push([10])
                if c['nopolice'] & 0xFF:
                    delay = int(delay / 2)
                    path.append('nopolice')
                m.push([4, M32])
                if ebp != 2:
                    delay = 5000
                    path.append('target_other')
                m.push([5, M32])
                if c['wflags'] & 7:
                    reset_timers()
                    path.append('backoff')
                else:
                    gun = mat_point([0, 0x40600000, 0xBF800000])
                    path.append('gun')
                    due = (w[0xA04] + delay) & M32
                    if now > due and not (prev > due):
                        path.append('los_check')
                        m.push([17, STACK, STACK] + gun + aim + [1, 0, 0, 0, 0, 0, 0])
                        if not (c['los'] & 0xFF):
                            reset_timers()
                            path.append('los_block')
                    if now > ((w[0xA04] + delay) & M32) and now > w[0x504]:
                        path.append('fire')
                        end = list(aim)
                        r1 = (rng.draw() & 0xFF) - 0x80
                        end[0] = bits(addr(mulr(from_int(r1), mem(KJIT)), mem(aim[0])))
                        r2 = (rng.draw() & 0xFF) - 0x80
                        end[1] = bits(addr(mulr(from_int(r2), mem(KJIT)), mem(aim[1])))
                        direction = [bits(subr(fl(aim[i]), mem(gun[i]))) for i in range(3)]
                        m.push([1, STACK] + direction)
                        n = c['nrm']
                        x3 = mulr(fl(n[0]), mem(K3))
                        x3b = bits(x3)
                        y3 = mulr(fl(n[1]), mem(K3))
                        z3 = mulr(fl(n[2]), mem(K3))
                        z3b = bits(z3)
                        end[0] = bits(addr(fl(x3b), mem(end[0])))
                        end[1] = bits(addr(y3, mem(end[1])))
                        end[2] = bits(addr(fl(aim[2]), z3))
                        start = [bits(addr(fl(x3b), mem(gun[0]))), bits(addr(fl(gun[1]), y3)), bits(addr(fl(gun[2]), fl(z3b)))]
                        m.push([18, STACK, STACK, 20] + start + end)
                        m.push([19, AUDIO, 0x91, 0x1F, tag(VEH)])
                        roll = addr(mulr(mulr(from_int(rng.draw()), mem(K7C)), subr(ONE, ZERO)), ZERO)
                        if lt_ordered(roll, mem(K015)):
                            w[0x504] = (now + 400) & M32
                            path.append('burst_hi')
                        else:
                            w[0x504] = (now + 150) & M32
                            path.append('burst_lo')
    # ---- SWAT
    if mission == 0x28 and nsw != 0:
        m.push([22, tag(VEH)])
        m.push([23, INTEV, 0x1A, tag(VEH)])
        path.append('swat_down')
    for i in range(4):
        if sw[i] == 0:
            continue
        sw[i] -= 1
        rel = find_swat(i)
        pos = mat_point(rel)
        m.push([15, tag(VEH, i), 8] + pos + [0, 0, 0, 0, 20000])
        path.append('rope')
        if sw[i] == 0:
            rel2 = find_swat(i)
            scaled = [bits(mulr(fl(rel2[j]), mem(K005))) for j in range(3)]
            m.push([14, STACK, mat_t, STACK] + scaled)
            k = min(cnt['m3'], 3)
            cnt['m3'] += 1
            out = c['m3o'][3 * k:3 * k + 3]
            m.push([16, tag(VEH, i), out[0], out[1], 0])
            path.append('rope_push')
    m.push([20, tag(VEH)])
    m.push([21, tag(VEH)])
    if c['ev184'] & 1:
        path.append('ev_flag')
        lim = bits(mulr(mulr(fl(c['ts']), mem(KJIT)), mem(KTENTH)))
        if f13:
            lim = bits(addr(fl(lim), fl(lim)))
            path.append('ev_double')
        v = mulr(from_int(rng.draw()), mem(K7C))
        if lt_ordered(v, mem(lim)):
            m.push([23, INTEV, 0x15, tag(VEH)])
            path.append('ev_fire')
    return dict(events=m.events, ev_hash=m.hash, ev_count=m.count, w=[w[o] & M32 for o in OFFS], sw=sw, a11=a11, rand_state=rng.state, path='+'.join(path))


def expected(c):
    return model_heli(c)


def check_self():
    assert to_f32_bits(r24(mul(from_int(5000), from_bits(0x38D1B717)))) == 0x3F000000
    assert to_f32_bits(from_bits(0x7F800001)) == 0x7FC00001
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(sqrt_v(from_int(2))) == 0x3FB504F3
    assert to_f32_bits(div(from_int(1), from_int(3))) == 0x3EAAAAAB
    assert ftol_low32(from_int(-7)) == -7
    assert is_zero(from_bits(0x80000000)) and not is_zero(from_bits(0xFFC00000))
    assert ((((2000 - 1001) & M32) * 0x10624DD3) >> 38) + 1 == 1
    assert ((((2001 - 1001) & M32) * 0x10624DD3) >> 38) + 1 == 2
    assert Rng(1).draw() == (((1 * LCG_A + LCG_C) & M32) >> 16) & 0x7FFF
    print('ok')


def check_row_obj(row):
    """(mensajes de desacuerdo, ruta del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('ev_hash', 'ev_count', 'events', 'rand_state', 'a11') if o[k] != e[k]]
        if o['unexpected_virtuals'] != 0:
            bad.append('unexpected_virtuals')
        if o['w'] != e['w']:
            bad.append('words')
        if o['sw'] != e['sw']:
            bad.append('swat_bytes')
        if any(o['changes']):
            bad.append('changes')
        if bad:
            msgs.append('DIFIERE %s caso %s campos %s %s' % (side, row['case'], bad, _first_diff(o, e)))
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
    for off, g, w in zip(OFFS, o['w'], e['w']):
        if g != w:
            return 'palabra 0x%X: %x != %x' % (off, g, w)
    return 'a11 %s/%s sw %s/%s rand %x/%x' % (o['a11'], e['a11'], o['sw'], e['sw'], o['rand_state'], e['rand_state'])


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
