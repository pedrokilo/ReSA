"""Oraculo independiente (Python) de Car84H: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x434900..0x43579A), lo que debe observarse de
CCarCtrl::SteerAICarWithPhysicsFollowPath (traza EN ORDEN de los colaboradores con sus argumentos y el estado del vehiculo en cada llamada, hash de todos los eventos, el estado final del vehiculo
(tramos, multiplicadores, carriles, estilo, mision, accion temporal y su tiempo, crucero, destino.x, velocidad.x, subtipo, temporizador), la matriz y las cuatro salidas; ningun otro cambio en las arenas) y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con enteros exactos. fpatan (GetATanOfXY, 0x53CC70) se evalua con 100 digitos (el EXE lo deja sin redondear en el registro).

Flujo leido (cdecl, 5 argumentos; ret): StopCarIfNodesAreInvalid (si devuelve verdadero no hace mas); adelante normalizado (rumbo del vehiculo mas adelante); las dos areas de tramos (+0x3A4, +0x3A6) deben
estar cargadas (si no: giro 0, acelerador 0, freno 1, sin freno de mano); direcciones escaladas (x 0.01 por +0x3B5 / +0x3B6), desplazamientos de carril (OneWayLaneOffset + carril, x 5.4; motos +1.458), puntos 1 y 2;
distancia al punto 1, longitud del tramo y producto; con < 5 m, < 8 m con producto positivo, o razon > 0.7 elige el siguiente nodo (PickNextNodeAccordingStrategy) y segun la mision (- 2, tabla de 0x4357C0) llama a
HeadingForTarget hacia el destino, el jugador o el objetivo y termina; si no, recalcula el punto 1; punto objetivo (adelantado 0.35 x distancia hacia atras salvo a mas de 40 m); rumbo, correccion con estilos 2, 5 y 6,
normalizacion a [-pi, pi] y limite FindMaxSteerAngle; factor de trafico por estilo (+0x3B9); multiplicadores por el angulo al carril y por la curva (FindSpeedMultiplier); velocidad deseada, gas y freno; con las misiones 8 y 12
sin accion temporal y a menos de 8 m del destino casi perpendicular, espera 2 s.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000. Indices: 0 vehiculo, 1 matriz, 2 salidas, 3 objetivo, 4 matriz del objetivo, 5..7 tramos, 8 caminos.
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


def model_follow(c):
    """CCarCtrl::SteerAICarWithPhysicsFollowPath (desensamblado leido): ver README.md"""
    m = Model()
    marks = []
    st = dict(cur=c['cur'] & 0xFFFF, nxt=c['next'] & 0xFFFF, s5=c['s5'] & 0xFF, s6=c['s6'] & 0xFF, l7=c['l7'] & 0xFF, l8=c['l8'] & 0xFF, style=c['style'] & 0xFF, mission=c['mission'] & 0xFF,
              tact=c['tact'] & 0xFF, ttime=c['ttime'] & M32, cruise=c['cruise'] & 0xFF, subtype=c['subtype'] & M32, timer=c['timer'] & M32, speed0=c['speed'][0] & M32, dest0=c['dest'][0] & M32,
              now=c['now'] & M32)
    mat = dict(r=list(c['mr']), f=list(c['mf']), u=list(c['mu']), p=list(c['mp']))
    out = [c['o0'][0] & M32, c['o0'][1] & M32, c['o0'][2] & M32, c['o0'][3] & 0xFF]
    evn = [0]
    mult_n = [0]
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
            st['mission'] = v & 0xFF
        elif k == 2:
            st['tact'] = v & 0xFF
        elif k == 3:
            st['style'] = v & 0xFF
        elif k == 4:
            st['cruise'] = v & 0xFF
        elif k == 5:
            st['now'] = v
        elif k == 6:
            st['speed0'] = v
        elif k == 7:
            mat['f'][0] = v
        elif k == 8:
            st['cur'] = v & 0xFFFF
        elif k == 9:
            st['nxt'] = v & 0xFFFF
        elif k == 10:
            st['l7'] = v & 0xFF
        elif k == 11:
            st['subtype'] = v
        elif k == 12:
            mat['p'][0] = v
        elif k == 13:
            st['dest0'] = v
        elif k == 14:
            st['s5'] = v & 0xFF
        elif k == 15:
            st['timer'] = v

    def state():
        mats = mat['r'] + mat['f'] + mat['u'] + mat['p']
        return dict(events=m.events, ev_hash=m.hash, ev_count=m.count,
                    v=[st['mission'], st['tact'], st['ttime'], st['cur'], st['nxt'], st['s5'], st['s6'], st['l7'], st['l8'], st['style'], st['cruise'], st['timer'], st['speed0'], st['subtype'], st['dest0'], st['now']],
                    mat=mats, out=list(out), marks=marks)

    def loaded(area):
        return area in areas

    def lb(word, off):
        area, ident = word >> 10, word & 0x3FF
        if area not in areas or ident >= 8:
            raise Unsafe('tramo fuera de las tablas')
        return slots[areas.index(area)][ident * 14 + off]

    def i8l(word, off):
        return signed8(lb(word, off))

    def i16l(word, off):
        return signed16(lb(word, off) | (lb(word, off + 1) << 8))

    def pos8(word, off):
        """int16 * 0.125 (0x858C48), exacto"""
        return mul(from_int(i16l(word, off)), from_bits(K_0125))

    def lane(word):
        """CCarPathLink::OneWayLaneOffset (0x44DB00): ST0 al volver"""
        flags = lb(word, 0xB)
        opp = flags & 7
        if opp == 0:
            return sub(from_bits(K_05), mul(from_int((flags >> 3) & 7), from_bits(K_05)))
        if (flags & 0x38) == 0:
            return sub(from_bits(K_05), mul(from_int(opp), from_bits(K_05)))
        return add(mul(from_int(lb(word, 0xA)), from_bits(K_LANEK)), from_bits(K_05))

    def direction(word, off, mult):
        return to_f32_bits(mul(mul(from_int(i8l(word, off)), from_bits(K_DIR)), from_int(signed8(mult))))

    def all_stop():
        out[0], out[1], out[2], out[3] = 0, 0, K_1, 0

    def heading(x, y):
        ev([3, tag(VEH, 0), 0, x, y, tag(OUTA, 0), tag(OUTA, 4), tag(OUTA, 8), tag(OUTA, 12), st['mission'], st['cur'], st['nxt']])
        out[0], out[1], out[2], out[3] = c['hf'][0] & M32, c['hf'][1] & M32, c['hf'][2] & M32, c['hf_hb'] & 0xFF
        after()

    # ---- StopCarIfNodesAreInvalid
    ev([1, tag(VEH, 0), st['cur'], st['nxt'], st['mission'], st['tact']])
    stop = c['stop_ret'] != 0
    after()
    if stop:
        marks.append('stop_true')
        return state()

    # ---- adelante normalizado (la matriz siempre existe)
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
    s5, s6 = st['s5'], st['s6']
    q1x, q1y = direction(cur, 8, s5), direction(cur, 9, s5)
    q2x, q2y = direction(nxt, 8, s6), direction(nxt, 9, s6)
    r1 = to_f32_bits(mul(add(lane(cur), from_int(signed8(st['l7']))), from_bits(K_LANE)))
    r2 = mul(add(lane(nxt), from_int(signed8(st['l8']))), from_bits(K_LANE))
    if signed32(st['subtype']) == 10:
        marks.append('bike')
        r1 = to_f32_bits(add(fl(r1), from_bits(K_BIKE)))
        r2 = add(r2, from_bits(K_BIKE))
    p1x = to_f32_bits(add(pos8(cur, 0), mul(fl(r1), fl(q1y))))
    p1y = to_f32_bits(sub(pos8(cur, 2), mul(fl(r1), fl(q1x))))
    p2x = to_f32_bits(add(pos8(nxt, 0), mul(r2, fl(q2y))))
    p2y = to_f32_bits(sub(pos8(nxt, 2), mul(r2, fl(q2x))))
    pos = mat['p']
    d1x = sub(fl(pos[0]), fl(p1x))
    d1y = sub(fl(pos[1]), fl(p1y))
    dist = to_f32_bits(sqrt_v(add(mul(d1y, d1y), mul(d1x, d1x))))
    ex = sub(fl(p2x), fl(p1x))
    ey = sub(fl(p2y), fl(p1y))
    seg = to_f32_bits(sqrt_v(add(mul(ey, ey), mul(ex, ex))))
    along = to_f32_bits(add(mul(ey, d1y), mul(ex, d1x)))

    pick = False
    if lt_ordered(fl(dist), from_bits(K_5)):
        pick = True
        marks.append('near5')
    elif gt(fl(along), from_bits(K_0)) and lt_ordered(fl(dist), from_bits(K_8)):
        pick = True
        marks.append('near8')
    else:
        ratio = div(fl(along), mul(fl(seg), fl(dist)))
        if gt(ratio, from_bits(K_07)):
            pick = True
            marks.append('aligned')
        elif nxt != cur:
            marks.append('skip_target')
        else:
            pick = True
            marks.append('same_link_pick')

    if pick:
        # PickNextNodeAccordingStrategy
        ev([2, tag(VEH, 0), st['cur'], st['nxt'], st['mission'], st['s5'], st['s6'], st['l7'], st['l8'], st['style'], st['tact'], st['now']])
        if c['pick_mut']:
            st['cur'], st['nxt'] = c['pick_cur'] & 0xFFFF, c['pick_next'] & 0xFFFF
            st['s5'], st['s6'], st['l7'], st['l8'] = c['pick_s5'] & 0xFF, c['pick_s6'] & 0xFF, c['pick_l7'] & 0xFF, c['pick_l8'] & 0xFF
            st['mission'] = c['pick_mission'] & 0xFF
        pick_ret = c['pick_ret'] != 0
        after()
        marks.append('pick_true' if pick_ret else 'pick_false')
        if pick_ret:
            mission = signed8(st['mission'])
            idx = mission - 2
            if 0 <= idx <= 0x42:
                def target_xy():
                    if c['tmat']:
                        return c['tmp'][0], c['tmp'][1]
                    return c['tpos'][0], c['tpos'][1]
                if mission in (8, 12):
                    marks.append('mission_%d' % mission)
                    st['mission'] = 9 if mission == 8 else 0xD
                    heading(st['dest0'], c['dest'][1])
                    return state()
                if mission in (2, 4, 0x2B):
                    marks.append({2: 'mission_2', 4: 'mission_4', 0x2B: 'mission_2b'}[mission])
                    st['mission'] = 5 if mission == 4 else 3 if mission == 2 else 0x2C
                    ev([4, STACK, 0xFFFFFFFF])
                    after()
                    heading(c['player'][0], c['player'][1])
                    return state()
                if mission in (15, 17):
                    marks.append('mission_%d' % mission)
                    st['mission'] = 0x10 if mission == 15 else 0x12
                    tx, ty = target_xy()
                    heading(tx, ty)
                    return state()
                if 0x41 <= mission <= 0x44:
                    marks.append('mission_4x')
                    st['mission'] = mission - 0x24
                    tx, ty = target_xy()
                    heading(tx, ty)
                    return state()
                marks.append('mission_default')
            else:
                marks.append('mission_out')
        # recalculo del punto 1
        marks.append('recompute')
        cur, nxt = st['cur'], st['nxt']
        if not loaded(cur >> 10):
            marks.append('unloaded_cur')
            all_stop()
            return state()
        if not loaded(nxt >> 10):
            marks.append('unloaded_next')
            all_stop()
            return state()
        r1 = to_f32_bits(mul(add(lane(cur), from_int(signed8(st['l7']))), from_bits(K_LANE)))
        pos = mat['p']
        ax = sub(add(pos8(cur, 0), mul(fl(r1), fl(q1y))), fl(pos[0]))
        ay = sub(sub(pos8(cur, 2), mul(fl(r1), fl(q1x))), fl(pos[1]))
        dist = to_f32_bits(sqrt_v(add(mul(ay, ay), mul(ax, ax))))
        s5, s6 = st['s5'], st['s6']
        q1x, q1y = direction(cur, 8, s5), direction(cur, 9, s5)
        q2x, q2y = direction(nxt, 8, s6), direction(nxt, 9, s6)

    # ---- punto objetivo
    x1t, y1t = pos8(cur, 0), pos8(cur, 2)
    r1q1y = mul(fl(r1), fl(q1y))
    px = to_f32_bits(add(x1t, r1q1y))
    r1q1x = to_f32_bits(mul(fl(r1), fl(q1x)))
    py = to_f32_bits(sub(y1t, fl(r1q1x)))
    tx = to_f32_bits(sub(fl(px), mul(mul(fl(dist), fl(q1x)), from_bits(K_LOOK))))
    ty = to_f32_bits(sub(fl(py), mul(mul(fl(dist), fl(q1y)), from_bits(K_LOOK))))
    if gt(fl(dist), from_bits(K_40)):
        marks.append('far40')
        tx, ty = px, py
    else:
        marks.append('lookahead')
    pos = mat['p']
    xdiff = to_f32_bits(sub(fl(tx), fl(pos[0])))
    ydiff = to_f32_bits(sub(fl(ty), fl(pos[1])))
    h1 = to_f32_bits(atan_of_xy(fl(xdiff), fl(ydiff)))
    vh = to_f32_bits(atan_of_xy(fl(fnx), fl(fny)))
    if st['style'] in (2, 5, 6):
        marks.append('weave')
        ev([5, tag(VEH, 0), 0, h1, vh, K_1])
        weave = c['weave'] & M32
        after()
        angle = sub(fl(weave), fl(vh))
    else:
        marks.append('noweave')
        angle = sub(fl(h1), fl(vh))
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
    ev([6, tag(VEH, 0)])
    max_steer = c['max_steer'] & M32
    after()
    if lt_ordered(fl(steer), chs(fl(max_steer))):
        marks.append('steer_clamp_lo')
        steer = to_f32_bits(chs(fl(max_steer)))
    if gt(fl(steer), fl(max_steer)):
        marks.append('steer_clamp_hi')
        steer = to_f32_bits(fl(max_steer))

    # ---- velocidad y factor de trafico
    sv = [st['speed0'], c['speed'][1], c['speed'][2]]
    fwd = mat['f']
    dot_sp = add(add(mul(fl(sv[2]), fl(fwd[2])), mul(fl(sv[1]), fl(fwd[1]))), mul(fl(sv[0]), fl(fwd[0])))
    speed_along = to_f32_bits(mul(dot_sp, from_bits(K_60)))
    sty = signed8(st['style'])
    if 0 <= sty <= 6 and sty in (0, 1, 4, 6):
        marks.append('style_traffic')
        ev([7, tag(VEH, 0), st['style']])
        max_traffic = c['max_traffic'] & M32
        after()
        factor_v = to_f32_bits(div(fl(max_traffic), from_int(st['cruise'])))
    else:
        marks.append('style_other')
        factor_v = K_1
    sty2 = signed8(st['style'])
    if 0 <= sty2 <= 6 and sty2 in (0, 1, 5, 6):
        marks.append('light_called')
        ev([8, tag(VEH, 0), 0])
        light = c['light'] != 0
        after()
        if light:
            marks.append('light_stop')
            ev([10, tag(VEH, 0)])
            after()
            factor_v = K_0
    ev([9, tag(VEH, 0)])
    bridge = c['bridge'] != 0
    after()
    if bridge:
        marks.append('bridge_stop')
        ev([10, tag(VEH, 0)])
        after()
        factor_v = K_0

    # ---- multiplicadores
    cur_e = st['cur']
    w1 = mul(add(lane(cur_e), from_int(signed8(st['l7']))), from_bits(K_LANE))
    pos3 = mat['p']
    ahead_y = sub(sub(pos8(cur_e, 2), mul(w1, fl(q1x))), fl(pos3[1]))
    ahead_x = sub(add(pos8(cur_e, 0), mul(w1, fl(q1y))), fl(pos3[0]))
    xarg, yarg = to_f32_bits(ahead_x), to_f32_bits(ahead_y)
    ang1 = to_f32_bits(sub(atan_of_xy(fl(xarg), fl(yarg)), fl(vh)))
    ev([11, ang1, K_04, K_12, K_04])
    mult1 = c['mult'][mult_n[0] & 1] & M32
    mult_n[0] += 1
    after()
    hl1 = to_f32_bits(atan_of_xy(fl(q1x), fl(q1y)))
    bend = to_f32_bits(sub(fl(hl1), atan_of_xy(fl(q2x), fl(q2y))))
    ev([11, bend, K_01, K_12, K_04])
    mult2 = c['mult'][mult_n[0] & 1] & M32
    mult_n[0] += 1
    after()
    if gt(fl(dist), from_bits(K_40)) or st['cruise'] < 12:
        marks.append('bend_one')
        m3 = from_bits(K_1)
    else:
        marks.append('bend_formula')
        m3 = sub(from_bits(K_1), mul(sub(from_bits(K_1), mul(fl(dist), from_bits(K_DIST))), sub(from_bits(K_1), fl(mult2))))
    first_lower = lt_ordered(fl(mult1), m3)
    lowest = fl(mult1) if first_lower else m3
    if lt_ordered(lowest, fl(factor_v)):
        if first_lower:
            marks.append('f_mult1')
            factor = mult1
        else:
            marks.append('f_bend')
            factor = to_f32_bits(m3)
    else:
        marks.append('f_traffic')
        factor = factor_v

    # ---- velocidad deseada, gas y freno
    desired = mul(from_int(st['cruise']), fl(factor))
    out[2] = K_0
    err = to_f32_bits(sub(desired, fl(speed_along)))
    if lt_ordered(desired, from_bits(K_005)) and lt_ordered(fl(err), from_bits(K_003)):
        marks.append('brake_full')
        out[2], out[1] = K_1, K_0
    elif gt(fl(err), from_bits(K_0)):
        marks.append('gas_branch')
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
                st['timer'] = 0x41200000
    else:
        marks.append('brake_branch')
        out[1] = K_0
        brk = mul(fl(err), from_bits(K_BRK))
        if lt_ordered(from_bits(K_05), brk):
            marks.append('brake_clamp05')
            out[2] = K_05
        else:
            out[2] = to_f32_bits(brk)
    out[0] = steer
    out[3] = 0

    # ---- misiones 8 y 12 sin accion temporal
    mission = signed8(st['mission'])
    if mission not in (8, 12):
        return state()
    marks.append('end_mission')
    if st['tact'] != 0:
        marks.append('end_tact')
        return state()
    here = mat['p']
    dest = [st['dest0'], c['dest'][1], c['dest'][2]]
    off3 = [to_f32_bits(sub(fl(here[k]), fl(dest[k]))) for k in range(3)]
    mag_reg = sqrt_v(add(add(mul(fl(off3[0]), fl(off3[0])), mul(fl(off3[1]), fl(off3[1]))), mul(fl(off3[2]), fl(off3[2]))))
    mag = to_f32_bits(mag_reg)
    if not lt_ordered(mag_reg, from_bits(K_8)):
        marks.append('end_far')
        return state()
    proj = add(add(mul(fl(here[2]), fl(fwd[2])), mul(fl(here[1]), fl(fwd[1]))), mul(fl(here[0]), fl(fwd[0])))
    if lt_ordered(proj, from_bits(K_0)):
        proj = chs(proj)
    if not lt_ordered(div(proj, fl(mag)), from_bits(K_02)):
        marks.append('end_nowait')
        return state()
    marks.append('end_wait')
    st['tact'] = 3
    st['ttime'] = (st['now'] + 0x7D0) & M32
    return state()


def expected(c):
    return model_follow(c)


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
