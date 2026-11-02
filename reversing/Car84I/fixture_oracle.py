"""Oraculo independiente (Python) de Car84I: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x436A90..0x437B22), lo que debe observarse de
CCarCtrl::SteerAICarWithPhysics_OnlyMission (traza EN ORDEN de los colaboradores con sus argumentos y el estado del vehiculo en cada llamada, hash de todos los eventos, el estado final del vehiculo
(mision, accion temporal y su tiempo, CTimer, velocidad.x, semilla, banderas, distancia de seguimiento, destino.x, posicion.x), las matrices del vehiculo y de los dos vehiculos del jugador, el objetivo, los
vehiculos del jugador y las cuatro salidas; ningun otro cambio en las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con enteros exactos.

Flujo leido (cdecl, 5 argumentos; ret): la mision (byte con signo en +0x3BA; sin signo > 0x44 no hace nada) indexa una tabla de 44 destinos. Las misiones 0, 10 y 11 dejan giro 0, gas 0, freno 0.5 y freno de mano;
la mayoria llama a un colaborador con el vehiculo y las cuatro salidas (o solo el vehiculo, o con el destino, el objetivo o el jugador); cuatro calculan antes de llamar a HeadingForTarget: 3 (embestir al jugador de
cerca: punto desplazado por la semilla), 44 (acercarse al jugador por delante o por detras), 53 (seguir a un coche: gas o freno por la diferencia de velocidad) y 60 (junto al objetivo, a 10 m de el).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000. Indices: 0 vehiculo, 1 matriz, 2 salidas, 3 objetivo, 4 matriz del objetivo,
5 y 6 vehiculos del jugador, 7 y 8 sus matrices, 9 modelos, 10 pasajero, 11 zona de retorno de los falsos.
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



# ---------------------------------------------------------------- modelo de CCarCtrl::SteerAICarWithPhysics_OnlyMission (0x436A90..0x437B22)
K_ZERO, K_ONE, K_HALF, K_GAP, K_SPGAIN, K_TWO, K_SEEDK, K_DOTK, K_SPLIM, K_TEN, K_SIXTY, K_FIVE, K_BRK, K_GASK, K_SEVEN = (
    0x00000000, 0x3F800000, 0x3F000000, 0x3E4CCCCD, 0x41400000, 0x40000000, 0x3BCCCCCD, 0xBCA3D70A, 0x3ECCCCCD, 0x41200000, 0x42700000, 0x40A00000, 0xBDCCCCCD, 0x3D4CCCCD, 0x40E00000)
K_P4, K_M4 = 0x40800000, 0xC0800000
VEH, MATA, OUTA, TGTA, TMATA, PV0, PV1, PVM0, PVM1, MODELA, PEDA, MISCA = range(12)
M1 = 0xFFFFFFFF


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


def kf(bits):
    return from_bits(bits)


# mision -> colaborador. Las de cinco punteros (veh + 4 salidas) y las de aire (solo el vehiculo)
FIVE = {}
for _m in (1, 2, 4, 8, 12, 15, 17, 43, 52, 54, 61, 65, 66, 67, 68):
    FIVE[_m] = 14
FIVE.update({33: 15, 34: 16, 21: 17, 64: 18, 26: 19, 45: 20, 49: 20, 46: 21, 50: 21})
ONE = {20: 0, 22: 1, 23: 2, 56: 2, 35: 3, 59: 3, 36: 4, 37: 5, 38: 6, 39: 7, 41: 7, 40: 8, 42: 9, 47: 10, 48: 10, 51: 11, 57: 12, 58: 13, 62: 14, 63: 15}


def model_mission(c):
    """CCarCtrl::SteerAICarWithPhysics_OnlyMission (desensamblado leido): ver README.md"""
    m = Model()
    marks = []
    st = dict(mission=c['mission'] & 0xFF, tact=c['tact'] & 0xFF, ttime=c['ttime'] & M32, now=c['now'] & M32, rseed=c['rseed'] & 0xFFFF, flags=c['flags'] & M32, follow=c['follow'] & 0xFF)
    spd = [x & M32 for x in c['speed']]
    pp = [x & M32 for x in c['pp']]
    dest = [x & M32 for x in c['dest']]
    mat = dict(r=list(c['mr']), f=list(c['mf']), u=list(c['mu']), p=list(c['mp']))
    tgt = dict(pp=list(c['tpp']), mp=list(c['tmp']), sp=list(c['tspeed']))
    pvm = [dict(r=list(c['pr%d' % j]), f=list(c['pf%d' % j]), u=list(c['pu%d' % j]), p=list(c['pp2_%d' % j])) for j in range(2)]
    pvsp = [list(c['pspeed%d' % j]) for j in range(2)]
    out = [c['o0'][0] & M32, c['o0'][1] & M32, c['o0'][2] & M32, c['o0'][3] & 0xFF]
    cnt = dict(ev=0, v=0, c=0, s=0, mt=0)
    has_mat = c['has_mat'] != 0
    own_p = mat['p'] if has_mat else pp
    tgt_p = tgt['mp'] if c['tmat'] else tgt['pp']

    def ev(e):
        m.push([x & M32 for x in e])

    def after():
        idx = cnt['ev']
        cnt['ev'] += 1
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
            st['ttime'] = v
        elif k == 4:
            st['now'] = v
        elif k == 5:
            spd[0] = v
        elif k == 6:
            own_p[0] = v
        elif k == 7:
            dest[0] = v
        elif k == 8:
            st['follow'] = v & 0xFF
        elif k == 9:
            st['rseed'] = v & 0xFFFF
        elif k == 10:
            st['flags'] = v
        elif k == 11:
            tgt_p[0] = v
        elif k == 12:
            tgt['sp'][0] = v
        elif k == 13:
            pvsp[0][0] = v
        elif k == 14:
            mat['f'][0] = v
        elif k == 15:
            pvm[0]['r'][0] = v
        elif k == 16:
            pvm[1]['f'][0] = v

    def state_tail():
        return [st['mission'], st['tact'], st['ttime'], st['now']]

    # ---- falsos de los colaboradores
    def f_coors():
        ev([1, STACK, M1])
        r = list(c['pc%d' % (cnt['c'] & 1)])
        cnt['c'] += 1
        after()
        return r

    def f_veh():
        ev([2, M1, 0])
        r = None if c['pv_null'] else (cnt['v'] & 1)
        cnt['v'] += 1
        after()
        return r

    def f_speed():
        ev([3, M1])
        r = cnt['s'] & 1
        cnt['s'] += 1
        after()
        return r

    def f_entity():
        ev([4, M1])
        after()
        return tag(MISCA, 0x20)

    def put_out():
        out[0], out[1], out[2], out[3] = c['ov'][0] & M32, c['ov'][1] & M32, c['ov'][2] & M32, c['ov'][3] & 1

    def otags():
        return [tag(OUTA, 0), tag(OUTA, 4), tag(OUTA, 8), tag(OUTA, 0xC)]

    def heading(tgt_tag, x, y):
        ev([9, tag(VEH), tgt_tag, x, y] + otags() + state_tail())
        put_out()
        after()

    def five(kind):
        ev([kind, tag(VEH)] + otags() + state_tail())
        put_out()
        after()

    def one(k):
        ev([30 + k, tag(VEH)] + state_tail())
        after()

    def vsub(a, b):
        return [to_f32_bits(sub(fl(a[k]), fl(b[k]))) for k in range(3)]

    def mag_reg(v):
        return sqrt_v(add(add(mul(fl(v[0]), fl(v[0])), mul(fl(v[1]), fl(v[1]))), mul(fl(v[2]), fl(v[2]))))

    def hspeed(sp):
        return sqrt_v(add(mul(fl(sp[0]), fl(sp[0])), mul(fl(sp[1]), fl(sp[1]))))

    def flip(bits):
        return (bits ^ 0x80000000) & M32

    def done():
        return finish(c, m, marks, st, spd, pp, dest, mat, tgt, pvm, pvsp, out)

    mis = signed8(st['mission'])
    marks.append('mission_%d' % mis if 0 <= mis <= 0x44 else 'mission_hi')
    if (mis & M32) > 0x44:
        return done()

    if mis in FIVE:
        five(FIVE[mis])
    elif mis in ONE:
        one(ONE[mis])
    elif mis in (0, 10, 11):
        out[0], out[1], out[2], out[3] = 0, 0, K_HALF, 1
    elif mis in (9, 13, 24):
        heading(0, dest[0], dest[1])
    elif mis == 14:
        ev([12, tag(VEH), dest[0], dest[1], tag(OUTA, 0), tag(OUTA, 4), tag(OUTA, 8)] + state_tail())
        out[0], out[1], out[2] = c['ov'][0] & M32, c['ov'][1] & M32, c['ov'][2] & M32
        after()
        out[3] = 0
    elif mis in (16, 55):
        heading(tag(TGTA), tgt_p[0], tgt_p[1])
    elif mis in (18, 19):
        x, y = tgt_p[0], tgt_p[1]
        if mis == 18:
            ev([10, tag(VEH), tag(TGTA), x, y, tgt['sp'][0], tgt['sp'][1]] + otags() + state_tail())
        else:
            ev([11, tag(VEH), x, y, tgt['sp'][0], tgt['sp'][1]] + otags() + state_tail())
        put_out()
        after()
    elif mis in (5, 6):
        s1 = f_speed()
        c1 = f_coors()
        s2 = f_speed()
        sx = c['psp%d' % s1][0]
        sy = c['psp%d' % s2][1]
        c2 = f_coors()
        if mis == 5:
            ent = f_entity()
            ev([10, tag(VEH), ent, c1[0], c2[1], sx, sy] + otags() + state_tail())
        else:
            ev([11, tag(VEH), c1[0], c2[1], sx, sy] + otags() + state_tail())
        put_out()
        after()
    elif mis == 25:
        c1 = f_coors()
        c2 = f_coors()
        heading(0, c1[0], c2[1])
    elif mis in (29, 30, 31, 32):
        ti, oi = c['ti'] - 1, c['oi'] - 1
        if mis in (29, 30):
            v = add(add(fl(c['cmx'][ti]), fl(c['cmx'][oi])), kf(K_TWO))
            if mis == 29:
                v = chs(v)
            a3, a4 = to_f32_bits(v), 0
        else:
            v = add(add(fl(c['cmy'][ti]), fl(c['cmy'][oi])), kf(K_SEVEN))
            if mis == 31:
                v = chs(v)
            a3, a4 = 0, to_f32_bits(v)
        ev([13, tag(VEH), tag(TGTA), a3, a4] + otags() + state_tail())
        put_out()
        after()
    elif mis == 3:
        marks.append('m3')
        pc = f_coors()
        x, y = pc[0], pc[1]
        e1 = f_veh()
        dot_bits = 0
        if e1 is None:
            marks.append('m3_nopv')
        else:
            e3 = f_veh()
            a, b = pvm[e3]['f'], mat['f']
            dot_bits = to_f32_bits(add(add(mul(fl(a[2]), fl(b[2])), mul(fl(a[1]), fl(b[1]))), mul(fl(a[0]), fl(b[0]))))
            if (st['rseed'] & 1) and gt(fl(dot_bits), kf(K_HALF)):
                marks.append('m3_A')
                e4 = f_veh()
                theirw = c['cmx'][c['pi'][e4] - 1]
                ownw = c['cmx'][c['oi'] - 1]
                side = (st['rseed'] & 2) != 0
                marks.append('m3_side' if side else 'm3_noside')
                e5 = f_veh()
                gap = to_f32_bits(sub(add(fl(ownw), fl(theirw)), kf(K_GAP)))
                if side:
                    x = to_f32_bits(add(mul(fl(gap), fl(pvm[e5]['r'][0])), fl(x)))
                    e6 = f_veh()
                    y = to_f32_bits(add(mul(fl(gap), fl(pvm[e6]['r'][1])), fl(y)))
                else:
                    x = to_f32_bits(sub(fl(x), mul(fl(gap), fl(pvm[e5]['r'][0]))))
                    e6 = f_veh()
                    y = to_f32_bits(sub(fl(y), mul(fl(gap), fl(pvm[e6]['r'][1]))))
                e7 = f_veh()
                rel = vsub(pvsp[e7], spd)
                thr = add(mul(mag_reg(rel), kf(K_SPGAIN)), kf(K_TWO))
                dy = sub(fl(own_p[1]), fl(y))
                dx = sub(fl(own_p[0]), fl(x))
                dist = sqrt_v(add(mul(dy, dy), mul(dx, dx)))
                if lt_ordered(dist, thr):
                    marks.append('m3_near')
                    if st['tact'] == 0:
                        marks.append('m3_turn')
                        st['tact'] = 7 if (st['rseed'] & 2) else 8
                        st['ttime'] = (st['now'] + 0xFA) & M32
                    else:
                        marks.append('m3_tact_busy')
                else:
                    marks.append('m3_far')
            else:
                marks.append('m3_B')
                e4 = f_veh()
                off = (st['rseed'] & 0xFF) - 128
                x = to_f32_bits(add(mul(mul(from_int(off), fl(pvm[e4]['r'][0])), kf(K_SEEDK)), fl(x)))
                e5 = f_veh()
                off = (st['rseed'] & 0xFF) - 128
                y = to_f32_bits(add(mul(mul(from_int(off), fl(pvm[e5]['r'][1])), kf(K_SEEDK)), fl(y)))
            if lt_ordered(fl(dot_bits), kf(K_ZERO)):
                marks.append('m3_dotneg')
                s1 = f_speed()
                scaled = mul(fl(dot_bits), kf(K_DOTK))
                x = to_f32_bits(add(mul(scaled, fl(c['psp%d' % s1][0])), fl(x)))
                kst = stored(scaled)
                s2 = f_speed()
                y = to_f32_bits(add(mul(kst, fl(c['psp%d' % s2][1])), fl(y)))
            if st['flags'] & 1:
                marks.append('m3_law')
                e8 = f_veh()
                h = hspeed(pvsp[e8])
                if gt(h, kf(K_SPLIM)):
                    marks.append('m3_fast')
                    e9 = f_veh()
                    ev([5, tag(PV0 + e9)])
                    after()
                    if c['passenger']:
                        marks.append('m3_say')
                        ev([6, tag(PEDA), 0x26, 0, K_ONE, 0, 0, 0])
                        after()
                    else:
                        marks.append('m3_nopass')
        e10 = f_veh()
        heading(0 if e10 is None else tag(PV0 + e10), x, y)
    elif mis == 44:
        marks.append('m44')
        p1 = f_coors()
        rel = vsub(p1, own_p)
        if lt_ordered(mag_reg(rel), kf(K_TEN)):
            marks.append('m44_stop')
            out[0], out[1], out[2], out[3] = 0, 0, K_ONE, 0
        else:
            t2 = f_coors()
            tx, ty = t2[0], t2[1]
            e1 = f_veh()
            if e1 is None:
                marks.append('m44_nopv')
            else:
                p2 = f_coors()
                rel2 = vsub(own_p, p2)
                e5 = f_veh()
                e6 = f_veh()
                val = mul(add(fl(pvm[e6]['f'][1]), fl(pvm[e5]['f'][0])), fl(rel2[0]))
                if gt(val, kf(K_ZERO)):
                    marks.append('m44_posside')
                    e7 = f_veh()
                    ev([8, STACK, tag(PVM0 + e7), K_P4, 0, 0])
                    front = list(c['mt%d' % (cnt['mt'] & 1)])
                    cnt['mt'] += 1
                    after()
                    e8 = f_veh()
                    ev([8, STACK, tag(PVM0 + e8), K_M4, 0, 0])
                    back = list(c['mt%d' % (cnt['mt'] & 1)])
                    cnt['mt'] += 1
                    after()
                    d1 = to_f32_bits(mag_reg(vsub(own_p, front)))
                    d2 = mag_reg(vsub(own_p, back))
                    if gt(d2, fl(d1)):
                        marks.append('m44_front')
                        tx, ty = front[0], front[1]
                    else:
                        marks.append('m44_back')
                        tx, ty = back[0], back[1]
                else:
                    marks.append('m44_negside')
            e10 = f_veh()
            heading(0 if e10 is None else tag(PV0 + e10), tx, ty)
    elif mis == 53:
        marks.append('m53')
        heading(tag(TGTA), tgt_p[0], tgt_p[1])
        rel = vsub(own_p, tgt_p)
        mag = mag_reg(rel)
        mag_st = to_f32_bits(mag)
        thr = add(from_int(st['follow']), kf(K_TEN))
        if not lt_ordered(mag, thr):
            marks.append('m53_far')
        else:
            tv = mul(hspeed(tgt['sp']), kf(K_SIXTY))
            own_sp = stored(mul(hspeed(spd), kf(K_SIXTY)))
            gap = sub(fl(mag_st), from_int(st['follow']))
            if lt_ordered(gap, kf(K_ZERO)):
                marks.append('m53_gapneg')
                gs = mul(gap, kf(K_FIVE))
            else:
                marks.append('m53_gappos')
                gs = add(gap, gap)
            err = sub(add(tv, gs), own_sp)
            if lt_ordered(err, kf(K_ZERO)):
                marks.append('m53_brake')
                r = mul(err, kf(K_BRK))
                out[1] = 0
                if lt_ordered(kf(K_ONE), r):
                    marks.append('m53_brakeclamp')
                    r = kf(K_ONE)
                out[2] = to_f32_bits(r)
            else:
                marks.append('m53_gas')
                r = mul(err, kf(K_GASK))
                if lt_ordered(kf(K_ONE), r):
                    marks.append('m53_gasclamp')
                    r = kf(K_ONE)
                out[1] = to_f32_bits(r)
                out[2] = 0
    elif mis == 60:
        marks.append('m60')
        if c['tgt_null']:
            marks.append('m60_null')
        else:
            t3 = list(tgt_p)
            v14 = to_f32_bits(sub(fl(t3[1]), fl(own_p[1])))
            v18 = to_f32_bits(sub(fl(own_p[0]), fl(t3[0])))
            ev([7, STACK, v14, v18, 0])
            nv = list(c['nv'])
            after()
            v = [to_f32_bits(mul(fl(nv[k]), kf(K_TEN))) for k in range(3)]
            if st['rseed'] & 1:
                marks.append('m60_flip')
                v = [flip(b) for b in v]
            else:
                marks.append('m60_noflip')
            x = to_f32_bits(add(fl(v[0]), fl(t3[0])))
            y = to_f32_bits(add(fl(v[1]), fl(t3[1])))
            heading(0, x, y)
    # 7, 27, 28: nada
    return done()


def finish(c, m, marks, st, spd, pp, dest, mat, tgt, pvm, pvsp, out):
    def words(d):
        return [d['r'][0], d['r'][1], d['r'][2], d['f'][0], d['f'][1], d['f'][2], d['u'][0], d['u'][1], d['u'][2], d['p'][0], d['p'][1], d['p'][2]]
    return dict(marks=marks, events=m.events, ev_hash=m.hash, ev_count=m.count,
                v=[st['mission'], st['tact'], st['ttime'], st['now'], spd[0], st['rseed'], st['flags'], st['follow'], dest[0], pp[0]],
                mat=words(mat), pvmat=[words(pvm[0]), words(pvm[1])], tgt=[tgt['pp'][0], tgt['sp'][0], tgt['mp'][0]], pv=[pvsp[0][0], pvsp[1][0]], out=list(out))


def expected(c):
    return model_mission(c)


def check_self():
    assert to_f32_bits(mul(from_int(1), from_bits(K_SEEDK))) == K_SEEDK
    assert signed8(0xFF) == -1 and signed8(0x7F) == 127
    assert to_f32_bits(sub(from_bits(K_HALF), mul(from_int(3), from_bits(K_HALF)))) == 0xBF800000
    print('ok')


def _first_diff(o, e):
    for i, (g, w) in enumerate(zip(o['events'], e['events'])):
        if g != w:
            return 'evento %d: %s != %s' % (i, g, w)
    if len(o['events']) != len(e['events']):
        return 'numero de eventos %d != %d' % (len(o['events']), len(e['events']))
    return 'v %s/%s mat %s/%s pvmat %s/%s tgt %s/%s pv %s/%s out %s/%s' % (o['v'], e['v'], o['mat'], e['mat'], o['pvmat'], e['pvmat'], o['tgt'], e['tgt'], o['pv'], e['pv'], o['out'], e['out'])


def check_row_obj(row):
    """(mensajes de desacuerdo, marcas del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('ev_hash', 'ev_count', 'events', 'v', 'mat', 'pvmat', 'tgt', 'pv', 'out') if o[k] != e[k]]
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
