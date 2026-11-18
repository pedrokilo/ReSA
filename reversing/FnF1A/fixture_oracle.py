"""Oraculo independiente (Python) de FnF1A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 10 funciones del lote F1A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x617FC0  IKChainManager_c::Constructor: eh_vector_constructor_iterator(this, 0x58, 0x20, 0x617850, 0x617860); 0x617850 -> 0x4A8DB0: [elem] = 0, [elem+4] = 0 (nada mas); 0x4A8DD0 sobre this+0xB00 y this+0xB0C
            (tres dwords a cero cada una); eax = this. Marco SEH (fs:[0]) que se restaura.
   1  0x618030  IKChainManager_c::Destructor: 0x4A8DE0 (ret) sobre this+0xB0C y this+0xB00; eh_vector_destructor_iterator (0x821E02) con 0x617860 (-> 0x4A8DC0, ret): no escribe nada
   2  0x6E3EB0  WaterCreatureManager_c::Constructor: igual con 0x80 elementos de 0x34 bytes (constructor 0x6E3E30: 0x4A8DB0 y [elem+8] = 0) y listas en this+0x1A00 y this+0x1A0C; eax = this
   3  0x6E3F20  WaterCreatureManager_c::Destructor: no escribe nada
   4  0x6E67A0  IsPointInCameraDepthRange(punto, radio, banderin): camara = [0xC1703C]; near = [camara+0x80]; far = [camara+0x84]; matriz local (CMatrix) unida a la RwMatrix de camara+0x20;
            z = (((m[10]*p.z) + m[2]*p.x) + m[6]*p.y) + m[14] con cada operacion x87 a 24 bits (exponente extendido) y el resultado guardado como float; ST0 = near - radio (24 bits);
            si no (ST0 < z) (comparacion ordenada) devuelve 0; si z < far devuelve 1; si no devuelve (byte bajo del banderin) == 0. Resultado en AL
   5  0x509230  BugstarWriteScreenShotEntry: ver bug_expected()
   6  0x81E720  DecompressDxt1(ancho, alto, destino, origen): bloques ancho/4 x alto/4 (division truncada), 8 bytes por bloque, destino con separacion de ancho pixeles; devuelve 1 si algun pixel uso el indice 3 en tres colores
   7  0x81EA70  DecompressDxt3: 16 bytes por bloque (alfa de 4 bits en los 8 primeros, color en los 8 ultimos; cuarto color transparente 0x00FFFF00 en tres colores)
   8  0x81EDC0  DecompressDxt5: igual con el alfa de 8 valores del bloque de alfa DXT5 (0x81F0C0)
   9  0x81F0C0  DecodeDxt5AlphaBlock(destino, bloque, separacion, mascara): pixel = (pixel & mascara) | (alfa << 24)

Los falsos anotan: [0x508B80, ECX (etiqueta de pila), entero, hash del texto, longitud]; [0x508D30, ECX, hash de clave, hash de tipo, hash del texto, longitud]; [0x509190, ECX (etiqueta de arena), 0, hash de los datos, longitud].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["IKChainManager_c::Constructor", "IKChainManager_c::Destructor", "WaterCreatureManager_c::Constructor", "WaterCreatureManager_c::Destructor", "CWaterLevel::IsPointInCameraDepthRange",
         "CAudioZones::BugstarWriteScreenShotEntry", "D3D9RasterFormat::DecompressDxt1", "D3D9RasterFormat::DecompressDxt3", "D3D9RasterFormat::DecompressDxt5", "D3D9RasterFormat::DecodeDxt5AlphaBlock"]
ENTRIES = (0x617FC0, 0x618030, 0x6E3EB0, 0x6E3F20, 0x6E67A0, 0x509230, 0x81E720, 0x81EA70, 0x81EDC0, 0x81F0C0)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
IK, WC, NOP, CAM, BUG, DXT1, DXT3, DXT5, ALPHA = 'ik wc nop cam bug dxt1 dxt3 dxt5 alpha'.split()
KIND = [IK, NOP, WC, NOP, CAM, BUG, DXT1, DXT3, DXT5, ALPHA]
HASHED = (DXT1, DXT3, DXT5, ALPHA)


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


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


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

    def rdbytes(self, off, n):
        return bytes(self.get(GUARD + off + k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changed(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append((i - GUARD, self.m[i]))
        return out

    def changes(self):
        return [[o, b] for o, b in self.changed()]

    def chash(self):
        ch = self.changed()
        h = 2166136261
        for off, b in ch:
            for k in range(4):
                h = ((h ^ (((off & M32) >> (8 * k)) & 0xFF)) * 16777619) & M32
            h = ((h ^ b) * 16777619) & M32
        return [len(ch), h]


class World:
    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def tagv(self, v):
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == CAM:
        _k += ['near_fails', 'in_range', 'beyond_far_checked', 'beyond_far_unchecked', 'nan_z', 'nan_input', 'inf_input', 'near_tie', 'far_tie', 'flag_high_bits', 'mode0', 'mode1', 'mode2', 'mode3']
    elif _kind == BUG:
        _k += ['coord_big', 'coord_nan', 'coord_inf', 'coord_negative', 'empty_title', 'empty_grid', 'empty_image', 'image_large', 'coord_tiny']
    elif _kind in (DXT1, DXT3, DXT5):
        _k += ['three_color', 'four_color', 'equal_endpoints', 'width_not_mult4', 'height_not_mult4', 'width_nonpositive', 'height_nonpositive', 'no_blocks', 'multi_block_rows', 'multi_blocks_per_row']
        if _kind == DXT1:
            _k += ['transparent_used', 'transparent_unused']
    elif _kind == ALPHA:
        _k += ['a0_gt_a1', 'a0_le_a1', 'a0_eq_a1', 'stride_negative', 'stride_zero', 'mask_zero', 'mask_full', 'stride_small']
    KEYS[_fn] = _k


# ------------------------------------------------------------------------------------------------ x87 con control de precision de 24 bits (exponente extendido)
NAN = ('n',)


def fl(bits):
    bits &= M32
    e, m = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    s = -1 if bits >> 31 else 1
    if e == 255:
        return NAN if m else ('i', s)
    if e == 0:
        return ('f', Fraction(s * m, 1 << 149))
    return ('f', Fraction(s * (m | 0x800000), 1 << 23) * Fraction(2) ** (e - 127))


def _round_bits(fr, mant_bits, min_exp=None):
    """redondea (par) una fraccion a `mant_bits` bits de mantisa; con min_exp, el cuanto no baja de 2**min_exp (denormales de float32)"""
    if fr == 0:
        return fr
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    q = Fraction(2) ** (e - mant_bits + 1)
    if min_exp is not None and q < Fraction(2) ** min_exp:
        q = Fraction(2) ** min_exp
    n = a / q
    fl_ = n.numerator // n.denominator
    rem = n - fl_
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl_ % 2 == 1):
        fl_ += 1
    return (1 if fr > 0 else -1) * fl_ * q


def r24(v):
    return ('f', _round_bits(v[1], 24)) if v[0] == 'f' else v


def to_f32(v):
    """fstp dword: float32 (denormales y desbordamiento a infinito, redondeo al mas cercano)"""
    if v[0] != 'f':
        return v
    q = _round_bits(v[1], 24, -149)
    if abs(q) >= Fraction(2) ** 128:
        return ('i', 1 if q > 0 else -1)
    return ('f', q)


def sgn(v):
    return (v[1] > 0) - (v[1] < 0)


def mul(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        sa = a[1] if a[0] == 'i' else sgn(a)
        sb = b[1] if b[0] == 'i' else sgn(b)
        return NAN if sa == 0 or sb == 0 else ('i', sa * sb)
    return r24(('f', a[1] * b[1]))


def add(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    return r24(('f', a[1] + b[1]))


def neg(v):
    return ('i', -v[1]) if v[0] == 'i' else (v if v[0] == 'n' else ('f', -v[1]))


def num(v):
    return math.inf * v[1] if v[0] == 'i' else v[1]


def lt(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return False
    return num(a) < num(b)


def cam_expected(a):
    near, far, radius, flag = fl(a[8]), fl(a[9]), fl(a[7]), a[10] & 0xFF
    m = [fl(a[11 + i]) for i in range(16)]
    px, py, pz = fl(a[4]), fl(a[5]), fl(a[6])
    z = mul(m[10], pz)
    z = add(z, mul(m[2], px))
    z = add(z, mul(m[6], py))
    z = add(z, m[14])
    z = to_f32(z)
    nr = add(near, neg(radius))
    marks = ['mode%d' % a[29]]
    if not lt(nr, z):
        marks.append('near_fails')
        ret = 0
    elif lt(z, far):
        marks.append('in_range')
        ret = 1
    else:
        marks.append('beyond_far_checked' if flag else 'beyond_far_unchecked')
        ret = 0 if flag else 1
    if z[0] == 'n':
        marks.append('nan_z')
    if any(v[0] == 'n' for v in [near, far, radius, px, py, pz] + m):
        marks.append('nan_input')
    if any(v[0] == 'i' for v in [near, far, radius, px, py, pz] + m):
        marks.append('inf_input')
    if z[0] == 'f' and nr[0] == 'f' and nr[1] == z[1]:
        marks.append('near_tie')
    if z[0] == 'f' and far[0] == 'f' and far[1] == z[1]:
        marks.append('far_tie')
    if flag > 1:
        marks.append('flag_high_bits')
    return ret, marks


# ------------------------------------------------------------------------------------------------ BugstarWriteScreenShotEntry
def fmt_coord(bits):
    """sprintf("%f", (double)valor) del CRT del EXE (0x821BB5, CRT antiguo): NaN "1.#QNAN0", infinito "1.#INF00" (con "-" si es negativo), cero sin signo, y para los demas valores los 17 primeros digitos
    significativos (redondeo medio hacia arriba) seguidos de ceros, redondeados medio hacia arriba a seis decimales. Calibrado con el lado EXE del banco (README.md)."""
    v = f32(bits)
    if math.isnan(v):
        return b'-1.#QNAN0' if (bits >> 31) else b'1.#QNAN0'
    if math.isinf(v):
        return b'1.#INF00' if v > 0 else b'-1.#INF00'
    if v == 0:
        return b'0.000000'
    import decimal
    ctx = decimal.Context(prec=80, rounding=decimal.ROUND_HALF_UP)
    d = decimal.Decimal(abs(v))
    e = d.adjusted()
    d17 = d.quantize(decimal.Decimal(1).scaleb(e - 16), context=ctx)
    d6 = d17.quantize(decimal.Decimal('0.000001'), context=ctx)
    return (('-' if v < 0 else '') + format(d6, 'f')).encode()


def bug_expected(a, W, events, marks):
    D = W.mem[3]
    title = bytes(1 + rnd(a[13], i) % 255 for i in range(a[11]))
    grid = bytes(1 + rnd(a[13] + 1, i) % 255 for i in range(a[12]))
    image = D.rdbytes(a[9], a[10])
    buf = bytearray()
    this = W.raw(0, a[0])

    def ev(*v):
        events.append(list(v))

    def snapshot():
        return fnv(bytes(buf)), len(buf)

    def put(b):
        buf.extend(b)
    put(struct.pack('<II', 0, len(title)) + title)
    ev(0x508B80, 0x7F000000, 1, *snapshot())
    put(struct.pack('<II', 0xA, 2) + b'SA')
    for key, bits in (('XCoord', a[4]), ('YCoord', a[5]), ('ZCoord', a[6])):
        text = fmt_coord(bits)
        ev(0x508D30, 0x7F000000, fnv(key.encode()), fnv(b'text'), *snapshot())
        put(struct.pack('<I', len(text)) + text)
        put(b'\0\0\0\0')
        v = f32(bits)
        if math.isnan(v):
            marks.append('coord_nan')
        elif math.isinf(v):
            marks.append('coord_inf')
        else:
            if abs(v) >= 1e15:
                marks.append('coord_big')
            if v < 0:
                marks.append('coord_negative')
            if v != 0 and abs(v) < 1e-5:
                marks.append('coord_tiny')
    ev(0x508D30, 0x7F000000, fnv(b'grid'), fnv(b'text'), *snapshot())
    put(struct.pack('<I', len(grid)) + grid)
    put(b'\0\0\0\0')
    ev(0x508D30, 0x7F000000, fnv(b'ScreenShot'), fnv(b'image'), *snapshot())
    put(struct.pack('<I', len(image)) + image)
    put(b'\0\0\0\0')
    ev(0x509190, W.tagv(this), 0, fnv(bytes(buf)), len(buf))
    if not title:
        marks.append('empty_title')
    if not grid:
        marks.append('empty_grid')
    if not image:
        marks.append('empty_image')
    if len(image) > 100:
        marks.append('image_large')
    return bytes(buf).hex()


# ------------------------------------------------------------------------------------------------ descompresion DXT
def c565(c):
    return ((c >> 11) << 3) & 0xFF, ((c >> 5) << 2) & 0xFF, (c << 3) & 0xFF


def pack(r, g, b, al):
    return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((al & 0xFF) << 24)


def palette(blk, transparent):
    c0, c1 = blk[0] | blk[1] << 8, blk[2] | blk[3] << 8
    r0, g0, b0 = c565(c0)
    r1, g1, b1 = c565(c1)
    p = [pack(r0, g0, b0, 255), pack(r1, g1, b1, 255)]
    if c0 > c1:
        p.append(pack((r1 + r0 * 2) // 3, (g1 + g0 * 2) // 3, (b1 + b0 * 2) // 3, 255))
        p.append(pack((r0 + r1 * 2) // 3, (g0 + g1 * 2) // 3, (b0 + b1 * 2) // 3, 255))
        return p, False, c0 == c1
    p.append(pack((r0 + r1) // 2, (g0 + g1) // 2, (b0 + b1) // 2, 255))
    p.append(transparent)
    return p, True, c0 == c1


def alpha_table(a0, a1):
    t = [a0, a1]
    if a0 > a1:
        t += [(6 * a0 + 1 * a1) // 7, (5 * a0 + 2 * a1) // 7, (4 * a0 + 3 * a1) // 7, (3 * a0 + 4 * a1) // 7, (2 * a0 + 5 * a1) // 7, (1 * a0 + 6 * a1) // 7]
    else:
        t += [(4 * a0 + 1 * a1) // 5, (3 * a0 + 2 * a1) // 5, (2 * a0 + 3 * a1) // 5, (1 * a0 + 4 * a1) // 5, 0, 255]
    return t


def alpha_values(blk):
    """16 alfas (pixel 0 = primer indice) de un bloque de alfa DXT5 de 8 bytes"""
    t = alpha_table(blk[0], blk[1])
    bits = int.from_bytes(bytes(blk[2:8]), 'little')
    return [t[(bits >> (3 * i)) & 7] for i in range(16)]


def dxt_expected(fn, a, W, marks):
    kind = KIND[fn]
    w = a[4] - (1 << 32) if a[4] >= 1 << 31 else a[4]
    h = a[5] - (1 << 32) if a[5] >= 1 << 31 else a[5]
    B, C = W.mem[1], W.mem[2]
    bpr = int(w / 4)
    bh = int(h / 4)
    bsize = 8 if kind == DXT1 else 16
    dst, src = a[6], a[7]
    nblocks = bpr * bh if (bpr > 0 and bh > 0) else 0
    for j in range(nblocks):                           # retoques del banco: extremos de color (y de alfa) iguales
        if (a[8] >> (j % 32)) & 1:
            base = src + j * bsize
            co = base + (0 if kind == DXT1 else 8)
            C.wr(co + 2, 1, C.rd(co, 1))
            C.wr(co + 3, 1, C.rd(co + 1, 1))
            if kind == DXT5:
                C.wr(base + 1, 1, C.rd(base, 1))
    W.snap()
    if w % 4 and w > 0:
        marks.append('width_not_mult4')
    if h % 4 and h > 0:
        marks.append('height_not_mult4')
    if w <= 0:
        marks.append('width_nonpositive')
    if h <= 0:
        marks.append('height_nonpositive')
    if nblocks == 0:
        marks.append('no_blocks')
    if bh > 1 and bpr > 0:
        marks.append('multi_block_rows')
    if bpr > 1 and bh > 0:
        marks.append('multi_blocks_per_row')
    used3 = False
    j = 0
    for br in range(bh if bh > 0 else 0):
        for bc in range(bpr if bpr > 0 else 0):
            base = src + j * bsize
            j += 1
            blk = C.rdbytes(base, bsize)
            cb = blk if kind == DXT1 else blk[8:]
            pal, three, eq = palette(cb, 0 if kind == DXT1 else 0x00FFFF00)
            marks.append('three_color' if three else 'four_color')
            if eq:
                marks.append('equal_endpoints')
            pix = [[0] * 4 for _ in range(4)]
            for r in range(4):
                bits = cb[4 + r]
                for px in range(4):
                    idx = (bits >> (2 * px)) & 3
                    if idx == 3 and three:
                        used3 = True
                    pix[r][px] = pal[idx]
            if kind == DXT3:
                for r in range(4):
                    word = blk[2 * r] | blk[2 * r + 1] << 8
                    for px in range(4):
                        nib = (word >> (4 * px)) & 0xF
                        pix[r][px] = (pix[r][px] & 0x00FFFFFF) | ((nib | nib << 4) << 24)
            elif kind == DXT5:
                al = alpha_values(blk[:8])
                for r in range(4):
                    for px in range(4):
                        pix[r][px] = (pix[r][px] & 0x00FFFFFF) | (al[r * 4 + px] << 24)
            for r in range(4):
                for px in range(4):
                    B.wr(dst + ((4 * br + r) * w + 4 * bc + px) * 4, 4, pix[r][px])
    ret = None
    if kind == DXT1:
        ret = 1 if used3 else 0
        marks.append('transparent_used' if used3 else 'transparent_unused')
    return ret


def alpha_expected(a, W, marks):
    B, C = W.mem[1], W.mem[2]
    stride = a[4] - (1 << 32) if a[4] >= 1 << 31 else a[4]
    mask, dst, src = a[5], a[6], a[7]
    if a[8] & 1:
        C.wr(src + 1, 1, C.rd(src, 1))
    W.snap()
    blk = C.rdbytes(src, 8)
    al = alpha_values(blk)
    marks.append('a0_gt_a1' if blk[0] > blk[1] else 'a0_le_a1')
    if blk[0] == blk[1]:
        marks.append('a0_eq_a1')
    if stride < 0:
        marks.append('stride_negative')
    if stride == 0:
        marks.append('stride_zero')
    if 0 < stride < 8:
        marks.append('stride_small')
    if mask == 0:
        marks.append('mask_zero')
    if mask == M32:
        marks.append('mask_full')
    for r in range(4):
        for px in range(4):
            off = dst + (r * stride + px) * 4
            old = B.rd(off, 4)
            B.wr(off, 4, (old & mask) | (al[r * 4 + px] << 24))


def put_strings(a, W):
    C = W.mem[2]
    for off, ln, seed in ((a[8], a[11], a[13]), (a[7], a[12], a[13] + 1)):
        for i in range(ln):
            C.wr(off + i, 1, 1 + rnd(seed, i) % 255)
        C.wr(off + ln, 1, 0)


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, extra = [], None, {}
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == CAM:
        cam = a[28]
        for i in range(16):
            B.wr(cam + 0x20 + 4 * i, 4, a[11 + i])
        B.wr(cam + 0x80, 4, a[8])
        B.wr(cam + 0x84, 4, a[9])
        for i in range(3):
            Cc.wr(a[27] + 4 * i, 4, a[4 + i])
        W.snap()
        ret, m = cam_expected(a)
        marks += m
    elif k in (IK, WC):
        W.snap()
        if k == IK:
            count, size, lists = 0x20, 0x58, (0xB00, 0xB0C)
            for i in range(count):
                A.wr(o + i * size, 4, 0)
                A.wr(o + i * size + 4, 4, 0)
        else:
            count, size, lists = 0x80, 0x34, (0x1A00, 0x1A0C)
            for i in range(count):
                A.wr(o + i * size, 4, 0)
                A.wr(o + i * size + 4, 4, 0)
                A.wr(o + i * size + 8, 4, 0)
        for lo in lists:
            for q in range(3):
                A.wr(o + lo + 4 * q, 4, 0)
        ret = W.tagv(this)
    elif k == NOP:
        W.snap()
    elif k == BUG:
        put_strings(a, W)
        W.snap()
        extra['blob'] = bug_expected(a, W, events, marks)
    elif k in (DXT1, DXT3, DXT5):
        ret = dxt_expected(fn, a, W, marks)
    elif k == ALPHA:
        alpha_expected(a, W, marks)
    out = {'events': events, 'ret': ret, 'marks': marks, 'extra': extra}
    if k in HASHED:
        out['chash'] = {n: m.chash() for n, m in zip('ABCD', W.mem)}
    else:
        out['changes'] = {n: m.changes() for n, m in zip('ABCD', W.mem)}
    return out


def check_side(c, o, e, side_row):
    if 'chash' in e:
        if o.get('chash') != e['chash']:
            return 'chash: %s != %s' % (str(o.get('chash'))[:300], str(e['chash'])[:300])
    elif o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != []:
        return 'gchanges: %s' % (str(o.get('gchanges'))[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if 'blob' in e['extra'] and o.get('blob') != e['extra']['blob']:
        return 'texto: %s != %s' % (o.get('blob'), e['extra']['blob'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX/AL (valor devuelto, ya comprobado)"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
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
