"""Oraculo independiente (Python) de Heli83E: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x6C4830..0x6C4E50), lo que debe observarse de
CHeli::ProcessControlInputs (traza EN ORDEN de los colaboradores con todos sus argumentos, hash de todos los eventos, modo de control 0xC1CC04, los campos que escribe en el helicoptero y arenas sin
otros cambios) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con fracciones exactas. Con dos NaN en una operacion gana el de mayor mantisa.
El pow del CRT del EXE (0x822130, decaimiento 0.98^paso del raton) no se modela con exactitud: el oraculo comprueba su salida con tolerancia relativa 1e-5 (la igualdad bit a bit entre EXE y DLL la comprueba el banco).

Flujo leido (thiscall, 1 dword; ret 4; void): (acelerador - freno) * 1/255 -> 0x998; con raton en 3a persona y vuelo con raton (0xB6EC2E y 0xC1CC03): raton movido (0xB73424/0xB73428 != 0) o ejes fijados con modo
raton (0xC1CC04 == 2) y mando en reposo -> modo raton (0xC1CC04 = 2; sin m_bVehicleMouseLook: 0x994 -= x * 0.0025, 0x990 += y * 0.0025; con |eje| < 0.5: eje = 0.98^paso * eje); si el mando se mueve o el modo
no es raton -> teclado (0xC1CC04 = 0; 0x990 = arriba/abajo / 128, 0x994 = -izquierda/derecha / 128); sin raton siempre teclado. Ejes acotados a [-1, 1]. 0x98C = mirar derecha (byte), -1.0 si mira izquierda;
con bocina y arriba.z > 0: 0x98C = 0, 0x990 y 0x994 = (normalizada(cruz) . velocidad) * estabilidad, acotados a [-2, 2]. Siempre: bit 5 de 0x428 a 0, 0x494 = 0, 0x4A0 = 1.0, 0x49C = 0. Con DisablePlayerControls
(palabra en pad+0x10E): FindPlayerPed(-1)->KeepAreaAroundPlayerClear() y velocidad limitada a 0.28. Con salud < 250: 0x998 = -0.1 y 0x98C += 0.5.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un puntero a CPad::Pads, 0x10000000 + (4 << 20) + 0x134 * mando + desplazamiento; un local de pila, 0x20000000.
Indices: 0 helicoptero, 1 matriz, 2 datos de manejo en vuelo, 3 CPlayerPed de FindPlayerPed, 4 CPad::Pads.
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
VEH, MATV, FLY, PLAYER, PADS = range(5)


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



def ptag(pad, off=0):
    """etiqueta de un puntero dentro de CPad::Pads (region sin guarda): mando `pad` (0x134 bytes cada uno) + desplazamiento"""
    return 0x10000000 + (PADS << 20) + 0x134 * pad + off


K255, K128, KSTEP, KDECAY, K05 = 0x3B808081, 0x3C000000, 0x3B23D70A, 0x3F7AE148, 0x3F000000
KLIM, K250, KNEG01 = 0x3E8F5C29, 0x437A0000, 0xBDCCCCCD
ONE_B, NEG1_B, TWO_B, NEG2_B = 0x3F800000, 0xBF800000, 0x40000000, 0xC0000000
TWO, NEG2, NEG1 = from_bits(TWO_B), from_bits(NEG2_B), from_bits(NEG1_B)


def not_zero(b):
    """fld; fcomp 0.0; test ah,44h; jp: salta salvo igualdad ordenada (un NaN cuenta como distinto de cero)"""
    return not is_zero(fl(b))


def model_heli(c):
    """CHeli::ProcessControlInputs 0x6C4830..0x6C4E50 (desensamblado leido): ver README.md"""
    m = Model(c)
    path = []
    pad = c['arg'] & 0xFF
    pt = ptag(pad)
    veh = tag(VEH)
    mode = c['mode'] & M32
    lr_v = ('exact', c['lr0'])         # m_fSteeringLeftRight (0x994)
    ud_v = ('exact', c['ud0'])         # m_fSteeringUpDown (0x990)

    # GetPad(arg); GetAccelerate; GetBrake: (acelerador - freno) * 1/255
    m.push([10, pt])
    acc16 = signed16(c['acc'])
    m.push([11, pt])
    brk16 = signed16(c['brk'])
    acc_bits = bits(mulr(from_int(acc16 - brk16), mem(K255)))

    def keyboard_update():
        nonlocal mode, lr_v, ud_v
        mode = 0
        m.push([13, pt])
        ud = signed16(c['ud'])
        ud_v = ('exact', bits(mulr(from_int(ud), mem(K128))))
        m.push([12, pt])
        lr = signed16(c['lr'])
        lr_v = ('exact', bits(mulr(from_int(-lr), mem(K128))))
        path.append('kb_update')

    def exact_of(v):
        return v[1]

    if (c['m3'] & 0xFF) != 0 and (c['fly'] & 0xFF) != 0:
        path.append('cheat')
        mouse = False
        if not_zero(c['mx']):
            mouse = True
            path.append('mouse_x')
        elif not_zero(c['my']):
            mouse = True
            path.append('mouse_y')
        else:
            moving = gt(abs_v(fl(exact_of(lr_v))), ZERO) or gt(abs_v(fl(exact_of(ud_v))), ZERO)
            if moving and mode == 2:
                m.push([12, pt])
                if signed16(c['lr']) == 0:
                    m.push([13, pt])
                    if signed16(c['ud']) == 0:
                        mouse = True
                        path.append('mouse_pad')
        if mouse:
            mode = 2
            if (c['vml'] & 0xFFFF) == 0:
                path.append('vml_zero')
                t = mulr(fl(c['mx']), mem(KSTEP))
                lr_v = ('exact', bits(subr(mem(exact_of(lr_v)), t)))
                t = mulr(fl(c['my']), mem(KSTEP))
                ud_v = ('exact', bits(addr(t, mem(exact_of(ud_v)))))
            else:
                path.append('vml_set')
            if lt_ordered(abs_v(fl(exact_of(lr_v))), mem(K05)):
                lr_v = ('pow', exact_of(lr_v))
                path.append('pow_lr')
            if lt_ordered(abs_v(fl(exact_of(ud_v))), mem(K05)):
                ud_v = ('pow', exact_of(ud_v))
                path.append('pow_ud')
        else:
            m.push([12, pt])
            update = signed16(c['lr']) != 0
            if not update:
                m.push([13, pt])
                update = signed16(c['ud']) != 0
            if update or mode != 2:
                keyboard_update()
                path.append('kb_after_cheat')
            else:
                path.append('idle_mouse')
    else:
        path.append('no_cheat')
        keyboard_update()

    # cota a [-1, 1] (primero 0x990, luego 0x994); con NaN se conserva el valor
    def clamp1(v):
        if v[0] == 'pow':
            return ('approx', v[1])
        b = v[1]
        x = fl(b)
        if gt(x, fl(ONE_B)):
            path.append('clamp_hi')
            return ('exact', ONE_B)
        if gt(NEG1, x):
            path.append('clamp_lo')
            return ('exact', NEG1_B)
        return ('exact', b)

    ud_o = clamp1(ud_v)
    lr_o = clamp1(lr_v)

    # mirar a un lado
    m.push([14, pt])
    skid = bits(from_int(c['look_r'] & 0xFF))
    if c['look_r'] & 0xFF:
        path.append('look_r')
    m.push([15, pt])
    if c['look_l'] & 0xFF:
        skid = NEG1_B
        path.append('look_l')
    m.push([16, pt])
    if c['horn'] & 0xFF:
        path.append('horn')
        if gt(fl(c['mu'][2]), ZERO):
            path.append('horn_up')
            skid = 0
            ms = c['ms']
            cross_a = [0, 0, ONE_B]

            def stabilise(nrm, factor):
                s1 = mulr(fl(nrm[1]), mem(ms[1]))
                s2 = mulr(fl(nrm[2]), mem(ms[2]))
                s = addr(s1, s2)
                s = addr(s, mulr(fl(nrm[0]), mem(ms[0])))
                t = mulr(s, mem(factor))
                if gt(t, TWO):
                    path.append('stab_hi')
                    return ('exact', TWO_B)
                if gt(NEG2, t):
                    path.append('stab_lo')
                    return ('exact', NEG2_B)
                path.append('stab_mid')
                return ('exact', bits(t))

            # CrossProduct(&v, &(0,0,1), matriz->derecha); Normalise(&v)
            m.push([2, STACK, STACK, tag(MATV, 0)] + cross_a + list(c['mr']))
            m.push([1, STACK] + list(c['crs0']))
            ud_o = stabilise(c['nrm0'], c['pitch'])
            # CrossProduct(&w, matriz->adelante, &(0,0,1)); copia a v; Normalise(&v)
            m.push([2, STACK, tag(MATV, 0x10), STACK] + list(c['mf']) + cross_a)
            m.push([1, STACK] + list(c['crs1']))
            lr_o = stabilise(c['nrm1'], c['roll'])
        else:
            path.append('horn_flat')

    flags = c['flags'] & 0xDF
    ms_out = list(c['ms'])
    if (c['dpc'] & 0xFFFF) != 0:
        path.append('dpc')
        m.push([20, M32])
        m.push([21, tag(PLAYER)])
        x, y, z = fl(c['ms'][0]), fl(c['ms'][1]), fl(c['ms'][2])
        length = sqrtr(addr(addr(mulr(x, x), mulr(y, y)), mulr(z, z)))
        if gt(length, fl(KLIM)):
            path.append('cap')
            scale = divr(fl(KLIM), length)
            ms_out = [bits(mulr(scale, mem(c['ms'][0]))), bits(mulr(scale, mem(c['ms'][1]))), bits(mulr(scale, mem(c['ms'][2])))]
        else:
            path.append('nocap')
    if lt_ordered(fl(c['health']), fl(K250)):
        path.append('health_low')
        acc_bits = KNEG01
        skid = bits(addr(fl(skid), mem(K05)))
    return dict(events=m.events, ev_hash=m.hash, ev_count=m.count, mode=mode, ud=ud_o, lr=lr_o, skid=skid, acc=acc_bits, flags=flags, ms=ms_out, path='+'.join(path))


def f32v(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def approx_ok(obs, a):
    """salida de un eje tras el pow del CRT (no modelable exactamente): coincide con float(pow(0.98, ts) * v) con tolerancia, incluida la cota a [-1, 1]"""
    o = f32v(obs)
    if a != a:
        return o != o
    if o != o:
        return False
    if a > 1.0 + 1e-5:
        return obs == ONE_B
    if a < -1.0 - 1e-5:
        return obs == NEG1_B
    if abs(abs(a) - 1.0) <= 1e-5 and obs in (ONE_B, NEG1_B):
        return True
    return abs(o - a) <= max(abs(a) * 1e-5, 1e-44)


def pow_value(c, v_bits):
    import math
    base, ts, v = f32v(KDECAY), f32v(c['ts']), f32v(v_bits)
    try:
        p = math.pow(base, ts)
    except OverflowError:
        p = float('inf')
    except ValueError:
        p = float('nan')
    return p * v


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
    assert bits(mulr(from_int(255), mem(K255))) == 0x3F800000
    assert approx_ok(0x3F7AE148, 0.98) and not approx_ok(0x3F800000, 0.5)
    print('ok')


def check_row_obj(row):
    """(mensajes de desacuerdo, ruta del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        out = o['out']
        bad = [k for k in ('mode', 'ev_hash', 'ev_count', 'events') if o[k] != e[k]]
        if o['unexpected_virtuals'] != 0:
            bad.append('unexpected_virtuals')
        want = {'skid': out[0], 'acc': out[3], 'flags': out[7]}
        for key in ('skid', 'acc', 'flags'):
            if want[key] != e[key]:
                bad.append(key)
        for idx, key, steer0 in ((1, 'ud', None), (2, 'lr', None)):
            exp = e[key]
            if exp[0] == 'exact':
                if out[idx] != exp[1]:
                    bad.append(key)
            elif not approx_ok(out[idx], pow_value(c, exp[1])):
                bad.append(key + '_pow')
        if out[4] != 0 or out[5] != 0 or out[6] != ONE_B:
            bad.append('steer_gas_brake')
        if out[8:11] != e['ms']:
            bad.append('speed')
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
    return 'mode %s/%s out %s modelo ud %s lr %s skid %x acc %x flags %x ms %s' % (o['mode'], e['mode'], o['out'], e['ud'], e['lr'], e['skid'], e['acc'], e['flags'], e['ms'])


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
