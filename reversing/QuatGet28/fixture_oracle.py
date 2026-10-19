"""Oraculo independiente del banco QuatGet28: CQuaternion::Get(RwMatrix*) const (0x59C080).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente
extendido: cada fadd/fsub/fmul se redondea (al par) a 24 bits de mantisa sin limite de exponente; fstp dword redondea ademas al rango float32 (subnormales de cuantum 2^-149,
desbordes a infinito). Las cargas fld dword son exactas. ST0 = q = {x,y,z,w} = [ecx], [ecx+4], [ecx+8], [ecx+0xC]; eax = RwMatrix* = [esp+0x24]:

  59C083 fld x ; fadd st,st -> 2x          59C08B fld y ; fadd st,st -> 2y          59C090 fld z ; fadd st,st -> 2z ; fstp [esp] = Z2 (float)    pila: 2y, 2x
  59C099 A = fl(2x * x)  -> [esp+1C]        B = fl(2y * x) -> [esp+4]                 C = fl(Z2 * x) -> [esp+0C]
         D = fl(2y * y)  -> [esp+18]        E = fl(Z2 * y) -> [esp+14]                F = fl(Z2 * z) -> [esp+8]
  59C0D2 fxch ; G = fl(2x * w) -> [esp+10] ; Y2W = 2y * w (en registro, SIN pasar por float) ; Z2W = Z2 * w (en registro)
  59C0E5 m0  [eax+00] = fl(1 - (F + D))     m4  [eax+10] = fl(B - Z2W)                m8  [eax+20] = fl(Y2W + C)
         m1  [eax+04] = fl(Z2W + B)         m5  [eax+14] = fl(1 - (F + A))            m9  [eax+24] = fl(E - G)
         m2  [eax+08] = fl(C - Y2W)         m6  [eax+18] = fl(G + E)                  m10 [eax+28] = fl(1 - (D + A))
  (1,0 = dword [0x858624]). Todas las lecturas del cuaternion preceden a la primera escritura (59C0F3). No se escribe nada mas (flags, relleno y posicion intactos).

NaN (solo silenciosos en las fixtures): una operacion con un unico NaN devuelve ese NaN tal cual (sin invertir el signo en fsub); con dos NaN devuelve el de mayor significando; si
tienen el mismo significando y distinto signo el resultado es DESCONOCIDO (comodin '??' en la salida: solo se exige EXE == DLL); operacion invalida (inf - inf, inf + (-inf), 0 * inf) sin
NaN de entrada devuelve el indefinido real (0xFFC00000). Un resultado desconocido contamina los resultados que dependen de el.
"""
import math
import random
import struct
from fractions import Fraction

CASES = 4096
NAME = 'CQuaternion::Get(RwMatrix*)'
NAMES = [NAME]
MAT_OFFSET, MAT_BYTES, MAT_ARENA = 0x40, 0x40, 0xC0
Q_OFFSET, Q_BYTES, Q_ARENA = 0x10, 0x10, 0x30
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


def store(v):
    """fstp dword: bits float32 (None = desconocido) y el valor releido (fld dword)."""
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
    """store + fld: valor tras pasar por memoria como float32."""
    b = store(v)
    return V('unk') if b is None else load(b)


ONE = load(0x3F800000)


def compute(q):
    """q = [x, y, z, w] (bits) -> lista de 9 salidas (bits o None) en el orden m0, m4, m8, m1, m5, m9, m2, m6, m10."""
    x, y, z, w = (load(b) for b in q)
    x2 = add(x, x)
    y2 = add(y, y)
    z2 = fl(add(z, z))
    a = fl(mul(x2, x))
    b = fl(mul(y2, x))
    c = fl(mul(z2, x))
    d = fl(mul(y2, y))
    e = fl(mul(z2, y))
    f = fl(mul(z2, z))
    g = fl(mul(x2, w))
    y2w = mul(y2, w)
    z2w = mul(z2, w)
    m0 = store(sub(ONE, add(f, d)))
    m4 = store(sub(b, z2w))
    m8 = store(add(y2w, c))
    m1 = store(add(z2w, b))
    m5 = store(sub(ONE, add(f, a)))
    m9 = store(sub(e, g))
    m2 = store(sub(c, y2w))
    m6 = store(add(g, e))
    m10 = store(sub(ONE, add(d, a)))
    return dict(m0=m0, m1=m1, m2=m2, m4=m4, m5=m5, m6=m6, m8=m8, m9=m9, m10=m10)


OFFSETS = dict(m0=0x00, m1=0x04, m2=0x08, m4=0x10, m5=0x14, m6=0x18, m8=0x20, m9=0x24, m10=0x28)   # bytes dentro de la RwMatrix


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def expected(inp, case):
    """-> (observed esperado con comodines '??' en mat_arena, stats)."""
    mat = bytearray(bytes.fromhex(inp['mat_in']))
    q = list(inp['q'])
    alias = inp['alias']
    if alias >= 0:
        mat[alias:alias + 16] = struct.pack('<4I', *q)
    outs = compute(q)
    wild = set()
    stats = {'family': inp['family'], 'alias': alias >= 0}
    vals = []
    for name, off in OFFSETS.items():
        bits = outs[name]
        if bits is None:
            wild.update(range(MAT_OFFSET + off, MAT_OFFSET + off + 4))
            vals.append(None)
        else:
            mat[off:off + 4] = struct.pack('<I', bits)
            vals.append(bits)
    # los comodines de bytes escritos se anulan en la parte alias que se solape (el valor final es el de la escritura desconocida: sigue siendo comodin)
    arena = bytearray([0xC3] * MAT_ARENA)
    arena[MAT_OFFSET:MAT_OFFSET + MAT_BYTES] = mat
    qarena = bytearray([0xB7] * Q_ARENA)
    if alias < 0:
        qarena[Q_OFFSET:Q_OFFSET + 16] = struct.pack('<4I', *q)
    qv = [load(b) for b in q]
    stats['nan_in'] = any(v.is_nan() for v in qv)
    stats['inf_in'] = any(v.kind == 'inf' for v in qv)
    stats['finite_in'] = all(v.kind == 'f' for v in qv)
    known = [v for v in vals if v is not None]
    stats['unknown'] = len(known) != len(vals)
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['indefinite_out'] = any(v == INDEFINITE for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['zero_out'] = any(v in (0, 0x80000000) for v in known)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    stats['overflow_out'] = stats['finite_in'] and stats['inf_out']
    stats['all_known'] = not stats['unknown']
    return {'mat_arena': hexbytes(arena, wild), 'q_arena': hexbytes(qarena), 'guard_ok': True}, stats


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines '??'."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if isinstance(w, str):
            if len(o) != len(w):
                return False
            for i in range(0, len(w), 2):
                if w[i:i + 2] != '??' and w[i:i + 2] != o[i:i + 2]:
                    return False
        elif o != w:
            return False
    return True


def selftest():
    one, zero, nzero, half = 0x3F800000, 0, 0x80000000, 0x3F000000
    # identidad (0,0,0,1): diagonal 1, resto +0
    o = compute([0, 0, 0, one])
    assert o == dict(m0=one, m1=0, m2=0, m4=0, m5=one, m6=0, m8=0, m9=0, m10=one), o
    # rotacion de 90 grados sobre z: z = w = 0,5 -> 2z = 1; F = 0,5; Z2W = 0,5
    o = compute([0, 0, half, half])
    assert o['m0'] == half and o['m5'] == half and o['m10'] == one, o
    assert o['m1'] == half and o['m4'] == 0xBF000000 and o['m2'] == 0 and o['m6'] == 0 and o['m8'] == 0 and o['m9'] == 0, o
    # ceros con signo: (-0,-0,-0,-0): todo producto es +0; 1 - (0 + 0) = 1; B - Z2W = +0 - +0 = +0; Z2W + B = +0
    o = compute([nzero] * 4)
    assert o['m0'] == one and o['m4'] == 0 and o['m1'] == 0 and o['m8'] == 0 and o['m2'] == 0 and o['m6'] == 0, o
    # (x=1, resto 0): 2x = 2; A = 2; m5 = 1 - (0 + 2) = -1; m10 = -1; m0 = 1 - (0 + 0) = 1
    o = compute([one, 0, 0, 0])
    assert o['m0'] == one and o['m5'] == 0xBF800000 and o['m10'] == 0xBF800000, o
    # infinito: x = inf, resto 0 -> 2x = inf; A = inf*inf = inf; B = 0 * inf = indefinido
    o = compute([0x7F800000, 0, 0, 0])
    assert o['m5'] == 0xFF800000 and o['m10'] == 0xFF800000 and o['m0'] == one, o
    assert o['m4'] == INDEFINITE and o['m1'] == INDEFINITE, o
    # NaN unico se propaga tal cual; dos NaN: gana el de mayor significando; empate con signos opuestos -> desconocido
    nan1, nan2 = 0x7FC00001, 0x7FC00005
    o = compute([nan1, one, 0, 0])
    assert o['m0'] is not None
    o = compute([nan1, nan2, 0, 0])
    assert o['m1'] == nan2 and o['m4'] == nan2, o            # B = 2y * x: gana nan2 (y); m4 = B - Z2W sigue siendo ese NaN
    o = compute([0x7FC00007, 0xFFC00007, 0, 0])
    assert o['m1'] is None and o['m4'] is None, o
    # coherencia con una cadena float32 por operacion (valida para operandos en rango: double no pierde precision, 53 >= 2*24 + 2)
    def f32(v):
        try:
            return struct.unpack('<f', struct.pack('<f', v))[0]
        except OverflowError:
            return math.copysign(math.inf, v)

    def bits(v):
        return struct.unpack('<I', struct.pack('<f', v))[0]
    rng = random.Random(28)
    for _ in range(3000):
        qf = [f32(rng.uniform(-3, 3) * 2.0 ** rng.randint(-8, 4)) for _ in range(4)]
        x, y, z, w = qf
        x2, y2, z2 = f32(x + x), f32(y + y), f32(z + z)
        a, b, c = f32(x2 * x), f32(y2 * x), f32(z2 * x)
        d, e, f = f32(y2 * y), f32(z2 * y), f32(z2 * z)
        g, y2w, z2w = f32(x2 * w), f32(y2 * w), f32(z2 * w)
        want = dict(m0=f32(1 - f32(f + d)), m4=f32(b - z2w), m8=f32(y2w + c), m1=f32(z2w + b), m5=f32(1 - f32(f + a)), m9=f32(e - g), m2=f32(c - y2w), m6=f32(g + e),
                    m10=f32(1 - f32(d + a)))
        got = compute([bits(v) for v in qf])
        for k, v in want.items():
            assert got[k] == bits(v), (qf, k, got[k], bits(v))
    # redondeo a float32 de subnormales y desbordes
    assert store(V('f', Fraction(1, 1 << 150), neg=False)) == 0                  # medio cuantum -> al par (0)
    assert store(V('f', Fraction(3, 1 << 150), neg=False)) == 2                  # 1,5 cuantum -> 2
    assert store(V('f', Fraction(2) ** 128, neg=False)) == 0x7F800000
    assert store(V('f', -Fraction(2) ** 127, neg=True)) == 0xFF000000
    # alias: la matriz esperada lleva el cuaternion superpuesto y se sobrescribe donde la salida escribe
    inp = {'family': 13, 'q': [one, 0, 0, 0], 'alias': 0x10, 'mat_in': '00' * 0x40, 'eax': 0, 'edx': 0, 'stack_seed': 0}
    want, st = expected(inp, 0)
    assert st['alias'] and want['mat_arena'][2 * (MAT_OFFSET + 0x14):2 * (MAT_OFFSET + 0x14) + 8] == '000080bf'
    assert want['mat_arena'][2 * (MAT_OFFSET + 0x10):2 * (MAT_OFFSET + 0x10) + 8] == '00000000'     # m4 = B - Z2W = +0 (el 1,0 de x se sobrescribe)
    assert matches({'mat_arena': 'ab' * 4, 'q_arena': 'cd', 'guard_ok': True}, {'mat_arena': 'ab??ab', 'q_arena': 'cd', 'guard_ok': True}) is False
    assert matches({'mat_arena': 'abcdab', 'q_arena': 'cd', 'guard_ok': True}, {'mat_arena': 'ab??ab', 'q_arena': 'cd', 'guard_ok': True}) is True
    print('ok')


if __name__ == '__main__':
    selftest()
