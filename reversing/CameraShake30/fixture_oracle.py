"""Oraculo independiente del banco CameraShake30: CCamera::ProcessShake(float) (0x516560). Se modela desde el DESENSAMBLADO (no desde el C++ de la DLL) a partir de la fila `input`.
x87 con control de precision a 24 bits y exponente extendido (fadd/fsub/fmul/fdiv/fsqrt redondean al par a 24 bits; fstp dword redondea al rango float32):

  516560..516580  ebx = &camaras[activa] (this + 0x174 + activa * 0x238); si [0xB70048] == 0: 65 escrituras de parametros en la tabla de CHandShaker (INIT) y [0xB70048] = 1
  516971..51698C  entrada = 0xB6ECA0 + m_nShakeType * 0x94; llama a CHandShaker::Process (0x50D930) con ECX = entrada y el argumento m_fShakeIntensity (+0xC90)
  516994..5169AA  angulo = float(entrada.m_ang.z (+8) * m_fShakeIntensity)
  5169AE..5169CE  front = float(front * matriz) con 0x59C810 (matriz = entrada + 0x3C; x = (m1*v1 + m2*v2) + v0*m0, y = (m5*v1 + m4*v0) + m6*v2, z = (m9*v1 + m8*v0) + m10*v2);
                  Normalise 0x59C910: s = (x*x + y*y) + z*z; si s <= 0 solo x = 1.0f; si no r = 1/sqrt(s) y cada componente float(r * v)
  5169D3..516A07  up = {float(fsin(angulo)), 0, float(fcos(angulo))} (fsin/fcos del x87 a 64 bits)
  516A12..516A2E  side = front x up (0x59C730: x = b2*a1 - a2*b1, y = a2*b0 - b2*a0, z = a0*b1 - b0*a1; cada producto a 24 bits, resta a 24 bits, float), Normalise(side); up = side x front
  516A33..516A70  si front.x == 0 y front.y == 0 (fcomp con 0.0, solo igualdad ordenada): ambos = 0x38D1B717
  516A7A..516ACF  right = front x up; Normalise(right); up = right x front
fsin/fcos: NaN -> el mismo NaN; +-inf -> indefinido (0xFFC00000); +-0 -> seno +-0, coseno 1; |A| >= 2^63 -> el operando queda igual (C2); 2^20 <= |A| < 2^63 -> resultado impreciso del hardware
(reduccion con pi de 66 bits): DESCONOCIDO; el resto: sin/cos de doble precision redondeado a float (el x87 da 64 bits; el empate de redondeo es despreciable).
Una operacion con un unico NaN devuelve ese NaN; con dos, el de mayor significando; mismo significando y signos opuestos -> DESCONOCIDO ('??', se exige EXE == DLL); 0*inf o inf-inf sin NaN
dan el indefinido real (0xFFC00000). Los valores desconocidos se propagan. Se escribe solo front y up de la camara activa, la tabla (INIT + lo que escribe el falso) y [0xB70048].
"""
import random
import math
import struct
from fractions import Fraction

INDEFINITE = 0xFFC00000


class V:
    """Valor x87: kind 'f' finito (val Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits float32 silencioso), 'unk' (NaN de carga util/signo desconocidos)."""
    __slots__ = ('kind', 'val', 'neg', 'bits')

    def __init__(self, kind, val=Fraction(0), neg=False, bits=0):
        self.kind, self.val, self.neg, self.bits = kind, val, neg, bits

    def is_nan(self):
        return self.kind in ('nan', 'unk')

    def sign(self):
        return self.neg if (self.kind == 'inf' or (self.kind == 'f' and self.val == 0)) else self.val < 0

    def is_zero(self):
        return self.kind == 'f' and self.val == 0


def load(bits):
    """fld dword: exacta (los subnormales se cargan tal cual)."""
    bits &= 0xFFFFFFFF
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man:
            return V('nan', bits=bits | 0x400000)       # sNaN -> qNaN (las fixtures no los generan)
        return V('inf', neg=bool(sign))
    if exp == 0:
        val = Fraction(man, 1 << 149)
    else:
        val = Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127))
    if sign:
        val = -val
    return V('f', val, neg=bool(sign))


def from_int(n):
    """fild dword."""
    return V('f', Fraction(n), neg=n < 0)


def r24(x):
    """Redondeo exacto (al par) de un racional a 24 bits de mantisa, exponente sin limite (x87 con PC=24)."""
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


def nan_pick(a, b):
    """Resultado de una operacion con al menos un NaN."""
    if a.kind == 'unk' or b.kind == 'unk':
        return V('unk')
    if a.is_nan() and b.is_nan():
        ma, mb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if ma > mb:
            return a
        if mb > ma:
            return b
        if (a.bits >> 31) == (b.bits >> 31):
            return a
        return V('unk')                                  # empate con signos opuestos: no resuelto
    return a if a.is_nan() else b


def indefinite():
    return V('nan', bits=INDEFINITE)


def neg(v):
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return V('inf', neg=not v.neg)
    if v.val == 0:
        return V('f', Fraction(0), neg=not v.neg)
    return V('f', -v.val, neg=v.val > 0)


def add(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf':
            return a if a.neg == b.neg else indefinite()
        return a if a.kind == 'inf' else b
    s = a.val + b.val
    if s == 0:
        if a.val == 0 and b.val == 0:
            return V('f', Fraction(0), neg=a.neg and b.neg)
        return V('f', Fraction(0), neg=False)            # cancelacion exacta: +0 (redondeo al mas cercano)
    s = r24(s)
    return V('f', s, neg=s < 0)


def sub(a, b):
    return add(a, neg(b))


def mul(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf' or b.kind == 'inf':
        if a.is_zero() or b.is_zero():
            return indefinite()
        return V('inf', neg=sg)
    p = a.val * b.val
    if p == 0:
        return V('f', Fraction(0), neg=sg)
    p = r24(p)
    return V('f', p, neg=p < 0)


def div(a, b):
    """a / b (fdiv / fdivr)."""
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf':
        return indefinite() if b.kind == 'inf' else V('inf', neg=sg)
    if b.kind == 'inf':
        return V('f', Fraction(0), neg=sg)
    if b.val == 0:
        return indefinite() if a.val == 0 else V('inf', neg=sg)
    if a.val == 0:
        return V('f', Fraction(0), neg=sg)
    q = r24(a.val / b.val)
    return V('f', q, neg=q < 0)


def fsqrt24(p):
    """sqrt de un racional positivo redondeada al par a 24 bits (exacta, con bit pegajoso)."""
    p = Fraction(p)
    k = 400
    n = p * (Fraction(2) ** (2 * k))
    ni = n.numerator // n.denominator
    r = math.isqrt(ni)
    inexact = r * r != n
    v = Fraction(2 * r + 1, 2) if inexact else Fraction(r)
    return r24(v / (Fraction(2) ** k))


def sqrt(a):
    """fsqrt (solo se llama con a >= 0, +0 o NaN/inf)."""
    if a.is_nan():
        return a
    if a.kind == 'inf':
        return a if not a.neg else indefinite()
    if a.val == 0:
        return a
    if a.val < 0:
        return indefinite()
    s = fsqrt24(a.val)
    return V('f', s, neg=False)


def store(v):
    """fstp dword: bits float32 (None = desconocido)."""
    if v.kind == 'unk':
        return None
    if v.kind == 'nan':
        return v.bits | 0x400000
    if v.kind == 'inf':
        return 0xFF800000 if v.neg else 0x7F800000
    if v.val == 0:
        return 0x80000000 if v.neg else 0
    sign = 0x80000000 if v.val < 0 else 0
    a = abs(v.val)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (max(e, -126) - 23)
    m = a / quantum
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    res = q * quantum
    if res >= Fraction(2) ** 128:
        return sign | 0x7F800000
    if res == 0:
        return sign
    return sign | struct.unpack('<I', struct.pack('<f', float(res)))[0] & 0x7FFFFFFF


def fl(v):
    """store + fld: valor tras pasar por memoria como float32 (V('unk') si el valor es desconocido)."""
    b = store(v)
    return V('unk') if b is None else load(b)


def unordered_or_cmp(a, b):
    """fcom: 'unordered' | 'less' | 'equal' | 'greater' (+0 == -0)."""
    if a.is_nan() or b.is_nan():
        return 'unordered'
    av = math.inf if a.kind == 'inf' and not a.neg else (-math.inf if a.kind == 'inf' else a.val)
    bv = math.inf if b.kind == 'inf' and not b.neg else (-math.inf if b.kind == 'inf' else b.val)
    return 'less' if av < bv else ('greater' if av > bv else 'equal')


ONE = load(0x3F800000)
ZERO = load(0)


def is_float_bits_nan(b):
    return ((b >> 23) & 0xFF) == 0xFF and (b & 0x7FFFFF) != 0


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def hex_wild_match(obs, want):
    """Compara dos cadenas hex (observada, esperada con '??' como comodin por byte)."""
    if len(obs) != len(want):
        return False
    for j in range(0, len(want), 2):
        if want[j:j + 2] != '??' and want[j:j + 2] != obs[j:j + 2]:
            return False
    return True

CASES = 4096
NAME = 'CCamera::ProcessShake'
NAMES = [NAME]
# Constantes de memoria: DEBEN coincidir con Fixtures.hpp (las comprueba fixture_selfcheck.py contra el volcado compilado de Fixtures.hpp).
LAYOUT = {'guard': 0x40, 'obj_bytes': 0xD78, 'arena': 0xDF8, 'table_bytes': 0x378, 'entry_bytes': 0x94, 'entries': 6}
TABLE_ADDR = 0xB6ECA0
ENTRY_ANGZ, ENTRY_MATRIX = 0x08, 0x3C
MATRIX_WORDS = (0, 1, 2, 4, 5, 6, 8, 9, 10)     # indices de float dentro de CMatrix de las 9 palabras que escribe el falso (right, forward, up)
TINY = 0x38D1B717
INIT = {  # rellena la tabla (0x516586..0x516967), extraido del desensamblado y comprobado con la lista de Camera.cpp
    0x00B6ED40: 0x3CA3D70A, 0x00B6ED44: 0x3CA3D70A, 0x00B6ED48: 0x3C23D70A, 0x00B6ED4C: 0x3951B717, 0x00B6ED50: 0x3951B717, 0x00B6ED54: 0x38D1B717,
    0x00B6ED64: 0x3FA66666, 0x00B6ED68: 0x3FA66666, 0x00B6ED6C: 0x3FB33333, 0x00B6EDB8: 0x3E99999A, 0x00B6EDBC: 0x3F800000, 0x00B6EDC0: 0x0000000F,
    0x00B6EDC4: 0x3A83126F, 0x00B6EDD4: 0x3CA3D70A, 0x00B6EDD8: 0x3CA3D70A, 0x00B6EDDC: 0x3D23D70A, 0x00B6EDE0: 0x3951B717, 0x00B6EDE4: 0x3951B717,
    0x00B6EDE8: 0x38D1B717, 0x00B6EDF8: 0x3FA66666, 0x00B6EDFC: 0x3FA66666, 0x00B6EE00: 0x3FB33333, 0x00B6EE4C: 0x3E99999A, 0x00B6EE50: 0x3F800000,
    0x00B6EE54: 0x00000014, 0x00B6EE58: 0x3A83126F, 0x00B6EE68: 0x3CA3D70A, 0x00B6EE6C: 0x3CA3D70A, 0x00B6EE70: 0x3C23D70A, 0x00B6EE74: 0x3951B717,
    0x00B6EE78: 0x3951B717, 0x00B6EE7C: 0x38D1B717, 0x00B6EE8C: 0x3FA66666, 0x00B6EE90: 0x3FA66666, 0x00B6EE94: 0x3FB33333, 0x00B6EEE0: 0x3E99999A,
    0x00B6EEE4: 0x3F800000, 0x00B6EEE8: 0x0000000A, 0x00B6EEEC: 0x3A03126F, 0x00B6EEFC: 0x3CA3D70A, 0x00B6EF00: 0x3CA3D70A, 0x00B6EF04: 0x3C23D70A,
    0x00B6EF08: 0x3951B717, 0x00B6EF0C: 0x3951B717, 0x00B6EF10: 0x38D1B717, 0x00B6EF20: 0x3FA66666, 0x00B6EF24: 0x3FA66666, 0x00B6EF28: 0x3FB33333,
    0x00B6EF74: 0x3E99999A, 0x00B6EF78: 0x3F800000, 0x00B6EF7C: 0x00000014, 0x00B6EF80: 0x3B03126F, 0x00B6EF90: 0x3CA3D70A, 0x00B6EF94: 0x3CA3D70A,
    0x00B6EF98: 0x3C23D70A, 0x00B6EF9C: 0x3951B717, 0x00B6EFA0: 0x3951B717, 0x00B6EFA4: 0x38D1B717, 0x00B6EFB4: 0x3FA66666, 0x00B6EFB8: 0x3FA66666,
    0x00B6EFBC: 0x3FB33333, 0x00B6F008: 0x3E99999A, 0x00B6F00C: 0x3F800000, 0x00B6F010: 0x00000002, 0x00B6F014: 0x3B449BA6,
}




def L(bits):
    return V('unk') if bits is None else load(bits)


def S(v):
    return store(v)


def matinv(m, v):
    """0x59C810: out = v * M (m = indices de float de CMatrix 0..10)."""
    x = add(add(mul(L(m[1]), L(v[1])), mul(L(m[2]), L(v[2]))), mul(L(v[0]), L(m[0])))
    y = add(add(mul(L(m[5]), L(v[1])), mul(L(m[4]), L(v[0]))), mul(L(m[6]), L(v[2])))
    z = add(add(mul(L(m[9]), L(v[1])), mul(L(m[8]), L(v[0]))), mul(L(m[10]), L(v[2])))
    return [S(x), S(y), S(z)]


def normalise(v):
    """0x59C910 -> (bits, camino cero)."""
    x, y, z = L(v[0]), L(v[1]), L(v[2])
    s = add(add(mul(x, x), mul(y, y)), mul(z, z))
    if unordered_or_cmp(s, ZERO) in ('less', 'equal'):
        return [0x3F800000, v[1], v[2]], True
    r = div(ONE, sqrt(s))
    return [S(mul(r, x)), S(mul(r, y)), S(mul(r, z))], False


def cross(a, b):
    """0x59C730 (a = segundo argumento, b = tercero)."""
    x = sub(mul(L(b[2]), L(a[1])), mul(L(a[2]), L(b[1])))
    y = sub(mul(L(a[2]), L(b[0])), mul(L(b[2]), L(a[0])))
    z = sub(mul(L(a[0]), L(b[1])), mul(L(b[0]), L(a[1])))
    return [S(x), S(y), S(z)]


def sincos(a):
    """-> (bits del seno, bits del coseno, clase) con None = desconocido."""
    if a is None:
        return None, None, 'unknown'
    v = load(a)
    if v.kind == 'nan':
        return a | 0x400000, a | 0x400000, 'nan'
    if v.kind == 'inf':
        return INDEFINITE, INDEFINITE, 'inf'
    if v.val == 0:
        return a, 0x3F800000, 'zero'
    if abs(v.val) >= Fraction(2) ** 63:
        return a, a, 'huge'
    if abs(v.val) >= Fraction(2) ** 20:
        return None, None, 'imprecise'
    x = float(v.val)
    return S(V('f', Fraction(math.sin(x)), neg=math.sin(x) < 0)), S(V('f', Fraction(math.cos(x)), neg=math.cos(x) < 0)), 'normal'


def is_inf(b):
    return b is not None and ((b >> 23) & 0xFF) == 0xFF and not (b & 0x7FFFFF)


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    table = bytearray(bytes.fromhex(inp['table_in']))
    if inp['flag'] == 0:
        for addr, val in INIT.items():
            table[addr - TABLE_ADDR:addr - TABLE_ADDR + 4] = struct.pack('<I', val)
    entry = inp['type'] * LAYOUT['entry_bytes']
    table[entry + ENTRY_ANGZ:entry + ENTRY_ANGZ + 4] = struct.pack('<I', inp['ang_z'])
    m = {}
    for i, idx in enumerate(MATRIX_WORDS):
        table[entry + ENTRY_MATRIX + 4 * idx:entry + ENTRY_MATRIX + 4 * idx + 4] = struct.pack('<I', inp['mat'][i])
        m[idx] = inp['mat'][i]
    angle = S(mul(load(inp['ang_z']), load(inp['intensity'])))
    turned = matinv(m, inp['front'])
    front, zero1 = normalise(turned)
    sine, cosine, aclass = sincos(angle)
    up = [sine, 0, cosine]
    side, zero2 = normalise(cross(front, up))
    up = cross(side, front)
    tiny = unordered_or_cmp(L(front[0]), ZERO) == 'equal' and unordered_or_cmp(L(front[1]), ZERO) == 'equal'
    if tiny:
        front = [TINY, TINY, front[2]]
    right, zero3 = normalise(cross(front, up))
    up = cross(right, front)
    hexw = lambda ws: ''.join('????????' if w is None else struct.pack('<I', w).hex() for w in ws)
    observed = {'front': hexw(front), 'up': hexw(up), 'rest_ok': True, 'guard_ok': True, 'table': table.hex(), 'flag': 1, 'calls': 1, 'trace': [inp['type'], inp['intensity']]}
    known = [w for w in front + up if w is not None]
    sq = [abs(load(b).val) ** 2 for b in turned if b is not None and load(b).kind == 'f']
    sum_sq = sum(sq) if len(sq) == 3 else None
    stats = {'family': inp['family'], 'flag0': inp['flag'] == 0, 'flag1': inp['flag'] == 1, 'type%d' % inp['type']: True, 'norm1_zero': zero1, 'norm2_zero': zero2, 'norm3_zero': zero3,
             'tiny_override': tiny, 'nan_out': any(is_float_bits_nan(w) for w in known), 'inf_out': any(is_inf(w) for w in known),
             'subnormal_out': any(((w >> 23) & 0xFF) == 0 and (w & 0x7FFFFF) for w in known), 'unknown': len(known) != 6,
             'angle_zero': aclass == 'zero', 'angle_huge': aclass == 'huge', 'angle_inf': aclass == 'inf', 'angle_imprecise': aclass == 'imprecise', 'angle_nan': aclass == 'nan',
             'angle_mid': aclass == 'normal' and abs(load(angle).val) >= 1024, 'angle_normal': aclass == 'normal',
             'ext_exp_hi': sum_sq is not None and sum_sq >= Fraction(2) ** 128 and not zero1 and all(((w >> 23) & 0xFF) != 0xFF for w in front if w is not None),
             'ext_exp_lo': sum_sq is not None and 0 < sum_sq < Fraction(2) ** -126 and not zero1,
             'intensity_zero': inp['intensity'] & 0x7FFFFFFF == 0, 'intensity_nan': is_float_bits_nan(inp['intensity']), 'active%d' % inp['active']: True,
             'mat_nonfinite': any(((w >> 23) & 0xFF) == 0xFF for w in inp['mat']), 'front_nonfinite': any(((w >> 23) & 0xFF) == 0xFF for w in inp['front']),
             }
    return observed, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines ('??' en front/up, None en trace)."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if k in ('front', 'up'):
            if not hex_wild_match(o, w):
                return False
        elif k == 'trace':
            if len(o) != len(w) or any(x is not None and x != y for x, y in zip(w, o)):
                return False
        elif o != w:
            return False
    return True


def f32(v):
    return struct.unpack('<f', struct.pack('<f', v))[0]


def bits(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


def flt(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def row(**kw):
    ident = [bits(1.0), 0, 0, 0, bits(1.0), 0, 0, 0, bits(1.0)]
    inp = {'family': 0, 'type': 2, 'intensity': bits(1.0), 'active': 1, 'front': [bits(0.5), bits(0.25), bits(0.75)], 'up': [0, 0, 0], 'ang_z': bits(0.01), 'mat': ident, 'flag': 1,
           'table_in': '5a' * LAYOUT['table_bytes']}
    inp.update(kw)
    return inp


def f32_chain(front, mat9, angz, inten):
    """Misma funcion con una cadena float32 por operacion (valida para valores tipicos, sin desbordes)."""
    mm = {idx: mat9[i] for i, idx in enumerate(MATRIX_WORDS)}
    A = f32(angz * inten)
    v = front
    t = [f32(f32(f32(mm[1] * v[1]) + f32(mm[2] * v[2])) + f32(v[0] * mm[0])),
         f32(f32(f32(mm[5] * v[1]) + f32(mm[4] * v[0])) + f32(mm[6] * v[2])),
         f32(f32(f32(mm[9] * v[1]) + f32(mm[8] * v[0])) + f32(mm[10] * v[2]))]

    def nrm(a):
        s = f32(f32(f32(a[0] * a[0]) + f32(a[1] * a[1])) + f32(a[2] * a[2]))
        if s <= 0:
            return [1.0, a[1], a[2]]
        r = f32(1.0 / f32(math.sqrt(s)))
        return [f32(r * a[0]), f32(r * a[1]), f32(r * a[2])]

    def crs(a, b):
        return [f32(f32(b[2] * a[1]) - f32(a[2] * b[1])), f32(f32(a[2] * b[0]) - f32(b[2] * a[0])), f32(f32(a[0] * b[1]) - f32(b[0] * a[1]))]
    fr = nrm(t)
    up = [f32(math.sin(A)), 0.0, f32(math.cos(A))]
    side = nrm(crs(fr, up))
    up = crs(side, fr)
    if fr[0] == 0 and fr[1] == 0:
        fr[0] = fr[1] = flt(TINY)
    right = nrm(crs(fr, up))
    up = crs(right, fr)
    return fr, up


def selftest():
    # caso a mano: identidad, front (0.5, 0.25, 0.75), angulo 0.01: coincide con la cadena float32
    inp = row()
    want, st = simulate(inp)
    fr, up = f32_chain([0.5, 0.25, 0.75], [flt(w) for w in inp['mat']], 0.01, 1.0)
    got = struct.unpack('<6I', bytes.fromhex(want['front'] + want['up']))
    assert got == tuple(bits(v) for v in fr + up), (got, [hex(bits(v)) for v in fr + up])
    assert want['flag'] == 1 and want['trace'] == [2, bits(1.0)] and want['table'] != inp['table_in']
    e = 2 * 0x94
    t = bytes.fromhex(want['table'])
    assert struct.unpack_from('<I', t, e + 8)[0] == bits(0.01) and struct.unpack_from('<I', t, e + 0x3C)[0] == bits(1.0) and struct.unpack_from('<I', t, e + 0x4C + 4)[0] == bits(1.0)
    assert t[e + 0x3C + 12:e + 0x3C + 16] == b'\x5a' * 4                          # la palabra de relleno (flags) no se toca
    # flag == 0: INIT sobre la tabla (0xB6ED40 = entrada 1, +0xC) y flag final 1
    want, st = simulate(row(flag=0))
    t = bytes.fromhex(want['table'])
    assert struct.unpack_from('<I', t, 0xB6ED40 - TABLE_ADDR)[0] == 0x3CA3D70A and struct.unpack_from('<I', t, 0xB6F014 - TABLE_ADDR)[0] == 0x3B449BA6 and st['flag0']
    assert len(INIT) == 65
    # front nulo con la identidad: el vector girado es nulo -> solo x = 1.0 (los ceros restantes se conservan); luego up/side se calculan con ese front
    want, st = simulate(row(front=[0, 0x80000000, 0]))
    assert st['norm1_zero'] and struct.unpack('<3I', bytes.fromhex(want['front']))[0] == 0x3F800000
    # front = (0, 0, 1), identidad, angulo 0: side = front x up = 0 -> Normalise pone x = 1 -> up = side x front; front.x == front.y == 0 -> TINY
    want, st = simulate(row(front=[0, 0, bits(1.0)], ang_z=0))
    assert st['norm2_zero'] and st['tiny_override'] and st['angle_zero']
    assert struct.unpack('<3I', bytes.fromhex(want['front'])) == (TINY, TINY, bits(1.0)), want['front']
    # intensidad NaN: angulo NaN -> seno y coseno NaN; el resto se propaga
    want, st = simulate(row(intensity=0x7FC00001))
    assert st['angle_nan'] and st['nan_out']
    # angulo infinito: indefinido; >= 2^63 deja el operando; 2^20..2^63 desconocido
    assert simulate(row(ang_z=0x7F800000))[1]['angle_inf'] and simulate(row(ang_z=bits(2.0 ** 70)))[1]['angle_huge'] and simulate(row(ang_z=bits(2.0 ** 40)))[1]['angle_imprecise']
    assert simulate(row(ang_z=bits(2.0 ** 40)))[1]['unknown']
    # exponente extendido: front enorme (2^100): la suma de cuadrados (2^200) desborda float pero no el x87; 1/sqrt = 2^-100
    want, st = simulate(row(front=[bits(2.0 ** 100), 0, 0]))
    assert st['ext_exp_hi'] and struct.unpack('<3I', bytes.fromhex(want['front']))[0] == bits(1.0), want['front']
    # coherencia con la cadena float32 en 3000 casos tipicos
    rng = random.Random(30)
    for _ in range(3000):
        fr0 = [f32(rng.uniform(-1, 1)) for _ in range(3)]
        mat9 = [f32(1 + rng.uniform(-.1, .1)), f32(rng.uniform(-.05, .05)), f32(rng.uniform(-.05, .05)), f32(rng.uniform(-.05, .05)), f32(1 + rng.uniform(-.1, .1)), f32(rng.uniform(-.05, .05)),
                f32(rng.uniform(-.05, .05)), f32(rng.uniform(-.05, .05)), f32(1 + rng.uniform(-.1, .1))]
        angz, inten = f32(rng.uniform(-.03, .03)), f32(rng.uniform(0, 3))
        want, st = simulate(row(front=[bits(v) for v in fr0], mat=[bits(v) for v in mat9], ang_z=bits(angz), intensity=bits(inten), flag=rng.randrange(2)))
        fr, up = f32_chain(fr0, mat9, angz, inten)
        got = struct.unpack('<6I', bytes.fromhex(want['front'] + want['up']))
        assert got == tuple(bits(v) for v in fr + up), (got, [hex(bits(v)) for v in fr + up])
    assert matches({'front': 'abcd', 'trace': [1, 2], 'x': 1}, {'front': 'ab??', 'trace': [1, None], 'x': 1})
    assert not matches({'front': 'abce', 'trace': [1, 2], 'x': 1}, {'front': 'ab??', 'trace': [1, 3], 'x': 1})
    print('ok')


if __name__ == '__main__':
    selftest()
