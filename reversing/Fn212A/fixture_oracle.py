"""Oraculo independiente (Python) de Fn212A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 212A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) y una x87 de 24 bits
(productos redondeados a 24 bits de mantisa con exponente ampliado; guardado a float con denormales; NaN tranquilizados; 0*inf = NaN indefinido).

Desensamblado del EXE (sobre el gta-sa.exe canonico, dumpbin /disasm).
   0,1  0x5D94B0 y 0x5D9510: push esi; esi = ecx; si [esi+8] > 0 (con signo): si byte [esi+10h] != 0: operator delete 8214BD([esi]) y 8214BD([esi+4]); [esi], [esi+4], [esi+8], [esi+0Ch] = 0;
        si byte [esp+8] & 1: operator delete 8214BD(esi); eax = esi; ret 4
   2    0x5DA640: 7F9FF0(0); 7F9FB0([mesh+10h]); si [header+8] != 0: 7FA320([header+0Ch], [mesh+14h], 0, [mesh+18h], [mesh+1Ch], [mesh+20h]) si no 7FA360([header+0Ch], [mesh+14h], [mesh+20h]); las lecturas de header y mesh
        van DESPUES de las dos primeras llamadas
   3    0x5DA6A0: byte bajo de flags & 84h: con textura 7FDE70(tex, 0) y 7FC340(0,1,4) (0,2,2) (0,3,0) (0,4,4) (0,5,2) (0,6,0); sin textura 7FDE70(0, 0), 7FC340(0,1,3) (0,3,0) (0,4,3) (0,6,0); luego el dibujo de 0x5DA640 en linea
   4    0x5DA790: ver la funcion `env_mat` (material D3D9 global 0x8D12D8; x87 de 24 bits; estados 7FC2D0; 7FC430; eax = 1)
   5    0x5DAAC0: 7FC4D0(8D131Ch, 8D1328h, 20h); ret
   6..18  constructores de CPool (0x550180, ...): edi = n; eax = 821195(n * tamano) (32 bits); [esi] = eax; eax = 821195(n); [esi+4] = eax; byte [esi+10h] = 1; [esi+8] = n; [esi+0Ch] = -1;
        para i en 0..n-1 (con signo): byte [[esi+4]+i] = 80h; eax = esi; ret 8
  19..31  cuerpos de destructor de CPool (0x5501E0, ...): si [esi+8] > 0 (con signo): si byte [esi+10h] != 0: delete[] 8213AE([esi]) y 8213AE([esi+4]); [esi], [esi+4], [esi+8], [esi+0Ch] = 0; ret

Los falsos anotan: [0x8214BD o 0x8213AE, puntero (etiqueta o crudo)]; [0x821195, tamano, dword [pool+0], dword [pool+4]]; [0x7F9FF0 o 0x7F9FB0, argumento]; [0x7FA320, 6 argumentos]; [0x7FA360, 3];
[0x7FDE70, textura, etapa]; [0x7FC340, etapa, tipo, valor]; [0x7FC2D0, estado, valor]; [0x7FC430, los 17 dwords del material]; [0x7FC4D0, propiedades, color, banderas].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
POOLS = ['PtrNodeSingleLink', 'PtrNodeDoubleLink', 'EntryInfoNode', 'Ped', 'Vehicle', 'Building', 'Object', 'Dummy', 'ColModel', 'Task', 'Event', 'PointRoute', 'PatrolRoute']
NAMES = (['CCustomCarEnvMapPipeline::PoolScalarDeletingDestructorB', 'CCustomCarEnvMapPipeline::PoolScalarDeletingDestructorC', 'CCustomCarEnvMapPipeline::RxD3D9InstanceDataRender',
          'CCustomCarEnvMapPipeline::RxD3D9InstanceDataRenderLighting', 'CCustomCarEnvMapPipeline::SetEnvMapMaterial', 'CCustomCarEnvMapPipeline::SetDefaultSurfaceProperties']
         + ['CPools::%sPoolConstructor' % p for p in POOLS] + ['CPools::%sPoolDestructorBody' % p for p in POOLS])
ENTRIES = (0x5D94B0, 0x5D9510, 0x5DA640, 0x5DA6A0, 0x5DA790, 0x5DAAC0,
           0x550180, 0x550250, 0x550320, 0x5503F0, 0x5504C0, 0x550570, 0x550640, 0x5506F0, 0x5507C0, 0x550890, 0x550960, 0x550A30, 0x550B00,
           0x5501E0, 0x5502B0, 0x550380, 0x550450, 0x550520, 0x5505D0, 0x5506A0, 0x550750, 0x550820, 0x5508F0, 0x5509C0, 0x550A90, 0x550B60)
ELEM = (8, 12, 0x14, 0x7C4, 0xA18, 0x38, 0x19C, 0x38, 0x30, 0x80, 0x44, 0x64, 0x1A4)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SCALARDEL, RENDER, RENDERLIT, ENVMAT, DEFSURF, POOLCTOR, DTORBODY = range(7)
KIND = [SCALARDEL, SCALARDEL, RENDER, RENDERLIT, ENVMAT, DEFSURF] + [POOLCTOR] * 13 + [DTORBODY] * 13
MARKER, MATERIAL, AMBIENT_SAT = 0xA9B0B0, 0x8D12D8, 0x8E2418
INV255 = 0x3B808081

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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


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


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c, stl):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        # las funciones de la STL rellenan la arena B siempre con datos aleatorios (los demas rellenos harian invisibles las copias)
        self.mem = tuple(Mem(0 if (stl and i == 1) else a[2], a[92 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def getg(self, addr, n):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


# ------------------------------------------------------------------------------------------------ x87 de 24 bits (modelo independiente)
# valores de registro: ('z', signo), ('i', signo), ('n', signo, significando de 64 bits), ('f', signo, Fraction > 0)
def _rne(x):
    """redondeo al entero mas cercano, empates al par (x Fraction >= 0)"""
    q, r = divmod(x.numerator, x.denominator)
    twice = 2 * r
    if twice > x.denominator or (twice == x.denominator and (q & 1)):
        q += 1
    return q


def _log2floor(x):
    e = x.numerator.bit_length() - x.denominator.bit_length()
    while Fraction(2) ** e > x:
        e -= 1
    while Fraction(2) ** (e + 1) <= x:
        e += 1
    return e


def ld32(bits):
    s, e, fr = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 255:
        return ('i', s) if fr == 0 else ('n', s, (1 << 63) | (1 << 62) | (fr << 40))
    if e == 0:
        return ('z', s) if fr == 0 else ('f', s, Fraction(fr) * Fraction(1, 1 << 149))
    return ('f', s, Fraction((1 << 23) | fr) * (Fraction(2) ** (e - 127 - 23)))


def fild(n):
    return ('z', 0) if n == 0 else ('f', 0, Fraction(n))


def mul(a, b):
    if a[0] == 'n' or b[0] == 'n':
        if a[0] == 'n' and b[0] == 'n':
            return a if a[2] >= b[2] else b
        return a if a[0] == 'n' else b
    if {a[0], b[0]} == {'i', 'z'}:
        return ('n', 1, 0xC000000000000000)
    s = a[1] ^ b[1]
    if a[0] == 'i' or b[0] == 'i':
        return ('i', s)
    if a[0] == 'z' or b[0] == 'z':
        return ('z', s)
    x = a[2] * b[2]
    e = _log2floor(x)
    q = Fraction(2) ** (e - 23)
    return ('f', s, _rne(x / q) * q)


def st32(v):
    if v[0] == 'n':
        return (v[1] << 31) | 0x7F800000 | ((v[2] >> 40) & 0x7FFFFF)
    if v[0] == 'i':
        return (v[1] << 31) | 0x7F800000
    if v[0] == 'z':
        return v[1] << 31
    s, x = v[1], v[2]
    e = max(_log2floor(x), -126)
    q = Fraction(2) ** (e - 23)
    m = _rne(x / q)
    val = m * q
    if val >= Fraction(2) ** 128:
        return (s << 31) | 0x7F800000
    if m == 0:
        return s << 31
    if m < (1 << 23):
        return (s << 31) | m
    e2 = _log2floor(val)
    return (s << 31) | ((e2 + 127) << 23) | int((val / (Fraction(2) ** e2) - 1) * (1 << 23))


def is_special(bits):
    return (bits >> 23) & 0xFF == 255


def is_denormal(bits):
    return (bits >> 23) & 0xFF == 0 and bits & 0x7FFFFF != 0



COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SCALARDEL:
        _k += ['size_pos', 'size_notpos', 'owns', 'owns_not', 'storage_freed', 'self_freed', 'self_kept']
    elif _kind == DTORBODY:
        _k += ['size_pos', 'size_notpos', 'owns', 'owns_not', 'storage_freed']
    elif _kind == POOLCTOR:
        _k += ['cap_pos', 'cap_zero', 'cap_neg', 'cap_wraps']
    elif _kind in (RENDER, RENDERLIT):
        _k += ['indexed', 'not_indexed', 'mut_ps', 'mut_vs', 'no_mut']
        if _kind == RENDERLIT:
            _k += ['textured', 'untextured']
    elif _kind == ENVMAT:
        _k += ['mod', 'plain', 'prelit', 'unlit', 'color_white', 'ambient_one', 'ambient_scaled', 'special_float', 'denormal_input', 'zero_channel']
    elif _kind == DEFSURF:
        _k += ['noop']
    KEYS[_fn] = _k


def draw(W, hdr, mesh, events, a, marks):
    """el dibujo de 0x5DA640 (y su copia en linea de 0x5DA6A0): las lecturas van despues de las dos primeras llamadas"""
    events.append([0x7F9FF0, 0])
    if a[13] & 1:
        W.wr(hdr + 8, 4, a[14]); W.wr(hdr + 0xC, 4, a[15]); W.wr(mesh + 0x10, 4, a[16]); W.wr(mesh + 0x14, 4, a[17])
    events.append([0x7F9FB0, W.tagv(W.rd(mesh + 0x10))])
    if a[13] & 2:
        W.wr(mesh + 0x18, 4, a[18]); W.wr(mesh + 0x1C, 4, a[19]); W.wr(mesh + 0x20, 4, a[20]); W.wr(hdr + 8, 4, a[21])
    marks.append('mut_ps' if a[13] & 1 else 'no_mut')
    if a[13] & 2:
        marks.append('mut_vs')
    if W.rd(hdr + 8) != 0:
        marks.append('indexed')
        events.append([0x7FA320, W.rd(hdr + 0xC), W.rd(mesh + 0x14), 0, W.rd(mesh + 0x18), W.rd(mesh + 0x1C), W.rd(mesh + 0x20)])
    else:
        marks.append('not_indexed')
        events.append([0x7FA360, W.rd(hdr + 0xC), W.rd(mesh + 0x14), W.rd(mesh + 0x20)])


def env_mat(a, color, props_amb, props_spec, props_diff):
    """SetEnvMapMaterial: devuelve (eventos, material de 17 dwords modificado, marcas); el material previo es a[16..32]"""
    m = list(a[16:33])
    flags, spec, power = a[9], a[10], a[11]
    sat = [a[12], a[13], a[14]]
    events, marks = [], []
    m[8] = m[9] = m[10] = spec
    m[16] = power
    bl = flags & 0x48
    ch = [(color >> (8 * i)) & 0xFF for i in range(4)]
    inv = ld32(INV255)
    if (bl & 0x40) and color != 0xFFFFFFFF:
        marks.append('mod')
        d = mul(ld32(props_diff), inv)
        m[0] = st32(mul(fild(ch[0]), d)); m[1] = st32(mul(fild(ch[1]), d)); m[2] = st32(mul(fild(ch[2]), d))
        m[3] = st32(mul(fild(ch[3]), inv))
        amb = ld32(st32(mul(ld32(props_amb), inv)))
        lit = [st32(mul(mul(fild(ch[k]), ld32(sat[k])), amb)) for k in range(3)]
        if bl & 8:
            marks.append('prelit')
            events += [[0x7FC2D0, 0x8B, (ch[3] << 24) | (ch[0] << 16) | (ch[1] << 8) | ch[2]], [0x7FC2D0, 0x8D, 1], [0x7FC2D0, 0x93, 1], [0x7FC2D0, 0x94, 0]]
            m[4] = m[5] = m[6] = 0
            m[12], m[13], m[14] = lit
        else:
            marks.append('unlit')
            events += [[0x7FC2D0, 0x8B, M32], [0x7FC2D0, 0x8D, 0], [0x7FC2D0, 0x93, 0], [0x7FC2D0, 0x94, 0]]
            m[4], m[5], m[6] = lit
            m[12] = m[13] = m[14] = 0
    else:
        marks.append('plain')
        if color == 0xFFFFFFFF and (bl & 0x40):
            marks.append('color_white')
        dv = st32(ld32(props_diff))
        m[0] = m[1] = m[2] = dv
        m[3] = 0x3F800000
        events += [[0x7FC2D0, 0x8B, M32], [0x7FC2D0, 0x93, 0]]
        if bl & 8:
            marks.append('prelit')
            events += [[0x7FC2D0, 0x8D, 1], [0x7FC2D0, 0x94, 1]]
        else:
            marks.append('unlit')
            events += [[0x7FC2D0, 0x8D, 0], [0x7FC2D0, 0x94, 0]]
            m[12] = m[13] = m[14] = 0
        if props_amb == 0x3F800000:
            marks.append('ambient_one')
            m[4], m[5], m[6] = sat
        else:
            marks.append('ambient_scaled')
            m[4], m[5], m[6] = [st32(mul(ld32(sat[k]), ld32(props_amb))) for k in range(3)]
    events.append([0x7FC430] + m)
    if any(is_special(x) for x in (props_amb, props_diff, props_spec, a[10], a[11], *sat)):
        marks.append('special_float')
    if any(is_denormal(x) for x in (props_amb, props_diff, *sat)):
        marks.append('denormal_input')
    if 0 in ch[:3]:
        marks.append('zero_channel')
    return events, m, marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c, False)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k in (SCALARDEL, DTORBODY):
        p0 = a[5] if a[4] == 0 else (W.raw(1, a[9]) if a[4] == 1 else 0)
        p1 = a[7] if a[6] == 0 else (W.raw(2, a[11]) if a[6] == 1 else 0)
        A.wr(o + 0, 4, p0); A.wr(o + 4, 4, p1); A.wr(o + 8, 4, a[8]); A.wr(o + 0xC, 4, a[13]); A.wr(o + 0x10, 1, a[12] & 0xFF)
        W.snap()
        dele = 0x8214BD if k == SCALARDEL else 0x8213AE
        if s32(a[8]) > 0:
            marks.append('size_pos')
            if a[12] & 0xFF:
                marks += ['owns', 'storage_freed']
                events.append([dele, W.tagv(p0)])
                events.append([dele, W.tagv(p1)])
            else:
                marks.append('owns_not')
            A.wr(o + 0, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        else:
            marks.append('size_notpos')
        if k == SCALARDEL:
            if a[10] & 1:
                events.append([dele, W.tagv(this)])
                marks.append('self_freed')
            else:
                marks.append('self_kept')
            ret = W.tagv(this)
    elif k == POOLCTOR:
        cap = s32(a[4])
        pre0, pre4 = A.rd(o, 4), A.rd(o + 4, 4)
        ret1 = W.raw(1, a[5]) if cap > 0 else a[8]
        ret2 = W.raw(2, a[6]) if cap > 0 else a[9]
        W.snap()
        elem = ELEM[fn - 6]
        events.append([0x821195, (cap * elem) & M32, pre0, pre4])
        A.wr(o + 0, 4, ret1)
        events.append([0x821195, cap & M32, ret1, pre4])
        A.wr(o + 4, 4, ret2)
        A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, cap & M32); A.wr(o + 0xC, 4, M32)
        for i in range(cap):
            W.wr(ret2 + i, 1, 0x80)
        ret = W.tagv(this)
        marks.append('cap_pos' if cap > 0 else ('cap_zero' if cap == 0 else 'cap_neg'))
        if cap * elem > M32 or cap * elem < 0:
            marks.append('cap_wraps')
    elif k in (RENDER, RENDERLIT):
        mesh = W.raw(1, a[5])
        A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7])
        B.wr(a[5] + 0x10, 4, a[8])
        for i in range(4):
            B.wr(a[5] + 0x14 + 4 * i, 4, a[9 + i])
        W.snap()
        if k == RENDERLIT:
            tex = W.raw(3, a[25]) if a[24] == 1 else a[23]
            if a[22] & 0x84:
                marks.append('textured')
                events.append([0x7FDE70, W.tagv(tex), 0])
                for t, v in ((1, 4), (2, 2), (3, 0), (4, 4), (5, 2), (6, 0)):
                    events.append([0x7FC340, 0, t, v])
            else:
                marks.append('untextured')
                events.append([0x7FDE70, 0, 0])
                for t, v in ((1, 3), (3, 0), (4, 3), (6, 0)):
                    events.append([0x7FC340, 0, t, v])
        draw(W, this, mesh, events, a, marks)
    elif k == ENVMAT:
        for i in range(3):
            B.wr(a[5] + 4 * i, 4, (a[6], a[7], a[8])[i])
        for i in range(17):
            W.setg(MATERIAL + 4 * i, 4, a[16 + i])
        for i in range(4):
            W.setg(AMBIENT_SAT + 4 * i, 4, a[12 + i])
        gorder = [(MATERIAL, 0x44), (AMBIENT_SAT, 0x10)]
        W.snap()
        evs, m, mk = env_mat(a, a[3], a[6], a[7], a[8])
        events += evs
        marks += mk
        for i in range(17):
            W.setg(MATERIAL + 4 * i, 4, m[i])
        ret = 1
    elif k == DEFSURF:
        W.setg(MARKER, 1, 0x5A)
        gorder = [(MARKER, 1)]
        W.snap()
        events.append([0x7FC4D0, 0x8D131C, 0x8D1328, 0x20])
        marks.append('noop')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
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
