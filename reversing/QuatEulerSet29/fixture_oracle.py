"""Oraculo independiente del banco QuatEulerSet29: CQuaternion::Set(float x, float y, float z) (0x59C530).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido
(fadd/fsub/fmul redondean al par a 24 bits; fstp dword redondea al rango float32). ECX = cuaternion {x,y,z,w}; [esp+4..0xC] = angulos x, y, z por valor (`ret 0Ch`):

  59C533 fld [x] ; fmul 0.5 (0x858B8C) ; fld st ; fcos          -> hx = x/2 ; cx = cos(hx) (sin redondear)
  59C541 fld [y] ; fmul 0.5 ; fld st ; fcos                     -> hy ; cy
  59C54F fld [z] ; fmul 0.5 ; fld st ; fcos                     -> hz ; cz
  59C55D fstp [cz_f] ; fxch ; fsin ; fstp [sx_f] ; fxch ; fsin ; fstp [sy_f] ; fxch ; fsin ; fstp [sz_f]   -> cz, sx, sy, sz guardados como float; cx y cy quedan en la pila
  59C579 fxch ; fld st ; fmul st,st(2) ; fstp [cycx]  -> cycx = f(cy*cx)
  59C583 fld [sy] ; fmul [sx] ; fstp [sysx]            -> sysx = f(sy*sx)
  59C58F fld [sx] ; fmul st,st(1) ; fstp [sxcy]       -> sxcy = f(sx*cy) ; fstp st ; fmul [sy] -> cxsy = r24(cx*sy) en registro
  59C59F w = f( r24(sysx*sz) + r24(cycx*cz) )         -> [ecx+0Ch]
  59C5B4 x = f( r24(cycx*sz) - r24(sysx*cz) )         -> [ecx]
  59C5C8 y = f( r24(sz*cxsy) + r24(sxcy*cz) )         -> [ecx+4]
  59C5DB z = f( r24(cxsy*cz) - r24(sxcy*sz) )         -> [ecx+8]

fsin/fcos: se modelan con math.sin/math.cos (doble precision) para |hx| < 2^26; el hardware reduce con un pi de 66 bits y el modelo es fiable al ulp de 24 bits (un empate de redondeo a
24 bits tendria probabilidad ~2^-29 por operacion; la comparacion EXE == DLL es estricta en bits y el oraculo admite 2 ulp). Para 2^26 <= |h| < 2^63 el seno/coseno del hardware dependeria de
la reduccion de 66 bits: valor DESCONOCIDO ('??', solo se exige EXE == DLL). Para |h| >= 2^63 fsin/fcos dejan el operando intacto (C2 = 1): sin(h) = cos(h) = h. NaN -> el mismo NaN;
infinito -> indefinido real (0xFFC00000).

NaN (solo silenciosos en las fixtures): una operacion con un unico NaN devuelve ese NaN tal cual; con dos NaN devuelve el de mayor significando; mismo significando y distinto signo ->
DESCONOCIDO (comodin '??'; solo EXE == DLL); operacion invalida (0 * inf, inf - inf) sin NaN de entrada devuelve el indefinido real (0xFFC00000).
"""
import math
import random
import struct
from fractions import Fraction

CASES = 4096
NAME = 'CQuaternion::Set(float, float, float)'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x40, 0xC0
INDEFINITE = 0xFFC00000
TWO26 = 2.0 ** 26
TWO63 = 2.0 ** 63


class V:
    """Valor x87: kind 'f' finito (val Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits float32 silencioso), 'unk' (NaN de carga util/signo desconocidos o valor no modelado)."""
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
    """fstp dword + fld dword: valor tras pasar por memoria como float32."""
    b = store(v)
    return V('unk') if b is None else load(b)


HALF = load(0x3F000000)


def trig(v, fn):
    """fsin / fcos (fn = math.sin / math.cos) del hardware sobre un valor de registro."""
    if v.kind == 'unk':
        return V('unk')
    if v.kind == 'nan':
        return v
    if v.kind == 'inf':
        return indefinite()
    h = float(v.val)                 # exacto: v.val es un racional de 24 bits (medio angulo de un float)
    if abs(h) >= TWO63:
        return v                     # C2 = 1: operando intacto
    if abs(h) >= TWO26:
        return V('unk')              # reduccion de 66 bits del hardware: no modelada
    if h == 0 and fn is math.sin:
        return v                     # sin(+-0) = +-0
    r = fn(h)
    return V('f', Fraction(r), neg=r < 0)


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    win = bytearray(bytes.fromhex(inp['win_in']))
    qoff = inp['qoff']
    ang = [load(b) for b in inp['angles']]
    hx, hy, hz = (mul(a, HALF) for a in ang)
    cx = trig(hx, math.cos)
    cy = trig(hy, math.cos)
    cz = fl(trig(hz, math.cos))
    sx = fl(trig(hx, math.sin))
    sy = fl(trig(hy, math.sin))
    sz = fl(trig(hz, math.sin))
    cycx = fl(mul(cy, cx))
    sysx = fl(mul(sy, sx))
    sxcy = fl(mul(sx, cy))
    cxsy = mul(cx, sy)
    w = store(add(mul(sysx, sz), mul(cycx, cz)))
    x = store(sub(mul(cycx, sz), mul(sysx, cz)))
    y = store(add(mul(sz, cxsy), mul(sxcy, cz)))
    z = store(sub(mul(cxsy, cz), mul(sxcy, sz)))
    outs = [x, y, z, w]
    wild = set()
    for i, bits in enumerate(outs):
        if bits is None:
            wild.update(range(qoff + 4 * i, qoff + 4 * i + 4))
        else:
            win[qoff + 4 * i:qoff + 4 * i + 4] = struct.pack('<I', bits)
    arena = bytearray([0xC3] * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    arena_hex = hexbytes(arena, {WIN_OFFSET + i for i in wild})
    known = [b for b in outs if b is not None]
    stats = {'family': inp['family']}
    stats['nan_in'] = any(v.is_nan() for v in ang)
    stats['inf_in'] = any(v.kind == 'inf' for v in ang)
    stats['finite_in'] = all(v.kind == 'f' for v in ang)
    stats['huge_in'] = any(v.kind == 'f' and abs(v.val) >= 2 ** 64 for v in ang)
    stats['mid_in'] = any(v.kind == 'f' and 2 ** 27 <= abs(v.val) < 2 ** 64 for v in ang)
    stats['zero_in'] = any(v.is_zero() for v in ang)
    stats['unaligned'] = qoff % 4 != 0
    stats['unknown'] = len(known) != 4
    stats['all_known'] = not stats['unknown']
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['indefinite_out'] = any(v == INDEFINITE for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['zero_out'] = any(v in (0, 0x80000000) for v in known)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    stats['finite_out'] = all(((v >> 23) & 0xFF) != 0xFF for v in known) and len(known) == 4
    return {'arena': arena_hex, 'guard_ok': True}, stats


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def expected(inp, case):
    return simulate(inp)


ULP_TOL = 2   # tolerancia del oraculo en los floats de salida (fsin/fcos del hardware no se modelan al ulp); EXE == DLL sigue siendo estricto en bits


def _within_ulps(a_hex, b_hex, tol):
    x = struct.unpack("<I", bytes.fromhex(a_hex))[0]
    y = struct.unpack("<I", bytes.fromhex(b_hex))[0]
    if (x >> 31) != (y >> 31) or ((x >> 23) & 0xFF) == 0xFF or ((y >> 23) & 0xFF) == 0xFF:
        return False
    return abs(x - y) <= tol


def matches(obs, want, qoff=None):
    """Compara lo observado con lo esperado: comodines '??' (cadenas) y, si se da qoff, tolerancia de ULP_TOL ulp en cada float escrito (sin comodin en su palabra)."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if not isinstance(w, str):
            if o != w:
                return False
            continue
        if len(o) != len(w):
            return False
        base = None if qoff is None else 2 * (WIN_OFFSET + qoff)
        for j in range(0, len(w), 2):
            if w[j:j + 2] in ('??', o[j:j + 2]):
                continue
            if base is None or not (base <= j < base + 32):
                return False
            i = (j - base) // 8
            ws, os_ = w[base + 8 * i:base + 8 * i + 8], o[base + 8 * i:base + 8 * i + 8]
            if '?' in ws or not _within_ulps(ws, os_, ULP_TOL):
                return False
    return True


def run_one(angles, qoff=0x10, win_in=None):
    inp = {'family': 0, 'angles': list(angles), 'qoff': qoff, 'win_in': win_in or '00' * WIN_BYTES}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'].replace('??', '00'))
    return want, arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES], st


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    one = 0x3F800000
    u = lambda win, off: struct.unpack_from('<I', win, off)[0]
    # angulos 0: (x, y, z, w) = (0, 0, 0, 1)
    want, win, st = run_one([0, 0, 0])
    assert struct.unpack_from('<4I', win, 0x10) == (0, 0, 0, one), struct.unpack_from('<4I', win, 0x10)
    # solo x = pi: q = (0, sin(pi/2), 0, cos(pi/2)) (el EXE escribe sen(ax/2) en el segundo componente) con los productos de 24 bits; w ~ cos(f32(pi)/2) ~ -4,37e-8
    want, win, st = run_one([0x40490FDB, 0, 0])
    assert abs(flt(u(win, 0x14)) - 1.0) < 1e-6 and flt(u(win, 0x1C)) < 0 and abs(flt(u(win, 0x1C))) < 1e-6, [hex(v) for v in struct.unpack_from('<4I', win, 0x10)]
    # solo z = pi/2: el EXE escribe sen(az/2) en el primer componente
    want, win, st = run_one([0, 0, 0x3FC90FDB])
    assert abs(flt(u(win, 0x10)) - math.sin(math.pi / 4)) < 1e-6 and abs(flt(u(win, 0x1C)) - math.cos(math.pi / 4)) < 1e-6
    # NaN en x: todo el cuaternion es NaN (sin/cos de NaN = NaN y los productos lo propagan)
    want, win, st = run_one([0x7FC00005, one, one])
    assert all(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in struct.unpack_from('<4I', win, 0x10)), [hex(v) for v in struct.unpack_from('<4I', win, 0x10)]
    # infinito en y: indefinido real en las cuatro componentes (sin/cos(inf) = indefinido)
    want, win, st = run_one([0, 0x7F800000, 0])
    assert struct.unpack_from('<4I', win, 0x10) == (INDEFINITE,) * 4, [hex(v) for v in struct.unpack_from('<4I', win, 0x10)]
    # angulo enorme (>= 2^64): fsin/fcos dejan el operando; con z = 2^64 (h = 2^63): sz = cz = 2^63, productos gigantes pero finitos en float? 2^63*... w = cycx*cz = 2^63
    want, win, st = run_one([0, 0, 0x5F800000])
    assert (flt(u(win, 0x10)), flt(u(win, 0x14)), flt(u(win, 0x18)), flt(u(win, 0x1C))) == (2.0 ** 63, 0.0, 0.0, 2.0 ** 63), [hex(v) for v in struct.unpack_from('<4I', win, 0x10)]
    # cobertura de la desalineacion: el cuaternion puede empezar en un byte cualquiera
    want, win, st = run_one([one, one, one], qoff=0x0D)
    assert struct.unpack_from('<4I', win, 0x0D)[3] != 0
    # coherencia con una cadena float32 (un redondeo a float por operacion, seno/coseno en doble): el doble redondeo es despreciable
    rng = random.Random(29)
    for _ in range(3000):
        a = [f32(rng.uniform(-7, 7) * 2.0 ** rng.randint(-8, 0)) for _ in range(3)]
        h = [f32(v * 0.5) for v in a]
        cx, cy, cz = (math.cos(v) for v in h)
        czf = f32(cz)
        sxf, syf, szf = (f32(math.sin(v)) for v in h)
        cycx = f32(cy * cx)
        sysx = f32(syf * sxf)
        sxcy = f32(sxf * cy)
        cxsy = f32(cx * syf)
        w = f32(f32(sysx * szf) + f32(cycx * czf))
        x = f32(f32(cycx * szf) - f32(sysx * czf))
        y = f32(f32(szf * cxsy) + f32(sxcy * czf))
        z = f32(f32(cxsy * czf) - f32(sxcy * szf))
        want, win, st = run_one([bits(v) for v in a])
        got = struct.unpack_from('<4I', win, 0x10)
        for g, e in zip(got, (x, y, z, w)):
            assert abs(flt(g) - e) <= 4 * 2.0 ** -23 * max(abs(e), 1e-30) or abs(flt(g) - e) < 1e-30, (a, [flt(v) for v in got], (x, y, z, w))
    assert matches({'arena': 'abcdab', 'guard_ok': True}, {'arena': 'ab??ab', 'guard_ok': True})
    assert not matches({'arena': 'abcdab', 'guard_ok': True}, {'arena': 'ab??ac', 'guard_ok': True})
    print('ok')


if __name__ == '__main__':
    selftest()
