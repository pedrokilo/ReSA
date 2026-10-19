"""Oraculo independiente del banco QuatMultiply29: CQuaternion::Multiply(const CQuaternion& a, const CQuaternion& b) (0x59C270).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido
(fadd/fsub/fmul redondean al par a 24 bits; fstp dword redondea al rango float32). ECX = salida o; EDX = &b ([esp+8]); EAX = &a ([esp+4]); `ret 8`. Las lecturas y escrituras se hacen en
este orden, sobre la MEMORIA (la salida puede solaparse con a o con b y los pasos 4-7 releen lo ya escrito):

  59C278 fld b.z ; fmul a.y ; fld a.z ; fmul b.y ; fsubp ; fstp o.x               o.x = r(b.z*a.y) - r(a.z*b.y)
  59C288 fld a.z ; fmul b.x ; fld a.x ; fmul b.z ; fsubp ; fstp o.y               o.y = r(a.z*b.x) - r(a.x*b.z)
  59C297 fld a.x ; fmul b.y ; fld b.x ; fmul a.y ; fsubp ; fstp o.z               o.z = r(a.x*b.y) - r(b.x*a.y)
  59C2A6 fld b.w ; fmul a.x ; fld a.w ; fmul b.x ; faddp ; fadd o.x ; fstp o.x    o.x = (r(b.w*a.x) + r(a.w*b.x)) + o.x   (o.x releido de memoria)
  59C2B6 fld a.w ; fmul b.y ; fld b.w ; fmul a.y ; faddp ; fadd o.y ; fstp o.y
  59C2CA fld a.z ; fmul b.w ; fld a.w ; fmul b.z ; faddp ; fadd o.z ; fstp o.z
  59C2DE fld a.w ; fmul b.w ; fld a.x ; fmul b.x ; fld a.z ; fmul b.z ; faddp ; fld a.y ; fmul b.y ; faddp ; fsubp ; fstp o.w
                                                                                  o.w = r(a.w*b.w) - ((r(a.x*b.x) + r(a.z*b.z)) + r(a.y*b.y))

Cada resultado intermedio se redondea a 24 bits (exponente extendido) y solo el fstp lo lleva al rango float32. Los NaN siguen las reglas del x87: un unico NaN pasa tal cual, con dos NaN
gana el de mayor significando; mismo significando y signos distintos -> DESCONOCIDO (comodin '??': ahi solo se exige EXE == DLL); operacion invalida sin NaN de entrada -> indefinido real
(0xFFC00000). Solo NaN silenciosos en las fixtures.
"""
import math
import random
import struct
from fractions import Fraction

CASES = 4096
NAME = 'CQuaternion::Multiply'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x60, 0xE0
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
    """a - b (un NaN no cambia de signo: se resuelve antes de negar)."""
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
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


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    win = bytearray(bytes.fromhex(inp['win_in']))
    wild = set()
    qa, qb = list(inp['qa']), list(inp['qb'])
    oo, ao, bo = inp['oo'], inp['ao'], inp['bo']
    win[ao:ao + 16] = struct.pack('<4I', *qa)
    win[bo:bo + 16] = struct.pack('<4I', *qb)               # si a y b se superponen gana b

    def rd(off):
        if any(i in wild for i in range(off, off + 4)):
            return V('unk')
        return load(struct.unpack_from('<I', win, off)[0])

    def wr(off, bits):
        for i in range(off, off + 4):
            wild.discard(i)
        if bits is None:
            wild.update(range(off, off + 4))
        else:
            win[off:off + 4] = struct.pack('<I', bits)

    A = lambda i: rd(ao + 4 * i)
    B = lambda i: rd(bo + 4 * i)
    O = lambda i: rd(oo + 4 * i)
    X, Y, Z, W = 0, 1, 2, 3
    outs = []

    def put(i, v):
        bits = store(v)
        outs.append(bits)
        wr(oo + 4 * i, bits)

    # 59C278..59C286
    t = mul(B(Z), A(Y)); u = mul(A(Z), B(Y)); put(X, sub(t, u))
    # 59C288..59C294
    t = mul(A(Z), B(X)); u = mul(A(X), B(Z)); put(Y, sub(t, u))
    # 59C297..59C2A3
    t = mul(A(X), B(Y)); u = mul(B(X), A(Y)); put(Z, sub(t, u))
    # 59C2A6..59C2B4
    t = mul(B(W), A(X)); u = mul(A(W), B(X)); s = add(t, u); put(X, add(s, O(X)))
    # 59C2B6..59C2C7
    t = mul(A(W), B(Y)); u = mul(B(W), A(Y)); s = add(t, u); put(Y, add(s, O(Y)))
    # 59C2CA..59C2DB
    t = mul(A(Z), B(W)); u = mul(A(W), B(Z)); s = add(t, u); put(Z, add(s, O(Z)))
    # 59C2DE..59C2FA
    p0 = mul(A(W), B(W)); p1 = mul(A(X), B(X)); p2 = mul(A(Z), B(Z)); s1 = add(p1, p2)
    p3 = mul(A(Y), B(Y)); s2 = add(s1, p3); put(W, sub(p0, s2))

    arena = bytearray([0xC3] * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    arena_hex = hexbytes(arena, {WIN_OFFSET + i for i in wild})
    qv = [load(b) for b in qa + qb]
    stats = {'family': inp['family']}
    stats['nan_in'] = any(v.is_nan() for v in qv)
    stats['inf_in'] = any(v.kind == 'inf' for v in qv)
    stats['finite_in'] = all(v.kind == 'f' for v in qv)
    ov = lambda p, q: p < q + 16 and q < p + 16
    stats['alias'] = ov(oo, ao) or ov(oo, bo) or ov(ao, bo)
    stats['out_eq_a'] = oo == ao and oo != bo
    stats['out_eq_b'] = oo == bo and oo != ao
    stats['all_eq'] = oo == ao == bo
    stats['a_eq_b'] = ao == bo and oo != ao
    stats['partial'] = stats['alias'] and not (oo == ao or oo == bo or ao == bo)
    known = [b for b in outs if b is not None]
    stats['unknown'] = len(known) != 7 or len(wild) > 0
    stats['all_known'] = not stats['unknown']
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['inf_from_finite'] = stats['finite_in'] and stats['inf_out']
    stats['indefinite_out'] = any(v == INDEFINITE for v in known)
    stats['zero_out'] = any(v in (0, 0x80000000) for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    stats['finite_out'] = all(((v >> 23) & 0xFF) != 0xFF for v in known)
    return {'arena': arena_hex, 'guard_ok': True}, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines '??' (cadenas)."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if isinstance(w, str):
            if len(o) != len(w):
                return False
            for i in range(0, len(w), 8):
                ws, os_ = w[i:i + 8], o[i:i + 8]
                if ws == os_ or all(ws[j:j + 2] == '??' or ws[j:j + 2] == os_[j:j + 2] for j in range(0, len(ws), 2)):
                    continue
                return False
        elif o != w:
            return False
    return True


def run_one(qa, qb, oo=0, ao=0x10, bo=0x20, win_in=None):
    inp = {'family': 0, 'qa': list(qa), 'qb': list(qb), 'oo': oo, 'ao': ao, 'bo': bo, 'win_in': win_in or '00' * WIN_BYTES}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'].replace('??', '00'))
    return want, arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES], st


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    u = lambda win, off: struct.unpack_from('<I', win, off)[0]
    one, two = 0x3F800000, 0x40000000
    # identidad (0,0,0,1) * q = q (disjuntos)
    qv = [bits(1.5), bits(-2.25), bits(0.375), bits(4.0)]
    want, win, st = run_one([0, 0, 0, one], qv)
    assert struct.unpack_from('<4I', win, 0) == tuple(qv), struct.unpack_from('<4I', win, 0)
    want, win, st = run_one(qv, [0, 0, 0, one])
    assert struct.unpack_from('<4I', win, 0) == tuple(qv)
    # i * j = k : (1,0,0,0)*(0,1,0,0) = (0,0,1,0)  (z = a.x*b.y - b.x*a.y)
    want, win, st = run_one([one, 0, 0, 0], [0, one, 0, 0])
    assert struct.unpack_from('<4I', win, 0) == (0, 0, one, 0), struct.unpack_from('<4I', win, 0)
    # i * i = -1: w = 0 - (1 + 0 + 0)
    want, win, st = run_one([one, 0, 0, 0], [one, 0, 0, 0])
    assert struct.unpack_from('<4I', win, 0) == (0, 0, 0, 0xBF800000), struct.unpack_from('<4I', win, 0)
    # x*y de la formula con la cadena float32 por operacion (el doble redondeo es despreciable: 53 >= 2*24 + 2)
    rng = random.Random(29)
    for _ in range(3000):
        a = [f32(rng.uniform(-3, 3) * 2.0 ** rng.randint(-5, 3)) for _ in range(4)]
        b = [f32(rng.uniform(-3, 3) * 2.0 ** rng.randint(-5, 3)) for _ in range(4)]
        ox = f32(f32(b[2] * a[1]) - f32(a[2] * b[1]))
        oy = f32(f32(a[2] * b[0]) - f32(a[0] * b[2]))
        oz = f32(f32(a[0] * b[1]) - f32(b[0] * a[1]))
        ox = f32(f32(f32(b[3] * a[0]) + f32(a[3] * b[0])) + ox)
        oy = f32(f32(f32(a[3] * b[1]) + f32(b[3] * a[1])) + oy)
        oz = f32(f32(f32(a[2] * b[3]) + f32(a[3] * b[2])) + oz)
        ow = f32(f32(a[3] * b[3]) - f32(f32(f32(a[0] * b[0]) + f32(a[2] * b[2])) + f32(a[1] * b[1])))
        want, win, st = run_one([bits(v) for v in a], [bits(v) for v in b])
        got = struct.unpack_from('<4I', win, 0)
        assert got == tuple(bits(v) for v in (ox, oy, oz, ow)), (a, b, [hex(g) for g in got])
    # alias salida == a: emulacion float32 secuencial sobre una memoria compartida (a y salida son la misma lista)
    def emu(m, oi, ai, bi):
        g = lambda base, k: m[base + k]
        def st(k, v): m[oi + k] = f32(v)
        st(0, f32(g(bi, 2) * g(ai, 1)) - f32(g(ai, 2) * g(bi, 1)))
        st(1, f32(g(ai, 2) * g(bi, 0)) - f32(g(ai, 0) * g(bi, 2)))
        st(2, f32(g(ai, 0) * g(bi, 1)) - f32(g(bi, 0) * g(ai, 1)))
        st(0, f32(f32(g(bi, 3) * g(ai, 0)) + f32(g(ai, 3) * g(bi, 0))) + g(oi, 0))
        st(1, f32(f32(g(ai, 3) * g(bi, 1)) + f32(g(bi, 3) * g(ai, 1))) + g(oi, 1))
        st(2, f32(f32(g(ai, 2) * g(bi, 3)) + f32(g(ai, 3) * g(bi, 2))) + g(oi, 2))
        st(3, f32(g(ai, 3) * g(bi, 3)) - f32(f32(f32(g(ai, 0) * g(bi, 0)) + f32(g(ai, 2) * g(bi, 2))) + f32(g(ai, 1) * g(bi, 1))))
    for oi, ai, bi in ((0, 0, 4), (0, 4, 0), (0, 0, 0), (4, 0, 4), (2, 0, 4), (4, 3, 7), (1, 5, 2)):
        for _ in range(200):
            vals = [f32(rng.uniform(-3, 3)) for _ in range(12)]
            m = list(vals)
            emu(m, oi, ai, bi)
            win_f = [0.0] * 24
            # preparacion del oraculo: a en ao luego b en bo (b gana); la ventana se modela en floats de 4 bytes
            for k in range(4):
                win_f[ai + k] = vals[k]
            for k in range(4):
                win_f[bi + k] = vals[4 + k]
            m2 = list(win_f)
            emu(m2, oi, ai, bi)
            qa = [bits(win_f[ai + k]) for k in range(4)]
            qb = [bits(win_f[bi + k]) for k in range(4)]
            want, win, st_ = run_one(qa, qb, oo=oi * 4, ao=ai * 4, bo=bi * 4)
            for k in range(4):
                assert u(win, (oi + k) * 4) == bits(m2[oi + k]), (oi, ai, bi, k)
    # salida == a == b (b sobreescribe a)
    qq = [bits(0.5), bits(-1.5), bits(2.0), bits(0.25)]
    want, win, st = run_one(qq, qq, oo=0x10, ao=0x10, bo=0x10)
    assert st['all_eq'] and not st['out_eq_a']
    # un unico NaN pasa tal cual (x=0, w=0 en o) y 0 * inf da el indefinido real
    want, win, st = run_one([0x7FC00009, one, one, one], [one, one, one, one])
    assert u(win, 0x00) == 0x7FC00009 and u(win, 0x0C) == 0x7FC00009, (hex(u(win, 0)), hex(u(win, 0x0C)))
    want, win, st = run_one([0, 0, 0, 0], [0x7F800000, 0, 0, 0])
    assert u(win, 0x0C) == INDEFINITE and st['indefinite_out'], hex(u(win, 0x0C))
    # el exponente extendido: 2^100*2^100 no desborda en el registro; (2^100*2^100) + (2^100 * -2^100) = 0 y el fstp da +0; el producto solo (w) sale +inf
    big = 0x71800000  # 2^100
    want, win, st = run_one([0, 0, 0, big], [0, 0, 0, big])
    assert u(win, 0x0C) == 0x7F800000 and st['inf_from_finite'], hex(u(win, 0x0C))
    want, win, st = run_one([big, 0, 0, big], [big, 0, 0, bits(-2.0 ** 100)])
    assert u(win, 0x00) == 0, hex(u(win, 0x00))
    assert matches({'arena': 'abcdab', 'guard_ok': True}, {'arena': 'ab??ab', 'guard_ok': True})
    assert not matches({'arena': 'abcdab', 'guard_ok': True}, {'arena': 'abcdac', 'guard_ok': True})
    print('ok')


if __name__ == '__main__':
    selftest()
