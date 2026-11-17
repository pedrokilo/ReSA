"""Oraculo independiente (Python) de Fn214A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 7 funciones del lote 214A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) y una x87 de 24 bits
(sumas, productos, cociente y raiz redondeados a 24 bits de mantisa con exponente ampliado; guardado a float con denormales; NaN tranquilizados; 0*inf = NaN indefinido).

Desensamblado del EXE (sobre el gta-sa.exe canonico, dumpbin /disasm).
   0  0x45C4B0 CPlayerPedData::operator= (thiscall, origen en la pila, ret 4): eax = this; copia en orden creciente dwords +0..+0x1C, bytes +0x20/+0x21, dwords +0x24..+0x30; la bandera de +0x34 se arma en
        cuatro pasos que RELEEN el origen: v = ((viejo ^ ESI) & 0x200) ^ ESI; luego para las mascaras 0x400, 0x800, 0x1000: v = ((origen[+0x34] ^ v) & mascara) ^ v (ESI = el del llamador); dwords +0x38/+0x3C, bytes
        +0x40..+0x43, dwords +0x44..+0x70, +0x74..+0x7C, +0x80, bytes +0x84..+0x86, words +0x88/+0x8A, bytes +0x8C..+0x8E, dwords +0x90..+0x9C, +0xA0 (en dos pasos: acaba igual al origen), +0xA4, +0xA8. No copia +0x22/+0x23,
        +0x87, +0x8F.
   1  0x45DEF0 CPlayerInfo::operator= (thiscall, ret 4; ESI = this): [esi] = [origen]; 0x45C4B0 con ECX = this + 4 y origen + 4; luego dwords +0xB0..+0xD0, bytes +0xD4/+0xD5, dword +0xD8, bytes +0xDC..+0xDF, dwords
        +0xE0..+0x12C, word +0x130, dwords +0x134..+0x140, word +0x144, dword +0x148, bytes +0x14C..+0x154, word +0x156, dwords +0x158..+0x160, bucle de 32 bytes desde +0x164, dword +0x184, byte +0x188, dword +0x18C.
        No copia +0xD6/+0xD7, +0x155, +0x189..+0x18B. eax = this.
   2  0x45CD10 guardado del paquete de tren (thiscall ECX = paquete, veh e indice en la pila, ret 8): byte [paquete] = 0x12; 0x45B730(veh, indice) con ECX = paquete; despues [+0x34] = [veh+0x5A4], [+0x38] = [veh+0x5A8],
        [+0x3C] = [veh+0x5AC], byte [+0x48] = byte [veh+0x5C0], [+0x40] = [+0x44] = 0; si [veh+0x5D0] != 0: [+0x40] = ((veh+0x5D0 - [[0xB74494]]) / 0xA18, con signo, truncado) + 1; igual con [veh+0x5D4] y [+0x44].
   3  0x45CDB0 lectura del paquete de tren (thiscall ECX = paquete, tren e interpolacion en la pila, ret 8): 0x45BA90(tren, interp) con ECX = paquete; [tren+0x5A4] = [+0x34], +0x5A8 = [+0x38], +0x5AC = [+0x3C], byte
        +0x5C0 = byte [+0x48]; si [+0x40] != 0: si [tren+0x5D0] != 0: 0x571A00 con ECX = viejo y &[tren+0x5D0]; slot = [0x97F67C + 4 * ref]; pool = [0xB74494]; si byte [[pool+4] + slot] & 0x80: nulo; si no [pool] + slot * 0xA18;
        [tren+0x5D0] = eso; si no es nulo 0x571B70 con ECX = eso y &[tren+0x5D0]; igual con [+0x44] y +0x5D4 (el signo del byte en vez del bit 7: igual).
   4  0x46B1A0 AddToSuppressedCarModels (cdecl): si algun [0xA44940 + 4 * i] == modelo (i < 40, indice de 8 bits): nada; si no la primera i < 40 con [..] == -1, o 40 si no hay: [0xA44940 + 4 * i] = modelo.
   5  0x48B570 SetMissionDoesntRequireAnim (cdecl): [esp+4] += 0x63E7 y salta a 0x409C90.
   6  0x4D1610 CQuaternion::Normalise (thiscall ECX = {x, y, z, w}): suma ((x*x + y*y) + z*z) + w*w en x87 de 24 bits sin guardar a float; si es igual a 0 (comparacion ordenada): solo w = 1.0f; si no (NaN
        incluido) r = 1.0f / sqrt(suma) y cada componente (x, y, z, w) = float(r * componente).

Los falsos anotan: [0x45B730, paquete, vehiculo, indice, byte +0, dwords +0x34, +0x40, +0x44 al entrar]; [0x45BA90, paquete, tren, bits de la interpolacion, dwords +0x5A4, +0x5A8, +0x5AC y byte +0x5C0 al entrar];
[0x571A00 o 0x571B70, entidad, direccion de la referencia, valor de la referencia al llamar]; [0x409C90, argumento].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF
NAMES = ['CPlayerPedData::operator=', 'CPlayerInfo::operator=', 'CReplay::StoreTrainUpdate', 'tReplayTrainBlock::ExtractTrainUpdateData', 'CTheScripts::AddToSuppressedCarModels',
         'CStreaming::SetMissionDoesntRequireAnim', 'CQuaternion::Normalise']
ENTRIES = (0x45C4B0, 0x45DEF0, 0x45CD10, 0x45CDB0, 0x46B1A0, 0x48B570, 0x4D1610)
SPAN = (0xAC, 0x190, 0x4C, 0x4C, 4, 4, 0x10)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
PEDCOPY, INFOCOPY, STORETRAIN, EXTRACTTRAIN, SUPPRESS, SETANIM, QUAT = range(7)
KIND = [PEDCOPY, INFOCOPY, STORETRAIN, EXTRACTTRAIN, SUPPRESS, SETANIM, QUAT]
MARKER, POOLPTR, CONVTABLE, CONVBYTES, SUPPRESSED, SUPPRESSEDBYTES = 0xA9B0B0, 0xB74494, 0x97F67C, 0x1BC, 0xA44940, 0xC0
CONV_ENTRIES = 110
# regiones globales (en el orden de Setup/regionsOf del banco) que toca cada funcion
GORDER = {0: [(MARKER, 1)], 1: [(MARKER, 1)], 2: [(MARKER, 1), (POOLPTR, 4)], 3: [(MARKER, 1), (POOLPTR, 4), (CONVTABLE, CONVBYTES)], 4: [(MARKER, 1), (SUPPRESSED, SUPPRESSEDBYTES)], 5: [(MARKER, 1)], 6: [(MARKER, 1)]}
# arenas aleatorias siempre (las copias serian invisibles con rellenos constantes): indices de arena por funcion
FORCED = {0: (1,), 1: (1,), 2: (1, 2), 3: (1, 2)}

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
        # las arenas de `stl` (indices) se rellenan siempre con datos aleatorios (los demas rellenos harian invisibles las copias)
        self.mem = tuple(Mem(0 if i in stl else a[2], a[92 + i]) for i in range(4))
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





# ------------------------------------------------------------------------------------------------ x87 de 24 bits: suma, cociente y raiz (modelo independiente; mul, st32 y ld32 estan arriba)
def _round24(x):
    """x Fraction > 0 redondeada a 24 bits de mantisa (al mas cercano, empate al par), exponente sin limite"""
    e = _log2floor(x)
    q = Fraction(2) ** (e - 23)
    return _rne(x / q) * q


def _nan_pick(a, b):
    """dos operandos con al menos un NaN: gana el de mayor significando; con el mismo, el positivo"""
    if a[0] == 'n' and b[0] == 'n':
        if a[2] != b[2]:
            return a if a[2] > b[2] else b
        return a if a[1] <= b[1] else b
    return a if a[0] == 'n' else b


def _signed(v):
    return v[2] if v[1] == 0 else -v[2]


def add(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan_pick(a, b)
    if a[0] == 'i' or b[0] == 'i':
        if a[0] == 'i' and b[0] == 'i':
            return a if a[1] == b[1] else ('n', 1, 0xC000000000000000)
        return a if a[0] == 'i' else b
    if a[0] == 'z' and b[0] == 'z':
        return ('z', a[1]) if a[1] == b[1] else ('z', 0)
    if a[0] == 'z':
        return b
    if b[0] == 'z':
        return a
    s = _signed(a) + _signed(b)
    if s == 0:
        return ('z', 0)
    return ('f', 0 if s > 0 else 1, _round24(abs(s)))


def div(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan_pick(a, b)
    s = a[1] ^ b[1]
    if a[0] == 'i':
        return ('n', 1, 0xC000000000000000) if b[0] == 'i' else ('i', s)
    if b[0] == 'i':
        return ('z', s)
    if b[0] == 'z':
        return ('n', 1, 0xC000000000000000) if a[0] == 'z' else ('i', s)
    if a[0] == 'z':
        return ('z', s)
    return ('f', s, _round24(a[2] / b[2]))


def sqrt(a):
    if a[0] == 'n':
        return a
    if a[0] == 'z':
        return a
    if a[1] == 1:
        return ('n', 1, 0xC000000000000000)
    if a[0] == 'i':
        return a
    x = a[2]
    e = _log2floor(x) // 2
    q = Fraction(2) ** (e - 23)
    # m = floor(sqrt(x) / q) con enteros: sqrt(x / q^2)
    y = x / (q * q)
    m = isqrt(y.numerator // y.denominator)
    # candidatos m, m + 1: se elige el mas cercano comparando cuadrados (sin empates)
    if m < (1 << 23) or m >= (1 << 24):
        e += 1 if m >= (1 << 24) else -1
        q = Fraction(2) ** (e - 23)
        y = x / (q * q)
        m = isqrt(y.numerator // y.denominator)
    half = Fraction(2 * m + 1, 2)
    if y > half * half:
        m += 1
    return ('f', 0, m * q)


def vehicle_index(x32):
    """el cociente con signo por 0xA18 que calcula el EXE con la constante magica 0x6572EC3 (imul; sar 6; + bit de signo); devuelve indice + 1 como dword"""
    prod = s32(x32) * 0x6572EC3
    edx = s32(prod >> 32)
    edx >>= 6
    sign = (edx & M32) >> 31
    return (sign + edx + 1) & M32


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (PEDCOPY, INFOCOPY):
        _k += ['src_b', 'src_a', 'src_self']
    elif _kind == STORETRAIN:
        _k += ['prev_null', 'prev_pool', 'prev_raw', 'next_null', 'next_pool', 'next_raw', 'mut', 'no_mut', 'index_negative']
    elif _kind == EXTRACTTRAIN:
        _k += ['prev_ref', 'next_ref', 'prev_old', 'next_old', 'slot_null', 'slot_found', 'mut', 'no_mut', 'register_called']
    elif _kind == SUPPRESS:
        _k += ['present', 'added_free', 'full_oob', 'minus_one_arg', 'present_beyond']
    elif _kind == SETANIM:
        _k += ['small', 'negative', 'wraps']
    elif _kind == QUAT:
        _k += ['zero_sum', 'normalised', 'nan_in', 'inf_in', 'denormal_in', 'huge', 'tiny', 'typical']
    KEYS[_fn] = _k


def put_words(mem, off, size, seed):
    """lo que escribe FillWords(seed) en [off, off + size) de una arena (dwords little-endian rnd(seed, i / 4))"""
    for i in range(0, size, 4):
        mem.wr(off + i, 4, rnd(seed, i // 4))


# Mapa de copia de CPlayerInfo::operator= (desplazamiento, bytes) en el orden del EXE, SIN la parte de CPlayerPedData (+4..+0xAF)
INFO_COPIES = ([(0, 4)] + [(o, 4) for o in range(0xB0, 0xD4, 4)] + [(0xD4, 1), (0xD5, 1), (0xD8, 4), (0xDC, 1), (0xDD, 1), (0xDE, 1), (0xDF, 1)]
               + [(o, 4) for o in range(0xE0, 0x130, 4)] + [(0x130, 2)] + [(o, 4) for o in range(0x134, 0x144, 4)] + [(0x144, 2), (0x148, 4)] + [(o, 1) for o in range(0x14C, 0x155)]
               + [(0x156, 2)] + [(0x158, 4), (0x15C, 4), (0x160, 4)] + [(0x164 + k, 1) for k in range(32)] + [(0x184, 4), (0x188, 1), (0x18C, 4)])
# Mapa de copia de CPlayerPedData::operator= (desplazamiento, bytes) SIN la bandera +0x34 ni +0xA0
PED_COPIES = ([(o, 4) for o in range(0, 0x20, 4)] + [(0x20, 1), (0x21, 1)] + [(o, 4) for o in range(0x24, 0x34, 4)] + [(0x38, 4), (0x3C, 4)] + [(0x40 + k, 1) for k in range(4)]
              + [(o, 4) for o in range(0x44, 0x80, 4)] + [(0x80, 4), (0x84, 1), (0x85, 1), (0x86, 1), (0x88, 2), (0x8A, 2), (0x8C, 1), (0x8D, 1), (0x8E, 1)]
              + [(o, 4) for o in range(0x90, 0xA0, 4)] + [(0xA4, 4), (0xA8, 4)])


def ped_copy(W, dst, src, esi):
    """CPlayerPedData::operator= en el orden del EXE (con las relecturas del origen)"""
    old = W.rd(dst + 0x34)
    for off, n in [c for c in PED_COPIES if c[0] < 0x34]:
        W.wr(dst + off, n, W.rd(src + off, n))
    v = (((old ^ esi) & 0x200) ^ esi) & M32
    W.wr(dst + 0x34, 4, v)
    for mask in (0x400, 0x800, 0x1000):
        v = (((W.rd(src + 0x34) ^ v) & mask) ^ v) & M32
        W.wr(dst + 0x34, 4, v)
    for off, n in [c for c in PED_COPIES if c[0] >= 0x34 and c[0] < 0xA0]:
        W.wr(dst + off, n, W.rd(src + off, n))
    # +0xA0 en dos pasos (cada uno relee el origen); acaba igual al origen
    s = W.rd(src + 0xA0)
    v = (W.rd(dst + 0xA0) ^ (((W.rd(dst + 0xA0) ^ s) & 0x7FFFFFFF))) & M32
    W.wr(dst + 0xA0, 4, v)
    v2 = (W.rd(src + 0xA0) ^ ((W.rd(src + 0xA0) ^ v) & 0x7FFFFFFF)) & M32
    W.wr(dst + 0xA0, 4, v2)
    for off, n in [c for c in PED_COPIES if c[0] > 0xA0]:
        W.wr(dst + off, n, W.rd(src + off, n))


def draw_unused():
    return None


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c, FORCED.get(fn, ()))
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    W.setg(MARKER, 1, 0x5A)
    if k in (PEDCOPY, INFOCOPY):
        span = SPAN[fn]
        if a[4] == 0:
            src = W.raw(1, a[5])
            marks.append('src_b')
        else:
            put_words(A, a[5], span, a[6])
            src = W.raw(0, a[5])
            marks.append('src_a' if a[4] == 1 else 'src_self')
        W.snap()
        if k == PEDCOPY:
            ped_copy(W, this, src, (this - 4) & M32)
        else:
            W.wr(this, 4, W.rd(src))
            ped_copy(W, (this + 4) & M32, (src + 4) & M32, this)   # ESI = this (el CPlayerInfo)
            for off, n in INFO_COPIES[1:]:
                W.wr(this + off, n, W.rd(src + off, n))
        ret = W.tagv(this)
    elif k == STORETRAIN:
        veh = W.raw(1, a[5])
        base = W.raw(2, a[14]) if a[13] == 1 else a[14]
        for w in range(2):
            b = 7 + 3 * w
            v = 0 if a[b] == 0 else (((base + a[b + 1] * 0xA18 + a[b + 2]) & M32) if a[b] == 1 else a[b + 1])
            B.wr(a[5] + 0x5D0 + 4 * w, 4, v)
            marks.append(('prev_' if w == 0 else 'next_') + ('null', 'pool', 'raw')[a[b]])
        D.wr(a[15], 4, base)
        D.wr(a[15] + 4, 4, W.raw(2, 0x800))
        W.setg(POOLPTR, 4, W.raw(3, a[15]))
        W.snap()
        A.wr(o, 1, 0x12)
        events.append([0x45B730, W.tagv(this), W.tagv(veh), a[6], A.rd(o, 1), A.rd(o + 0x34, 4), A.rd(o + 0x40, 4), A.rd(o + 0x44, 4)])
        if a[16]:
            marks.append('mut')
            A.wr(o, 1, a[17] & 0xFF)
            for i in range(6):
                A.wr(o + 0x34 + 4 * i, 4, a[18 + i])
        else:
            marks.append('no_mut')
        A.wr(o + 0x34, 4, B.rd(a[5] + 0x5A4, 4)); A.wr(o + 0x38, 4, B.rd(a[5] + 0x5A8, 4)); A.wr(o + 0x3C, 4, B.rd(a[5] + 0x5AC, 4))
        A.wr(o + 0x48, 1, B.rd(a[5] + 0x5C0, 1))
        A.wr(o + 0x40, 4, 0); A.wr(o + 0x44, 4, 0)
        for w in range(2):
            v = B.rd(a[5] + 0x5D0 + 4 * w, 4)
            if v:
                x = (v - base) & M32
                if s32(x) < 0:
                    marks.append('index_negative')
                A.wr(o + 0x40 + 4 * w, 4, vehicle_index(x))
    elif k == EXTRACTTRAIN:
        train = W.raw(1, a[5])
        A.wr(o + 0x34, 4, a[23]); A.wr(o + 0x38, 4, a[24]); A.wr(o + 0x3C, 4, a[25]); A.wr(o + 0x48, 1, a[26])
        A.wr(o + 0x40, 4, a[4]); A.wr(o + 0x44, 4, a[6])
        for w in range(2):
            b = 7 + 2 * w
            v = 0 if a[b] == 0 else (W.raw(2, a[b + 1]) if a[b] == 1 else a[b + 1])
            B.wr(a[5] + 0x5D0 + 4 * w, 4, v)
        base = W.raw(2, a[13]) if a[12] == 1 else a[13]
        D.wr(a[14], 4, base)
        D.wr(a[14] + 4, 4, W.raw(2, a[15]))
        W.setg(POOLPTR, 4, W.raw(3, a[14]))
        for i in range(CONV_ENTRIES):
            W.setg(CONVTABLE + 4 + 4 * i, 4, table_slot(a[16], a[17], i))
        W.snap()
        events.append([0x45BA90, W.tagv(this), W.tagv(train), a[11], B.rd(a[5] + 0x5A4, 4), B.rd(a[5] + 0x5A8, 4), B.rd(a[5] + 0x5AC, 4), B.rd(a[5] + 0x5C0, 1)])
        if a[18]:
            marks.append('mut')
            B.wr(a[5] + 0x5A4, 4, a[19]); B.wr(a[5] + 0x5A8, 4, a[20]); B.wr(a[5] + 0x5AC, 4, a[21]); B.wr(a[5] + 0x5C0, 1, a[22] & 0xFF)
        else:
            marks.append('no_mut')
        B.wr(a[5] + 0x5A4, 4, A.rd(o + 0x34, 4)); B.wr(a[5] + 0x5A8, 4, A.rd(o + 0x38, 4)); B.wr(a[5] + 0x5AC, 4, A.rd(o + 0x3C, 4))
        B.wr(a[5] + 0x5C0, 1, A.rd(o + 0x48, 1))
        for w in range(2):
            ref = A.rd(o + 0x40 + 4 * w, 4)
            if ref == 0:
                continue
            marks.append('prev_ref' if w == 0 else 'next_ref')
            slotAddr = train + 0x5D0 + 4 * w
            old = B.rd(a[5] + 0x5D0 + 4 * w, 4)
            if old:
                marks.append('prev_old' if w == 0 else 'next_old')
                events.append([0x571A00, W.tagv(old), W.tagv(slotAddr), W.tagv(old)])
            slot = W.getg((CONVTABLE + 4 * ref) & M32, 4)
            pool = W.getg(POOLPTR, 4)
            objects, flags = W.rd(pool, 4), W.rd((pool + 4) & M32, 4)
            fb = W.rd((flags + slot) & M32, 1)
            ent = 0 if fb & 0x80 else (objects + s32(slot) * 0xA18) & M32
            marks.append('slot_null' if ent == 0 else 'slot_found')
            B.wr(a[5] + 0x5D0 + 4 * w, 4, ent)
            if ent:
                marks.append('register_called')
                events.append([0x571B70, W.tagv(ent), W.tagv(slotAddr), W.tagv(ent)])
    elif k == SUPPRESS:
        for i in range(48):
            W.setg(SUPPRESSED + 4 * i, 4, a[8 + i])
        W.snap()
        arr = [a[8 + i] for i in range(48)]
        model = a[56]
        if model in arr[:40]:
            marks.append('present')
        else:
            slot = 0
            while slot < 40 and arr[slot] != M32:
                slot += 1
            marks.append('added_free' if slot < 40 else 'full_oob')
            W.setg(SUPPRESSED + 4 * slot, 4, model)
        if model == M32:
            marks.append('minus_one_arg')
        if a[5] == 1:
            marks.append('present_beyond')
    elif k == SETANIM:
        W.snap()
        v = (a[4] + 0x63E7) & M32
        events.append([0x409C90, v])
        marks.append('negative' if a[4] & 0x80000000 else 'small')
        if a[4] + 0x63E7 > M32:
            marks.append('wraps')
    elif k == QUAT:
        for i in range(4):
            A.wr(o + 4 * i, 4, a[4 + i])
        W.snap()
        q = [ld32(a[4 + i]) for i in range(4)]
        s = add(add(add(mul(q[0], q[0]), mul(q[1], q[1])), mul(q[2], q[2])), mul(q[3], q[3]))
        if s[0] == 'z':
            marks.append('zero_sum')
            A.wr(o + 12, 4, 0x3F800000)
        else:
            marks.append('normalised')
            r = div(ld32(0x3F800000), sqrt(s))
            for i in range(4):
                A.wr(o + 4 * i, 4, st32(mul(r, q[i])))
        for x in a[4:8]:
            e = (x >> 23) & 0xFF
            if e == 255 and x & 0x7FFFFF:
                marks.append('nan_in')
            elif e == 255:
                marks.append('inf_in')
            elif e == 0 and x & 0x7FFFFF:
                marks.append('denormal_in')
            elif e >= 127 + 64:
                marks.append('huge')
            elif 0 < e <= 127 - 40:
                marks.append('tiny')
            elif 127 - 8 <= e <= 127:
                marks.append('typical')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(GORDER[fn]), 'ret': ret, 'marks': marks}


def table_slot(s, p, i):
    r = rnd(s, 500 + i)
    p &= 3
    if p == 0:
        return r % 110
    if p == 1:
        return M32 if r % 3 == 0 else rnd(s, 700 + i) % 110
    if p == 2:
        return (r % 0xC00 - 0x600) & M32
    return rnd(s, 700 + i) % 110 if r % 10 == 0 else M32


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
