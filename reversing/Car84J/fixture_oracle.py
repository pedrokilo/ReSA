"""Oraculo independiente (Python) de Car84J: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x435830..0x436537), lo que debe observarse de
CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing (traza EN ORDEN de los colaboradores con sus argumentos y el estado del vehiculo en cada llamada, hash de todos los eventos, el estado final del vehiculo
(modelo, tramos, signos, carriles, mision, crucero, banderas, nodos, +0x39C/+0x3A0, destino.x, contador y ruta de nodos, velocidad.x, subtipo, temporizador), la matriz y las cuatro salidas; ningun otro cambio en las arenas)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con enteros exactos. fpatan (GetATanOfXY, 0x53CC70) se evalua con 100 digitos (el EXE lo deja sin redondear en el registro).

Flujo leido (cdecl, 5 argumentos; ret): StopCarIfNodesAreInvalid (si devuelve verdadero no hace mas); velocidad a lo largo del avance ((z + y) + x por 60); adelante normalizado; las dos areas de tramos (+0x3A4, +0x3A6) deben
estar cargadas (si no: giro 0, acelerador 0, freno 1, sin freno de mano); distancia al primer tramo, longitud del tramo y producto; con < 8 m, < 12 m con producto positivo, razon > 0.7 o tramos iguales avanza al siguiente nodo:
con menos de 4 nodos en la ruta pide DoPathSearch (quita el primero y, si coincide con el siguiente, otro; sin ruta mision 9 y HeadingForTarget al destino); desplaza nodos, enlaces, signos y carril (busqueda sin limite del enlace
del nodo actual que lleva al nuevo siguiente); ventana de hasta cuatro enlaces de adelanto (FindLinkBetweenNodes, signo por (area, nodo)); DealWithBend_Racing sobre ventanas de cuatro enlaces (el primero que devuelve verdadero
fija el enlace de la curva); sin curva el rumbo parte del rumbo del vehiculo y se recorta con los enlaces de adelanto (del 5.o al 2.o), el siguiente y la mezcla del tramo actual y el siguiente (peso FindPercDependingOnDistToLink);
con curva se recorta con el siguiente si el enlace de la curva no es el actual y la mezcla pesa (perc + 2) * 0.33333334; FindAngleToWeaveThroughTraffic (escala 2), normalizacion a [-pi, pi] y limite de giro por el modulo de la
velocidad (0.2, o min(0.7, 0.9 - modulo)); puente (factor 0) y factor de la curva; velocidad deseada, gas y freno.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000. Indices: 0 vehiculo, 1 matriz, 2 salidas, 3..5 tramos, 6 nodos, 7 enlaces de nodo, 8 enlaces de navegacion.
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




# ---------------------------------------------------------------- funciones trascendentes de la x87 (precision muy superior a la de 64 bits: el EXE las evalua en el registro sin redondear)
from decimal import Decimal, getcontext
getcontext().prec = 100


def _atan_small(x):
    x2 = x * x
    term = x
    s = x
    k = 1
    eps = Decimal(10) ** -95
    while True:
        term *= -x2
        k += 2
        t = term / k
        if abs(t) < eps:
            return s
        s += t


PI_DEC = 16 * _atan_small(Decimal(1) / 5) - 4 * _atan_small(Decimal(1) / 239)


def atan_dec(x):
    if x == 0:
        return Decimal(0)
    neg = x < 0
    x = abs(x)
    inv = x > 1
    if inv:
        x = 1 / x
    k = 0
    while x > Decimal('0.05'):
        x = x / (1 + (1 + x * x).sqrt())
        k += 1
    r = _atan_small(x) * (2 ** k)
    if inv:
        r = PI_DEC / 2 - r
    return -r if neg else r


def sin_cos_dec(a):
    """seno y coseno de a (Decimal, |a| < 4) por series de Taylor"""
    a2 = a * a
    eps = Decimal(10) ** -95
    s, c = a, Decimal(1)
    ts, tc = a, Decimal(1)
    n = 1
    while True:
        tc = -tc * a2 / (n * (n + 1))
        ts = -ts * a2 / ((n + 1) * (n + 2))
        n += 2
        c += tc
        s += ts
        if abs(tc) < eps and abs(ts) < eps:
            return s, c


def dec_of(x):
    v = Decimal(x.m) * (Decimal(2) ** x.e)
    return -v if x.s else v


def v_of(d):
    """Decimal -> V con 110 bits de mantisa (sin redondear a 24 bits)"""
    if d == 0:
        return V('f', 1 if d.is_signed() else 0, 0, 0)
    s = 1 if d < 0 else 0
    num, den = abs(d).as_integer_ratio()
    k = 110 - (num.bit_length() - den.bit_length())
    m = (num << k) // den if k >= 0 else num // (den << -k)
    return V('f', s, m, -k)


def fpatan(a):
    """fld a; fld1; fpatan: atan(a / 1), sin redondear"""
    if a.kind == 'nan':
        return quiet(a)
    if a.kind == 'inf':
        v = v_of(PI_DEC / 2)
        v.s = a.s
        return v
    if a.m == 0:
        return a
    return v_of(atan_dec(dec_of(a)))


def fcos(a):
    if a.kind == 'nan':
        return quiet(a)
    if a.kind == 'inf':
        return indefinite()
    if a.m == 0:
        return from_bits(0x3F800000)
    return v_of(sin_cos_dec(dec_of(a))[1])


def fsin(a):
    if a.kind == 'nan':
        return quiet(a)
    if a.kind == 'inf':
        return indefinite()
    if a.m == 0:
        return a
    return v_of(sin_cos_dec(dec_of(a))[0])


def chs(x):
    """fchs: invierte el signo tambien de un NaN"""
    if x.kind == 'nan':
        return V('nan', bits=x.bits ^ 0x80000000, snan=x.snan)
    return neg_v(x)


def lt_ordered(a, b):
    """a < b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and gt(b, a)




def from_dbits(b64):
    """operando de memoria de 64 bits (fmul qword...): solo valores finitos (las constantes del EXE)"""
    sign = b64 >> 63
    ex = (b64 >> 52) & 0x7FF
    man = b64 & ((1 << 52) - 1)
    assert ex != 0x7FF
    if ex == 0:
        return V('f', sign, man, -1074)
    return V('f', sign, man | (1 << 52), ex - 1075)


# ---------------------------------------------------------------- modelo de CCarCtrl::SteerAICarWithPhysicsFollowPath (0x434900..0x43579A)
K_0, K_1, K_05, K_02, K_005, K_003, K_2, K_3, K_5, K_8, K_07, K_40, K_60 = 0x00000000, 0x3F800000, 0x3F000000, 0x3E4CCCCD, 0x3D4CCCCD, 0x3CF5C28F, 0x40000000, 0x40400000, 0x40A00000, 0x41000000, 0x3F333333, 0x42200000, 0x42700000
K_PI, K_2PI, K_NPI, K_HALFPI, K_THREEHALF, K_M1 = 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x3FC90FDB, 0x4096CBE4, 0xBF800000
K_LANE, K_BIKE, K_LOOK, K_DIR, K_DIST, K_BRK, K_0125, K_025, K_04, K_12, K_01 = 0x40ACCCCD, 0x3FBA9FBF, 0x3EB33333, 0x3C23D70A, 0x3CCCCCCD, 0xBDAAAAAB, 0x3E000000, 0x3E800000, 0x3ECCCCCD, 0x3F99999A, 0x3DCCCCCD
K_LANEK = 0x3C3DA12F      # 0x858EEC (0.011574074)
VEH, MATA, OUTA, TGTA, TMATA = 0, 1, 2, 3, 4


def signed8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


class Model:
    def __init__(self):
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


def is_zero(a):
    return a.kind == 'f' and a.m == 0


def le_ord(a, b):
    """a <= b ordenado (NaN -> falso)"""
    return a.kind != 'nan' and b.kind != 'nan' and not gt(a, b)


def atan_of_xy(x, y):
    """CGeneral::GetATanOfXY (0x53CC70): x, y ya cargados. Devuelve ST0 al terminar (sin guardar)"""
    zero = from_bits(0)
    if is_zero(x) and is_zero(y):
        return zero
    ax = neg_v(x) if lt_ordered(x, zero) else x
    ay = neg_v(y) if lt_ordered(y, zero) else y
    xpos = gt(x, zero)
    ypos = gt(y, zero)
    m1 = from_bits(K_M1)
    if lt_ordered(ax, ay):
        if ypos:
            if xpos:
                return sub(from_bits(K_HALFPI), fpatan(div(x, y)))
            return add(fpatan(mul(div(m1, y), x)), from_bits(K_HALFPI))
        if xpos:
            return add(fpatan(mul(div(m1, y), x)), from_bits(K_THREEHALF))
        return sub(from_bits(K_THREEHALF), fpatan(div(x, y)))
    if ypos:
        if xpos:
            return fpatan(div(y, x))
        return sub(from_bits(K_PI), fpatan(mul(div(m1, x), y)))
    if xpos:
        return sub(from_bits(K_2PI), fpatan(mul(div(m1, x), y)))
    return add(fpatan(div(y, x)), from_bits(K_PI))


class Unsafe(Exception):
    """la fixture haria leer al EXE fuera de las tablas del banco"""


K_7, K_TWELVE, K_09, K_THIRD, K_BRK10, K_M005, K_SIXTY = 0x40E00000, 0x41400000, 0x3F666666, 0x3EAAAAAB, 0xBD888889, 0xBD4CCCCD, 0x42700000
K_SEARCH = 0x497423FE


def signed16b(u):
    u &= 0xFFFF
    return u - 0x10000 if u & 0x8000 else u


def model_racing(c):
    """CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing (desensamblado 0x435830..0x436537 leido): ver README.md"""
    m = Model()
    marks = []
    st = dict(cur=c['cur'] & 0xFFFF, nxt=c['next'] & 0xFFFF, prevl=c['prev_l'] & 0xFFFF, s4=c['s4'] & 0xFF, s5=c['s5'] & 0xFF, s6=c['s6'] & 0xFF, l7=c['l7'] & 0xFF, l8=c['l8'] & 0xFF,
              mission=c['mission'] & 0xFF, cruise=c['cruise'] & 0xFF, subtype=c['subtype'] & M32, timer=c['timer'] & M32, model=c['model'] & 0xFFFF, f3db=c['f3db'] & 0xFF,
              curn=c['cur_n'] & M32, nextn=c['next_n'] & M32, prevn=c['prev_n'] & M32, fieldc=c['field_c'] & M32, scale=c['scale'] & M32,
              count=c['pcount'] & 0xFFFF, speed0=c['speed'][0] & M32, dest0=c['dest'][0] & M32)
    nodes = [x & M32 for x in c['pn']]
    mat = dict(r=list(c['mr']), f=list(c['mf']), u=list(c['mu']), p=list(c['mp']))
    out = [c['o0'][0] & M32, c['o0'][1] & M32, c['o0'][2] & M32, c['o0'][3] & 0xFF]
    evn = [0]
    cnt = dict(link=0, bend=0, clip=0)
    areas = list(c['areas'])
    slots = [bytes(b for w in c['links%d' % s] for b in (w & 0xFF, (w >> 8) & 0xFF, (w >> 16) & 0xFF, (w >> 24) & 0xFF)) for s in range(3)]

    def ev(e):
        m.push([x & M32 for x in e])

    def after():
        idx = evn[0]
        evn[0] += 1
        if idx != c['mut_at']:
            return
        marks.append('mut_applied')
        marks.append('mut_kind_%d' % c['mut_kind'])
        k, v = c['mut_kind'], c['mut_val'] & M32
        if k == 1:
            st['speed0'] = v
        elif k == 2:
            mat['f'][0] = v
        elif k == 3:
            mat['p'][0] = v
        elif k == 4:
            st['cur'] = v & 0xFFFF
        elif k == 5:
            st['nxt'] = v & 0xFFFF
        elif k == 6:
            st['s5'] = v & 0xFF
        elif k == 7:
            st['s6'] = v & 0xFF
        elif k == 8:
            st['cruise'] = v & 0xFF
        elif k == 9:
            st['subtype'] = v
        elif k == 10:
            st['timer'] = v
        elif k == 11:
            st['count'] = v & 0xFFFF
        elif k == 12:
            nodes[0] = v
        elif k == 13:
            st['nextn'] = v
        elif k == 14:
            st['dest0'] = v
        elif k == 15:
            st['model'] = v & 0xFFFF
        elif k == 16:
            st['f3db'] = v & 0xFF
        elif k == 17:
            st['l8'] = v & 0xFF

    def state():
        mats = mat['r'] + mat['f'] + mat['u'] + mat['p']
        v = [st['model'], st['cur'], st['nxt'], st['prevl'], st['s4'], st['s5'], st['s6'], st['l7'], st['l8'], st['mission'], st['cruise'], st['f3db'], st['curn'], st['nextn'], st['prevn'],
             st['fieldc'], st['scale'], st['dest0'], st['count'], st['speed0'], st['subtype'], st['timer']] + nodes
        return dict(events=m.events, ev_hash=m.hash, ev_count=m.count, v=v, mat=mats, out=list(out), marks=marks)

    def loaded(area):
        return area in areas

    def lb(word, off):
        area, ident = word >> 10, word & 0x3FF
        if area not in areas or ident >= 8:
            raise Unsafe('tramo fuera de las tablas')
        return slots[areas.index(area)][ident * 14 + off]

    def i16l(word, off):
        return signed16(lb(word, off) | (lb(word, off + 1) << 8))

    def pos8(word, off):
        """int16 * 0.125 (0x858C48), exacto"""
        return mul(from_int(i16l(word, off)), from_bits(K_0125))

    def all_stop():
        out[0], out[1], out[2], out[3] = 0, 0, K_1, 0

    def cur_cnt():
        return signed16b(st['count'])

    def less(a, b):
        """CNodeAddress::operator< (0x420980): area (16 bits bajos) y despues nodo (16 bits altos), sin signo"""
        aa, an, ba, bn = a & 0xFFFF, a >> 16, b & 0xFFFF, b >> 16
        return aa < ba or (aa == ba and an < bn)

    def remove_one():
        """CAutoPilot::RemoveOnePathNode (0x41B950): decrementa el contador y desplaza la ruta tantas posiciones como el contador nuevo"""
        st['count'] = (st['count'] - 1) & 0xFFFF
        i = 0
        if cur_cnt() > 0:
            while True:
                nodes[i] = nodes[i + 1]
                i += 1
                if not (i < cur_cnt()):
                    break

    def heading(x, y):
        ev([3, tag(VEH, 0), 0, x, y, tag(OUTA, 0), tag(OUTA, 4), tag(OUTA, 8), tag(OUTA, 12), st['mission'], st['cur'], st['nxt']])
        out[0], out[1], out[2], out[3] = c['hf'][0] & M32, c['hf'][1] & M32, c['hf'][2] & M32, c['hf_hb'] & 0xFF
        after()

    # ---- StopCarIfNodesAreInvalid
    ev([1, tag(VEH, 0), st['cur'], st['nxt'], st['curn'], st['nextn']])
    stop = c['stop_ret'] != 0
    after()
    if stop:
        marks.append('stop_true')
        return state()

    # ---- velocidad a lo largo del avance: (z + y) + x, por 60.0 (la matriz siempre existe)
    sv = [st['speed0'], c['speed'][1], c['speed'][2]]
    fwd = mat['f']
    dot_sp = add(add(mul(fl(sv[2]), fl(fwd[2])), mul(fl(sv[1]), fl(fwd[1]))), mul(fl(sv[0]), fl(fwd[0])))
    speed_along = to_f32_bits(mul(dot_sp, from_bits(K_SIXTY)))

    # ---- adelante normalizado
    fx, fy = fl(mat['f'][0]), fl(mat['f'][1])
    length = sqrt_v(add(mul(fy, fy), mul(fx, fx)))
    if is_zero(length):
        fnx, fny = K_1, mat['f'][1] & M32
        marks.append('fwd_zero')
    else:
        inv = div(from_bits(K_1), length)
        fnx = to_f32_bits(mul(inv, fx))
        fny = to_f32_bits(mul(inv, fy))

    # ---- tramos
    cur, nxt = st['cur'], st['nxt']
    if not loaded(cur >> 10):
        marks.append('unloaded_cur')
        all_stop()
        return state()
    if not loaded(nxt >> 10):
        marks.append('unloaded_next')
        all_stop()
        return state()
    x1, y1 = pos8(cur, 0), pos8(cur, 2)
    x2, y2 = pos8(nxt, 0), pos8(nxt, 2)
    pos = mat['p']
    d1x = sub(fl(pos[0]), x1)
    d1y = sub(fl(pos[1]), y1)
    dist = to_f32_bits(sqrt_v(add(mul(d1y, d1y), mul(d1x, d1x))))
    ex = sub(x2, x1)
    ey = sub(y2, y1)
    seg = to_f32_bits(sqrt_v(add(mul(ey, ey), mul(ex, ex))))
    along = to_f32_bits(add(mul(ey, d1y), mul(ex, d1x)))

    pick = False
    if lt_ordered(fl(dist), from_bits(K_8)):
        pick = True
        marks.append('near8')
    elif gt(fl(along), from_bits(K_0)) and lt_ordered(fl(dist), from_bits(K_TWELVE)):
        pick = True
        marks.append('near12')
    else:
        ratio = div(fl(along), mul(fl(seg), fl(dist)))
        if gt(ratio, from_bits(K_07)):
            pick = True
            marks.append('aligned')
        elif nxt == cur:
            pick = True
            marks.append('same_link_pick')
        else:
            marks.append('skip_nopick')

    if pick:
        if cur_cnt() < 4:
            marks.append('pick_search')
            vortex = 1 if st['model'] == 0x21B else 0
            ev([2, 1, 0, mat['p'][0], mat['p'][1], mat['p'][2], st['nextn'], st['dest0'], c['dest'][1], c['dest'][2], tag(VEH, 0x3F8), tag(VEH, 0x418), 8, 0, K_SEARCH, 0, K_SEARCH, (st['f3db'] >> 6) & 1,
                1, vortex, 0, st['count'], st['curn'], st['nextn']])
            n = signed16b(c['path_cnt'])
            st['count'] = n & 0xFFFF
            for i in range(8):
                if i < n:
                    nodes[i] = c['ps'][i] & M32
            after()
            if cur_cnt() > 0:
                remove_one()
                marks.append('search_rm1')
                if cur_cnt() > 0 and st['nextn'] == nodes[0]:
                    remove_one()
                    marks.append('search_rm2')
            if cur_cnt() < 1:
                marks.append('search_fail')
                st['mission'] = 9
                heading(st['dest0'], c['dest'][1])
                return state()
        else:
            marks.append('pick_nosearch')
        # desplazamiento de nodos, enlaces, signos y carril
        marks.append('shift')
        chosen = nodes[0]
        old_cur, old_next = st['curn'], st['nextn']
        st['nextn'], st['prevn'], st['curn'] = chosen, old_cur, old_next
        remove_one()
        link_cur, link_next, sign_cur = st['cur'], st['nxt'], st['s5']
        st['prevl'] = link_cur
        st['s5'] = st['s6']
        st['s4'] = sign_cur
        st['cur'] = link_next
        st['l7'] = st['l8']
        here = st['curn']
        area, node = here & 0xFFFF, here >> 16
        if area not in areas or node >= 8:
            raise Unsafe('nodo actual fuera de las tablas')
        slot = areas.index(area)
        base = signed16b(c['node_base'][slot])
        idx = 0
        while True:
            if base + idx >= 40:
                raise Unsafe('busqueda de enlace sin fin')
            if (c['nl%d' % slot][base + idx] & M32) == st['nextn']:
                break
            idx += 1
        st['nxt'] = c['navi%d' % slot][base + idx] & 0xFFFF
        if not loaded(st['nxt'] >> 10) or (st['nxt'] & 0x3FF) >= 8:
            raise Unsafe('enlace de navegacion fuera de las tablas')
        st['s6'] = 0xFF if less(st['curn'], st['nextn']) else 1

    # ---- ventana de enlaces de adelanto (el rumbo empieza con la distancia: su mitad baja la pisa cada FindLinkBetweenNodes)
    head = dist
    A = [st['cur'], st['nxt']] + [0xFFFF] * 6
    S = [st['s5'], st['s6']] + [0] * 6
    Bn = [st['curn'], st['nextn']] + [0xFFFF] * 6
    for k in range(4):
        node = nodes[k]
        if k < cur_cnt() and (node & 0xFFFF) != 0xFFFF:
            Bn[2 + k] = node
            ev([4, 1, 0x20000000, Bn[1 + k], node])
            A[2 + k] = c['fl'][cnt['link'] & 3] & 0xFFFF
            cnt['link'] += 1
            head = (head & 0xFFFF0000) | A[2 + k]      # el resultado de 16 bits se escribe en la pila sobre la mitad baja del rumbo (misma direccion)
            after()
            S[2 + k] = 0xFF if less(Bn[1 + k], Bn[2 + k]) else 1
        else:
            A[2 + k] = 0xFFFF
            Bn[2 + k] = 0xFFFF | (Bn[2 + k] & 0xFFFF0000)
    A[6] = A[7] = 0xFFFF
    marks.append('look_%d' % sum(1 for k in range(2, 6) if A[k] != 0xFFFF))

    # ---- curva (DealWithBend_Racing)
    fac = K_1
    found = 0xFFFF
    for k in range(4):
        ev([5, tag(VEH, 0), A[k], A[k + 1], A[k + 2], A[k + 3]] + [(S[k + j] & 0xFF) if A[k + j] != 0xFFFF else 0 for j in range(4)] +
           [speed_along, STACK, STACK, STACK, STACK, STACK, st['cur'], st['nxt']])
        i = cnt['bend'] & 3
        cnt['bend'] += 1
        if c['bend_w'][i] & 1:
            head = c['bend_head'][i] & M32
        if c['bend_w'][i] & 2:
            fac = c['bend_fac'][i] & M32
        ret = c['bend_ret'][i] != 0
        after()
        if ret:
            found = A[k]
            marks.append('bend_found')
            marks.append('bend_idx_%d' % k)
            break
    vh = to_f32_bits(atan_of_xy(fl(fnx), fl(fny)))

    def clip(link, sign, xb, yb):
        nonlocal head
        ev([6, tag(VEH, 0), link, sign & 0xFF, STACK, head, xb, yb, st['cur'], st['nxt'], st['s5'], st['s6']])
        i = cnt['clip'] % 6
        cnt['clip'] += 1
        if c['clip_w'][i]:
            head = c['clip_v'][i] & M32
        after()

    def clip_link(link, sign):
        clip(link, sign, to_f32_bits(pos8(link, 0)), to_f32_bits(pos8(link, 2)))

    def blend_clip(t):
        cur_e, nxt_e = st['cur'], st['nxt']
        rest = sub(from_bits(K_1), t)
        by = to_f32_bits(add(mul(pos8(cur_e, 2), t), mul(pos8(nxt_e, 2), rest)))
        bx = to_f32_bits(add(mul(pos8(cur_e, 0), t), mul(pos8(nxt_e, 0), rest)))
        clip(cur_e, st['s5'], bx, by)

    def perc_call():
        ev([7, tag(VEH, 0), st['cur']])
        p = c['perc'] & M32
        after()
        return fl(p)

    if found == 0xFFFF:
        marks.append('no_bend')
        head = vh
        for e in (5, 4, 3, 2):
            if A[e] != 0xFFFF:
                marks.append('clip_look')
                clip_link(A[e], S[e])
        clip_link(st['nxt'], st['s6'])
        marks.append('clip_next')
        blend_clip(perc_call())
    else:
        if st['cur'] != found:
            clip_link(st['nxt'], st['s6'])
            marks.append('clip_next')
        p = perc_call()
        blend_clip(mul(add(p, from_bits(K_2)), from_bits(K_THIRD)))

    # ---- correccion del rumbo, normalizacion y limite de giro
    ev([8, tag(VEH, 0), 0, head, vh, K_2])
    weave = c['weave'] & M32
    after()
    marks.append('weave')
    angle = sub(fl(weave), fl(vh))
    steer = to_f32_bits(angle)
    if lt_ordered(angle, from_bits(K_NPI)):
        marks.append('norm_low')
        while True:
            angle = add(angle, from_bits(K_2PI))
            if not lt_ordered(angle, from_bits(K_NPI)):
                break
        steer = to_f32_bits(angle)
    if gt(angle, from_bits(K_PI)):
        marks.append('norm_high')
        while True:
            angle = sub(angle, from_bits(K_2PI))
            if not gt(angle, from_bits(K_PI)):
                break
        steer = to_f32_bits(angle)
    sv = [st['speed0'], c['speed'][1], c['speed'][2]]
    mag = sqrt_v(add(add(mul(fl(sv[0]), fl(sv[0])), mul(fl(sv[1]), fl(sv[1]))), mul(fl(sv[2]), fl(sv[2]))))
    clamped = False
    if gt(mag, from_bits(K_07)):
        marks.append('limit_fast')
        limit = K_02
    else:
        marks.append('limit_slow')
        room = sub(from_bits(K_09), mag)
        if lt_ordered(from_bits(K_07), room):
            marks.append('limit_room')
            limit = K_07
        else:
            limit = to_f32_bits(room)
    neg = to_f32_bits(chs(fl(limit)))
    if lt_ordered(angle, fl(neg)):
        marks.append('clamp_lo')
        clamped = True
        steer = neg
        angle = fl(neg)
    if gt(angle, fl(limit)):
        marks.append('clamp_hi')
        steer = limit
        clamped = True

    # ---- puente y factor
    ev([9, tag(VEH, 0)])
    bridge = c['bridge'] != 0
    after()
    traf = K_1
    if bridge:
        marks.append('bridge_stop')
        ev([10, tag(VEH, 0)])
        after()
        traf = K_0
    if lt_ordered(fl(traf), fl(fac)):
        marks.append('fac_traffic')
        lowest = traf
    else:
        marks.append('fac_bend')
        lowest = fac

    # ---- velocidad deseada, gas y freno
    desired = mul(from_int(st['cruise']), fl(lowest))
    out[2] = K_0
    err = to_f32_bits(sub(desired, fl(speed_along)))
    if lt_ordered(desired, from_bits(K_005)) and lt_ordered(fl(err), from_bits(K_003)):
        marks.append('brake_full')
        out[2], out[1] = K_1, K_0
    elif gt(fl(err), from_bits(K_0)):
        marks.append('gas_branch')
        if clamped and gt(fl(speed_along), from_bits(K_7)):
            marks.append('gas_c7')
            out[1] = K_0
        else:
            if lt_ordered(fl(speed_along), from_bits(K_2)):
                marks.append('gas_slow')
                gas_reg = mul(fl(err), from_bits(K_025))
            else:
                marks.append('gas_fast')
                gas_reg = mul(fl(err), from_bits(K_0125))
            if lt_ordered(from_bits(K_1), gas_reg):
                marks.append('gas_clamp1')
                out[1] = K_1
            else:
                out[1] = to_f32_bits(gas_reg)
            if signed32(st['subtype']) == 10 and gt(fl(err), from_bits(K_3)):
                marks.append('bike_branch')
                if le_ord(fl(st['timer']), from_bits(K_0)):
                    marks.append('bike_timer_set')
                    st['timer'] = 0x41700000
    else:
        marks.append('brake_branch')
        out[1] = K_0
        if signed32(st['subtype']) == 10:
            marks.append('brake10')
            brk = mul(fl(err), from_bits(K_BRK10))
            if lt_ordered(from_bits(K_1), brk):
                marks.append('brake10_clamp')
                out[2] = K_1
            else:
                out[2] = to_f32_bits(brk)
        else:
            marks.append('brake_other')
            brk = mul(fl(err), from_bits(K_M005))
            if lt_ordered(from_bits(K_05), brk):
                marks.append('brake_clamp05')
                out[2] = K_05
            else:
                out[2] = to_f32_bits(brk)
    out[0] = steer
    out[3] = 0
    return state()


def expected(c):
    return model_racing(c)


def check_self():
    assert to_f32_bits(mul(from_int(1), from_bits(K_DIR))) == K_DIR
    assert signed8(0xFF) == -1 and signed8(0x7F) == 127
    assert to_f32_bits(sub(from_bits(K_05), mul(from_int(3), from_bits(K_05)))) == 0xBF800000
    assert to_f32_bits(atan_of_xy(from_bits(K_1), from_bits(K_0))) == K_2PI
    print('ok')


def _first_diff(o, e):
    for i, (g, w) in enumerate(zip(o['events'], e['events'])):
        if g != w:
            return 'evento %d: %s != %s' % (i, g, w)
    if len(o['events']) != len(e['events']):
        return 'numero de eventos %d != %d' % (len(o['events']), len(e['events']))
    return 'v %s/%s mat %s/%s out %s/%s' % (o['v'], e['v'], o['mat'], e['mat'], o['out'], e['out'])


def check_row_obj(row):
    """(mensajes de desacuerdo, marcas del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('ev_hash', 'ev_count', 'events', 'v', 'mat', 'out') if o[k] != e[k]]
        if any(o['changes']):
            bad.append('changes')
        if bad:
            msgs.append('DIFIERE %s caso %s campos %s %s' % (side, row['case'], bad, _first_diff(o, e)))
    return msgs, e['marks']


def check_row(line):
    row = json.loads(line)
    if row.get('event') != 'comparison':
        return None
    msgs, marks = check_row_obj(row)
    return row['case'], msgs, marks


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
