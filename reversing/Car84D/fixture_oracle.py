"""Oraculo independiente (Python) de Car84D: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x42EC90..0x42F363), lo que debe observarse de
CCarCtrl::DragCarToPoint (traza EN ORDEN de los colaboradores con todos sus argumentos, hash de todos los eventos, banderas y byte +0x3E1 del vehiculo, entidad y suelos guardados, volante, las dos
matrices y las banderas de las dos entidades; ningun otro cambio en las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La x87 del juego trabaja a 24 bits de mantisa con exponente extendido: cada operacion (fsub, fmul, fadd, fdiv, fsqrt) redondea el resultado exacto a 24 bits sin limite de exponente y solo el
fstp dword redondea a float (con desnormales y desbordamiento a infinito). Se modela con enteros exactos. fpatan, fsin y fcos se evaluan con 100 digitos (el EXE los deja sin redondear en el registro).

Flujo leido (cdecl, 2 argumentos; ret): medio largo = (max.y - min.y) * 0.95f; margen 100.0f si el byte +0x3E1 (con signo) > 16, si no 3.0f. P = posicion - (adelante * medio) * 0.5f; ProcessLineSphere(P -> P + adelante * 1.5f
con z = 0, esfera del destino y radio medio): con corte P = punto de corte; si no P se acerca al destino a distancia medio. Suelo delantero (destino.xy, z = posicion.z + (adelante.z * medio) * 0.5f) y trasero (P,
z = posicion.z - ...): IsStoredPolyStillValidVerticalLine, si falla ProcessVerticalLine (z +- 1.5f / 2.0f) y de nuevo (z +- margen); con acierto de ProcessVerticalLine se copian los bits 30 y 31 de las banderas de la
entidad, se guarda la entidad y el byte +0x3E1 vale 0; sin acierto se conserva el suelo guardado (+0x4E4 / +0x4E8). El byte +0x3E1 se incrementa antes del primer sondeo. Orientacion: derecha = ((destino.y - P.y) / medio,
(destino.x - P.x) * (-1.0f / medio), 0); cabeceo a = atan((suelo delantero - suelo trasero) / medio); adelante = (-(cos a * derecha.y), cos a * derecha.x, sin a); arriba = derecha x adelante (CrossProduct); posicion =
punto medio con z + GetHeightAboveRoad; volante = ((adelante nueva x adelante vieja).z * -10.0f) acotado a [-0.5f, 0.5f] (NaN: 0.5f).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un local de pila, 0x20000000.
Indices: 0 vehiculo, 1 matriz A, 2 matriz B, 3 informacion de modelo, 4 caja de colision, 5 entidad A, 6 entidad B, 7 destino.
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


# ---------------------------------------------------------------- modelo de CCarCtrl::DragCarToPoint (0x42EC90..0x42F363)
K1, K2, K3, K4, K5, K6, K7, KM05 = 0x3F733333, 0x3F000000, 0x3FC00000, 0x40000000, 0x3F800000, 0xBF800000, 0xC1200000, 0xBF000000
W_FAR, W_NEAR = 0x42C80000, 0x40400000
VEH, MATA, MATB, MI, COL, ENTA, ENTB, TGT = range(8)


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


def model_drag(c):
    """CCarCtrl::DragCarToPoint (desensamblado leido): ver README.md"""
    m = Model()
    marks = []
    st = dict(flags=c['vflags'] & M32, cnt=c['cnt'] & 0xFF, entp=c['entp0'] & M32, steer=c['steer0'] & M32, g4e4=c['g4e4'] & M32, g4e8=c['g4e8'] & M32, mptr=0, ev=0, isv=0, pvl=0, cross=0)
    mats = [dict(r=list(c['mr']), f=list(c['mf']), u=list(c['mu']), p=list(c['mp'])), dict(r=list(c['br']), f=list(c['bf']), u=list(c['bu']), p=list(c['bp']))]
    eflags = [c['eflags'][0] & M32, c['eflags'][1] & M32]

    def cur():
        return mats[st['mptr']]

    def mtag(off=0):
        return tag(MATA + st['mptr'], off)

    def ev(e):
        m.push([x & M32 for x in e])

    def after():
        idx = st['ev']
        st['ev'] += 1
        if idx != c['mut_at']:
            return
        marks.append('mut_applied')
        marks.append('mut_kind_%d' % c['mut_kind'])
        k, v = c['mut_kind'], c['mut_val'] & M32
        if k == 1:
            cur()['p'][2] = v
        elif k == 2:
            st['flags'] = v
        elif k == 3:
            st['g4e4'] = v
        elif k == 4:
            st['g4e8'] = v
        elif k == 5:
            st['mptr'] = 1
        elif k == 6:
            st['cnt'] = v & 0xFF
        elif k == 7:
            eflags[0] = v
        elif k == 8:
            eflags[1] = v
        elif k == 9:
            cur()['p'][0] = v
            cur()['p'][1] = v

    tgtx, tgty = c['tgt'][0], c['tgt'][1]
    half = mul(sub(fl(c['col_max']), mem(c['col_min'])), mem(K1))
    halfB = to_f32_bits(half)
    margin = W_FAR if signed8(c['cnt']) > 16 else W_NEAR
    marks.append('margin_far' if margin == W_FAR else 'margin_near')
    if half.kind == 'nan':
        marks.append('h_nan')
    elif half.kind == 'f' and half.m == 0:
        marks.append('h_zero')
    elif halfB & 0x7FFFFFFF == 0x7F800000:
        marks.append('h_inf')
    fwd = list(c['mf'])
    old_fwd = list(c['mf'])
    pos = cur()['p']
    pxB = to_f32_bits(sub(mem(pos[0]), mul(mul(fl(fwd[0]), half), mem(K2))))
    pyB = to_f32_bits(sub(mem(pos[1]), mul(mul(fl(fwd[1]), half), mem(K2))))

    # segmento y esfera
    fxk = mul(fl(fwd[0]), mem(K3))
    fykB = to_f32_bits(mul(fl(fwd[1]), mem(K3)))
    endx = to_f32_bits(add(fxk, mem(pxB)))
    endy = to_f32_bits(add(fl(fykB), mem(pyB)))
    ev([1, st['cnt'], pxB, pyB, 0, endx, endy, 0, tgtx, tgty, 0, halfB, 0x3F800000, STACK, STACK, STACK, STACK])
    hit = c['s_ret'] != 0
    after()
    if hit:
        marks.append('sphere_hit')
        pxB, pyB = c['s_cp'][0] & M32, c['s_cp'][1] & M32
    else:
        marks.append('sphere_miss')
        dy = sub(fl(tgty), mem(pyB))
        dx = sub(fl(tgtx), mem(pxB))
        length = sqrt_v(add(mul(dx, dx), mul(dy, dy)))
        ratio = div(mem(halfB), length)
        pxB = to_f32_bits(add(mul(sub(fl(pxB), mem(tgtx)), ratio), mem(tgtx)))
        pyB = to_f32_bits(add(mul(sub(fl(pyB), mem(tgty)), ratio), mem(tgty)))

    def probe(x, y, height_reg, poly_off, incr, label):
        zc = to_f32_bits(height_reg)
        zlow = to_f32_bits(sub(height_reg, mem(margin)))
        zhigh = to_f32_bits(add(fl(zc), mem(margin)))
        if incr:
            st['cnt'] = (st['cnt'] + 1) & 0xFF
        kv = st['isv'] if st['isv'] < 2 else 1
        st['isv'] += 1
        ev([2, st['cnt'], st['flags'], x, y, zhigh, zlow, STACK, STACK, tag(VEH, poly_off)])
        ret = c['v_ret'][kv] != 0
        after()
        if ret:
            marks.append(label + '_valid')
            return c['v_z'][kv] & M32
        got = None
        for n_try, (z, d) in enumerate(((to_f32_bits(add(fl(zc), mem(K3))), to_f32_bits(sub(fl(zc), mem(K4)))), (zhigh, zlow))):
            kp = st['pvl'] if st['pvl'] < 4 else 3
            st['pvl'] += 1
            ev([3, st['cnt'], st['flags'], x, y, z, d, 1, 0, 0, 0, 0, 0, STACK, STACK, STACK, tag(VEH, poly_off)])
            found = c['p_ret'][kp] != 0
            if found:
                got = (c['p_ent'][kp], c['p_z'][kp] & M32)
            after()
            if found:
                marks.append('%s_pvl%d' % (label, n_try + 1))
                break
        if got is None:
            marks.append(label + '_none')
            return None
        entity, ground = got
        ef = eflags[entity]
        st['entp'] = tag(ENTA + entity)
        f1 = st['flags'] ^ ((st['flags'] ^ ef) & 0x40000000)
        st['flags'] = ef ^ ((ef ^ f1) & 0x7FFFFFFF)
        st['cnt'] = 0
        marks.append('flags_copied')
        return ground

    # suelo delantero (destino) y trasero (P)
    hz = mul(mul(fl(fwd[2]), mem(halfB)), mem(K2))
    hzB = to_f32_bits(hz)
    posz = cur()['p'][2]
    zc_reg = add(hz, mem(posz))
    g = probe(tgtx, tgty, zc_reg, 0x51C, True, 'front')
    front_g = g if g is not None else st['g4e4']
    st['g4e4'] = front_g
    posz = cur()['p'][2]
    zr_reg = sub(fl(posz), mem(hzB))
    g = probe(pxB, pyB, zr_reg, 0x548, False, 'rear')
    rear_g = g if g is not None else st['g4e8']
    st['g4e8'] = rear_g

    # orientacion de la matriz
    inv = div(fl(K5), mem(halfB))
    dz = sub(fl(front_g), mem(rear_g))
    vv = mul(dz, inv)
    if vv.kind == 'nan':
        marks.append('pitch_nan')
    elif vv.kind == 'inf':
        marks.append('pitch_inf')
    ang = fpatan(vv)
    cs = fcos(ang)
    rxB = to_f32_bits(mul(sub(fl(tgty), mem(pyB)), inv))
    ryB = to_f32_bits(mul(sub(fl(tgtx), mem(pxB)), div(fl(K6), mem(halfB))))
    fx = to_f32_bits(chs(mul(cs, mem(ryB))))
    fy = to_f32_bits(mul(cs, mem(rxB)))
    fz = to_f32_bits(fsin(ang))
    mt = cur()
    mt['r'] = [rxB, ryB, 0]
    mt['f'] = [fx, fy, fz]
    ev([4, STACK, mtag(0), mtag(0x10)] + mt['r'] + mt['f'])
    after()
    midx = to_f32_bits(mul(add(fl(tgtx), mem(pxB)), mem(K2)))
    midy = to_f32_bits(mul(add(fl(tgty), mem(pyB)), mem(K2)))
    gsB = to_f32_bits(add(fl(rear_g), mem(front_g)))
    mt = cur()
    mt['u'] = [x & M32 for x in c['crs0']]
    midz = to_f32_bits(mul(fl(gsB), mem(K2)))
    ev([5, tag(VEH, 0)] + mt['p'] + mt['u'])
    after()
    midzl = to_f32_bits(add(fl(c['height']), mem(midz)))
    mt = cur()
    ev([6, tag(VEH, 0)] + mt['p'] + mt['u'])
    after()
    cur()['p'] = [midx, midy, midzl]
    mt = cur()
    ev([4, STACK, mtag(0x10), STACK] + mt['f'] + old_fwd)
    after()
    v = mul(fl(c['crs1'][2]), mem(K7))
    if not lt_ordered(v, fl(K2)):
        st['steer'] = K2
        marks.append('steer_nan' if v.kind == 'nan' else 'steer_hi')
    elif gt(v, fl(KM05)):
        st['steer'] = to_f32_bits(v)
        marks.append('steer_mid')
    else:
        st['steer'] = KM05
        marks.append('steer_lo')
    return dict(events=m.events, ev_hash=m.hash, ev_count=m.count,
                v=[st['flags'], st['cnt'], st['entp'], st['steer'], st['g4e4'], st['g4e8'], tag(MATA + st['mptr'], 0)],
                mat=[mats[0]['r'] + mats[0]['f'] + mats[0]['u'] + mats[0]['p'], mats[1]['r'] + mats[1]['f'] + mats[1]['u'] + mats[1]['p']],
                ent=list(eflags), marks=marks)


def expected(c):
    return model_drag(c)


def check_self():
    assert to_f32_bits(mul(from_int(5000), from_bits(0x38D1B717))) == 0x3F000000
    assert to_f32_bits(sqrt_v(from_int(4))) == 0x40000000
    assert to_f32_bits(fpatan(from_bits(0x3F800000))) == 0x3F490FDB       # atan(1) = pi/4
    assert to_f32_bits(fcos(v_of(Decimal(0)))) == 0x3F800000
    assert to_f32_bits(fsin(v_of(PI_DEC / 6))) == 0x3F000000              # sin(pi/6) = 0.5
    assert to_f32_bits(fcos(v_of(PI_DEC / 3))) == 0x3F000000
    assert to_f32_bits(fpatan(from_bits(0x7F800000))) == 0x3FC90FDB
    assert to_f32_bits(chs(from_bits(0x7FC00000))) == 0xFFC00000
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
        bad = [k for k in ('ev_hash', 'ev_count', 'events', 'v', 'ent') if o[k] != e[k]]
        if o['unexpected_virtuals'] != 0:
            bad.append('unexpected_virtuals')
        for i in range(2):
            if o['mat'][i] != e['mat'][i]:
                bad.append('mat%d' % i)
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
