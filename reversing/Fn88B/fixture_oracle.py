"""Oraculo del banco Fn88B, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Una funcion (16384 casos): CWaterLevel::RenderSeaBedSegmentNear 0x6E6A10 (cdecl, 6 argumentos, `ret` del llamador), x87 con palabra de control de 24 bits (exponente extendido).
Version subdividida de RenderSeaBedSegment (0x6E6870): reparte el segmento en nx * ny celdas y anade al bufer temporal de agua cuatro vertices (36 bytes) y seis indices por celda.
  dx = maxX - minX (24 bits; guardado como float: dxf);  nx = max(1, ftol(dx * 4))   (ftol = _ftol2 del EXE, 0x821B40: trunca; fuera de rango o NaN da 0x80000000)
  dy = maxY - minY;  dyf = float(dy);  ny = max(1, ftol(dy * 4))
  n = [0xC4B950] (contador de vertices), m = [0xC4B954] (contador de indices); V = 0xC4D958 + 36 n, I = 0xC4B958 + 2 m
  para i en 0..nx-1:  x0 = minX + (i * dxf) / nx;  x1 = minX + ((i + 1) * dxf) / nx;  x1f = float(x1)    (cada operacion a 24 bits; nx entra como entero exacto)
     u0 = float(8 * x0);  X0 = (x_exacto + x0) * 500 - 3000 (queda en la pila x87, sin redondear a float);  u1 = 8 * x1f;  X1 = (float(x) + x1f) * 500 - 3000
     para j en 0..ny-1:  y0 = minY + (j * dyf) / float(ny); y1 = minY + ((j + 1) * dyf) / float(ny); y0f = float(y0), y1f = float(y1)
        Y0 = (y_exacto + y0f) * 500 - 3000;  Y1 = (y_exacto + y1f) * 500 - 3000;  v0 = 8 * y0f;  v1 = 8 * y1f
        v[n]   = (X0, Y0, -70, normal previa, 0xFF505050, u0, v0)    v[n+1] = (X0, Y1, -70, ..., u0, v1)
        v[n+2] = (X1, Y0, -70, ..., u1, v0)                          v[n+3] = (X1, Y1, -70, ..., u1, v1)         (cada uno guardado como float)
        indices n, n+1, n+2, n+3, n+1, n+2 (words bajos);  n += 4;  m += 6
  al terminar [0xC4B950] = n y [0xC4B954] = m.   Las normales (bytes 12..23 de cada vertice) no se escriben.
El banco registra, en vez de los bytes, el FNV-1a de los vertices e indices escritos (el cuerpo de los casos grandes ocuparia demasiado en el registro) y los dos primeros vertices en hexadecimal.

Autoprueba: `python -I fixture_oracle.py` (imprime «ok»).
"""
import struct
from fractions import Fraction

CASES = 16384
NAMES = ['CWaterLevel::RenderSeaBedSegmentNear']
ADDRESS = [0x6E6A10]
M32 = 0xFFFFFFFF
# Constantes de la region: DEBEN coincidir con Fixtures.hpp (fixture_selfcheck.py las compara con el volcado compilado).
LAYOUT = dict(region_address=0xC4B950, region_bytes=0x14008, idx_off=8, vtx_off=0x2008, vertex_bytes=36, guard_bytes=0x40)
VERTEX = 36
Z_BITS = 0xC28C0000
COLOR = 0xFF505050
MAX_CELLS = 64


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


def fnv32(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def region_bytes(mode, seed, start, count):
    """bytes [start, start+count) de la imagen previa de la region (FillRegion de Fixtures.hpp); los contadores (0..7) los pone BuildRegionRef."""
    if mode == 1:
        return bytes(count)
    if mode == 2:
        return b'\xff' * count
    if mode == 3:
        return b'\xcd' * count
    if mode == 4:
        return b'\xa5' * count
    out = bytearray()
    first = start // 4
    last = (start + count + 3) // 4
    for w in range(first, last):
        out += struct.pack('<I', rnd(seed, w))
    off = start - first * 4
    return bytes(out[off:off + count])



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
K_QUARTER = Fraction(4)    # [0x858B90]
REAL_SEGMENTS = {(0, 0x3F800000, 0, 0x3F800000), (0, 0x3D23D70A, 0, 0x3F800000), (0x3F75C28F, 0x3F800000, 0, 0x3F800000), (0, 0x3F800000, 0, 0x3D23D70A), (0, 0x3F800000, 0x3F75C28F, 0x3F800000)}


def int_value(v):
    return ('z', 0) if v == 0 else Fraction(v)


def div2(a, b):
    """a / b con b finito y no nulo (nx, float(ny)); a de cualquier clase"""
    if is_nan(a):
        return a
    if is_inf(a):
        return ('i', a[1] ^ (1 if b < 0 else 0))
    if is_zero(a):
        return ('z', a[1] ^ (1 if b < 0 else 0))
    return r24(a / b)


def ftol(a):
    """_ftol2 del EXE (0x821B40): trunca a 64 bits (fistp qword + correccion) y el llamador usa EAX = 32 bits bajos con signo; NaN, infinito o |valor| >= 2^63 dan 0x8000000000000000 -> 0"""
    if isinstance(a, tuple):
        return 0
    n = a.numerator // a.denominator if a >= 0 else -((-a.numerator) // a.denominator)
    if abs(n) >= 1 << 63:
        return 0
    low = n & 0xFFFFFFFF
    return low - (1 << 32) if low >= 1 << 31 else low


def ftol_indefinite(a):
    """True si _ftol2 devuelve el entero indefinido (NaN, infinito o |valor| >= 2^63)"""
    if isinstance(a, tuple):
        return a[0] in ('n', 'i')
    n = a.numerator // a.denominator if a >= 0 else -((-a.numerator) // a.denominator)
    return abs(n) >= 1 << 63


def cells(width):
    """max(1, ftol(width * 4)) con width ya redondeado a 24 bits"""
    n = ftol(mul(width, K_QUARTER))
    return n if n >= 1 else 1


def place(a, b):
    """(a + b) * 500 - 3000 con redondeo a 24 bits en cada operacion (valor de registro, sin guardar como float)"""
    return sub(mul(add(a, b), K_BLOCK), K_ORIGIN)


def is_special(bits):
    return ((bits >> 23) & 0xFF) == 0xFF


def is_denormal(bits):
    return (bits & 0x7F800000) == 0 and (bits & 0x7FFFFF) != 0


def expected_segment(row, case=None):
    x, y = row['x'], row['y']
    a, b, c, d = (from_bits(row[k]) for k in ('min_x', 'max_x', 'min_y', 'max_y'))
    xe, ye = int_value(x), int_value(y)
    xf = f32(xe)
    dxr = sub(b, a)
    dyr = sub(d, c)
    dxf, dyf = f32(dxr), f32(dyr)
    nx, ny = cells(dxr), cells(dyr)
    nxe = Fraction(nx)
    nyf = f32(Fraction(ny))
    n, m = row['vtx'], row['idx']
    verts = bytearray()
    idx = bytearray()
    outs = []
    base_v = LAYOUT['vtx_off'] + 36 * n
    total = 4 * nx * ny
    pre = region_bytes(row['mode'], row['seed'], base_v, 36 * total)
    cell = 0
    for i in range(nx):
        x0 = add(div2(mul(int_value(i), dxf), nxe), a)
        x1 = add(div2(mul(int_value(i + 1), dxf), nxe), a)
        x1f = f32(x1)
        u0 = to_bits(mul(K_UV, x0))
        X0 = place(xe, x0)
        u1 = mul(x1f, K_UV)
        X1 = place(xf, x1f)
        for j in range(ny):
            y0f = f32(add(div2(mul(int_value(j), dyf), nyf), c))
            y1f = f32(add(div2(mul(int_value(j + 1), dyf), nyf), c))
            v0 = mul(y0f, K_UV)
            Y0 = place(ye, y0f)
            v1 = mul(y1f, K_UV)
            Y1 = place(ye, y1f)
            quad = ((X0, Y0, u0, to_bits(v0)), (X0, Y1, u0, to_bits(v1)), (X1, Y0, to_bits(u1), to_bits(v0)), (X1, Y1, to_bits(u1), to_bits(v1)))
            for k, (px, py, u, v) in enumerate(quad):
                o = 36 * (4 * cell + k)
                normals = pre[o + 12:o + 24]
                verts += struct.pack('<3I', to_bits(px), to_bits(py), Z_BITS) + normals + struct.pack('<3I', COLOR, u, v)
                outs += [to_bits(px), to_bits(py), u, v]
            nn = (n + 4 * cell) & M32
            idx += struct.pack('<6H', *[(nn + o) & 0xFFFF for o in (0, 1, 2, 3, 1, 2)])
            cell += 1
    verts = bytes(verts)
    idx = bytes(idx)
    obs = dict(vertices_fnv=fnv32(verts), indices_fnv=fnv32(idx), first_vertices=verts[:72].hex(), vtx_after=(n + total) & M32, idx_after=(m + 6 * nx * ny) & M32, rest_ok=True, guard_ok=True)
    wide_x = ftol(mul(dxr, K_QUARTER))
    wide_y = ftol(mul(dyr, K_QUARTER))
    stats = dict(
        family=row['family'],
        out_nan=any(is_special(o) and o & 0x7FFFFF for o in outs),
        out_inf=any((o & 0x7FFFFFFF) == 0x7F800000 for o in outs),
        out_denormal=any(is_denormal(o) for o in outs),
        out_negzero=any(o == 0x80000000 for o in outs),
        out_zero=any(o == 0 for o in outs),
        x_inexact=xf != xe,
        min_x_special=is_special(row['min_x']), max_x_special=is_special(row['max_x']), min_y_special=is_special(row['min_y']), max_y_special=is_special(row['max_y']),
        cancel=any(o == to_bits(Fraction(-3000)) for o in outs),
        cells_1=nx * ny == 1, cells_multi_x=nx > 1, cells_multi_y=ny > 1, cells_many=nx * ny >= 9, cells_4x4=nx >= 4 and ny >= 4, cells_ge32=nx * ny >= 32,
        nx_clamped=wide_x < 1, ny_clamped=wide_y < 1,
        nx_indefinite=ftol_indefinite(mul(dxr, K_QUARTER)), ny_indefinite=ftol_indefinite(mul(dyr, K_QUARTER)),
        vtx_zero=n == 0, vtx_high=n >= 1024, idx_zero=m == 0, idx_high=m >= 2048, vtx_fill=n + total == 2048, idx_fill=m + 6 * nx * ny == 4096,
        x_neg=x < 0, y_neg=y < 0, x_big=abs(x) >= 1 << 24, y_big=abs(y) >= 1 << 24, x_zero=x == 0,
        real_segment=(row['min_x'], row['max_x'], row['min_y'], row['max_y']) in REAL_SEGMENTS and row['family'] == 0)
    return obs, stats


FUNCTIONS = {NAMES[0]: expected_segment}


def expected(function, row, case=None):
    return FUNCTIONS[function](row, case)


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def selftest():
    # Bloque x = 3, y = 4, segmento (0, 1, 0, 1): 4 x 4 celdas; X0 de la primera celda = 3 * 500 - 3000 = -1500
    row = dict(func=0, n=0, family=0, x=3, y=4, min_x=0, max_x=0x3F800000, min_y=0, max_y=0x3F800000, vtx=5, idx=7, mode=1, seed=0)
    obs, st = expected_segment(row)
    assert obs['vtx_after'] == 5 + 64 and obs['idx_after'] == 7 + 96 and st['cells_4x4'] and st['cells_many']
    v = struct.unpack('<18I', bytes.fromhex(obs['first_vertices']))
    assert fl(v[0]) == -1500.0 and fl(v[1]) == -1000.0 and v[2] == Z_BITS and v[6] == COLOR and v[7] == 0 and v[8] == 0
    assert fl(v[9]) == -1500.0 and fl(v[10]) == -875.0 and fl(v[16]) == 0.0 and fl(v[17]) == 2.0
    # segmento de borde (0, 0.04, 0, 1): una celda en X (0.04 * 4 = 0.16) y cuatro en Y
    row = dict(row, max_x=0x3D23D70A)
    obs, st = expected_segment(row)
    assert obs['vtx_after'] == 5 + 16 and st['cells_multi_y'] and not st['cells_multi_x'] and st['cells_1'] is False
    # ancho negativo o NaN: una celda
    row = dict(row, min_x=0x3F800000, max_x=0, min_y=0x7FC12345)
    obs, st = expected_segment(row)
    assert obs['vtx_after'] == 5 + 4 and st['nx_clamped'] and st['out_nan'] and st['cells_1']
    # 4 * 2^63 no cabe en int64 (indefinido -> EAX = 0 -> una celda)
    row = dict(row, min_x=0, max_x=0x5F000000, min_y=0, max_y=0x3F800000)
    obs, st = expected_segment(row)
    assert st['nx_indefinite'] and obs['vtx_after'] == 5 + 16
    return 'ok'


if __name__ == '__main__':
    print(selftest())
