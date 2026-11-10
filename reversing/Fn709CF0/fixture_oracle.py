"""Oraculo independiente (Python) de Fn709CF0: reproduce, desde la ENTRADA efectiva del caso (los valores leidos de la memoria de las arenas por el banco) y la lectura del desensamblado del EXE, lo que debe
observarse de CShadows::ShadowRenderTriangleCB (0x709CF0) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica
x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas (x87model.py).

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico; EAX = normal, EBX = triangulo, EDI = parametros; esi se guarda; al volver EAX = EBX; `ret`):
  709CF0  sub esp,5Ch / push esi / mov esi,eax
  709CF6  RwV3dTransformPoints(mundo = pila, triangulo (EBX), 3, matriz = EDI+0x58)               [llamada 1; salida: 3 vectores `mundo`]
  709D07  producto = ((at.y * n.y) + (at.z * n.z)) + (at.x * n.x), at = EDI+0..8, n = [esi]; fcomp 0.0f (0x858B50): si producto > 0 (ordenado; falso con NaN) -> salida
  709D49  RwV3dTransformPoints(proyectado = pila, mundo, 3, matriz = EDI+0x0C)                     [llamada 2; salida: 3 vectores `proyectado`]
  709D5E  cinco grupos, cada uno exige que ALGUN vertice cumpla la condicion (si ninguno la cumple -> salida), en este orden:
            1: !(z < 0.0)   2: !(x < 0.0)   3: !(x > 1.0f, 0x858624)   4: !(y < 0.0)   5: !(y > 1.0)          (las comparaciones son ordenadas: con NaN la condicion se cumple)
  709E74  RenderBuffer::StartStoring(3, 3, &indices, &vertices) (0x708290; cdecl)
          vertice k (36 bytes): +0 = x del mundo (vertice 0: fld/fstp, un NaN senalizador sale silenciado; vertices 1 y 2: copia de 32 bits), +4 = y del mundo (copia de 32 bits), +8 = z del mundo + 0.06f (0x859934, fadd x87 a 24 bits, guardado como float),
          +0x1C = x proyectado (copia), +0x20 = y proyectado (copia); +0xC..0x17 no se tocan;
          color (+0x18): si el dword [EDI+0x50] (fade) != 0: alfa = byte bajo de _ftol2(byte[EDI+0x4C] * (1.0f - z*z)) o 0 si 1.0f - z*z < 0 (ordenado; con NaN se calcula y da 0), z = z proyectado del vertice;
                         si no: byte[EDI+0x4C]; color = alfa * 0x01010101.
          indices: word 0, 1, 2 en el buffer de indices.
  70A029  RenderBuffer::StopStoring() (0x7077A0)
  70A02E  mov eax,ebx; pop esi; add esp,5Ch; ret

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

import x87model as X

GUARD = 0x40
WIN = 0x1000
M32 = 0xFFFFFFFF
NAMES = ['CShadows::ShadowRenderTriangleCB']
ENTRIES = (0x709CF0,)
KEYS = [['dot_pos_exit', 'dot_nonpos', 'dot_nan', 'g1_exit', 'g2_exit', 'g3_exit', 'g4_exit', 'g5_exit', 'drawn', 'fade', 'nofade', 'alpha_zero_neg', 'alpha_pos', 'alpha_nan',
         'z_nan', 'x_snan_quieted', 'overlap_in', 'edge_guard']]
ZERO = ('f', Fraction(0), 0)
ONE = ('f', Fraction(1), 0)
C006 = 0x3D75C28F   # 0.06f (dword de 0x859934)


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def prng(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', prng(seed, i // 4))
    return bytearray(out[:nbytes])


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = prng(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def lt0(b):
    return X.gt(ZERO, X.load(b))


def gt1(b):
    return X.gt(X.load(b), ONE)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def trunc_low_byte(v):
    """_ftol2 de un valor del modelo: truncamiento a 64 bits (fuera de rango o NaN -> 0x8000000000000000); devuelve el byte bajo"""
    if v == X.NAN or v[0] == 'i':
        return 0
    f = v[1]
    t = f.numerator // f.denominator if f >= 0 else -((-f).numerator // (-f).denominator)
    if t >= (1 << 63) or t < -(1 << 63):
        return 0
    return t & 0xFF


def alpha(zbits, sv):
    """alfa del vertice con fade: 1.0f - z*z (24 bits) < 0 -> 0; si no byte bajo de trunc(sv * resto)"""
    z = X.load(zbits)
    rem = X.sub(ONE, X.mul(z, z))
    if rem != X.NAN and X.gt(ZERO, rem):
        return 0
    prod = X.mul(('f', Fraction(sv), 0), rem)
    return trunc_low_byte(prod)


def expected(inp):
    eff = inp['effective']
    n, at, tri = eff['normal'], eff['at'], eff['tri']
    a = inp['a']
    world, proj = inp['world'], inp['proj']
    sv = eff['shadow_dword'] & 0xFF
    fade = eff['fade_dword']
    ptag = eff['tags'][2]
    ttag = eff['tags'][1]
    D = Mem(a[2], a[15])
    D.snap()
    marks = []
    events = []
    # llamada 1: triangulo -> mundo, matriz = parametros + 0x58
    events.append([0x10, 3, ttag, ptag + 0x58, 0, 1] + [x & M32 for x in tri])
    dot = X.add(X.add(X.mul(X.load(at[1]), X.load(n[1])), X.mul(X.load(at[2]), X.load(n[2]))), X.mul(X.load(at[0]), X.load(n[0])))
    if dot == X.NAN:
        marks.append('dot_nan')
    if any(is_nan_bits(w) for w in world):
        marks.append('z_nan' if is_nan_bits(world[2]) else 'z_nan')
    if X.gt(dot, ZERO):
        marks.append('dot_pos_exit')
        return finish(D, events, ttag, marks, inp)
    marks.append('dot_nonpos')
    # llamada 2: mundo (pila) -> proyectado, matriz = parametros + 0x0C
    events.append([0x11, 3, 0xFFFFFFFF, ptag + 0x0C, 1, 1] + [x & M32 for x in world])
    px = [proj[0], proj[3], proj[6]]
    py = [proj[1], proj[4], proj[7]]
    pz = [proj[2], proj[5], proj[8]]
    groups = [('g1_exit', [not lt0(z) for z in pz]), ('g2_exit', [not lt0(x) for x in px]), ('g3_exit', [not gt1(x) for x in px]),
              ('g4_exit', [not lt0(y) for y in py]), ('g5_exit', [not gt1(y) for y in py])]
    for name, ok in groups:
        if not any(ok):
            marks.append(name)
            return finish(D, events, ttag, marks, inp)
    marks.append('drawn')
    events.append([0x20, 3, 3, 1, 1])
    ia, va = a[9], a[10]
    colors = []
    if fade != 0:
        marks.append('fade')
        for k in range(3):
            al = alpha(pz[k], sv)
            colors.append((al * 0x01010101) & M32)
            if is_nan_bits(pz[k]) or X.load(pz[k]) == X.NAN:
                marks.append('alpha_nan')
            elif al == 0:
                marks.append('alpha_zero_neg')
            else:
                marks.append('alpha_pos')
    else:
        marks.append('nofade')
        colors = [(sv * 0x01010101) & M32] * 3
    for k in range(3):
        base = va + 0x24 * k
        wx, wy, wz = world[3 * k], world[3 * k + 1], world[3 * k + 2]
        # solo la x del vertice 0 pasa por la x87 (fld/fstp: un NaN senalizador sale silenciado); la de los vertices 1 y 2 es una copia de 32 bits
        if k == 0 and is_nan_bits(wx) and not (wx & 0x400000):
            marks.append('x_snan_quieted')
        qx = wx | 0x400000 if (k == 0 and is_nan_bits(wx)) else wx
        z2 = X.bits(X.add(X.load(wz), X.load(C006))) if not X.load(wz) == X.NAN else (wz | 0x400000)
        D.wr(base + 0x00, 4, qx)
        D.wr(base + 0x04, 4, wy)
        D.wr(base + 0x08, 4, z2)
        D.wr(base + 0x18, 4, colors[k])
        D.wr(base + 0x1C, 4, proj[3 * k])
        D.wr(base + 0x20, 4, proj[3 * k + 1])
    D.wr(ia + 0, 2, 0)
    D.wr(ia + 2, 2, 1)
    D.wr(ia + 4, 2, 2)
    events.append([0x30])
    return finish(D, events, ttag, marks, inp)


def finish(D, events, ttag, marks, inp):
    a = inp['a']
    if a[1] == 2:
        marks.append('overlap_in')
    if a[1] == 3:
        marks.append('edge_guard')
    return {'events': events, 'changes': {'A': [], 'B': [], 'C': [], 'D': D.changes()}, 'gchanges': [], 'ret': ttag, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
