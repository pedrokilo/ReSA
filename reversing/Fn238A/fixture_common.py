"""Utilidades compartidas del banco Fn238A: RNG, modelo de memoria de las arenas, trazado de la disposicion del mundo y aritmetica x87 a 24 bits.

El mundo sintetico (las cuatro arenas A..D de 0x2000 bytes con guardas de 0x40 y las regiones globales del juego que el banco fija) esta descrito aqui UNA vez; lo usan el generador de casos
(fixture_cases.py), el oraculo (fixture_oracle.py) y el emisor de la tabla de colaboradores para el C++ (collabs.py).
"""
import math
import struct
from fractions import Fraction

M32 = 0xFFFFFFFF
GUARD = 0x40
WIN = 0x2000


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
    return v - (1 << 32) if v & 0x80000000 else v


def PS(arena, off):
    """especificacion de puntero a una arena (el banco la resuelve con la direccion real): 0xFA000000 | arena << 16 | off"""
    return 0xFA000000 | (arena << 16) | off


def is_ps(v):
    v &= M32
    return (v >> 24) == 0xFA and ((v >> 16) & 0xFF) < 4


def tag_of(arena, off):
    """etiqueta del puntero (arena, desplazamiento): lo que el banco anota (Core.hpp Tag)"""
    return 0x10000000 + (arena << 20) + GUARD + off


# ----------------------------------------------------------------------------------------------- disposicion del mundo
# Arena A: script (this) en [0, 0x300); pools (cabecera + estados de ranura); vector de fuego; datos del jugador
A_PEDPOOL, A_PEDSLOT = 0x400, 0x420
A_VEHPOOL, A_VEHSLOT = 0x440, 0x460
A_OBJPOOL, A_OBJSLOT = 0x480, 0x4A0
A_FIRE = 0x600          # CVector devuelto por CFireManager::GetScriptFireCoords (por puntero)
A_PLAYERDATA = 0x700    # CPlayerPedData (0xAC bytes)
PED_STRIDE, VEH_STRIDE, OBJ_STRIDE = 0x7C4, 0xA18, 0x19C
NPED, NVEH, NOBJ = 4, 2, 3
B_PED = 0               # arena B: NPED peds
C_VEH = 0               # arena C: NVEH vehiculos
C_OBJ = 0x1440          # arena C: NOBJ objetos
D_TASK = 0x100          # arena D: memoria que devuelve CTask::operator new
D_MI = (0x400, 0x800)   # arena D: dos CVehicleModelInfo (0x400 bytes)
D_MAT = 0xC00           # arena D: matrices (0x60 bytes) de los peds
D_CRECT = 0x1000        # arena D: libre

GLOBALS = {
    'PedPoolPtr': 0xB74490, 'VehPoolPtr': 0xB74494, 'ObjPoolPtr': 0xB7449C,
    'ScriptParams': 0xA43C78, 'SeekMove': 0x8D237C, 'LARiots': 0xB72958, 'Emergency': 0x8A5B28,
    'LimitDist': 0x8A5E48, 'GameLogicFlags': 0x96A8B0, 'CutsceneStatus': 0xB5F84C, 'TimeMs': 0xB7CB84,
    'ActiveSeq': 0x8D2E98, 'ActiveGroups': 0xC098E0, 'Players': 0xB7CD98, 'UsedObjects': 0xA44B88,
    'ModelInfoPtrs': 0xA9B0C8, 'SitDuration': 0x86DB28,
}
MODELS = (0, 400, 401, 411, 592, 19999)
MODEL_MI = {0: 0, 400: 0, 401: 1, 411: 1, 592: 0, 19999: 1}
# regiones de memoria global que el banco vigila, fija por caso y restaura (direccion, bytes); deben coincidir con Collabs.inc
REGIONS = ([(0xA43C78 + 0x20 * k, 0x20) for k in range(4)] + [(0x8D237C, 4), (0xB72958, 4), (0x8A5B28, 4), (0x8A5E48, 8), (0x96A8B0, 4), (0xB5F84C, 4), (0xB7CB84, 4),
           (0x8D2E98, 4), (0xC098E0, 8), (0xB7CD98, 4), (0xB7CD98 + 0x190, 4), (0xB74490, 4), (0xB74494, 4), (0xB7449C, 4)]
           + [(0xA44B88 + 0x1C * k, 4) for k in range(1, 5)] + [(0xC0994C + 0x2D4 * g, 4) for g in range(8)] + [(0xA9B0C8 + 4 * m, 4) for m in MODELS])
# CReferences::aRefs (3000 * 8 bytes) y pEmptyList: las tareas registran referencias a las entidades al construirse
BIG_REGIONS = [(0xB9B9A8, 24000), (0xBA1768, 4)]

# ----------------------------------------------------------------------------------------------- modelo de memoria de las arenas (relleno pseudoaleatorio como Fixtures.hpp::FillKind)


class Mem:
    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r, self.words = kind, seed, {}, {}, {}

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

    def rd(self, off, n=4):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


# ----------------------------------------------------------------------------------------------- flotantes
def f2b(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def b2f(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def is_fin(b):
    return (b >> 23) & 0xFF != 0xFF


def _round24(fr):
    """Fraction -> Fraction redondeada a 24 bits de mantisa (al mas cercano, empate al par), exponente sin limite (x87 con control de precision de 24 bits)"""
    if fr == 0:
        return fr
    sign = -1 if fr < 0 else 1
    a = abs(fr)
    e = 0
    # normalizar a [2^23, 2^24)
    n, d = a.numerator, a.denominator
    e = n.bit_length() - d.bit_length()
    # a ~ 2^e; queremos mant en [2^23, 2^24): mant = a * 2^(23 - e')
    sh = 23 - e
    m = a * (Fraction(2) ** sh)
    while m >= (1 << 24):
        sh -= 1
        m = a * (Fraction(2) ** sh)
    while m < (1 << 23):
        sh += 1
        m = a * (Fraction(2) ** sh)
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (q & 1)):
        q += 1
    return sign * Fraction(q) / (Fraction(2) ** sh)


class X87:
    """valores x87 con precision de 24 bits y exponente ampliado: Fraction para los finitos distintos de cero; float de Python para +-0, inf y NaN"""

    @staticmethod
    def of(bits):
        f = b2f(bits)
        if f != f or f in (float('inf'), float('-inf')) or f == 0:
            return f
        return Fraction(f)

    @staticmethod
    def _sgn(x):
        return math.copysign(1.0, x) if not isinstance(x, Fraction) else (1.0 if x > 0 else -1.0)

    @staticmethod
    def _special(x):
        return not isinstance(x, Fraction) and (x != x or x in (float('inf'), float('-inf')))

    @staticmethod
    def mul(a, b):
        if isinstance(a, Fraction) and isinstance(b, Fraction):
            return _round24(a * b)
        if X87._special(a) or X87._special(b):
            fa = float(a) if not isinstance(a, Fraction) else (float('inf') * X87._sgn(a))
            fb = float(b) if not isinstance(b, Fraction) else (float('inf') * X87._sgn(b))
            return fa * fb
        return math.copysign(0.0, X87._sgn(a) * X87._sgn(b))

    @staticmethod
    def add(a, b):
        if isinstance(a, Fraction) and isinstance(b, Fraction):
            r = _round24(a + b)
            return r if r != 0 else Fraction(0) if False else 0.0
        if X87._special(a) or X87._special(b):
            fa = float(a) if not isinstance(a, Fraction) else float(a)
            fb = float(b) if not isinstance(b, Fraction) else float(b)
            return fa + fb
        if isinstance(a, Fraction):
            return a
        if isinstance(b, Fraction):
            return b
        return a + b   # dos ceros

    @staticmethod
    def sub(a, b):
        if isinstance(b, Fraction):
            return X87.add(a, -b)
        return X87.add(a, -b)

    @staticmethod
    def sqrt(a):
        if not isinstance(a, Fraction):
            if a != a or a == float('inf') or a == 0:
                return a
            return float('nan')   # raiz de -inf
        if a < 0:
            return float('nan')
        if a == 0:
            return a
        # raiz con 80 bits extra y redondeo a 24 bits
        k = 120
        n = (a.numerator << (2 * k)) // a.denominator
        import math
        r = math.isqrt(n)
        approx = Fraction(r, 1 << k)
        # el pegajoso: si r*r != n hay cola; sumar un epsilon para desempatar correctamente
        if r * r != n or (a.numerator << (2 * k)) % a.denominator:
            approx += Fraction(1, 1 << (k + 40))
        return _round24(approx)

    @staticmethod
    def tobits(a):
        """fstp dword: conversion a float (redondeo al mas cercano, denormales y desbordamiento como el hardware)"""
        if not isinstance(a, Fraction):
            if a != a:
                return 0xFFC00000
            if a == 0:
                return 0x80000000 if math.copysign(1.0, a) < 0 else 0
            return 0x7F800000 if a > 0 else 0xFF800000
        if a == 0:
            return 0
        try:
            return f2b(float(a))
        except OverflowError:
            return 0x7F800000 if a > 0 else 0xFF800000
