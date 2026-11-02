"""Oraculo independiente (Python) de Car84G: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x437C20..0x4382D4), lo que debe observarse de
CCarCtrl::SteerAICarWithPhysics (traza EN ORDEN de los colaboradores con el estado del vehiculo en cada llamada, hash de todos los eventos, y el estado final del vehiculo: mision, accion temporal y su tiempo,
tiempos, nodos, banderas, objetivo, velocidad, volante, pedales, salidas del Vortex, indice de modelo, relojes y la matriz; ningun otro cambio en las arenas) y lo compara con las filas de comparisons.jsonl (lado
original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con enteros exactos. fsin y fcos se evaluan con 100 digitos (el EXE los deja sin redondear en el registro).

Flujo leido (cdecl, 1 argumento; ret): con id de grabacion >= 0 y bUseCarAI[id] == 0 no hace nada. SwitchBetweenPhysicsAndGhost; la mision (- 15) indexa la tabla de clases de 0x4382E4 (sin objetivo: mision 0 o 0x2A);
bIsStopped (bit 0 de +0x3DC): AreNodesLoadedForArea(x -+ 270, y -+ 270) y, si hay nodos, se borra el bit y JoinCarWithRoadAccordingToMission (freno 0.2f); bIsParked (bit 1) con mision distinta de 0 y 11: se invierten
los nodos, accion 22 durante 2 s. La accion temporal (1..24, tabla de saltos de 0x438310) fija giro, acelerador, freno y freno de mano; las acciones 3, 13, 14, 15..18 y la rama por defecto llaman a FindMaxSteerAngle o a
SteerAICarWithPhysics_OnlyMission. Al final: freno (+0x4A0), giro (+0x494), bit 5 de +0x428, acelerador (+0x49C) y, con el modelo 0x21B, +0x994 y +0x988.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000. Indices: 0 vehiculo, 1 matriz.
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


# ---------------------------------------------------------------- modelo de CCarCtrl::SteerAICarWithPhysics (0x437C20..0x4382D4)
K_0, K_05, K_02, K_1, K_004, K_0001, K_01, K_012, K_270 = 0x00000000, 0x3F000000, 0x3E4CCCCD, 0x3F800000, 0x3D23D70A, 0x3A83126F, 0x3DCCCCCD, 0x3C449BA6, 0x43870000
K_N05, K_N075, K_025, K_N025, K_N1 = 0xBF000000, 0xBF400000, 0x3E800000, 0xBE800000, 0xBF800000
D_005, D_05, D_N05 = 0x3FA999999999999A, 0x3FE0000000000000, 0xBFE0000000000000
VEH, MATA = 0, 1
# tabla de 0x4382E4 (mision - 15): 0 sin objetivo -> mision 0; 1 sin objetivo -> mision 0x2A; 2 nada
CLASS = [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]


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


def stored_bits(bits):
    """fld dword; fstp dword (un NaN de senal sale silenciado)"""
    return to_f32_bits(fl(bits))


def dot_gt(speed, fwd, k):
    """DotProduct (0x40FDB0): (a.z * b.z + a.y * b.y) + a.x * b.x en la pila x87; luego `fcomp k` y salto `jne` con st0 <= k o NaN. Devuelve True si dot > k (ordenado)"""
    t1 = mul(fl(speed[2]), mem(fwd[2]))
    t2 = mul(fl(speed[1]), mem(fwd[1]))
    s = add(t1, t2)
    t3 = mul(fl(speed[0]), mem(fwd[0]))
    return gt(add(s, t3), fl(k))


def model_steer(c):
    """CCarCtrl::SteerAICarWithPhysics (desensamblado leido): ver README.md"""
    m = Model()
    marks = []
    st = dict(mission=c['mission'] & 0xFF, tact=c['tact'] & 0xFF, ttime=c['ttime'] & M32, tstart=c['tstart'] & M32, tswitch=c['tswitch'] & M32,
              node=[x & M32 for x in c['node']], mov=c['mov'] & 0xFF, target=c['target'] & M32, obst=c['obst'] & M32, vflags=c['vflags'] & M32,
              speed=[x & M32 for x in c['speed']], steer=c['steer0'] & M32, gas=c['gas0'] & M32, brk=c['brk0'] & M32, model=c['model'] & 0xFFFF, v988=0, v994=0)
    glob = dict(now=c['now'] & M32, ts=c['ts'] & M32)
    mat = dict(r=list(c['mr']), f=list(c['mf']), u=list(c['mu']), p=list(c['mp']))
    evn = [0]
    null_matrix = c['mat_null'] != 0

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
            st['mov'] = v & 0xFF
        elif k == 3:
            st['tact'] = v & 0xFF
        elif k == 4:
            st['ttime'] = v
        elif k == 5:
            st['target'] = v
        elif k == 6:
            glob['now'] = v
        elif k == 7:
            st['speed'][0] = v
        elif k == 8:
            if not null_matrix:
                mat['f'][0] = v
        elif k == 9:
            st['steer'] = v
        elif k == 10:
            st['vflags'] = v
        elif k == 11:
            st['model'] = v & 0xFFFF
        elif k == 12:
            st['node'][0] = v
        elif k == 13:
            st['node'][1] = v
        elif k == 14:
            glob['ts'] = v

    def state():
        mats = mat['r'] + mat['f'] + mat['u'] + mat['p']
        return dict(events=m.events, ev_hash=m.hash, ev_count=m.count,
                    v=[st['mission'], st['tact'], st['ttime'], st['tstart'], st['tswitch'], st['node'][0], st['node'][1], st['node'][2], st['mov'], st['target'], st['obst'], st['vflags'],
                       st['speed'][0], st['speed'][1], st['speed'][2], st['steer'], st['gas'], st['brk'], st['model'], st['v988'], st['v994'], glob['now'], glob['ts']],
                    mat=mats, marks=marks)

    def finish(a, b, brake_reg, hb):
        st['brk'] = brake_reg & M32
        st['steer'] = a & M32
        st['vflags'] = ((st['vflags'] & ~0x20) | ((hb & 1) << 5)) & M32
        st['gas'] = b & M32
        if st['model'] == 0x21B:
            marks.append('vortex')
            st['v994'] = b & M32
            st['v988'] = a & M32
        if hb & 1:
            marks.append('handbrake_set')
        return state()

    def only_mission():
        ev([5, tag(VEH, 0), STACK, STACK, STACK, STACK, st['mission'], st['tact'], st['steer'], glob['now']])
        after()
        return c['om'][0] & M32, c['om'][1] & M32, c['om'][2] & M32, c['om_hb'] & 0xFF

    def expire(extra=None):
        if glob['now'] > st['ttime']:
            marks.append('expired')
            st['tact'] = 0
            if extra:
                extra()
        else:
            marks.append('not_expired')

    rec = signed8(c['rec'])
    if rec >= 0 and not (c['use_ai'] & 0xFF):
        marks.append('rec_skip')
        return state()
    marks.append('rec_ok')
    ev([1, tag(VEH, 0), st['mission'], st['tact'], st['mov'], st['target'], c['rec'] & 0xFF])
    after()
    idx = signed8(st['mission']) - 15
    st['obst'] = 0
    if 0 <= idx <= 0x28:
        cls = CLASS[idx]
        if cls != 2 and st['target'] == 0:
            st['mission'] = 0 if cls == 0 else 0x2A
            marks.append('class_none' if cls == 0 else 'class_2a')
    movement = st['mov']
    if movement & 1:
        marks.append('ta_stopped')
        pos = c['place'] if null_matrix else mat['p']
        if null_matrix:
            marks.append('stopped_nullmat')
        xk, yk = mem(K_270), mem(K_270)
        max_y = to_f32_bits(add(fl(pos[1]), yk))
        min_y = to_f32_bits(sub(fl(pos[1]), yk))
        max_x = to_f32_bits(add(fl(pos[0]), xk))
        min_x = to_f32_bits(sub(fl(pos[0]), xk))
        ev([2, 0x96F050, min_x, max_x, min_y, max_y, st['mov'], st['mission']])
        loaded = c['loaded'] != 0
        after()
        if loaded:
            marks.append('stopped_loaded')
            st['mov'] = st['mov'] & 0xFE
            ev([3, tag(VEH, 0), st['mov'], st['mission']])
            after()
        else:
            marks.append('stopped_wait')
        return finish(0, 0, stored_bits(K_02), 0)
    if (movement & 2) and st['mission'] != 0x0B and st['mission'] != 0:
        marks.append('parked_swap')
        old_cur, old_start = st['node'][0], st['node'][1]
        st['tact'] = 0x16
        st['ttime'] = (glob['now'] + 0x7D0) & M32
        st['mov'] = movement & 0xFD
        st['node'][1] = old_cur
        st['node'][0] = old_start
        st['node'][2] = old_cur
    ta = signed8(st['tact'])
    now = glob['now']
    if ta in (1, 24):
        marks.append('ta_wait' if ta == 1 else 'ta_brake')
        brake_reg = stored_bits(K_02 if ta == 1 else K_1)
        if glob['now'] > st['ttime']:
            marks.append('expired')
            st['tact'] = 0
            st['tstart'] = glob['now']
            st['tswitch'] = glob['now']
        else:
            marks.append('not_expired')
        return finish(0, 0, brake_reg, 0)
    if ta == 3:
        marks.append('ta_reverse')
        a, b, cc, hb = only_mission()
        a = to_f32_bits(chs(fl(a)))
        if dot_gt(st['speed'], mat['f'], K_004):
            marks.append('dot_hi')
            brake_reg, b = stored_bits(K_05), 0
        else:
            marks.append('dot_lo')
            brake_reg, b = stored_bits(K_0), K_N05
        if glob['now'] > st['ttime']:
            marks.append('expired')
            st['tact'] = 0
            st['tstart'] = glob['now']
        else:
            marks.append('not_expired')
        return finish(a, b, brake_reg, 0)
    if ta in (13, 14):
        marks.append('ta_revleft' if ta == 13 else 'ta_revright')
        ev([4, tag(VEH, 0), st['tact'], st['ttime']])
        fm = fl(c['find_max'])
        after()
        a = to_f32_bits(fm if ta == 13 else chs(fm))
        if glob['now'] > st['ttime']:
            marks.append('expired')
            st['tact'] = 0
            st['tstart'] = glob['now']
        else:
            marks.append('not_expired')
        return finish(a, K_N075, stored_bits(0), 0)
    if 4 <= ta <= 9:
        marks.append({4: 'ta_hb_left', 5: 'ta_hb_right', 6: 'ta_hb_straight', 7: 'ta_turn_left', 8: 'ta_turn_right', 9: 'ta_forward'}[ta])
        a, b, hb = {4: (K_1, 0, 1), 5: (K_N1, 0, 1), 6: (0, 0, 1), 7: (K_1, K_1, 0), 8: (K_N1, K_1, 0), 9: (0, K_05, 0)}[ta]
        expire()
        return finish(a, b, stored_bits(K_0), hb)
    if ta in (10, 11, 20, 21):
        marks.append({10: 'ta_swirve_l', 11: 'ta_swirve_r', 20: 'ta_swirve_ls', 21: 'ta_swirve_rs'}[ta])
        a = K_025 if ta in (11, 21) else K_N025
        timeout = st['ttime']
        if now > ((timeout - 0x4E2) & M32):
            marks.append('swirve_flip')
            a ^= 0x80000000
        if now > timeout:
            if ta in (20, 21):
                marks.append('swirve_wait')
                st['tact'] = 1
                st['ttime'] = (glob['now'] + 0xFA0) & M32
            else:
                marks.append('swirve_end')
                st['tact'] = 0
        else:
            marks.append('not_expired')
        return finish(a, 0, stored_bits(K_0001), 0)
    if ta == 19:
        marks.append('ta_headon')
        timeout = st['ttime']
        brake_reg = stored_bits(K_1 if now > timeout else K_0)
        cur = st['steer']
        step = mul(fl(glob['ts']), from_dbits(D_005))
        if gt(fl(cur), fl(K_0)):
            marks.append('headon_pos')
            v = add(step, mem(cur))
            if gt(v, from_dbits(D_05)):
                marks.append('headon_clamp_hi')
                v = from_dbits(D_05)
        else:
            marks.append('headon_neg')
            v = sub(fl(cur), step)
            if gt(from_dbits(D_N05), v):
                marks.append('headon_clamp_lo')
                v = from_dbits(D_N05)
        a = to_f32_bits(v)
        if now > timeout:
            marks.append('expired')
            st['tact'] = 0
        else:
            marks.append('not_expired')
        return finish(a, 0, brake_reg, 0)
    if ta == 22:
        marks.append('ta_revstraight')
        if dot_gt(st['speed'], mat['f'], K_01):
            marks.append('dot_hi')
            b, brake_reg = 0, stored_bits(K_05)
        else:
            marks.append('dot_lo')
            b, brake_reg = K_N05, stored_bits(K_0)
        expire()
        return finish(0, b, brake_reg, 0)
    if ta == 23:
        marks.append('ta_boost')
        a = st['steer']
        push = to_f32_bits(mul(fl(glob['ts']), mem(K_012)))
        if null_matrix:
            marks.append('boost_nullmat')
            h = fl(c['heading'])
            fwd = [to_f32_bits(chs(fsin(h))), to_f32_bits(fcos(h)), 0]
        else:
            fwd = list(mat['f'])
        scaled = [to_f32_bits(mul(fl(push), mem(fwd[2 - k]))) for k in range(3)][::-1]
        st['speed'] = [to_f32_bits(add(fl(st['speed'][k]), mem(scaled[k]))) for k in range(3)]
        expire()
        return finish(a, K_1, stored_bits(0), 0)
    if 15 <= ta <= 18:
        marks.append('ta_plane')
        expire()
    else:
        marks.append('ta_default')
    a, b, cc, hb = only_mission()
    return finish(a, b, stored_bits(cc), hb)


def expected(c):
    return model_steer(c)


def check_self():
    assert to_f32_bits(mul(from_int(5000), from_bits(0x38D1B717))) == 0x3F000000
    assert to_f32_bits(from_dbits(D_005)) == 0x3D4CCCCD
    assert to_f32_bits(from_dbits(D_N05)) == 0xBF000000
    assert to_f32_bits(chs(fsin(v_of(PI_DEC / 6)))) == 0xBF000000
    assert to_f32_bits(mul(from_bits(0x3F800000), from_dbits(D_005))) == 0x3D4CCCCD
    assert signed8(0xFF) == -1 and signed8(0x7F) == 127
    print('ok')


def _first_diff(o, e):
    for i, (g, w) in enumerate(zip(o['events'], e['events'])):
        if g != w:
            return 'evento %d: %s != %s' % (i, g, w)
    if len(o['events']) != len(e['events']):
        return 'numero de eventos %d != %d' % (len(o['events']), len(e['events']))
    return 'v %s/%s mat %s/%s' % (o['v'], e['v'], o['mat'], e['mat'])


def check_row_obj(row):
    """(mensajes de desacuerdo, marcas del modelo)"""
    c = row['input']
    msgs = []
    e = expected(c)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('ev_hash', 'ev_count', 'events', 'v', 'mat') if o[k] != e[k]]
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
