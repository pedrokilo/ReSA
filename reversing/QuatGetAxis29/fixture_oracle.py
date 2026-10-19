"""Oraculo independiente del banco QuatGetAxis29: CQuaternion::Get(RwV3d* axis, float* angle) (0x59C230).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido
(fadd/fsub/fmul/fdiv/fsqrt redondean al par a 24 bits; fstp dword redondea al rango float32). ECX = cuaternion {x,y,z,w}; [esp+4] = axis, [esp+8] = angle (`ret 8`):

  59C233 fld w ; fadd st,st                       -> X = 2w (exacto; el exponente extendido admite 2w mayor que el maximo float)
  59C238 call _CIacos (0x822380)                  -> ST0 = A (ver abajo)
  59C23D fld st ; fstp [angle]                    -> *angle = float(A)                                (PRIMERA escritura)
  59C249 fsin ; 59C24B fdivr [1.0]                -> I = 1 / sin(A) a 24 bits
  59C253 fld st ; fmul [q.x] ; fstp [axis.x] ; idem q.y -> axis.y ; fmul [q.z] ; fstp [axis.z]   (cada lectura de q ocurre DESPUES de las escrituras anteriores: las memorias se superponen)

_CIacos del EXE (0x822380; flc: FlcCrtAcos): NaN -> el mismo NaN y errno = 33; infinito o |X| > 1 -> indefinido real (0xFFC00000) y errno = 33; X = +1 -> fldz (A = +0, sin(A) = +0, I = +inf);
X = -1 -> fldpi de 64 bits (el seno de pi_64 lo calcula el hardware: eje DESCONOCIDO '??' y se exige EXE == DLL; el angulo es float(pi) = 0x40490FDB);
resto: A = fpatan(sqrt(r24(r24(1 + X) * r24(1 - X))), X) (todo a 24 bits salvo el fpatan, que da 64 bits; el CRT lo redondea a 53 bits al ver el inexacto), de modo que
sin(A) = s / sqrt(s^2 + X^2) con s = la raiz a 24 bits, e I = r24(sqrt(s^2 + X^2) / s). *angle = float(atan2(s, X)).
errno: el oraculo exige 33 en NaN/dominio (si [0xC9ABFC] == 0) y no exige nada en los demas casos (solo EXE == DLL); el banco apunta errno = 0x5A5A5A5A antes de cada lado.

NaN (solo silenciosos en las fixtures): una operacion con un unico NaN devuelve ese NaN tal cual; con dos NaN devuelve el de mayor significando; mismo significando y distinto signo ->
DESCONOCIDO (comodin '??'; solo EXE == DLL); operacion invalida (0 * inf, ...) sin NaN de entrada devuelve el indefinido real (0xFFC00000).
"""
import math
import random
import struct
from fractions import Fraction

CASES = 4096
NAME = 'CQuaternion::Get(RwV3d*, float*)'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x40, 0xC0
INDEFINITE = 0xFFC00000
ERRNO_SENTINEL = 0x5A5A5A5A
EDOM = 33


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


def sqrt_hp(p, bits=300):
    p = Fraction(p)
    n = p * (Fraction(2) ** (2 * bits))
    return Fraction(math.isqrt(n.numerator // n.denominator), 1 << bits)


def acos_chain(x):
    """x: V (X = 2w). -> (kind, angle_bits, I) con kind en 'nan' | 'domain' | 'plus1' | 'minus1' | 'normal'; I = V de 1 / sin(A)."""
    if x.kind == 'unk':
        return 'unk', None, V('unk')
    if x.kind == 'nan':
        return 'nan', x.bits | 0x400000, V('nan', bits=x.bits | 0x400000)
    if x.kind == 'inf' or abs(x.val) > 1:
        return 'domain', INDEFINITE, indefinite()
    if x.val == 1:
        return 'plus1', 0, V('inf', neg=False)
    if x.val == -1:
        return 'minus1', 0x40490FDB, V('unk')
    xv = x.val
    a = r24(1 + xv)
    b = r24(1 - xv)
    p = r24(a * b)
    s = fsqrt24(p)
    angle = struct.unpack('<I', struct.pack('<f', math.atan2(float(s), float(xv))))[0]
    hyp = sqrt_hp(s * s + xv * xv)
    return 'normal', angle, V('f', r24(hyp / s))


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    win = bytearray(bytes.fromhex(inp['win_in']))
    wild = set()
    q = list(inp['q'])
    qoff, aoff, goff = inp['qoff'], inp['aoff'], inp['goff']
    win[qoff:qoff + 16] = struct.pack('<4I', *q)

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

    w = rd(qoff + 12)
    x = add(w, w)
    kind, angle_bits, inv = acos_chain(x)
    wr(goff, angle_bits)
    results = []
    for i in range(3):
        v = rd(qoff + 4 * i)
        bits = store(mul(inv, v))
        results.append(bits)
        wr(aoff + 4 * i, bits)
    errno = None
    if kind in ('nan', 'domain'):
        errno = EDOM if inp['matherr'] == 0 else ERRNO_SENTINEL
    arena = bytearray([0xC3] * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    arena_hex = hexbytes(arena, {WIN_OFFSET + i for i in wild})
    qv = [load(b) for b in q]
    stats = {'family': inp['family'], 'kind_' + kind: True}
    stats['nan_in'] = any(v.is_nan() for v in qv)
    stats['inf_in'] = any(v.kind == 'inf' for v in qv)
    stats['finite_in'] = all(v.kind == 'f' for v in qv)
    stats['alias'] = (aoff < qoff + 16 and qoff < aoff + 12) or (goff < qoff + 16 and qoff < goff + 4) or (goff < aoff + 12 and aoff < goff + 4)
    known = [b for b in results + [angle_bits] if b is not None]
    stats['unknown'] = len(known) != 4 or len(wild) > 0
    stats['all_known'] = not stats['unknown']
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['indefinite_out'] = any(v == INDEFINITE for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['zero_out'] = any(v in (0, 0x80000000) for v in results if v is not None)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    stats['finite_out'] = all(((v >> 23) & 0xFF) != 0xFF for v in known)
    stats['errno_domain'] = errno is not None
    return {'arena': arena_hex, 'errno': errno, 'guard_ok': True}, stats


def expected(inp, case):
    return simulate(inp)


ULP_TOL = 2   # tolerancia del oraculo en los floats de salida (acos del CRT del EXE y fsin no se modelan al ulp); EXE == DLL sigue siendo estricto en bits


def _within_ulps(a_hex, b_hex, tol):
    import struct
    x = struct.unpack("<I", bytes.fromhex(a_hex))[0]
    y = struct.unpack("<I", bytes.fromhex(b_hex))[0]
    if (x >> 31) != (y >> 31) or ((x >> 23) & 0xFF) == 0xFF or ((y >> 23) & 0xFF) == 0xFF:
        return False
    return abs(x - y) <= tol


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines '??' (cadenas) y errno None (cualquiera)."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if k == 'errno':
            if w is not None and o != w:
                return False
        elif isinstance(w, str):
            if len(o) != len(w):
                return False
            for i in range(0, len(w), 8):
                ws, os_ = w[i:i + 8], o[i:i + 8]
                if ws == os_ or all(ws[j:j + 2] == '??' or ws[j:j + 2] == os_[j:j + 2] for j in range(0, len(ws), 2)):
                    continue
                if k == 'arena' and len(ws) == 8 and '?' not in ws and _within_ulps(ws, os_, ULP_TOL):
                    continue
                return False
        elif o != w:
            return False
    return True


def run_one(q, qoff=0, aoff=0x10, goff=0x20, matherr=0, win_in=None):
    inp = {'family': 0, 'q': list(q), 'qoff': qoff, 'aoff': aoff, 'goff': goff, 'win_in': win_in or '00' * WIN_BYTES, 'matherr': matherr}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'].replace('??', '00'))
    return want, arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES], st


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    one, half, quarter = 0x3F800000, 0x3F000000, 0x3E800000
    u = lambda win, off: struct.unpack_from('<I', win, off)[0]
    # w = 0: X = 0 -> A = pi/2 (angulo 0x3FC90FDB), I = 1, eje = xyz
    want, win, st = run_one([one, 0x40000000, 0xBF800000, 0])
    assert u(win, 0x20) == 0x3FC90FDB, hex(u(win, 0x20))
    assert struct.unpack_from('<3I', win, 0x10) == (one, 0x40000000, 0xBF800000)
    assert want['errno'] is None
    # w = 0,25: X = 0,5 -> angulo float(acos(0,5)) = 0x3F860A92; I ~ 1,1547005
    want, win, st = run_one([one, 0, 0, quarter])
    assert u(win, 0x20) == bits(f32(math.acos(0.5))) == 0x3F860A92, hex(u(win, 0x20))
    assert abs(flt(u(win, 0x10)) - 1.1547005) < 1e-6, flt(u(win, 0x10))
    # w = 0,75: X = 1,5 -> dominio: angulo e I indefinidos, errno = 33 (o el centinela si [0xC9ABFC] != 0)
    want, win, st = run_one([one, 0, 0, 0x3F400000])
    assert u(win, 0x20) == INDEFINITE and u(win, 0x10) == INDEFINITE and want['errno'] == EDOM
    assert run_one([one, 0, 0, 0x3F400000], matherr=1)[0]['errno'] == ERRNO_SENTINEL
    # w = 0,5: X = 1 -> A = +0, I = +inf: x = 1 -> +inf, 0 -> indefinido, -2 -> -inf
    want, win, st = run_one([one, 0, 0xC0000000, half])
    assert u(win, 0x20) == 0 and struct.unpack_from('<3I', win, 0x10) == (0x7F800000, INDEFINITE, 0xFF800000)
    # w = -0,5: X = -1 -> angulo 0x40490FDB, eje desconocido
    want, win, st = run_one([one, 0, 0, 0xBF000000])
    assert u(win, 0x20) == 0x40490FDB and want['arena'][2 * (WIN_OFFSET + 0x10):2 * (WIN_OFFSET + 0x1C)] == '??' * 12
    # w NaN: angulo y eje (x finito) = NaN de w; con otro NaN de mayor significando gana ese
    want, win, st = run_one([one, 0x7FC00009, 0x7FC00001, 0x7FC00005])
    assert u(win, 0x20) == 0x7FC00005 and struct.unpack_from('<3I', win, 0x10) == (0x7FC00005, 0x7FC00009, 0x7FC00005), struct.unpack_from('<3I', win, 0x10)
    assert want['errno'] == EDOM
    # w infinito: dominio
    want, win, st = run_one([one, 0, 0, 0x7F800000])
    assert u(win, 0x20) == INDEFINITE and want['errno'] == EDOM
    # w = -0 (X = -0): A = pi/2; I = 1; el eje conserva los signos
    want, win, st = run_one([0x80000000, 0, one, 0x80000000])
    assert u(win, 0x20) == 0x3FC90FDB and struct.unpack_from('<3I', win, 0x10) == (0x80000000, 0, one)
    # w enorme (2w mayor que el maximo float): dominio, no desborda
    want, win, st = run_one([one, 0, 0, 0x7F7FFFFF])
    assert u(win, 0x20) == INDEFINITE and want['errno'] == EDOM
    # coherencia con una cadena float32 por operacion (el doble redondeo es despreciable: 53 >= 2*24 + 2 en sqrt y producto)
    rng = random.Random(29)
    for _ in range(3000):
        w = f32(rng.uniform(-0.4999, 0.4999) * 2.0 ** rng.randint(-6, 0))
        xyz = [f32(rng.uniform(-3, 3)) for _ in range(3)]
        x = f32(w + w)
        a, b = f32(1 + x), f32(1 - x)
        s = f32(math.sqrt(f32(a * b)))
        A = math.atan2(s, x)
        inv = f32(1.0 / math.sin(A))
        want, win, st = run_one([bits(v) for v in xyz] + [bits(w)])
        assert u(win, 0x20) == bits(f32(A)), (w, hex(u(win, 0x20)), hex(bits(f32(A))))
        for i in range(3):
            assert u(win, 0x10 + 4 * i) == bits(f32(inv * xyz[i])), (w, xyz, i)
    # alias: el angulo (escrito primero) sustituye a q.x en la lectura (angulo sobre x)
    want, win, st = run_one([one, one, one, 0], qoff=0x10, aoff=0x24, goff=0x10)
    assert u(win, 0x10) == 0x3FC90FDB and u(win, 0x24) == 0x3FC90FDB, (hex(u(win, 0x10)), hex(u(win, 0x24)))
    # eje sobre el cuaternion (q en 0x10, eje en 0x0C..0x17): axis.x cae en 0x0C (fuera), axis.y sobre q.x (0x10), axis.z sobre q.y (0x14): z se lee tras escribirse y sobre q.y
    want, win, st = run_one([0x40000000, 0x40400000, 0x40800000, 0], qoff=0x10, aoff=0x0C, goff=0x30)
    assert struct.unpack_from('<4I', win, 0x0C) == (0x40000000, 0x40400000, 0x40800000, 0x40800000), struct.unpack_from('<4I', win, 0x0C)
    assert matches({'arena': 'abcdab', 'errno': 33, 'guard_ok': True}, {'arena': 'ab??ab', 'errno': None, 'guard_ok': True})
    assert not matches({'arena': 'abcdab', 'errno': 5, 'guard_ok': True}, {'arena': 'ab??ab', 'errno': 33, 'guard_ok': True})
    print('ok')


if __name__ == '__main__':
    selftest()
