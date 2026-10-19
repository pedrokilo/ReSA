"""Oraculo independiente del banco QuatSlerpT29: CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float t) (0x59C630).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido
(fadd/fsub/fmul/fdiv/fsqrt redondean al par a 24 bits; fstp dword redondea al rango float32; fsin da el seno SIN redondear). ECX = destino; [esp+4] = from, [esp+8] = to, [esp+0xC] = t (`ret 0Ch`).
Dos etapas, como el EXE (0x59C630 llama a 0x4D00E0 y luego a 0x59C300):

 Etapa 1, CalcTheta (0x4D00E0; los dos punteros de salida apuntan a los huecos de pila de los argumentos from/to, que la funcion ya copio a EBX/EDI):
  fld from.w ; fmul to.w ; fld from.z ; fmul to.z ; faddp ; fld from.y ; fmul to.y ; faddp ; fld from.x ; fmul to.x ; faddp      dot = ((w*w' + z*z') + y*y') + x*x'   (24 bits cada operacion)
  fst [tmp] (float) ; fcomp 1.0 ; "test ah,41h ; jne": SOLO si dot (sin redondear a float) > 1 se guarda 1.0f en tmp (NaN no recorta)     ; fld [tmp] ; call _CIacos (0x822380)
  fld st ; fstp [theta] ; fcom 0.0 ; "test ah,44h ; jnp": A == 0 (paridad impar) -> descarta A y escribe invSin = +0 ; si no (incluido NaN): fsin ; fdivr 1.0 ; fstp [invSin]
  _CIacos del EXE (flc: FlcCrtAcos): NaN -> el mismo NaN y errno = 33; infinito o |X| > 1 -> indefinido real (0xFFC00000) y errno = 33; X = +1 -> fldz (theta = +0, invSin = +0 por la rama de cero);
  X = -1 -> fldpi de 64 bits (theta = float(pi) = 0x40490FDB; invSin = 1/sin(pi_64): DESCONOCIDO '??', se exige EXE == DLL);
  resto: A = fpatan(sqrt(r24(r24(1 + X) * r24(1 - X))), X) (el CRT lo redondea a 53 bits), sin(A) = s / sqrt(s^2 + X^2) con s la raiz a 24 bits, invSin = r24(sqrt(s^2 + X^2) / s).
 Etapa 2, 0x59C300 con (from, to, theta, invSin, t) y destino ECX:
  theta == 0 (fcomp 0.0, "test ah,44h ; jp"; NaN no entra): copia `to` (cuatro mov de 32 bits, en orden x, y, z, w) al destino.
  theta > 0x3FC90FDB (float pi/2; NaN no entra): thp = fstp-float(0x40490FDB - theta) ; a = fsin(r24(r24(1 - t) * thp)) * invSin ; b = -(fsin(r24(thp * t)) * invSin)
  si no: a = fsin(r24(r24(1 - t) * theta)) * invSin ; b = fsin(r24(theta * t)) * invSin          (a y b quedan en registros, redondeados a 24 bits por el fmul final)
  x = a*from.x + b*to.x (lee from.x, luego to.x) ; y, z, w = b*to + a*from (lee to antes que from) ; cada componente se escribe (fstp dword) ANTES de leer la siguiente: si los
  bloques se superponen, las lecturas posteriores ven lo ya escrito (la simulacion de memoria es secuencial).
errno: el oraculo exige 33 en NaN/dominio (si [0xC9ABFC] == 0); en el resto no exige nada (EXE == DLL); el banco apunta errno = 0x5A5A5A5A antes de cada lado.

NaN (solo silenciosos en las fixtures): una operacion con un unico NaN devuelve ese NaN tal cual; con dos NaN devuelve el de mayor significando; mismo significando y distinto signo ->
DESCONOCIDO (comodin '??'; solo EXE == DLL); operacion invalida (0 * inf, ...) sin NaN de entrada devuelve el indefinido real (0xFFC00000); fchs invierte tambien el signo de un NaN.
No se modela fsin de un argumento infinito o de modulo >= 2^20 (el resultado queda desconocido).
Tolerancia: los floats de salida se aceptan a 2 ulp (acos del CRT del EXE y fsin no se modelan al ulp, ver QuatGetAxis29); la comparacion EXE == DLL es estricta en bits.
"""
import math
import random
import struct
from fractions import Fraction

CASES = 4096
NAME = 'CQuaternion::Slerp(const CQuaternion&, const CQuaternion&, float)'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x60, 0xE0
INDEFINITE = 0xFFC00000
ERRNO_SENTINEL = 0x5A5A5A5A
EDOM = 33
HALF_PI_F = 0x3FC90FDB   # [0x858FE4]
PI_F = 0x40490FDB        # [0x858CB8]


class V:
    """Valor x87: kind 'f' finito (val Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits float32 silencioso), 'unk' (NaN/valor desconocido)."""
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
    """Operando negado para fsub (un NaN se propaga sin cambiar)."""
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return V('inf', neg=not v.neg)
    if v.val == 0:
        return V('f', Fraction(0), neg=not v.neg)
    return V('f', -v.val, neg=v.val > 0)


def fchs(v):
    """fchs: invierte el signo, tambien el de un NaN."""
    if v.kind == 'unk':
        return v
    if v.kind == 'nan':
        return V('nan', bits=v.bits ^ 0x80000000)
    return neg(v)


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
    """x: V (el float tmp). -> (kind, angle_bits, I) con kind en 'nan' | 'domain' | 'plus1' | 'minus1' | 'normal' | 'unk'; I = V de 1 / sin(A)."""
    if x.kind == 'unk':
        return 'unk', None, V('unk')
    if x.kind == 'nan':
        return 'nan', x.bits | 0x400000, V('nan', bits=x.bits | 0x400000)
    if x.kind == 'inf' or abs(x.val) > 1:
        return 'domain', INDEFINITE, indefinite()
    if x.val == 1:
        return 'plus1', 0, V('inf', neg=False)
    if x.val == -1:
        return 'minus1', PI_F, V('unk')
    xv = x.val
    a = r24(1 + xv)
    b = r24(1 - xv)
    p = r24(a * b)
    s = fsqrt24(p)
    angle = struct.unpack('<I', struct.pack('<f', math.atan2(float(s), float(xv))))[0]
    hyp = sqrt_hp(s * s + xv * xv)
    return 'normal', angle, V('f', r24(hyp / s))


# ---------------------------------------------------------------------------------------------------------- seno de precision alta (punto fijo de 400 bits)
_PREC = 400
_SC = 1 << _PREC


def _atan_inv(x):
    x2 = x * x
    term = _SC // x
    total = term
    n = 1
    sign = -1
    while term:
        term //= x2
        n += 2
        total += sign * (term // n)
        sign = -sign
    return total


_PI = 16 * _atan_inv(5) - 4 * _atan_inv(239)
_TWO_PI = 2 * _PI


def sin_real(x):
    """sin(x) de un racional (|x| < 2^20), como Fraction con error absoluto < 2^-390."""
    x = Fraction(x)
    if abs(x) < Fraction(1, 1 << 60):
        return x
    xf = (x.numerator * _SC) // x.denominator
    k = (xf + _PI) // _TWO_PI
    r = xf - k * _TWO_PI
    r2 = (r * r) // _SC
    term = r
    total = r
    n = 1
    while term:
        term = -((term * r2) // _SC) // ((n + 1) * (n + 2))
        n += 2
        total += term
    return Fraction(total, _SC)


def sinv(v):
    """fsin sobre un V."""
    if v.kind == 'unk':
        return v
    if v.kind == 'nan':
        return v
    if v.kind == 'inf':
        return V('unk')                                   # fsin(inf): no modelado
    if v.val == 0:
        return v
    if abs(v.val) >= (1 << 20):
        return V('unk')                                   # reduccion de argumento enorme: no modelada
    s = sin_real(v.val)
    return V('f', s, neg=s < 0)


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    win = bytearray(bytes.fromhex(inp['win_in']))
    wild = set()
    fo, to, oo = inp['fo'], inp['to'], inp['oo']
    win[fo:fo + 16] = struct.pack('<4I', *inp['a'])
    win[to:to + 16] = struct.pack('<4I', *inp['b'])           # si se solapan gana `to`

    def rdraw(off):
        if any(i in wild for i in range(off, off + 4)):
            return None
        return struct.unpack_from('<I', win, off)[0]

    def rd(off):
        bits = rdraw(off)
        return V('unk') if bits is None else load(bits)

    def wr(off, bits):
        for i in range(off, off + 4):
            wild.discard(i)
        if bits is None:
            wild.update(range(off, off + 4))
        else:
            win[off:off + 4] = struct.pack('<I', bits)

    f_in = [rd(fo + 4 * i) for i in range(4)]
    g_in = [rd(to + 4 * i) for i in range(4)]
    tv = load(inp['t'])
    # ---- etapa 1: CalcTheta
    dot = mul(f_in[3], g_in[3])
    dot = add(dot, mul(f_in[2], g_in[2]))
    dot = add(dot, mul(f_in[1], g_in[1]))
    dot = add(dot, mul(f_in[0], g_in[0]))
    clamp = (dot.kind == 'f' and dot.val > 1) or (dot.kind == 'inf' and not dot.neg)
    tmp = None if dot.kind == 'unk' else store(dot)
    if clamp:
        tmp = 0x3F800000
    xv = V('unk') if tmp is None else load(tmp)
    kind, theta_bits, inv = acos_chain(xv)
    if kind == 'plus1':
        inv_bits = 0                                     # rama A == 0: invSin = +0
    elif kind == 'unk':
        inv_bits = None
    else:
        inv_bits = store(inv)
    errno = None
    if kind in ('nan', 'domain'):
        errno = EDOM if inp['matherr'] == 0 else ERRNO_SENTINEL
    # ---- etapa 2: 0x59C300
    th = V('unk') if theta_bits is None else load(theta_bits)
    iv = V('unk') if inv_bits is None else load(inv_bits)
    copy = th.kind == 'f' and th.val == 0
    flip = False
    if copy:
        for i in range(4):
            wr(oo + 4 * i, rdraw(to + 4 * i))
    else:
        one_minus_t = sub(ONE, tv)
        if th.kind == 'f' and th.val > load(HALF_PI_F).val:
            flip = True
            thp = fl(sub(load(PI_F), th))
            a = mul(sinv(mul(one_minus_t, thp)), iv)
            b = fchs(mul(sinv(mul(thp, tv)), iv))
        else:
            a = mul(sinv(mul(one_minus_t, th)), iv)
            b = mul(sinv(mul(th, tv)), iv)
        for i in range(4):
            fv = rd(fo + 4 * i)
            gv = rd(to + 4 * i)
            res = add(mul(a, fv), mul(b, gv)) if i == 0 else add(mul(b, gv), mul(a, fv))
            wr(oo + 4 * i, store(res))
    arena = bytearray([0xC3] * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    arena_hex = hexbytes(arena, {WIN_OFFSET + i for i in wild})
    outs = [rdraw(oo + 4 * i) for i in range(4)]
    known = [b for b in outs if b is not None]
    ins = f_in + g_in + [tv]
    stats = {'family': inp['family'], 'kind_' + kind: True}
    stats['copy'] = copy
    stats['flip'] = flip
    stats['clamp'] = clamp
    stats['nan_in'] = any(v.is_nan() for v in ins)
    stats['inf_in'] = any(v.kind == 'inf' for v in ins)
    stats['finite_in'] = all(v.kind == 'f' for v in ins)
    stats['alias_out'] = (oo < fo + 16 and fo < oo + 16) or (oo < to + 16 and to < oo + 16)
    stats['alias_inputs'] = fo < to + 16 and to < fo + 16
    stats['out_eq_from'] = oo == fo
    stats['out_eq_to'] = oo == to
    stats['unknown'] = len(known) != 4 or len(wild) > 0
    stats['all_known'] = not stats['unknown']
    stats['theta_half_pi'] = theta_bits == HALF_PI_F
    stats['theta_small'] = theta_bits is not None and 0 < theta_bits < 0x3E800000
    stats['t_unit'] = tv.kind == 'f' and 0 <= tv.val <= 1
    stats['t_outside'] = tv.kind == 'f' and (tv.val < 0 or tv.val > 1)
    stats['t_nan'] = tv.is_nan()
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['indefinite_out'] = any(v == INDEFINITE for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['zero_out'] = any(v in (0, 0x80000000) for v in known)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    stats['finite_out'] = len(known) == 4 and all(((v >> 23) & 0xFF) != 0xFF for v in known)
    stats['errno_domain'] = errno is not None
    return {'arena': arena_hex, 'errno': errno, 'guard_ok': True}, stats


def expected(inp, case):
    return simulate(inp)


ULP_TOL = 2   # tolerancia del oraculo en los floats de salida (acos del CRT del EXE y fsin no se modelan al ulp); EXE == DLL sigue siendo estricto en bits


def _within_ulps(a_hex, b_hex, tol):
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


def run_one(a, b, t, fo=0x00, to=0x10, oo=0x20, matherr=0, win_in=None):
    inp = {'family': 0, 'a': list(a), 'b': list(b), 't': t, 'fo': fo, 'to': to, 'oo': oo, 'win_in': win_in or '00' * WIN_BYTES, 'matherr': matherr}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'].replace('??', '00'))
    return want, arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES], st


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    one, half = 0x3F800000, 0x3F000000
    u = lambda win, off: struct.unpack_from('<I', win, off)[0]
    ident = [0, 0, 0, one]
    xaxis = [one, 0, 0, 0]
    # dot = 1 exacto -> acos(1) = +0 -> invSin = +0, theta == 0: copia `to` palabra a palabra (aunque sea un patron raro)
    want, win, st = run_one(ident, ident, 0x3F000000)
    assert struct.unpack_from('<4I', win, 0x20) == (0, 0, 0, one) and st['copy'] and st['kind_plus1'] and want['errno'] is None
    # dot > 1 (recorte a 1.0f) -> tambien copia `to`
    want, win, st = run_one([0, 0, 0, 0x40000000], [0, 0, 0, 0x40000000], 0x3F000000)
    assert struct.unpack_from('<4I', win, 0x20) == (0, 0, 0, 0x40000000) and st['copy'] and st['clamp']
    # ortogonales: theta = pi/2 (float 0x3FC90FDB, no es > pi/2), invSin = 1; t = 0.5 -> a = b = sin(pi/4) = 0.70710677
    want, win, st = run_one(ident, xaxis, 0x3F000000)
    assert st['theta_half_pi'] and not st['flip'] and not st['copy']
    s45 = f32(math.sin(f32(f32(0.5) * flt(HALF_PI_F))))
    assert u(win, 0x20) == bits(f32(s45 * 0.0 + s45 * 1.0)), hex(u(win, 0x20))      # x = a*0 + b*1
    assert u(win, 0x2C) == bits(s45), hex(u(win, 0x2C))                                # w = b*0 + a*1
    # t = 0 -> a = sin(theta)*inv = 1, b = sin(0)*inv = 0: out = from; t = 1 -> a = sin(0) = 0 (con signo), b = 1: out = to
    want, win, st = run_one(ident, xaxis, 0)
    assert struct.unpack_from('<4I', win, 0x20) == (0, 0, 0, one), struct.unpack_from('<4I', win, 0x20)
    want, win, st = run_one(ident, xaxis, one)
    assert struct.unpack_from('<4I', win, 0x20) == (one, 0, 0, 0), struct.unpack_from('<4I', win, 0x20)
    # dot < -1 (dominio): NaN indefinido en theta e invSin, errno = 33; todo el destino es NaN (0xFFC00000 donde from/to no aportan otro NaN)
    want, win, st = run_one([0, 0, 0, 0xBF800000], [0, 0, 0, 0x40000000], 0x3F000000)
    assert st['kind_domain'] and want['errno'] == EDOM and u(win, 0x2C) == INDEFINITE
    assert run_one([0, 0, 0, 0xBF800000], [0, 0, 0, 0x40000000], 0x3F000000, matherr=1)[0]['errno'] == ERRNO_SENTINEL
    # dot = -1 exacto: fldpi de 64 bits; el destino es desconocido
    want, win, st = run_one(ident, [0, 0, 0, 0xBF800000], 0x3F000000)
    assert st['kind_minus1'] and not st['all_known'] and want['errno'] is None
    # coherencia con una cadena float32 por operacion (el doble redondeo es despreciable: 53 >= 2*24 + 2 en sqrt/producto y los senos se evaluan en doble)
    rng = random.Random(29)
    n_flip = 0
    for _ in range(3000):
        fq = [f32(rng.uniform(-0.7, 0.7)) for _ in range(4)]
        gq = [f32(rng.uniform(-0.7, 0.7)) for _ in range(4)]
        t = f32(rng.uniform(-0.5, 1.5))
        dot = f32(f32(f32(f32(fq[3] * gq[3]) + f32(fq[2] * gq[2])) + f32(fq[1] * gq[1])) + f32(fq[0] * gq[0]))
        x = f32(1.0) if dot > 1 else dot
        if abs(x) > 1:
            continue
        if x == 1:
            continue
        s = f32(math.sqrt(f32(f32(1 + x) * f32(1 - x))))
        A = math.atan2(s, x)
        theta = f32(A)
        inv = f32(1.0 / math.sin(A))
        if theta > flt(HALF_PI_F):
            n_flip += 1
            tp = f32(flt(PI_F) - theta)
            a = f32(math.sin(f32(f32(1 - t) * tp)) * inv)
            b = -f32(math.sin(f32(tp * t)) * inv)
        else:
            a = f32(math.sin(f32(f32(1 - t) * theta)) * inv)
            b = f32(math.sin(f32(theta * t)) * inv)
        want, win, st = run_one([bits(v) for v in fq], [bits(v) for v in gq], bits(t))
        exp = [f32(f32(a * fq[0]) + f32(b * gq[0]))] + [f32(f32(b * gq[i]) + f32(a * fq[i])) for i in (1, 2, 3)]
        for i in range(4):
            got = u(win, 0x20 + 4 * i)
            assert abs(got - bits(exp[i])) <= 2 and (got >> 31) == (bits(exp[i]) >> 31) or got == bits(exp[i]), (fq, gq, t, i, hex(got), hex(bits(exp[i])))
    assert n_flip > 500, n_flip
    # solape: destino == from. x se escribe antes de leer y: los cuatro valores usan a y b (registros) y los from.y/z/w originales (solo se pisa from.x al final de x)
    want, win, st = run_one(ident, xaxis, 0x3F000000, fo=0x10, to=0x30, oo=0x10)
    assert st['out_eq_from']
    # destino == to: tras escribir x, to.y/z/w se leen intactos pero to.x ya no se vuelve a leer
    want, win, st = run_one(ident, xaxis, 0x3F000000, fo=0x30, to=0x10, oo=0x10)
    assert st['out_eq_to']
    assert matches({'arena': 'abcdab', 'errno': 33, 'guard_ok': True}, {'arena': 'ab??ab', 'errno': None, 'guard_ok': True})
    assert not matches({'arena': 'abcdab', 'errno': 5, 'guard_ok': True}, {'arena': 'ab??ab', 'errno': 33, 'guard_ok': True})
    # el seno de precision alta coincide con math.sin a doble precision
    for v in (Fraction(1, 3), Fraction(7, 2), Fraction(-100003, 7), Fraction(12345, 1)):
        assert abs(float(sin_real(v)) - math.sin(float(v))) < 1e-12, v
    print('ok')


if __name__ == '__main__':
    selftest()
