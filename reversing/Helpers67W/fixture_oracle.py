"""Oraculo del banco Helpers67W, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Una funcion (16384 casos): CWaterLevel::RenderSeaBedSegment 0x6E6870 (cdecl, 6 argumentos, `ret` del llamador), x87 con palabra de control de 24 bits (exponente extendido):
  n = [0xC4B950] (contador de vertices), m = [0xC4B954] (contador de indices); V = 0xC4D958 + 36 n (cuatro vertices de 36 bytes), I = 0xC4B958 + 2 m
  px0 = f32((x_exacto + minX) * 500 - 3000)   (fild + fadd: x entra como entero exacto en la x87, redondeo a 24 bits en la suma)
  py0 = f32((minY + y_exacto) * 500 - 3000);  py1 = f32((y_exacto + maxY) * 500 - 3000)
  xf  = f32(float(x));  px2 = f32((xf + maxX) * 500 - 3000)   (el segundo X sale del float guardado con fst en la pila: difiere del entero cuando |x| > 2^24)
  uMin = f32(minX*8), uMax = f32(maxX*8), vMin = f32(minY*8), vMax = f32(maxY*8)
  v0 = (px0, py0, -70, normal previa, 0xFF505050, uMin, vMin)   v1 = (px0, py1, -70, ..., uMin, vMax)   v2 = (px2, py0, -70, ..., uMax, vMin)   v3 = (px2, py1, -70, ..., uMax, vMax)
  indices = n, n+1, n+2, n+3, n+1, n+2 (words bajos);  [0xC4B950] = n + 4;  [0xC4B954] = m + 6.   Las normales (bytes 12..23 de cada vertice) no se escriben.

Autoprueba: `python -I fixture_oracle.py` (imprime «ok»).
"""
import struct
from fractions import Fraction

CASES = 16384
NAMES = ['CWaterLevel::RenderSeaBedSegment']
ADDRESS = [0x6E6870]
M32 = 0xFFFFFFFF
# Constantes de la region: DEBEN coincidir con Fixtures.hpp (fixture_selfcheck.py las compara con el volcado compilado).
LAYOUT = dict(region_address=0xC4B950, region_bytes=0x14008, idx_off=8, vtx_off=0x2008, vertex_bytes=36, guard_bytes=0x40)
VERTEX = 36
Z_BITS = 0xC28C0000
COLOR = 0xFF505050


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con exponente extendido
# Valores: Fraction (finito no nulo), ('z', signo), ('i', signo), ('n', signo, mantisa de 23 bits con el bit 22 puesto: NaN silencioso)
class Ambiguous(Exception):
    """dos NaN con el mismo significando y distinto signo: la x87 no define cual elige"""


DEFAULT_NAN = ('n', 1, 0x400000)


def from_bits(b):
    s = b >> 31
    e = (b >> 23) & 0xFF
    m = b & 0x7FFFFF
    if e == 0xFF:
        return ('n', s, m | 0x400000) if m else ('i', s)
    if e == 0:
        return ('z', s) if m == 0 else Fraction(-m if s else m, 1 << 149)
    v = Fraction(m | 0x800000) * (Fraction(2) ** (e - 150))
    return -v if s else v


def r24(x):
    """Redondeo a 24 bits de mantisa (mitad al par), exponente sin limite."""
    if x == 0:
        return x
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return s * n * ulp


def r24v(x):
    """r24 de un resultado exacto: cero exacto no pasa por aqui."""
    return r24(x)


def to_bits(v):
    """fstp dword: redondeo a float (mitad al par, subnormales, desborde a infinito)."""
    if isinstance(v, tuple):
        if v[0] == 'z':
            return v[1] << 31
        if v[0] == 'i':
            return (v[1] << 31) | 0x7F800000
        return (v[1] << 31) | 0x7F800000 | (v[2] & 0x7FFFFF)
    s = 1 if v < 0 else 0
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (max(e, -126) - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    r = n * ulp
    if r >= Fraction(2) ** 128:
        return (s << 31) | 0x7F800000
    if n == 0:
        return s << 31
    if r < Fraction(2) ** -126:
        return (s << 31) | n
    # normal: n*ulp = m * 2^(E-23), con 2^23 <= m < 2^24
    ee = r.numerator.bit_length() - r.denominator.bit_length()
    while Fraction(2) ** ee > r:
        ee -= 1
    while Fraction(2) ** (ee + 1) <= r:
        ee += 1
    mant = int(r / (Fraction(2) ** (ee - 23))) & 0x7FFFFF
    return (s << 31) | ((ee + 127) << 23) | mant


def f32(v):
    """valor tras guardarlo como float y volver a cargarlo"""
    return from_bits(to_bits(v))


def is_nan(a):
    return isinstance(a, tuple) and a[0] == 'n'


def is_inf(a):
    return isinstance(a, tuple) and a[0] == 'i'


def is_zero(a):
    return isinstance(a, tuple) and a[0] == 'z'


def pick_nan(a, b):
    if is_nan(a) and is_nan(b):
        if a[2] == b[2]:
            if a[1] != b[1]:
                raise Ambiguous
            return a
        return a if a[2] > b[2] else b
    return a if is_nan(a) else b


def sign_of(a):
    if isinstance(a, tuple):
        return a[1]
    return 1 if a < 0 else 0


def neg(a):
    if isinstance(a, tuple):
        return (a[0], 1 - a[1]) + a[2:]
    return -a


def add(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    if is_inf(a) and is_inf(b):
        return a if a[1] == b[1] else DEFAULT_NAN
    if is_inf(a):
        return a
    if is_inf(b):
        return b
    if is_zero(a) and is_zero(b):
        return a if a[1] == b[1] else ('z', 0)
    if is_zero(a):
        return r24(b)
    if is_zero(b):
        return r24(a)
    s = a + b
    if s == 0:
        return ('z', 0)
    return r24(s)


def sub(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    return add(a, neg(b))


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    s = sign_of(a) ^ sign_of(b)
    if is_inf(a) or is_inf(b):
        if is_zero(a) or is_zero(b):
            return DEFAULT_NAN
        return ('i', s)
    if is_zero(a) or is_zero(b):
        return ('z', s)
    return r24(a * b)


def div(a, b):
    """solo con divisor finito no nulo y dividendo finito"""
    if is_zero(a):
        return ('z', sign_of(a) ^ sign_of(b))
    return r24(a / b)


def sqrt24(a):
    if is_nan(a):
        return a
    if is_zero(a):
        return a
    if is_inf(a):
        return a if a[1] == 0 else DEFAULT_NAN
    if a < 0:
        return DEFAULT_NAN
    P = 90
    from math import isqrt
    N = a * (4 ** P)
    n = N.numerator // N.denominator
    s = isqrt(n)
    if Fraction(s * s) == N:
        return r24(Fraction(s, 2 ** P))
    return r24(Fraction(2 * s + 1, 2 ** (P + 1)))     # entre s y s+1: el bit pegajoso evita falsos empates


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


# ------------------------------------------------------------------------------------------------ la funcion
K_UV = Fraction(8)         # [0x859000]
K_BLOCK = Fraction(500)    # [0x858B58]
K_ORIGIN = Fraction(3000)  # [0x859A94]
REAL_SEGMENTS = {(0, 0x3F800000, 0, 0x3F800000), (0, 0x3D23D70A, 0, 0x3F800000), (0x3F75C28F, 0x3F800000, 0, 0x3F800000), (0, 0x3F800000, 0, 0x3D23D70A), (0, 0x3F800000, 0x3F75C28F, 0x3F800000)}


def int_value(v):
    return ('z', 0) if v == 0 else Fraction(v)


def place(a, b):
    """(a + b) * 500 - 3000 con redondeo a 24 bits en cada operacion, guardado como float"""
    return to_bits(sub(mul(add(a, b), K_BLOCK), K_ORIGIN))


def is_special(bits):
    return ((bits >> 23) & 0xFF) == 0xFF


def is_denormal(bits):
    return (bits & 0x7F800000) == 0 and (bits & 0x7FFFFF) != 0


def expected_segment(row, case=None):
    x, y = row['x'], row['y']
    a, b, c, d = (from_bits(row[k]) for k in ('min_x', 'max_x', 'min_y', 'max_y'))
    xe, ye = int_value(x), int_value(y)
    xf = f32(xe)                              # fild + fst dword [esp+0Ch]
    px0 = place(xe, a)
    py0 = place(c, ye)
    py1 = place(ye, d)
    px2 = place(xf, b)
    u_min, u_max = to_bits(mul(a, K_UV)), to_bits(mul(b, K_UV))
    v_min, v_max = to_bits(mul(c, K_UV)), to_bits(mul(d, K_UV))
    pre = bytes.fromhex(row['pre_vertices'])
    verts = bytearray()
    for k, (px, py, u, v) in enumerate(((px0, py0, u_min, v_min), (px0, py1, u_min, v_max), (px2, py0, u_max, v_min), (px2, py1, u_max, v_max))):
        normals = pre[k * VERTEX + 12:k * VERTEX + 24]
        verts += struct.pack('<3I', px, py, Z_BITS) + normals + struct.pack('<3I', COLOR, u, v)
    n, m = row['vtx'], row['idx']
    idx = struct.pack('<6H', *[(n + o) & 0xFFFF for o in (0, 1, 2, 3, 1, 2)])
    obs = dict(vertices=bytes(verts).hex(), indices=idx.hex(), vtx_after=(n + 4) & M32, idx_after=(m + 6) & M32, rest_ok=True, guard_ok=True)
    outs = (px0, px2, py0, py1, u_min, u_max, v_min, v_max)
    stats = dict(
        family=row['family'],
        out_nan=any(is_special(o) and o & 0x7FFFFF for o in outs),
        out_inf=any((o & 0x7FFFFFFF) == 0x7F800000 for o in outs),
        out_denormal=any(is_denormal(o) for o in outs),
        out_negzero=any(o == 0x80000000 for o in outs),
        out_zero=any(o == 0 for o in outs),
        x_inexact=xf != xe,
        asym_x=px2 != place(xe, b),
        min_x_special=is_special(row['min_x']), max_x_special=is_special(row['max_x']), min_y_special=is_special(row['min_y']), max_y_special=is_special(row['max_y']),
        uv_denormal=any(is_denormal(o) for o in (u_min, u_max, v_min, v_max)),
        cancel=any(o == to_bits(Fraction(-3000)) for o in (px0, px2, py0, py1)),
        vtx_zero=n == 0, vtx_max=n == 2044, idx_zero=m == 0, idx_max=m == 4090, vtx_high=n >= 1024, idx_high=m >= 2048,
        x_neg=x < 0, y_neg=y < 0, x_big=abs(x) >= 1 << 24, y_big=abs(y) >= 1 << 24, x_min=x == -(1 << 31), x_zero=x == 0,
        real_segment=(row['min_x'], row['max_x'], row['min_y'], row['max_y']) in REAL_SEGMENTS and row['family'] == 0)
    return obs, stats


FUNCTIONS = {NAMES[0]: expected_segment}


def expected(function, row, case=None):
    return FUNCTIONS[function](row, case)


def selftest():
    # Bloque x = 3, y = 4, segmento (0, 0.04, 0, 1): px0 = 3*500-3000 = -1500; px2 = (3 + float(0.04))*500 - 3000 ~ -1480
    row = dict(func=0, n=0, family=0, x=3, y=4, min_x=0, max_x=0x3D23D70A, min_y=0, max_y=0x3F800000, vtx=5, idx=7, pre_vertices='00' * 144, pre_indices='00' * 12)
    obs, st = expected_segment(row)
    v = struct.unpack('<36I', bytes.fromhex(obs['vertices']))
    assert fl(v[0]) == -1500.0 and fl(v[1]) == -1000.0 and v[2] == Z_BITS and v[6] == COLOR and v[7] == 0 and v[8] == 0
    assert fl(v[9]) == -1500.0 and fl(v[10]) == -500.0 and fl(v[16]) == 0.0 and fl(v[17]) == 8.0 and v[15] == COLOR
    assert abs(fl(v[18]) - -1480.0) < 1e-3 and fl(v[19]) == -1000.0 and fl(v[25]) == fl(0x3D23D70A) * 8
    assert obs['indices'] == struct.pack('<6H', 5, 6, 7, 8, 6, 7).hex() and obs['vtx_after'] == 9 and obs['idx_after'] == 13
    # float(x) para el segundo X: x = 2^24 + 1 no cabe en un float
    row = dict(row, x=(1 << 24) + 1, min_x=0x3F800000, max_x=0x3F800000, y=0)
    obs, st = expected_segment(row)
    v = struct.unpack('<36I', bytes.fromhex(obs['vertices']))
    assert st['x_inexact'] and st['asym_x'] and v[0] != v[18], (hex(v[0]), hex(v[18]))
    # NaN: pasa por todas las operaciones conservando la carga util
    row = dict(row, x=1, min_x=0x7FC12345, max_x=0x3F800000)
    obs, st = expected_segment(row)
    v = struct.unpack('<36I', bytes.fromhex(obs['vertices']))
    assert v[0] == 0x7FC12345 and v[7] == 0x7FC12345 and st['out_nan']
    # desborde a infinito y cancelacion exacta (1 + (-1) = +0 -> -3000)
    row = dict(row, min_x=0x7F7FFFFF, max_x=0xBF800000, x=1)
    obs, st = expected_segment(row)
    v = struct.unpack('<36I', bytes.fromhex(obs['vertices']))
    assert v[0] == 0x7F800000 and v[18] == to_bits(Fraction(-3000)) and st['out_inf'] and st['cancel']
    return 'ok'


if __name__ == '__main__':
    print(selftest())
