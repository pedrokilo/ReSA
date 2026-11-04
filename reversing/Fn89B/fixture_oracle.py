"""Oraculo independiente (Python) de Fn89B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x514030..0x514851, lo que debe observarse de
CCamera::AvoidTheGeometry (thiscall, ECX = la camara; source, target y out por puntero y el fov en pila, `ret 10h`; void) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas; fpatan (GetATanOfXY), fsin, fcos, fptan y _CIpow con Decimal de 120
digitos (la x87 trabaja con 64 bits: el resultado se redondea a 64 bits y se vuelve a redondear a 24 en la operacion siguiente).

Modelo (desensamblado): d = target - source (cada componente guardado como float; dx tambien en el registro); L = sqrt((dz2 + dx2) + dy2) y h = sqrt(dy2 + dx2) guardados; rumbo = GetATanOfXY(dx, dy) (o de +0x984/+0x988 si dx y
dy son cero) guardado como float; paso = h si h y dz son cero, si no GetATanOfXY(h, dz) sin redondear; direccion = (cos(rumbo) cos(paso), sin(rumbo) cos(paso), sin(paso)); out = target - direccion * L; la direccion se
normaliza (CVector::Normalise). Dos lineas de vision (la segunda solo con 0xB6EC65 distinto de cero), plano cercano (FindPlayerPed, distancia - 0.5, 0.15f), esfera de radio near * tan(fov * pi/180 * 0.5) * aspecto * 1.15
centrada en out + direccion * near, proyeccion, normal orientada, vector de limpieza, temporizador y sentido del ped objetivo, decaimiento 0.9 ** paso de tiempo, WellBufferMe y escala final del vector de limpieza.
Las mutaciones (una vez) escriben, dentro de un falso, en target.x (1a linea de vision), out.y (esfera) o el vector adelante.x (2a GetMatrix).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n', 1)                  # el NaN indefinido de la x87 (negativo); los NaN tienen signo que fchs invierte


def isn(a):
    return a[0] == 'n'


def nanres(a, b):
    """resultado de una operacion con algun operando NaN: ese NaN (con dos, el positivo)"""
    if isn(a) and isn(b):
        return a if a[1] == 0 else b      # dos NaN de igual mantisa: gana el de signo positivo (observado en el EXE; los NaN de las fixtures solo difieren en el signo)
    return a if isn(a) else b


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return ('n', neg) if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if isn(a) or isn(b):
        return nanres(a, b)
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if isn(a) or isn(b):
        return nanres(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if isn(a):
        return ('n', 1 - a[1])
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    if isn(a) or isn(b):
        return nanres(a, b)
    return add(a, neg(b))


def sqrt24(a):
    if isn(a):
        return a
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float (NaN: mantisa 0x400000 con el signo del NaN)"""
    if isn(a):
        return 0x7FC00000 | (a[1] << 31)
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def reload(bits):
    return NAN if bits is None else load(bits)


def fromint(j):
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def lt(a, b):
    """a < b ordenado (fcomp + test ah,5 / jp): falso con NaN"""
    if isn(a) or isn(b):
        return False
    return num(a) < num(b)


def gt(a, b):
    if isn(a) or isn(b):
        return False
    return num(a) > num(b)






def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero en esta funcion: norma > 0 o L > 0)"""
    if isn(a) or isn(b):
        return nanres(a, b)
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


# ---------------------------------------------------------------- arcotangente (fpatan(x, 1) con precision mucho mayor que la de 64 bits de la x87)
from decimal import Decimal, getcontext
getcontext().prec = 90


def _atan_small(x):
    x2 = x * x
    term = x
    s = x
    k = 1
    eps = Decimal(10) ** -85
    while True:
        term *= -x2
        k += 2
        t = term / k
        if abs(t) < eps:
            return s
        s += t


def _pi():
    return 16 * _atan_small(Decimal(1) / 5) - 4 * _atan_small(Decimal(1) / 239)


PI_DEC = _pi()


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


def fpatan(a):
    """fld a; fld1; fpatan: atan(a / 1) SIN redondear a 24 bits (la x87 de 64 bits)"""
    if isn(a):
        return a
    if a[0] == 'i':
        v = Fraction(PI_DEC / 2)
        return ('f', v if a[1] == 0 else -v, a[1])
    v = a[1]
    if v == 0:
        return ('f', Fraction(0), a[2])
    r = atan_dec(Decimal(v.numerator) / Decimal(v.denominator))
    f = Fraction(r)
    return ('f', f, 1 if f < 0 else 0)


def le(a, b):
    """a <= b ordenado (fcom + test ah,41h / jp): falso con NaN"""
    if isn(a) or isn(b):
        return False
    return num(a) <= num(b)


def is_zero(a):
    return not isn(a) and a[0] == 'f' and a[1] == 0


def atan_of_xy(x, y):
    """CGeneral::GetATanOfXY (0x53CC70): x, y ya cargados. Devuelve ST0 al terminar (sin guardar)"""
    zero = load(0)
    if is_zero(x) and is_zero(y):
        return zero
    ax = neg(x) if lt(x, zero) else x
    ay = neg(y) if lt(y, zero) else y
    xpos = gt(x, zero)
    ypos = gt(y, zero)
    m1 = load(KM1)
    if lt(ax, ay):
        if ypos:
            if xpos:
                return sub(load(KHALFPI), fpatan(div(x, y)))
            return add(fpatan(mul(div(m1, y), x)), load(KHALFPI))
        if xpos:
            return add(fpatan(mul(div(m1, y), x)), load(KTHREEHALF))
        return sub(load(KTHREEHALF), fpatan(div(x, y)))
    if ypos:
        if xpos:
            return fpatan(div(y, x))
        return sub(load(KPI), fpatan(mul(div(m1, x), y)))
    if xpos:
        return sub(load(KTWOPI), fpatan(mul(div(m1, x), y)))
    return add(fpatan(div(y, x)), load(KPI))



getcontext().prec = 120
KPI, KTWOPI, KNEGPI, KHALFPI, KTHREEHALF, KM1 = 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x3FC90FDB, 0x4096CBE4, 0xBF800000   # tablas de 0x53CC70 (0x858FE4 = pi/2, 0x863AC4 = 3 pi/2...)
import struct


def le(a, b):
    """a <= b ordenado (fcom + test ah,41h / jp): falso con NaN"""
    if isn(a) or isn(b):
        return False
    return num(a) <= num(b)


def is_zero(a):
    return not isn(a) and a[0] == 'f' and a[1] == 0


# ---------------------------------------------------------------- fsin / fcos / fptan / pow (Decimal de 120 digitos; el resultado de la x87 de 64 bits se aproxima redondeando a 64 bits)
def rndp(v, p):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (e - (p - 1))
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def dec(a):
    return Decimal(a[1].numerator) / Decimal(a[1].denominator)


def sincos_dec(x):
    if abs(x) > 10000:
        raise RuntimeError('argumento trigonometrico fuera de las fixtures')
    x2 = x * x
    eps = Decimal(10) ** -110
    s, c = x, Decimal(1)
    ts, tc = x, Decimal(1)
    n = 1
    while True:
        tc = -tc * x2 / (n * (n + 1))
        ts = -ts * x2 / ((n + 1) * (n + 2))
        n += 2
        c += tc
        s += ts
        if abs(tc) < eps and abs(ts) < eps:
            return s, c


def ext(d):
    """Decimal -> valor x87 (tupla) redondeado a 64 bits de mantisa"""
    if d == 0:
        return ('f', Fraction(0), 1 if d.is_signed() else 0)
    f = Fraction(d)
    return ('f', rndp(f, 64), 1 if f < 0 else 0)


def fsin(a):
    if isn(a):
        return a
    if a[0] == 'i':
        return NAN
    if a[1] == 0:
        return a
    return ext(sincos_dec(dec(a))[0])


def fcos(a):
    if isn(a):
        return a
    if a[0] == 'i':
        return NAN
    if a[1] == 0:
        return ('f', Fraction(1), 0)
    return ext(sincos_dec(dec(a))[1])


def ftan(a):
    """fptan: ST0 = tan(a) (se descarta el 1.0 que apila)"""
    if isn(a):
        return a
    if a[0] == 'i':
        return NAN
    if a[1] == 0:
        return a
    s, c = sincos_dec(dec(a))
    return ext(s / c)


def fpow(base, expo):
    """_CIpow: base ** expo (base positiva finita; exponente finito)"""
    if isn(expo) or isn(base) or expo[0] == 'i' or base[0] == 'i':
        raise RuntimeError('pow fuera de las fixtures')
    if expo[1] == 0:
        return ('f', Fraction(1), 0)
    v = (dec(expo) * dec(base).ln()).exp()
    return ext(v)


# ---------------------------------------------------------------- modelo de CCamera::AvoidTheGeometry (EXE 0x514030..0x514851)
STACK = 0x7F000000
ARENA_CAM, ARENA_SRC, ARENA_TGT, ARENA_OUT, ARENA_ENT, ARENA_MAT, ARENA_FWD, ARENA_PED = range(8)
VEC_OFF = 0x10
NAN_BITS = 0xFFC00000


def fbits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fc(x):
    return load(fbits(x))


ZERO_F = load(0)
ONE_F = load(0x3F800000)
HALF_F = load(0x3F000000)           # 0x8CC38C y 0x858B8C
F015 = load(0x3E19999A)             # 0x8CC390 (0.15f)
F09 = load(0x3F666666)              # 0x858C20 y 0x8CC81C (0.9f)
PI180 = load(0x3C8EFA35)            # 0x8595EC
K115 = load(0x3F933333)             # 0x8CC820 (1.15f)
NEAR015_BITS = 0x3E19999A
RWCAM_NORMAL = 0xCAFE0001


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def bits(a):
    b = store(a)
    return NAN_BITS if b is None else b


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def Mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def Rand(c, k):
    return Mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 89002) & M32)


def normalise(v):
    """CVector::Normalise (0x59C910): suma (x*x + y*y) + z*z; si es <= 0 pone x = 1.0 (y, z intactos); si no multiplica por 1 / sqrt"""
    x, y, z = (load(b) for b in v)
    s = add(add(mul(x, x), mul(y, y)), mul(z, z))
    if not isn(s) and num(s) <= 0:
        return [0x3F800000, v[1], v[2]]
    inv = div(ONE_F, sqrt24(s))
    return [bits(mul(inv, x)), bits(mul(inv, y)), bits(mul(inv, z))]


def dist3(o, p):
    """distancia por registros: (dx'^2 + dz'^2) + dy'^2 con las diferencias sin guardar"""
    dx = sub(load(o[0]), load(p[0]))
    dy = sub(load(o[1]), load(p[1]))
    dz = sub(load(o[2]), load(p[2]))
    return sqrt24(add(add(mul(dx, dx), mul(dz, dz)), mul(dy, dy)))


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    site = c['mutsite']
    mv = c['mutval'] & M32
    done = set()
    S = [v & M32 for v in c['src']]
    T = [v & M32 for v in c['tgt']]
    O = [v & M32 for v in c['outinit']]
    st = {'near': c['near'] & M32, 'timer': c['timer'] & M32, 'dirn': c['dirn'] & 0xFFFF, 'fwd': [v & M32 for v in c['fwd']], 'matcalls': 0}
    tnull = bool(c['tnull'])
    ent_tag = 0 if tnull else tag(ARENA_ENT, 0)

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def set_near(bits_):
        ev.append([3, RWCAM_NORMAL, bits_])
        st['near'] = bits_

    # ---- vectores
    dxr = sub(load(T[0]), load(S[0]))
    dx_b = bits(dxr)
    dy_b = bits(sub(load(T[1]), load(S[1])))
    dz_b = bits(sub(load(T[2]), load(S[2])))
    dy = load(dy_b)
    dz = load(dz_b)
    dx2 = mul(dxr, dxr)
    dy2 = mul(dy, dy)
    dz2 = mul(dz, dz)
    L_b = bits(sqrt24(add(add(dz2, dx2), dy2)))
    h_b = bits(sqrt24(add(dy2, dx2)))
    # ---- rumbo
    if is_zero(dxr) and is_zero(dy):
        mark('heading_fallback')
        hx, hy = load(c['h984'] & M32), load(c['h988'] & M32)
    else:
        hx, hy = load(dx_b), load(dy_b)
    heading_b = bits(atan_of_xy(hx, hy))
    # ---- paso
    hl = load(h_b)
    if is_zero(hl) and is_zero(dz):
        mark('pitch_zero')
        pitch = hl
    else:
        pitch = atan_of_xy(hl, dz)
    cosP = fcos(pitch)
    hv = load(heading_b)
    dirx_b = bits(mul(fcos(hv), cosP))
    diry_b = bits(mul(fsin(hv), cosP))
    dirz_b = bits(fsin(pitch))
    Lf = load(L_b)
    trx = mul(load(dirx_b), Lf)
    try_ = mul(load(diry_b), Lf)
    trz_b = bits(mul(load(dirz_b), Lf))
    O = [bits(sub(load(T[0]), trx)), bits(sub(load(T[1]), try_)), bits(sub(load(T[2]), load(trz_b)))]
    D = normalise([dirx_b, diry_b, dirz_b])
    # ---- lineas de vision
    def plos(a_tag, a, b_tag, b, flags):
        ev.append([1, a_tag, b_tag] + list(a) + list(b) + [STACK, STACK, ent_tag] + flags)

    plos(tag(ARENA_TGT, VEC_OFF), T, tag(ARENA_OUT, VEC_OFF), O, [1, 0, 0, 1, 0, 0, 1, 0])
    if once(1):
        T[0] = mv
    if c['los1']:
        mark('los1_hit')
        hit1 = [v & M32 for v in c['hit1']]
        O = list(hit1)
        if (c['b65'] & 0xFF) != 0:
            mark('b65')
            plos(tag(ARENA_OUT, VEC_OFF), O, tag(ARENA_TGT, VEC_OFF), T, [0, 1, 1, 1, 0, 0, 1, 0])
            if c['los2']:
                mark('los2_hit')
                hit2 = [v & M32 for v in c['hit2']]
                nearl = load(st['near'])
                if lt(dist3(O, hit2), nearl):
                    mark('second_near')
                    O = list(hit2)
                elif lt(dist3(O, hit1), load(st['near'])):
                    mark('first_near')
                    O = list(hit1)
    else:
        mark('los1_miss')
    # ---- jugador: plano cercano segun la distancia
    tt = [bits(sub(load(T[k]), load(O[k]))) for k in range(3)]
    ev.append([2, M32])
    if c['hasped']:
        mark('ped')
        lenr = sqrt24(add(add(mul(load(tt[0]), load(tt[0])), mul(load(tt[2]), load(tt[2]))), mul(load(tt[1]), load(tt[1]))))
        clearance = sub(lenr, HALF_F)
        if lt(clearance, load(st['near'])):
            mark('clear_lt_near')
            if le(clearance, F015):
                mark('clear_clamp')
                set_near(NEAR015_BITS)
            else:
                set_near(bits(clearance))
    else:
        mark('noped')
    # ---- esfera
    half = mul(mul(load(c['fov'] & M32), PI180), HALF_F)
    frustum = mul(mul(ftan(half), load(c['aspect'] & M32)), K115)
    nearf = load(st['near'])
    radius_b = bits(mul(nearf, frustum))
    sx_b = bits(mul(load(D[0]), nearf))
    sy_b = bits(mul(load(D[1]), nearf))
    sz = mul(load(D[2]), nearf)
    center = [bits(add(load(sx_b), load(O[0]))), bits(add(load(sy_b), load(O[1]))), bits(add(sz, load(O[2])))]
    ev.append([4] + center + [radius_b, 0, 1, 0, 0, 1, 0, 1])
    hit = bool(c['sphere'])
    if once(2):
        O[1] = mv
    clear = [0, 0, 0]
    timer_b = st['timer']
    if hit:
        mark('sphere_hit')
        point = [v & M32 for v in c['spt']]
        normal = [v & M32 for v in c['snorm']]
        pen = [bits(sub(load(point[k]), load(center[k]))) for k in range(3)]
        to = [sub(load(point[k]), load(O[k])) for k in range(3)]
        proj = add(add(mul(to[0], load(D[0])), mul(to[2], load(D[2]))), mul(to[1], load(D[1])))
        proj_b = bits(proj)
        if gt(proj, F015) and lt(proj, F09):
            mark('proj_mid')
            if lt(proj, load(st['near'])):
                mark('proj_near_set')
                set_near(proj_b)
        elif lt(proj, F015):
            mark('proj_low')
            set_near(NEAR015_BITS)
        else:
            mark('proj_high')
        penlen = sqrt24(add(add(mul(load(pen[0]), load(pen[0])), mul(load(pen[2]), load(pen[2]))), mul(load(pen[1]), load(pen[1]))))
        depth_b = bits(sub(load(radius_b), penlen))
        penn = normalise(pen)
        nrm = normalise(normal)
        depth = load(depth_b)
        pushx = mul(load(penn[0]), depth)
        pushy_b = bits(mul(load(penn[1]), depth))
        pushz_b = bits(mul(load(penn[2]), depth))
        facing = add(add(mul(neg(load(penn[0])), load(nrm[0])), mul(load(nrm[2]), neg(load(penn[2])))), mul(load(nrm[1]), neg(load(penn[1]))))
        if lt(facing, ZERO_F):
            mark('facing_flip')
            nrm = [bits(neg(load(v))) for v in nrm]
        slide = add(add(mul(neg(pushx), load(nrm[0])), mul(neg(load(pushz_b)), load(nrm[2]))), mul(neg(load(pushy_b)), load(nrm[1])))
        clear = [bits(mul(load(nrm[k]), slide)) for k in range(3)]
        if (not tnull) and (c['isped'] & 7) == 3:
            mark('ped_type')
            if lt(load(st['near']), add(F015, F015)):
                mark('near_lt_03')

                def getmatrix_dot():
                    ev.append([5, tag(ARENA_ENT, 0)])
                    st['matcalls'] += 1
                    if st['matcalls'] == 2 and once(3):
                        st['fwd'][0] = mv
                    f = st['fwd']
                    return add(add(mul(load(nrm[2]), load(f[2])), mul(load(nrm[1]), load(f[1]))), mul(load(nrm[0]), load(f[0])))

                ts = load(c['timestep'] & M32)
                d1 = getmatrix_dot()
                if lt(d1, ZERO_F):
                    mark('dot_neg')
                    tm = load(timer_b)
                    if lt(tm, ZERO_F):
                        mark('timer_neg')
                        timer_b = 0
                    timer_b = bits(add(ts, load(timer_b)))
                else:
                    d2 = getmatrix_dot()
                    if gt(d2, HALF_F):
                        mark('dot_big')
                        if gt(load(timer_b), ZERO_F):
                            mark('timer_pos')
                            timer_b = 0
                        timer_b = bits(sub(load(timer_b), ts))
                    else:
                        mark('dot_mid')
                if st['dirn'] == 0:
                    mark('dirn_zero')
                    pos = [v & M32 for v in c['tpos']]
                    to_ped = [bits(sub(load(pos[k]), load(O[k]))) for k in range(3)]
                    crossz = bits(sub(mul(load(to_ped[0]), load(nrm[1])), mul(load(nrm[0]), load(to_ped[1]))))
                    st['dirn'] = 0xFFFF if gt(load(crossz), ZERO_F) else 1
                    mark('dirn_pos' if st['dirn'] == 0xFFFF else 'dirn_one')
    else:
        mark('sphere_miss')
    # ---- cola
    decay = fpow(F09, load(c['timestep'] & M32))
    timer_b = bits(mul(decay, load(timer_b)))
    ev.append([6, 0x3F800000 if hit else 0, 0xB6EC38, 0xB6EC3C, 0x3E4CCCCD, 0x3D4CCCCD, 0])
    wb_cur = c['wboutcur'] & M32
    wb_spd = c['wboutspd'] & M32
    fac = load(wb_cur)
    clear = [bits(mul(fac, load(clear[k]))) for k in range(3)]

    # ---- cambios en las arenas respecto a su estado inicial
    changes = []

    def diff(arena, off, old, new, size=4):
        ob, nb = [(old >> (8 * j)) & 0xFF for j in range(size)], [(new >> (8 * j)) & 0xFF for j in range(size)]
        changes.extend([arena, off + j, nb[j]] for j in range(size) if nb[j] != ob[j])

    for k in range(3):
        # el vector de limpieza se sobrescribe siempre; su valor previo es el relleno de la arena de la camara
        diff(ARENA_CAM, 0x8FC + 4 * k, camera_fill(c, 0x8FC + 4 * k), clear[k])
    cam_2d_old = c['i2d'] & 0xFF
    if cam_2d_old != 1:
        changes.append([ARENA_CAM, 0x2D, 1])
    diff(ARENA_CAM, 0x164, c['timer'] & M32, timer_b)
    diff(ARENA_CAM, 0x168, c['dirn'] & 0xFFFF, st['dirn'], 2)
    for k in range(3):
        diff(ARENA_OUT, VEC_OFF + 4 * k, c['outinit'][k] & M32, O[k])
    diff(ARENA_TGT, VEC_OFF, c['tgt'][0] & M32, T[0])
    diff(ARENA_FWD, 0x10, c['fwd'][0] & M32, st['fwd'][0])
    changes.sort(key=lambda e: (e[0], e[1]))
    base = (c['seed'] + 1) & M32
    pre = [Rand(base, i) for i in range(8)]
    sp = [v & M32 for v in c['spt']] if hit else pre[0:3]
    sn = [v & M32 for v in c['snorm']] if hit else pre[4:7]
    globals_ = [0, st['near'], wb_cur, wb_spd] + sp + sn
    return {'changes': changes, 'events': ev, 'globals': globals_, 'real_exe': True, 'marks': marks}


def camera_fill(c, off, size=4):
    """contenido inicial de la arena de la camara (relleno pseudoaleatorio de Fixtures.hpp) en el desplazamiento dado"""
    seed = (c['seed'] * 13 + 50) & M32
    at = GUARD + off
    word = at // 4
    w = Rand(seed, word)
    if size == 4:
        if at % 4 != 0:
            raise RuntimeError('desplazamiento no alineado')
        return w
    return (w >> (8 * (at % 4))) & 0xFF


CHANGE_KEYS = ('real_exe', 'changes', 'globals')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
