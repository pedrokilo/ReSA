"""Oraculo independiente (Python) de Fn91A, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Cuatro funciones de CWaterLevel (todas cdecl, `ret` del llamador):
  0  AddToQuadsAndTrianglesList 0x6E5750 (blockX, blockY, index, type)
  1  GetGroundLevel 0x6EA8A0 (const CVector* pos, float* outZ, uint8* outColData, float range); devuelve AL
  2  RenderHighDetailWaterRectangle_OneLayer 0x6E91D0 (minX, maxX, Y1, Y2, P1..P4, layer, twiceArea, triangular, halfX, halfY)
  3  RenderHighDetailWaterTriangle_OneLayer 0x6E8780 (X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, layer, halfSquared, triangular, halfW)
La aritmetica x87 (24 bits de mantisa, exponente extendido) se modela con fracciones exactas (x87model.py, copia del modelo de Fn88B). Los colaboradores (vaciado 0x6E7680, olas 0x6E6EF0,
ProcessVerticalLine 0x5674E0) son falsos con salidas definidas por el caso; el oraculo reproduce esas salidas y los eventos de sus argumentos.

--- 0x6E5750 --- bloque = word en 0xC21B70 + 2 * (blockY + 12 * blockX); tipo = bits 14..15. entrada = ((type << 14) | index) & 0xFFFF; contador n = dword 0xC215F0; lista = words desde 0xC215F8.
  tipo 0: bloque = entrada.  tipo 2 o 1: lista[n] = bloque viejo; lista[n+1] = entrada; lista[n+2] = 0; contador = n + 3; bloque = (n | 0xC000) & 0xFFFF.
  tipo 3: lista[n-1] = entrada; lista[n] = 0; contador = n + 1.
--- 0x6EA8A0 --- origen = (x, y, z + range) (la suma en la x87 de 24 bits y guardada como float), distancia = -range; 0x5674E0(origen, distancia, &colpoint, &entidad, 1, 0, 0, 0, 1, 0, 0). Falla: AL = 0.
  Acierto: *outZ = colpoint.z (+8); si outColData != 0: [0] = colpoint +0x23, [1] = +0x24; AL = 1.
--- 0x6E91D0 / 0x6E8780 --- ver los comentarios de cada modelo (rect / tri) mas abajo: parten de la lectura del desensamblado.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from pathlib import Path
from fractions import Fraction

sys.path.insert(0, str(Path(__file__).resolve().parent))
from x87model import (M32, DEFAULT_NAN, add, sub, mul, sqrt24, div2, div_int, fi, ld, bits, to_bits, from_bits, f32, is_nan, is_inf, is_zero, floor_val, lt, gt, s32, idiv, ftol, crc32,
                      region_bytes)

NAMES = ['CWaterLevel::AddToQuadsAndTrianglesList', 'CWaterLevel::GetGroundLevel', 'CWaterLevel::RenderHighDetailWaterRectangle_OneLayer', 'CWaterLevel::RenderHighDetailWaterTriangle_OneLayer']
ADDRESS = [0x6E5750, 0x6EA8A0, 0x6E91D0, 0x6E8780]
CASES = 16384
BUF_VTX_OFF, BUF_IDX_OFF, VERTEX = 0x2008, 8, 36
COL_B, COL_G, COL_R = 8, 0x808, 0x1008
COMBO_BLOCKS = 0x580
STACK_TAG = 0x7F000000
NORMAL_ADDR = 0xC278D4

C_1 = Fraction(1)
C_4 = Fraction(4)
C_075 = ld(0x3F400000)
C_008 = ld(0x3DA3D70A)    # 0x859018
C_004 = ld(0x3D23D70A)    # 0x858CEC
C_0577 = ld(0x3F13B646)   # 0x872004
C_01 = ld(0x3DCCCCCD)     # 0x858B1C


# ------------------------------------------------------------------------------------------------ salidas del falso de las olas (Fixtures.hpp: WaveOut)
def Rand(c, k):
    from x87model import rnd
    return rnd(c, k)


def frac_bits(num, den):
    """float(num) / float(den) con num y den enteros pequenos: un solo redondeo a float"""
    if num == 0:
        return 0
    return to_bits(Fraction(num, den))


def wave_out(wseed, k, field):
    r = Rand(wseed, 5000 + k * 16 + field)
    if field == 0:
        if r % 11 == 0:
            return (0xFFC00000, 0, 0x80000000, 0x7F800000, 0xFF800000, 1, 0x7F7FFFFF)[(r >> 8) % 7]
        return frac_bits(Rand(wseed, 7000 + k) % 4001 - 2000, 500)
    if field == 1:
        if r % 16 < 8:
            return (0xFFC00000, 0, 0xBF800000, 0x7149F2CA, 0xF149F2CA, 0x7F800000, 0x43960000, 0x3B449BA6)[r % 16]
        return frac_bits(Rand(wseed, 8000 + k) % 3001, 1000)
    return frac_bits(r % 2001 - 1000, 250)


# ------------------------------------------------------------------------------------------------ pequenos helpers
def u32(v):
    return v & M32


def events_observed(events):
    stream = bytearray()
    for kind, vals in events:
        stream += struct.pack('<II', kind, len(vals))
        for v in vals:
            stream += struct.pack('<I', v & M32)
    first = [[kind] + [v & M32 for v in vals] for kind, vals in events[:4]]
    return {'n': len(events), 'crc': crc32(stream) if stream else 0, 'first': first}


def hexs(b):
    return bytes(b).hex()


# ------------------------------------------------------------------------------------------------ 0x6E5750
def quad(c):
    img = bytearray(region_bytes(c['mode'], c['rseed'] + 11, 0, 0x6A0))
    ref = bytes(img)
    slot = (c['qb'] + 12 * c['qa']) & M32
    struct.pack_into('<I', img, 0, c['qcount'] & M32)
    boff = COMBO_BLOCKS + 2 * slot
    struct.pack_into('<H', img, boff, c['qold'] & 0xFFFF)
    ref = bytearray(img)
    entry = (((c['qtype'] << 14) & M32) | (c['qi'] & M32)) & 0xFFFF
    old = struct.unpack_from('<H', img, boff)[0]
    typ = old >> 14
    n = struct.unpack_from('<I', img, 0)[0]

    def lw(pos, val):
        struct.pack_into('<H', img, (8 + 2 * pos) & M32, val & 0xFFFF)
    marks = ['t%d' % typ]
    if typ == 0:
        struct.pack_into('<H', img, boff, entry)
    elif typ in (1, 2):
        lw(n, old)
        lw(n + 1, entry)
        lw(n + 2, 0)
        struct.pack_into('<I', img, 0, (n + 3) & M32)
        struct.pack_into('<H', img, boff, (n | 0xC000) & 0xFFFF)
    else:
        lw(n - 1, entry)
        lw(n, 0)
        struct.pack_into('<I', img, 0, (n + 1) & M32)
        if n == 0:
            marks.append('t3_before_list')
    changes = [[i, img[i]] for i in range(len(img)) if img[i] != ref[i]]
    marks.append('type_%d' % min(c['qtype'], 4) if c['qtype'] < 4 else 'type_big')
    return {'observed': {'changes': changes, 'events': events_observed([])}, 'marks': marks}


# ------------------------------------------------------------------------------------------------ 0x6EA8A0
def ground(c):
    ref = bytearray(region_bytes(0, c['rseed'], 0, 0x100))
    for k in range(3):
        struct.pack_into('<I', ref, 0x40 + 4 * k, c['gpos'][k])
    img = bytearray(ref)
    z = add(ld(c['grange']), ld(c['gpos'][2]))
    oz = bits(z)
    dist = c['grange'] ^ 0x80000000
    ev = [(3, [c['gpos'][0], c['gpos'][1], oz, dist, STACK_TAG, STACK_TAG, 1, 0, 0, 0, 1, 0, 0])]
    marks = []
    if c['ghit']:
        struct.pack_into('<I', img, 0x50, c['gz'])
        if not c['gcolnull']:
            img[0x58] = c['gsurf'] & 0xFF
            img[0x59] = c['gpiece'] & 0xFF
        ret = 1
        marks.append('hit')
        marks.append('colnull' if c['gcolnull'] else 'col')
    else:
        ret = 0
        marks.append('miss')
    changes = [[i, img[i]] for i in range(len(img)) if img[i] != ref[i]]
    return {'observed': {'changes': changes, 'events': events_observed(ev), 'ret_al': ret}, 'marks': marks}


# ------------------------------------------------------------------------------------------------ rejilla de agua (rectangulo y triangulo)
class Buffers:
    """vertices (36 bytes), indices (words) y tablas de color con su imagen previa y las escrituras de la llamada"""

    def __init__(self, c):
        self.c = c
        self.v = {}
        self.iw = {}
        self.cb = {}
        self.cg = {}
        self.cr = {}

    def vrec(self, k):
        if k not in self.v:
            self.v[k] = bytearray(region_bytes(self.c['mode'], self.c['rseed'], BUF_VTX_OFF + VERTEX * k, VERTEX))
        return self.v[k]

    def setv(self, k, off, value):
        self.vrec(k)[off:off + 4] = struct.pack('<I', value & M32)

    def getiw(self, p):
        if p not in self.iw:
            self.iw[p] = struct.unpack('<H', region_bytes(self.c['mode'], self.c['rseed'], BUF_IDX_OFF + 2 * p, 2))[0]
        return self.iw[p]

    def setiw(self, p, value):
        self.iw[p] = value & 0xFFFF

    def table(self, name):
        return {'b': self.cb, 'g': self.cg, 'r': self.cr}[name]

    def getc(self, name, k):
        t = self.table(name)
        if k not in t:
            base = {'b': COL_B, 'g': COL_G, 'r': COL_R}[name]
            t[k] = region_bytes(self.c['mode'], self.c['rseed'] + 7, base + k, 1)[0]
        return t[k]

    def setc(self, name, k, value):
        self.table(name)[k] = value & 0xFF


def fade_factor(dx, dy, detail):
    """0x6E96xx / 0x6E8Bxx: d = sqrt(dy*dy + dx*dx) / detail; f = 0.0 si d > 1.0, (1.0 - d_float) * 4.0 si d_float > 0.75, 1.0 si no"""
    s = add(mul(dy, dy), mul(dx, dx))
    q = div_int(sqrt24(s), detail)
    ds = f32(q)
    if gt(q, Fraction(1)):
        return ('z', 0), 'fade0'
    if gt(ds, C_075):
        return mul(sub(Fraction(1), ds), C_4), 'fade_mid'
    return Fraction(1), 'fade1'


def fract_shift(v):
    return f32(sub(v, floor_val(v)))


def pack_color(alpha, r, g, b):
    return ((((((alpha << 8) | r) & M32) << 8 | g) << 8) | b) & M32


def color_byte(byte, mult_bits):
    return ftol(mul(fi(byte), ld(mult_bits))) & 0xFF


def wave_call(c, st, ev, x, y, big_amp_v, small_amp_v, z_bits):
    k = st['calls']
    st['calls'] += 1
    ev.append((2, [x, y, bits(big_amp_v), bits(small_amp_v), STACK_TAG, STACK_TAG, STACK_TAG, NORMAL_ADDR, z_bits]))
    out = [wave_out(c['wseed'], k, f) for f in range(6)]
    st['norm'] = out[3:6]
    if c['mut_call'] == k:
        st['nv'] = u32(st['nv'] + c['mut_dv'])
        st['ni'] = u32(st['ni'] + c['mut_di'])
        st['ci'] = u32(st['ci'] + c['mut_dc'])
    return out[0], out[1]


def finish_grid(c, fn, buf, st, ev, marks):
    from_v = c['vtx0']
    hx, hy = c['hx'], c['hy']
    K = (hx + 1) * (hy + 1) if (hx >= 0 and hy >= 0) else 0
    if fn == 3:
        K = (hx + 1) * (hx + 2) // 2 if hx >= 0 else 0
    sv = K + 8
    si = 6 * K + 3 * (hx if hx > 0 else 0) + 16 + 6
    sc = K + 8
    vb = bytearray()
    for k in range(c['vtx0'], c['vtx0'] + sv):
        vb += buf.vrec(k)
    ib = bytearray()
    for p in range(c['idx0'], c['idx0'] + si):
        ib += struct.pack('<H', buf.getiw(p))
    crc = 0
    for name in ('b', 'g', 'r'):
        data = bytes(buf.getc(name, k) for k in range(c['ci0'], c['ci0'] + sc))
        crc = crc32(data, crc)
    norm = bytearray(region_bytes(c['mode'], c['rseed'] + 17, 0, 16))
    norm[12] = c['flag'] & 0xFF
    if st['calls']:
        for j in range(3):
            struct.pack_into('<I', norm, 4 * j, st['norm'][j])
    obs = {'vtx_after': u32(st['nv']), 'idx_after': u32(st['ni']), 'ci_after': u32(st['ci']),
           'vertices_fnv': crc32(vb), 'indices_fnv': crc32(ib), 'colors_fnv': crc,
           'rest_ok': True, 'small_ok': True, 'norm_after': hexs(norm), 'events': events_observed(ev)}
    del from_v
    return {'observed': obs, 'marks': marks}


def fadd_fl(a, b):
    return f32(add(a, b))


def rect(c):
    """0x6E91D0. Rejilla de (halfX + 1) x (halfY + 1) vertices en dos triangulos (abajo si i * invX + jY * invY < 1.0, arriba si no), layer 0: olas y vertice completo; layer 1: textura y color."""
    hx, hy = c['hx'], c['hy']
    minX, maxX, Y1, Y2 = [s32(v) for v in c['g'][:4]]
    P = [(c['p'][4 * k], c['p'][4 * k + 1], c['p'][4 * k + 2]) for k in range(4)]
    layer = s32(c['layer'])
    detail = s32(c['detail'])
    alpha = c['alpha']
    buf = Buffers(c)
    st = {'nv': c['vtx0'], 'ni': c['idx0'], 'ci': c['ci0'], 'calls': 0, 'norm': None}
    ev = [(1, [c['pre_vtx'], c['pre_idx'], 0])]
    marks = ['rect', 'layer%s' % (layer if layer in (0, 1) else 'X')]
    invX = f32(div2(Fraction(1), Fraction(hx)))
    quotX = idiv(s32(maxX - minX), hx)
    invY = f32(div2(Fraction(1), Fraction(hy)))
    quotY = idiv(s32(Y2 - Y1), hy)

    def slope(a, b, inv):
        return f32(mul(sub(ld(a), ld(b)), inv))
    sAC = slope(P[1][0], P[0][0], invX)
    s68 = slope(P[2][0], P[0][0], invY)
    s9C = slope(P[1][1], P[0][1], invX)
    negQuotX = idiv(s32(minX - maxX), hx)
    s74 = slope(P[2][1], P[0][1], invY)
    sB0 = slope(P[1][2], P[0][2], invX)
    s84 = slope(P[2][2], P[0][2], invY)
    negQuotY = idiv(s32(Y1 - Y2), hy)
    sB4 = slope(P[2][0], P[3][0], invX)
    s8C = slope(P[1][0], P[3][0], invY)
    s6C = slope(P[2][1], P[3][1], invX)
    s94 = slope(P[1][1], P[3][1], invY)
    s80 = slope(P[2][2], P[3][2], invX)
    s78 = slope(P[1][2], P[3][2], invY)
    sh = c['shift']
    shiftU = shiftV = None
    if layer == 0:
        shiftU = f32(add(mul(fi(minX), C_008), ld(sh[5])))
        shiftV = f32(add(mul(fi(Y1), C_008), ld(sh[4])))
    elif layer == 1:
        shiftU = f32(add(mul(fi(minX), C_004), ld(sh[3])))
        shiftV = f32(add(mul(fi(Y1), C_004), ld(sh[2])))
    if layer in (0, 1):
        shiftU = fract_shift(shiftU)
        shiftV = fract_shift(shiftV)
    cam = c['cam_m'] if c['use_mat'] else c['cam_p']
    camX, camY = ld(cam[0]), ld(cam[1])
    if hy < 0:
        marks.append('neg_hy')
        return finish_grid(c, 2, buf, st, ev, marks)
    if hx < 0:
        marks.append('neg_hx')
    flagzero = (c['flag'] & 0xFF) == 0
    if not flagzero:
        marks.append('flag')
    idx_r = st['ni']
    rowStartY = s32(negQuotY * hy + Y2)
    yOff = 0
    ci_r = st['ci']
    vtx_r = st['nv']
    remY = hy
    quotYPos = s32(-negQuotY)
    jY = 0
    while True:
        if hx >= 0:
            jYf = f32(fi(jY))
            a0 = f32(mul(fi(jY), invY))
            xOff = 0
            quotXPos = s32(-negQuotX)
            xUp = s32(negQuotX * hx + maxX)
            remX = hx
            i = 0
            while True:
                lower = lt(add(mul(fi(i), invX), a0), Fraction(1))
                if lower:
                    x = s32(minX + xOff)
                    y = s32(Y1 + yOff)
                    iF = f32(fi(i))
                    z = f32(add(add(mul(jYf, s68), mul(iF, sAC)), ld(P[0][0])))
                    big = add(add(mul(jYf, s74), mul(iF, s9C)), ld(P[0][1]))
                    small = f32(add(add(mul(iF, sB0), mul(jYf, s84)), ld(P[0][2])))
                    marks.append('lower')
                else:
                    x = xUp
                    y = rowStartY
                    rXf = f32(fi(remX))
                    rYf = f32(fi(remY))
                    z = f32(add(add(mul(fi(remY), s8C), mul(rXf, sB4)), ld(P[3][0])))
                    big = add(add(mul(rYf, s94), mul(rXf, s6C)), ld(P[3][1]))
                    small = f32(add(add(mul(rYf, s78), mul(rXf, s80)), ld(P[3][2])))
                    marks.append('upper')
                xf = bits(fi(x))
                yf = bits(fi(y))
                dx = f32(sub(camX, fi(x)))
                dy = f32(sub(camY, fi(y)))
                fade, fmark = fade_factor(dx, dy, detail)
                marks.append(fmark)
                if layer == 0:
                    big_amp = mul(fade, big)
                    small_amp = mul(small, fade)
                    wave, cmult = wave_call(c, st, ev, u32(x), u32(y), f32(big_amp), f32(small_amp), bits(z))
                    vtx_r = st['nv']
                    buf.setv(vtx_r, 0x1C, bits(f32(add(mul(fi(xOff), C_008), shiftU))))
                    buf.setv(vtx_r, 0x20, bits(f32(add(mul(fi(yOff), C_008), shiftV))))
                    buf.setv(vtx_r, 0, xf)
                    buf.setv(vtx_r, 4, yf)
                    buf.setv(vtx_r, 8, wave)
                    wc = c['wcol']
                    r = color_byte(wc & 0xFF, cmult)
                    g_ = color_byte((wc >> 8) & 0xFF, cmult)
                    b_ = color_byte((wc >> 16) & 0xFF, cmult)
                    buf.setv(vtx_r, 0x18, pack_color(alpha[0], r, g_, b_))
                    ci_r = st['ci']
                    buf.setc('g', ci_r, g_)
                    idx_r = st['ni']
                    buf.setc('r', ci_r, r)
                    buf.setc('b', ci_r, b_)
                    ci_r = u32(ci_r + 1)
                    st['ci'] = ci_r
                elif layer == 1:
                    r = buf.getc('r', ci_r)
                    g_ = buf.getc('g', ci_r)
                    b_ = buf.getc('b', ci_r)
                    buf.setv(vtx_r, 0x1C, bits(f32(add(mul(fi(xOff), C_004), shiftU))))
                    buf.setv(vtx_r, 0x20, bits(f32(add(mul(fi(yOff), C_004), shiftV))))
                    buf.setv(vtx_r, 0x18, pack_color(alpha[1], r, g_, b_))
                    ci_r = u32(ci_r + 1)
                    st['ci'] = ci_r
                if flagzero:
                    if jY != 0 and i != 0:
                        base = vtx_r - hx
                        buf.setiw(idx_r, base - 2)
                        buf.setiw(idx_r + 1, base - 1)
                        buf.setiw(idx_r + 2, vtx_r - 1)
                        idx_r += 3
                        buf.setiw(idx_r, vtx_r)
                        buf.setiw(idx_r + 1, base - 1)
                        buf.setiw(idx_r + 2, vtx_r - 1)
                        idx_r += 3
                        st['ni'] = u32(idx_r)
                    vtx_r += 1
                    st['nv'] = u32(vtx_r)
                xOff = s32(xOff + quotX)
                xUp = s32(xUp + quotXPos)
                remX -= 1
                i += 1
                if not (i <= hx):
                    break
        jY += 1
        remY -= 1
        rowStartY = s32(rowStartY + quotYPos)
        yOff = s32(yOff + quotY)
        if not (jY <= hy):
            break
    return finish_grid(c, 2, buf, st, ev, marks)


def tri(c):
    """0x6E8780. Triangulo de (halfW + 1) filas; base = esquina 1 si X1 == X3, si no la 2 (diferencias invertidas). layer 0: olas y vertice completo; layer 1: textura y color; layer 2 no se prueba."""
    hw = c['hx']
    X1, Y1, X2, Y2, X3, Y3 = [s32(v) for v in c['g']]
    P = [(c['p'][4 * k], c['p'][4 * k + 1], c['p'][4 * k + 2]) for k in range(3)]
    layer = s32(c['layer'])
    detail = s32(c['detail'])
    alpha = c['alpha']
    buf = Buffers(c)
    st = {'nv': c['vtx0'], 'ni': c['idx0'], 'ci': c['ci0'], 'calls': 0, 'norm': None}
    ev = [(1, [c['pre_vtx'], c['pre_idx'], 0])]
    marks = ['tri', 'layer%s' % (layer if layer in (0, 1) else 'X')]
    inv = f32(div2(Fraction(1), Fraction(hw)))

    def slope(a, b):
        return f32(mul(sub(ld(a), ld(b)), inv))
    if X1 == X3:
        marks.append('base1')
        s44 = slope(P[1][0], P[0][0]); s68 = slope(P[2][0], P[0][0]); s64 = slope(P[1][1], P[0][1]); s54 = slope(P[2][1], P[0][1])
        quotX = idiv(s32(X2 - X1), hw)
        s6C = slope(P[1][2], P[0][2])
        quotY = idiv(s32(Y3 - Y1), hw)
        s4C = slope(P[2][2], P[0][2])
        baseX, baseY, B = X1, Y1, P[0]
    else:
        marks.append('base2')
        s44 = slope(P[0][0], P[1][0]); s68 = slope(P[2][0], P[1][0]); s64 = slope(P[0][1], P[1][1]); s54 = slope(P[2][1], P[1][1])
        quotX = idiv(s32(X1 - X2), hw)
        s6C = slope(P[0][2], P[1][2])
        quotY = idiv(s32(Y3 - Y2), hw)
        s4C = slope(P[2][2], P[1][2])
        baseX, baseY, B = X2, Y2, P[1]
    sh = c['shift']
    shiftU = shiftV = None
    if layer == 0:
        shiftU = f32(add(mul(fi(baseX), C_008), ld(sh[5])))
        shiftV = f32(add(mul(fi(baseY), C_008), ld(sh[4])))
    elif layer == 1:
        shiftU = f32(add(mul(fi(baseX), C_004), ld(sh[3])))
        shiftV = f32(add(mul(fi(baseY), C_004), ld(sh[2])))
    if layer in (0, 1):
        shiftU = fract_shift(shiftU)
        shiftV = fract_shift(shiftV)
    cam = c['cam_m'] if c['use_mat'] else c['cam_p']
    camX, camY = ld(cam[0]), ld(cam[1])
    if hw < 0:
        marks.append('neg_hw')
        return finish_grid(c, 3, buf, st, ev, marks)
    ci_r = st['ci']
    vtx_r = st['nv']
    idx_r = st['ni']
    yOff = 0
    curY = baseY
    remaining = hw
    j = 0
    while True:
        if remaining >= 0:
            jz = f32(mul(fi(j), s68))
            jbig = f32(mul(fi(j), s54))
            jsmall = f32(mul(fi(j), s4C))
            yf = f32(fi(curY))
            xOff = 0
            curX = baseX
            i = 0
            while True:
                z = f32(add(add(mul(fi(i), s44), jz), ld(B[0])))
                big = f32(add(add(mul(fi(i), s64), jbig), ld(B[1])))
                small = f32(add(add(mul(fi(i), s6C), jsmall), ld(B[2])))
                xf = bits(fi(curX))
                dx = f32(sub(camX, fi(curX)))
                dy = f32(sub(camY, yf))
                fade, fmark = fade_factor(dx, dy, detail)
                marks.append(fmark)
                if layer == 0:
                    big_amp = mul(fade, big)
                    small_amp = mul(small, fade)
                    wave, cmult = wave_call(c, st, ev, u32(curX), u32(curY), f32(big_amp), f32(small_amp), bits(z))
                    vtx_r = st['nv']
                    buf.setv(vtx_r, 0x1C, bits(f32(add(mul(fi(xOff), C_008), shiftU))))
                    buf.setv(vtx_r, 0x20, bits(f32(add(mul(fi(yOff), C_008), shiftV))))
                    buf.setv(vtx_r, 0, xf)
                    buf.setv(vtx_r, 4, bits(yf))
                    buf.setv(vtx_r, 8, wave)
                    wc = c['wcol']
                    r = color_byte(wc & 0xFF, 0x3F13B646)
                    g_ = color_byte((wc >> 8) & 0xFF, 0x3F13B646)
                    b_ = color_byte((wc >> 16) & 0xFF, 0x3F13B646)
                    ci_r = st['ci']
                    buf.setc('r', ci_r, r)
                    buf.setc('g', ci_r, g_)
                    buf.setc('b', ci_r, b_)
                    ci_r = u32(ci_r + 1)
                    buf.setv(vtx_r, 0x18, pack_color(alpha[0], r, g_, b_))
                    idx_r = st['ni']
                    st['ci'] = ci_r
                elif layer == 1:
                    r = buf.getc('r', ci_r)
                    g_ = buf.getc('g', ci_r)
                    b_ = buf.getc('b', ci_r)
                    buf.setv(vtx_r, 0x1C, bits(f32(add(mul(fi(xOff), C_004), shiftU))))
                    buf.setv(vtx_r, 0x20, bits(f32(add(mul(fi(yOff), C_004), shiftV))))
                    buf.setv(vtx_r, 0x18, pack_color(alpha[1], r, g_, b_))
                    ci_r = u32(ci_r + 1)
                    st['ci'] = ci_r
                if j != 0 and i != 0:
                    buf.setiw(idx_r, vtx_r)
                    buf.setiw(idx_r + 1, vtx_r - 1)
                    tail = vtx_r + j - hw
                    buf.setiw(idx_r + 2, tail - 3)
                    idx_r += 3
                    buf.setiw(idx_r, vtx_r)
                    buf.setiw(idx_r + 1, tail - 2)
                    buf.setiw(idx_r + 2, tail - 3)
                    idx_r += 3
                    st['ni'] = u32(idx_r)
                vtx_r += 1
                st['nv'] = u32(vtx_r)
                xOff = s32(xOff + quotX)
                curX = s32(curX + quotX)
                i += 1
                if not (i <= remaining):
                    break
        if j != 0:
            tail = vtx_r + j - hw
            buf.setiw(idx_r, vtx_r - 1)
            buf.setiw(idx_r + 1, tail - 2)
            buf.setiw(idx_r + 2, tail - 3)
            idx_r += 3
            st['ni'] = u32(idx_r)
        yOff = s32(yOff + quotY)
        curY = s32(curY + quotY)
        j += 1
        remaining -= 1
        if not (j <= hw):
            break
    return finish_grid(c, 3, buf, st, ev, marks)


def expected(c):
    fn = c['fn']
    if fn == 0:
        return quad(c)
    if fn == 1:
        return ground(c)
    if fn == 2:
        return rect(c)
    return tri(c)


def check_side(c, o, e):
    """o = lado observado (fila), e = esperado: devuelve la lista de campos que difieren"""
    want = e['observed']
    got = {k: v for k, v in o['observed'].items() if k != 'globals_hash'}
    return [k for k in want if got.get(k) != want[k]]


def check_row(row, e=None):
    c = row['input']
    e = e or expected(c)
    for label in ('original', 'candidate'):
        bad = check_side(c, row[label], e)
        if bad:
            return label, {k: (row[label]['observed'].get(k), e['observed'][k]) for k in bad}
    return None


def main(path):
    bad = 0
    count = 0
    for line in Path(path).read_text(encoding='utf-8').splitlines():
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        count += 1
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 5:
                print('DIFERENCIA', row['function'], row['case'], r)
    print(count, 'filas;', bad, 'diferencias con el oraculo')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
