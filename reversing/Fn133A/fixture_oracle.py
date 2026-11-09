"""Oraculo independiente (Python) de Fn133A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las seis funciones del lote 133A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial y una x87 minima (precision de 24 bits, exponente ampliado, NaN/infinitos, ceros con signo; fsin/fcos/fsqrt en punto fijo de alta precision) para las tres funciones con aritmetica flotante.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x5A7A20  CClothes::GetClothesModelFromName (cdecl, un puntero, `ret`; el listado decia CPedClothesDesc)
       diez comparaciones con `repe cmps` de longitud + 1 (cadena y NUL) contra torso(0), head(1), hands(2), legs(3), feet(4), necklace(5), watch(6), glasses(7), hat(8) y extra1(9) (0x8590CC, 0x8590E8,
       0x867130, 0x859124, 0x867128, 0x8670A0, 0x867098, 0x867090, 0x86708C, 0x85C3B0); EAX = el indice o 0 si ninguna coincide. No escribe memoria.
  1  0x72FC70  CameraSize (cdecl, camara, rectangulo, viewWindow, aspecto, `ret`)
       call 0x7F2D20 (modo actual) y 0x7F2CF0 (RwVideoMode*, indice); w0/h0 = ancho/alto del raster (camara+0x60, +0xC/+0x10); con rectangulo nulo se usa uno local: (0, 0, ancho y alto del modo si
       banderas & 1, si no w0 y h0); un rectangulo dado se usa sin tocarlo. Si w0 != rect.w o h0 != rect.h: raster z (camara+0x64) y raster reciben ancho y alto del rectangulo. Ventana de vista:
       banderas & 1: (viewWindow crudo, float(viewWindow / aspecto)); si no, con ancho > alto (con signo): (viewWindow crudo, float((alto * viewWindow) / ancho)); si no (float((ancho * viewWindow) / alto),
       viewWindow crudo). call 0x7EE410 (camara, RwV2d*); despues dwords [0xC17044] = ancho y [0xC17048] = alto del rectangulo.
  2  0x694850  CTaskComplexFollowLeaderInFormation::Offsets::Constructor (thiscall, `ret`; el listado decia CEntitySeekPosCalculatorFixedPos): escribe 0x84 constantes (ver OFFSETS) y devuelve this.
  3  0x616920  CPedPlacement::FindZCoorForPed (cdecl, CVector*, `ret`, AL)
       origen1 = (x crudo, y por fld/fstp, z + 0.5f), distancia = float(z - 100); 0x5674E0(origen1, distancia, &cp, &hit, 1, 1, 0, 0, 1, 0, 0); z1 = AL ? cp.z : -100.0; origen2 = (float(x + 0.1f), float(y + 0.1f), z + 0.5f),
       misma distancia; z2 igual; alto = z1 > z2 (ordenado) ? z1 : z2; si alto > -99.0 (ordenado): pos.z = float(alto + 1.0), AL = 1; si no AL = 0.
  4  0x65C530  CTaskComplexSmartFleeEntity::CreateSubTask (thiscall, tipo en la pila, `ret 4`, marco SEH que solo desenrolla)
       0xCB: 0x61A5A0(0x20); nulo -> 0; rand() (0x821B1E) & 0xFFFF, fild, * 1/32768, * 50, _ftol2 (truncado); constructor 0x62F310(tiempo, 0, 0, 8.0f) en el bloque; devuelve el bloque.
       0x38E: [this+0x34] = [0xB7CB84], [this+0x38] = [this+0x28], byte [this+0x3C] = 1; m_pos (this+0x10, tres dwords crudos) = posicion de la entidad ([this+0xC]): matriz ([ent+0x14] != 0) + 0x30 o ent + 4;
       0x61A5A0(0x44); nulo -> 0; constructor 0x65BD20(&m_pos, byte [this+0x24], [this+0x20], [this+0x1C]); devuelve el bloque. 0x516 y cualquier otro: 0.
       Los constructores corren reales (con el hook del ASI en ambos lados); su efecto sobre el bloque (desensamblado de 0x62F310, 0x61A390 y 0x65BD20, 0x61A3B0) se modela aqui, salvo la vtable ([+0]), que
       solo se anota como escrita.
  5  0x477DA0  CTheScripts::IsPointInAngledArea (cdecl, CVector2D*, x1, y1, x2, y2, ancho, `ret`, AL; el listado decia CTaskSimpleSetCharIgnoreWeaponRangeFlag)
       t = GetRadianAngleBetweenPoints(x1, y1, x2, y2) (0x53CBE0) + pi/2 (0x858FE4), normalizado a [0, 2pi] (0x858CBC) restando o sumando; s = sin(t) * ancho; c = -(cos(t) * ancho); esquinas (floats):
       L18 = x2 + s, L14 = c + y2, L10 = s + x1, y1c = float(c) + y1; dx = float(x2 - x1), d = y2 - y1 (sin guardar), sx = float(L10 - x1), sy = float(y1c - y1); lenAB = float(sqrt(d*d + dx*dx)),
       lado = float(sqrt(sy*sy + sx*sx)); qx = float(px - x1), qy = float(py - y1); si d*d + dx*dx > 0 (o NaN): inv = 1 / sqrt(...), nx = inv * dx, ny = d * float(inv); si no nx = 1, ny = d;
       proj = ny*qy + qx*nx: dentro solo si proj >= 0 y proj <= lenAB; despues CVector2D::Normalise (0x44E480) de (sx, sy) en el sitio (|v|^2 <= 0: x = 1; si no cada componente por 1/sqrt(|v|^2)
       sin redondear) y proj2 = qy*uy + qx*ux dentro de [0, lado]. Si [0x859CF8] != 0: 0x486990(x1, y1, x2, y2, L18, L14, L10, y1c). Devuelve AL.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF
NAMES = ['CClothes::GetClothesModelFromName', 'CameraSize', 'CTaskComplexFollowLeaderInFormation::Offsets::Constructor', 'CPedPlacement::FindZCoorForPed',
         'CTaskComplexSmartFleeEntity::CreateSubTask', 'CTheScripts::IsPointInAngledArea']
ENTRIES = (0x5A7A20, 0x72FC70, 0x694850, 0x616920, 0x65C530, 0x477DA0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
CLOTHES = ['torso', 'head', 'hands', 'legs', 'feet', 'necklace', 'watch', 'glasses', 'hat', 'extra1']
ONE, MONE, ZERO, Q125, MQ125, MQ25 = 0x3F800000, 0xBF800000, 0, 0x3FA00000, 0xBFA00000, 0xC0200000
# Offsets (8 pares) y MovingOffsets (8 pares) de 0x694850; la escala (+0x80) vale 1.0
OFFSETS = [(MONE, ONE), (ONE, ONE), (ZERO, ONE), (MONE, ZERO), (ONE, ZERO), (MONE, MONE), (ZERO, MONE), (ONE, MONE),
           (MONE, ZERO), (ONE, ZERO), (ZERO, MQ125), (MQ125, MQ125), (Q125, MQ125), (ZERO, MQ25), (MONE, MQ25), (ONE, MQ25)]


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


def fill_kind(nbytes, kind, seed):
    if kind == 0:
        return fill_words(nbytes, seed)
    return bytearray([0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)]) * nbytes


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def put_bytes(self, off, data):
        for k, b in enumerate(data):
            self.m[GUARD + off + k] = b

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


# ------------------------------------------------------------------------------------------------ x87 minima (PC = 24 bits, exponente ampliado)
class Num:
    """kind: 'fin' (v Fraction; neg = signo del cero), 'inf' (neg), 'nan' (bits = float32 silencioso)"""
    __slots__ = ('kind', 'v', 'neg', 'bits', 'snan')

    def __init__(self, kind, v=Fraction(0), neg=False, bits=0, snan=False):
        self.kind, self.v, self.neg, self.bits, self.snan = kind, v, neg, bits, snan

    def is_nan(self):
        return self.kind == 'nan'

    def sign(self):
        if self.kind == 'nan':
            return bool(self.bits >> 31)
        if self.kind == 'inf' or self.v == 0:
            return self.neg
        return self.v < 0


def INDEF():
    return Num('nan', bits=0xFFC00000)


def ilog2(x):
    """floor(log2(x)) de una Fraction > 0"""
    e = x.numerator.bit_length() - x.denominator.bit_length()
    return e if x >= Fraction(2) ** e else e - 1


def fld32(bits):
    """fld dword: un NaN senalizante se convierte en silencioso"""
    bits &= M32
    e, f = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    neg = bool(bits >> 31)
    if e == 0xFF:
        if f == 0:
            return Num('inf', neg=neg)
        return Num('nan', bits=bits | 0x400000)
    val = Fraction(f, 1 << 149) if e == 0 else Fraction(0x800000 | f, 1 << 23) * Fraction(2) ** (e - 127)
    return Num('fin', -val if neg else val, neg)


def mem32(bits):
    """operando `dword [mem]` de fadd/fsub/fmul/fdiv: un NaN senalizante llega SIN convertir (la x87 prefiere entonces el NaN silencioso del otro operando)"""
    n = fld32(bits)
    if n.is_nan() and not (bits & 0x400000):
        return Num('nan', bits=bits & M32, snan=True)
    return n


def fild(i):
    return Num('fin', Fraction(i), False)


def round_sig(x, bits):
    """redondea x (Fraction > 0) a `bits` bits significativos (al par); exponente sin limite"""
    e = ilog2(x)
    quantum = Fraction(2) ** (e - (bits - 1))
    q = x / quantum
    m = q.numerator // q.denominator
    rem = q - m
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (m & 1)):
        m += 1
    return m * quantum


def r24(x):
    if x == 0:
        return x
    return round_sig(abs(x), 24) * (1 if x > 0 else -1)


def _nan_result(a, b):
    """x87: un NaN -> ese NaN silenciado; SNaN y QNaN -> el QNaN; dos del mismo tipo -> el de mayor significando (empate: el primero)"""
    nans = [t for t in (a, b) if t is not None and t.is_nan()]
    if len(nans) == 1:
        return Num('nan', bits=nans[0].bits | 0x400000)
    if nans[0].snan != nans[1].snan:
        q = nans[0] if not nans[0].snan else nans[1]
        return Num('nan', bits=q.bits | 0x400000)
    sa, sb = nans[0].bits & 0x7FFFFF, nans[1].bits & 0x7FFFFF
    if sa == sb and (nans[0].bits ^ nans[1].bits) & 0x80000000:
        # empate de significando con signos distintos: el de signo positivo (observado en el EXE; el manual no lo fija)
        pick = nans[0] if not (nans[0].bits & 0x80000000) else nans[1]
        return Num('nan', bits=pick.bits | 0x400000)
    return Num('nan', bits=(nans[1].bits if sb > sa else nans[0].bits) | 0x400000)


def x_add(a, b):
    if a.is_nan() or b.is_nan():
        return _nan_result(a, b)
    if a.kind == 'inf' and b.kind == 'inf':
        return Num('inf', neg=a.neg) if a.neg == b.neg else INDEF()
    if a.kind == 'inf':
        return Num('inf', neg=a.neg)
    if b.kind == 'inf':
        return Num('inf', neg=b.neg)
    s = a.v + b.v
    if s == 0:
        return Num('fin', Fraction(0), a.sign() and b.sign())
    return Num('fin', r24(s))


def x_neg(a):
    if a.is_nan():
        return Num('nan', bits=a.bits ^ 0x80000000)
    if a.kind == 'inf':
        return Num('inf', neg=not a.neg)
    if a.v == 0:
        return Num('fin', Fraction(0), not a.neg)
    return Num('fin', -a.v)


def x_sub(a, b):
    if a.is_nan() or b.is_nan():
        return _nan_result(a, b)
    return x_add(a, x_neg(b))


def x_mul(a, b):
    if a.is_nan() or b.is_nan():
        return _nan_result(a, b)
    neg = a.sign() != b.sign()
    if a.kind == 'inf' or b.kind == 'inf':
        if (a.kind == 'fin' and a.v == 0) or (b.kind == 'fin' and b.v == 0):
            return INDEF()
        return Num('inf', neg=neg)
    p = a.v * b.v
    if p == 0:
        return Num('fin', Fraction(0), neg)
    return Num('fin', r24(p))


def x_div(a, b):
    if a.is_nan() or b.is_nan():
        return _nan_result(a, b)
    neg = a.sign() != b.sign()
    if a.kind == 'inf':
        return INDEF() if b.kind == 'inf' else Num('inf', neg=neg)
    if b.kind == 'inf':
        return Num('fin', Fraction(0), neg)
    if b.v == 0:
        return INDEF() if a.v == 0 else Num('inf', neg=neg)
    if a.v == 0:
        return Num('fin', Fraction(0), neg)
    return Num('fin', r24(a.v / b.v))


def sqrt_frac(x):
    """sqrt de una Fraction > 0 redondeada a 24 bits (al par), con bit pegajoso"""
    e2 = 2 * ((x.numerator.bit_length() - x.denominator.bit_length()) // 2)
    y = x / Fraction(2) ** e2
    P = 160
    big = (y.numerator << (2 * P)) // y.denominator
    r = isqrt(big)
    exact = r * r * y.denominator == (y.numerator << (2 * P)) and big * y.denominator == (y.numerator << (2 * P))
    val = Fraction(2 * r + (0 if exact else 1), 1 << (P + 1)) * Fraction(2) ** (e2 // 2)
    return round_sig(val, 24)


def x_sqrt(a):
    if a.is_nan():
        return Num('nan', bits=a.bits | 0x400000)
    if a.kind == 'inf':
        return INDEF() if a.neg else Num('inf')
    if a.v == 0:
        return Num('fin', Fraction(0), a.neg)
    if a.v < 0:
        return INDEF()
    return Num('fin', sqrt_frac(a.v))


FP = 220


def _sincos_fixed(t, want_cos):
    """seno o coseno de t (Fraction >= 0, <= ~7) con FP bits fraccionarios"""
    tf = (t.numerator << FP) // t.denominator
    one = 1 << FP
    term = one if want_cos else tf
    total = term
    k = 0 if want_cos else 1
    for i in range(1, 300):
        term = -(((term * tf) >> FP) * tf >> FP)
        term //= (k + 1) * (k + 2)
        k += 2
        total += term
        if term == 0 and i > 8:
            break
    return Fraction(total, one)


def x_sin(a):
    if a.is_nan():
        return Num('nan', bits=a.bits | 0x400000)
    if a.kind == 'inf':
        return INDEF()
    if a.v == 0:
        return Num('fin', Fraction(0), a.neg)
    if a.v < 0:
        return x_neg(x_sin(x_neg(a)))
    return Num('fin', _sincos_fixed(a.v, False))


def x_cos(a):
    if a.is_nan():
        return Num('nan', bits=a.bits | 0x400000)
    if a.kind == 'inf':
        return INDEF()
    return Num('fin', _sincos_fixed(abs(a.v), True))


def to_f32(a):
    """fstp dword: bits del float"""
    if a.is_nan():
        return a.bits
    if a.kind == 'inf':
        return 0xFF800000 if a.neg else 0x7F800000
    sign = 0x80000000 if a.sign() else 0
    x = abs(a.v)
    if x == 0:
        return sign
    e = ilog2(x)
    quantum = Fraction(2) ** (-149) if e < -126 else Fraction(2) ** (e - 23)
    q = x / quantum
    m = q.numerator // q.denominator
    rem = q - m
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (m & 1)):
        m += 1
    val = m * quantum
    if val >= Fraction(2) ** 128:
        return sign | 0x7F800000
    if val == 0:
        return sign
    if val < Fraction(2) ** (-126):
        return sign | int(val / Fraction(2) ** (-149))
    ee = ilog2(val)
    frac = int(val / Fraction(2) ** ee * (1 << 23)) - (1 << 23)
    return sign | ((ee + 127) << 23) | frac


def _key(n):
    if n.kind == 'inf':
        return (-2, Fraction(0)) if n.neg else (2, Fraction(0))
    return (0, n.v)


def fcmp(a, b):
    """-> 'lt', 'eq', 'gt' o 'un'"""
    if a.is_nan() or b.is_nan():
        return 'un'
    ka, kb = _key(a), _key(b)
    return 'lt' if ka < kb else ('gt' if ka > kb else 'eq')


C_HALF, C_TENTH, C_HUNDRED = fld32(0x3F000000), fld32(0x3DCCCCCD), fld32(0x42C80000)
C_ONE, C_M99 = fld32(0x3F800000), fld32(0xC2C60000)
C_HALFPI, C_TWOPI, C_ZERO = fld32(0x3FC90FDB), fld32(0x40C90FDB), fld32(0)

KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'm0', 'm1', 'm2', 'm3', 'm4', 'm5', 'm6', 'm7', 'm8', 'm9', 'miss', 'prefix', 'extra_char', 'changed', 'upper', 'empty', 'random', 'high_byte', 'near'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'rect_null', 'rect_given', 'exclusive', 'windowed', 'w_gt_h', 'w_le_h', 'update', 'noupdate', 'shared', 'vw_special', 'aspect_special', 'dim_zero', 'dim_neg', 'w_eq_h',
        'excl_aspect_zero', 'nan_result'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_unaligned', 'spill'],
    3: ['kind0', 'kind1', 'kind2', 'kind3', 'both_hit', 'first_only', 'second_only', 'none', 'z1_wins', 'z2_wins', 'found', 'notfound', 'pos_special', 'tie', 'boundary_pos', 'ret_high_garbage'],
    4: ['kind0', 'kind1', 'kind2', 'kind3', 'type_cb', 'type_38e', 'type_516', 'type_other', 'null_new', 'matrix', 'placement', 'scream_0', 'scream_1', 'scream_other', 'time_minus1', 'rand_max', 'rand_zero'],
    5: ['kind0', 'kind1', 'kind2', 'kind3', 'inside', 'outside', 'proj_neg', 'proj_gt_len', 'proj2_fail', 'len_zero', 'angle_nan', 'angle_loop_neg', 'angle_loop_pos', 'dbg_on', 'dbg_off', 'coord_special', 'side_zero', 'width_zero'],
}


def is_nan_bits(bits):
    bits &= M32
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def special_float(bits):
    bits &= M32
    return (bits & 0x7F800000) == 0x7F800000 or (bits & 0x7FFFFFFF) == 0 or (bits & 0x7F800000) == 0


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    s = bytes.fromhex(c['s'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[20]), Mem(fill, a[21]), Mem(fill, a[22])
    events, gch, ret = [], [], None

    if fn == 0:
        C.put_bytes(a[0], s + b'\0')
        A.snap(); B.snap(); C.snap()
        text = s.decode('latin-1')
        res = CLOTHES.index(text) if text in CLOTHES else 0
        if text in CLOTHES:
            marks.append('m%d' % res)
        else:
            marks.append('miss')
            marks.append({1: 'prefix', 2: 'extra_char', 3: 'changed', 4: 'upper', 5: 'empty', 6: 'random', 7: 'high_byte', 8: 'near'}.get(a[3], 'miss'))
        ret = res
    elif fn == 1:
        rs = fill_kind(8, fill, a[23])
        # imagen de referencia = lo que escribe Setup (los punteros de la camara son direcciones reales: aqui valen 0xAAAA000x, no cuentan como cambios)
        A.wr(a[0] + 0x60, 4, 0xAAAA0001); A.wr(a[0] + 0x64, 4, 0xAAAA0002)
        B.wr(a[3] + 0xC, 4, a[8]); B.wr(a[3] + 0x10, 4, a[9])
        if not a[5]:
            C.wr(a[4] + 0xC, 4, a[18]); C.wr(a[4] + 0x10, 4, a[19])
        A.wr(a[6] + 0, 4, rnd(a[23], 1)); A.wr(a[6] + 4, 4, rnd(a[23], 2)); A.wr(a[6] + 8, 4, a[10]); A.wr(a[6] + 12, 4, a[11])
        A.snap(); B.snap(); C.snap()
        flags = a[14]
        excl = bool(flags & 1)
        w0, h0 = s32(a[8]), s32(a[9])
        if a[7]:
            rw, rh = s32(a[10]), s32(a[11])
        elif excl:
            rw, rh = s32(a[12]), s32(a[13])
        else:
            rw, rh = w0, h0
        update = (w0 != rw) or (h0 != rh)
        if update:
            if not a[5]:
                C.wr(a[4] + 0xC, 4, rw & M32); C.wr(a[4] + 0x10, 4, rh & M32)
            B.wr(a[3] + 0xC, 4, rw & M32); B.wr(a[3] + 0x10, 4, rh & M32)
        vwn = fld32(a[16])
        if excl:
            x_bits = a[16]
            y_bits = to_f32(x_div(vwn, mem32(a[17])))
        elif rw > rh:
            x_bits = a[16]
            y_bits = to_f32(x_div(x_mul(fild(rh), mem32(a[16])), fild(rw)))
        else:
            y_bits = a[16]
            x_bits = to_f32(x_div(x_mul(fild(rw), mem32(a[16])), fild(rh)))
        events = [[0x7F2D20], [0x7F2CF0, a[15]], [0x7EE410, tag(0, a[0]), x_bits, y_bits]]
        new = bytearray(rs)
        new[0:4] = struct.pack('<I', rw & M32); new[4:8] = struct.pack('<I', rh & M32)
        gch = [[0, i, new[i]] for i in range(8) if new[i] != rs[i]]
        marks.append('exclusive' if excl else 'windowed')
        marks.append('rect_given' if a[7] else 'rect_null')
        marks.append('update' if update else 'noupdate')
        if a[5]:
            marks.append('shared')
        if not excl:
            marks.append('w_gt_h' if rw > rh else 'w_le_h')
            if rw == rh:
                marks.append('w_eq_h')
        if special_float(a[16]):
            marks.append('vw_special')
        if special_float(a[17]):
            marks.append('aspect_special')
        if excl and (a[17] & 0x7FFFFFFF) == 0:
            marks.append('excl_aspect_zero')
        if 0 in (rw, rh, w0, h0):
            marks.append('dim_zero')
        if min(rw, rh, w0, h0) < 0:
            marks.append('dim_neg')
        if is_nan_bits(x_bits) or is_nan_bits(y_bits):
            marks.append('nan_result')
    elif fn == 2:
        A.snap(); B.snap(); C.snap()
        o = a[0]
        for i, (x, y) in enumerate(OFFSETS):
            A.wr(o + 8 * i, 4, x); A.wr(o + 8 * i + 4, 4, y)
        A.wr(o + 0x80, 4, ONE)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + 0x84 > WIN:
            marks.append('spill')
        ret = tag(0, o)
    elif fn == 3:
        o = a[0]
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5])
        A.snap(); B.snap(); C.snap()
        xb, yb, zb = a[3], a[4], a[5]
        yq = (yb | 0x400000) if is_nan_bits(yb) else yb          # fld + fstp
        zn = fld32(zb)
        zprobe = to_f32(x_add(zn, C_HALF))
        dist = to_f32(x_sub(zn, C_HUNDRED))
        x2 = to_f32(x_add(fld32(xb), C_TENTH))
        y2 = to_f32(x_add(fld32(yq), C_TENTH))
        flags = [1, 1, 0, 0, 1, 0, 0]
        events = [[0x5674E0, xb, yq, zprobe, dist] + flags, [0x5674E0, x2, y2, zprobe, dist] + flags]
        hit1, hit2 = bool(a[6] & 0xFF), bool(a[8] & 0xFF)
        z1 = fld32(a[7] if hit1 else 0xC2C80000)
        z2 = fld32(a[9] if hit2 else 0xC2C80000)
        z1wins = fcmp(z1, z2) == 'gt'
        high = z1 if z1wins else z2
        ok = fcmp(high, C_M99) == 'gt'
        if ok:
            A.wr(o + 8, 4, to_f32(x_add(high, C_ONE)))
        ret = 1 if ok else 0
        marks.append('both_hit' if hit1 and hit2 else 'first_only' if hit1 else 'second_only' if hit2 else 'none')
        marks.append('z1_wins' if z1wins else 'z2_wins')
        marks.append('found' if ok else 'notfound')
        if special_float(xb) or special_float(yb) or special_float(zb):
            marks.append('pos_special')
        if hit1 and hit2 and a[7] == a[9]:
            marks.append('tie')
        if high.kind == 'fin' and abs(high.v + 99) < Fraction(1, 1000):
            marks.append('boundary_pos')
        if (a[6] | a[8]) & 0xFFFFFF00:
            marks.append('ret_high_garbage')
    elif fn == 4:
        o = a[0]
        ttype = a[6]
        ent = a[3]
        if a[5]:
            C.wr(a[4] + 0x30, 4, a[16]); C.wr(a[4] + 0x34, 4, a[17]); C.wr(a[4] + 0x38, 4, a[18])
        else:
            B.wr(ent + 4, 4, a[16]); B.wr(ent + 8, 4, a[17]); B.wr(ent + 12, 4, a[18])
        A.snap(); B.snap(); C.snap()
        blk = a[14]
        null = bool(a[15])
        marks.append('matrix' if a[5] else 'placement')
        if null:
            marks.append('null_new')
        if ttype == 0xCB:
            marks.append('type_cb')
            events.append([0x61A5A0, 0x20])
            if not null:
                events.append([0x821B1E])
                n = a[13] & 0xFFFF
                tm = (n * 50) // 32768
                # constructor 0x62F310: [+4] = 0, [+8] = tiempo, [+0xC] = [+0x10] = 0, bytes +0x14, +0x15 = 0, byte +0x18 = 0, +0x19 = 0, [+0x1C] = 8.0f; vtable en +0 (solo se anota)
                B.wr(blk + 4, 4, 0); B.wr(blk + 8, 4, tm); B.wr(blk + 0xC, 4, 0); B.wr(blk + 0x10, 4, 0)
                B.wr(blk + 0x14, 1, 0); B.wr(blk + 0x15, 1, 0); B.wr(blk + 0x18, 1, 0); B.wr(blk + 0x19, 1, 0); B.wr(blk + 0x1C, 4, 0x41000000)
                ret = tag(1, blk)
                marks.append('rand_max' if n == 0xFFFF else 'rand_zero' if n == 0 else 'rand_mid')
            else:
                ret = 0
        elif ttype == 0x38E:
            marks.append('type_38e')
            A.wr(o + 0x34, 4, a[12]); A.wr(o + 0x38, 4, a[10]); A.wr(o + 0x3C, 1, 1)
            for k in range(3):
                A.wr(o + 0x10 + 4 * k, 4, a[16 + k])
            events.append([0x61A5A0, 0x44])
            if not null:
                # constructor 0x65BD20 (CTaskComplex 0x61A3B0: [+4] = [+8] = 0): pos en +0x18, byte +0x24, tiempo +0x28, distancia +0x2C, +0x30 = 7, temporizador a cero y, con tiempo != -1, arrancado
                B.wr(blk + 4, 4, 0); B.wr(blk + 8, 4, 0)
                for k in range(3):
                    B.wr(blk + 0x18 + 4 * k, 4, a[16 + k])
                B.wr(blk + 0x24, 1, a[9] & 0xFF)
                B.wr(blk + 0x28, 4, a[7]); B.wr(blk + 0x2C, 4, a[8]); B.wr(blk + 0x30, 4, 7)
                B.wr(blk + 0x34, 4, 0); B.wr(blk + 0x38, 4, 0); B.wr(blk + 0x3C, 1, 0); B.wr(blk + 0x3D, 1, 0); B.wr(blk + 0x40, 1, 0); B.wr(blk + 0x41, 1, 0)
                if a[7] != M32:
                    B.wr(blk + 0x34, 4, a[12]); B.wr(blk + 0x38, 4, a[7]); B.wr(blk + 0x3C, 1, 1)
                ret = tag(1, blk)
            else:
                ret = 0
            sc = a[9] & 0xFF
            marks.append('scream_0' if sc == 0 else 'scream_1' if sc == 1 else 'scream_other')
            if a[7] == M32:
                marks.append('time_minus1')
        else:
            marks.append('type_516' if ttype == 0x516 else 'type_other')
            ret = 0
    else:
        o = a[0]
        A.snap(); B.snap(); C.snap()
        x1, y1, x2, y2, w, px, py = a[5], a[6], a[7], a[8], a[9], a[3], a[4]
        ev = [[0x53CBE0, x1, y1, x2, y2]]
        X1, Y1, X2, Y2, W = fld32(x1), fld32(y1), fld32(x2), fld32(y2), fld32(w)
        ang = fld32(a[10])
        t = x_add(ang, C_HALFPI)
        loops = 0
        while fcmp(t, C_ZERO) == 'lt':
            t = x_add(t, C_TWOPI)
            loops += 1
        if loops:
            marks.append('angle_loop_neg')
        n2 = 0
        while fcmp(t, C_TWOPI) == 'gt':
            t = x_sub(t, C_TWOPI)
            n2 += 1
        if n2:
            marks.append('angle_loop_pos')
        if ang.is_nan():
            marks.append('angle_nan')
        sn = x_mul(x_sin(t), mem32(w))
        L18 = to_f32(x_add(X2, sn))
        cw = x_neg(x_mul(x_cos(t), mem32(w)))
        negc = fld32(to_f32(cw))
        L14 = to_f32(x_add(cw, mem32(y2)))
        L10 = to_f32(x_add(sn, mem32(x1)))
        y1c = to_f32(x_add(negc, mem32(y1)))
        dx = fld32(to_f32(x_sub(X2, mem32(x1))))
        d = x_sub(Y2, mem32(y1))
        sx = fld32(to_f32(x_sub(fld32(L10), mem32(x1))))
        sy = fld32(to_f32(x_sub(fld32(y1c), mem32(y1))))
        len2 = x_add(x_mul(d, d), x_mul(dx, dx))
        lenab = fld32(to_f32(x_sqrt(len2)))
        side2 = x_add(x_mul(sy, sy), x_mul(sx, sx))
        lenside = fld32(to_f32(x_sqrt(side2)))
        qx = fld32(to_f32(x_sub(fld32(px), mem32(x1))))
        qy = fld32(to_f32(x_sub(fld32(py), mem32(y1))))
        if fcmp(len2, C_ZERO) in ('gt', 'un'):
            inv = x_div(C_ONE, x_sqrt(len2))
            invf = fld32(to_f32(inv))
            nx = x_mul(inv, dx)
            ny = x_mul(d, invf)
        else:
            nx, ny = C_ONE, d
            marks.append('len_zero')
        proj = x_add(x_mul(ny, qy), x_mul(qx, nx))
        inside = False
        if fcmp(proj, C_ZERO) in ('gt', 'eq') and fcmp(proj, lenab) in ('lt', 'eq'):
            # CVector2D::Normalise (0x44E480) en el sitio sobre (sx, sy)
            norm2 = x_add(x_mul(sx, sx), x_mul(sy, sy))
            ux, uy = sx, sy
            if fcmp(norm2, C_ZERO) in ('gt', 'un'):
                inv2 = x_div(C_ONE, x_sqrt(norm2))
                ux = fld32(to_f32(x_mul(inv2, sx)))
                uy = fld32(to_f32(x_mul(inv2, sy)))
            else:
                ux = fld32(ONE)
                marks.append('side_zero')
            proj2 = x_add(x_mul(qy, uy), x_mul(qx, ux))
            if fcmp(proj2, C_ZERO) in ('gt', 'eq') and fcmp(proj2, lenside) in ('lt', 'eq'):
                inside = True
            else:
                marks.append('proj2_fail')
        else:
            marks.append('proj_neg' if fcmp(proj, C_ZERO) not in ('gt', 'eq') else 'proj_gt_len')
        if a[11] & 0xFF:
            ev.append([0x486990, x1, y1, x2, y2, L18, L14, L10, y1c])
            marks.append('dbg_on')
        else:
            marks.append('dbg_off')
        events = ev
        ret = 1 if inside else 0
        marks.append('inside' if inside else 'outside')
        if any(special_float(v) for v in (x1, y1, x2, y2, px, py)):
            marks.append('coord_special')
        if (w & 0x7FFFFFFF) == 0:
            marks.append('width_zero')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    ch, ex = dict(o.get('changes')), e['changes']
    if c['fn'] == 4:
        # la vtable (los 4 primeros bytes del bloque creado) solo se anota como escrita: debe haber cambiado y se descarta antes de comparar el resto
        a = [x & M32 for x in c['a']]
        if not a[15] and a[6] in (0xCB, 0x38E):
            lo = a[14]
            inside = [x for x in ch['B'] if lo <= x[0] < lo + 4]
            if not inside:
                return 'la vtable del objeto creado no se escribio'
            ch['B'] = [x for x in ch['B'] if not (lo <= x[0] < lo + 4)]
    if ch != ex:
        return 'changes: %s != %s' % (str(ch)[:300], str(ex)[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
