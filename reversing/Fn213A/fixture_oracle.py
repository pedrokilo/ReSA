"""Oraculo independiente (Python) de Fn213A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 213A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`).
   0-3   0x550BD0, 0x550CA0, 0x550D70, 0x550E40  constructor de CPool(capacidad, nombre) `ret 8`: operator new[] (0x821195) de capacidad * elemento (imul/lea+shl de 32 bits; elemento 0x24, 0x20, 0x294, 0xC4) -> [this];
         operator new[] de capacidad -> [this+4]; [this+0x10] = 1 (byte), [this+8] = capacidad, [this+0xC] = -1; si capacidad (con signo) > 0: para cada estado i < capacidad (con signo): byte = (byte | 0x80) & 0x80 = 0x80.
         Devuelve this. No se guarda el nombre.
   4-7   0x550C30, 0x550D00, 0x550DD0, 0x550EA0  cuerpo del destructor: si [this+8] (con signo) > 0: si el byte [this+0x10] != 0: delete[] (0x8213AE) de [this] y de [this+4]; despues [this], [this+4], [this+8], [this+0xC] = 0.
   8-24  0x5512F0 + 0x60 * k (17)  igual y despues, si el bit 0 del byte [esp+8]: operator delete (0x8214BD) de this; devuelve this (sin comprobar nulo)
   25    0x5521D0  IsMemberOfList(elemento): nodo = [this]; mientras nodo != 0: si [nodo] == elemento devuelve 1; nodo = [nodo+4]; devuelve 0 (solo AL)
   26    0x45B730  escritura del paquete de coche (thiscall ECX = paquete; veh, indice): ver `store_expected`
   27    0x45BF90  [paquete] = 0x0F; 0x45B730; dos llamadas virtuales GetRideAnimData (vtabla +0xBC): byte(ftol(Lean (+8) * 50)) -> +0x34; byte(ftol(BarSteer (+4) * 50)) -> +0x35
   28    0x45C070  igual con el tipo 2
   29    0x45BA90  lectura con interpolacion del paquete de vehiculo (ver `extract_expected`); veh nulo: nada
   30    0x45BFF0  0x45BA90 y despues BarSteer (+4) <- int8(+0x35) * 0.02f, Lean (+8) <- int8(+0x34) * 0.02f (llamadas virtuales GetRideAnimData), [veh+0x5C8] = 0 y CBike::CalculateLeanMatrix (0x6B7150)
   31    0x45C0D0  igual (mismos bytes)

Aritmetica x87 a 24 bits del EXE: cada `fmul` redondea el producto a 24 bits de mantisa (exponente ampliado) y `_ftol2` (0x821B40) lo trunca a 64 bits (fuera de rango -> 0x8000000000000000); solo se guarda el byte
bajo. Las conversiones de lectura son `fild` (entero exacto) * constante float y `fstp dword` (el producto ya esta a 24 bits). Constantes del EXE (bits): 0x858C4C = 1000.0, 0x858C84 = 0.25, 0x858628 = 100.0,
0x858B90 = 4.0, 0x859C3C = -4.0, 0x859C38 = 8000.0, 0x859C34 = 40.764328, 0x858B40 = 50.0, 0x859C30 = 20.222929, 0x858B38 = 0.02, 0x859C44 = 0x3CC8F5C3, 0x859C48 = 0x3903126F, 0x859C40 = 0x3D4A8AD9,
0x858C58 = 0.01, 0x858624 = 1.0.

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp). Las pilas locales (matrices temporales) se etiquetan 0x7F000000.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CPools::NodeRoutePoolConstructor", "CPools::TaskAllocatorPoolConstructor", "CPools::PedIntelligencePoolConstructor", "CPools::PedAttractorPoolConstructor",
         "CPools::NodeRoutePoolDestructorBody", "CPools::TaskAllocatorPoolDestructorBody", "CPools::PedIntelligencePoolDestructorBody", "CPools::PedAttractorPoolDestructorBody",
         "CPools::PtrNodeSingleLinkPoolScalarDeletingDestructor", "CPools::PtrNodeDoubleLinkPoolScalarDeletingDestructor", "CPools::EntryInfoNodePoolScalarDeletingDestructor", "CPools::PedPoolScalarDeletingDestructor",
         "CPools::VehiclePoolScalarDeletingDestructor", "CPools::BuildingPoolScalarDeletingDestructor", "CPools::ObjectPoolScalarDeletingDestructor", "CPools::DummyPoolScalarDeletingDestructor",
         "CPools::ColModelPoolScalarDeletingDestructor", "CPools::TaskPoolScalarDeletingDestructor", "CPools::EventPoolScalarDeletingDestructor", "CPools::PointRoutePoolScalarDeletingDestructor",
         "CPools::PatrolRoutePoolScalarDeletingDestructor", "CPools::NodeRoutePoolScalarDeletingDestructor", "CPools::TaskAllocatorPoolScalarDeletingDestructor", "CPools::PedIntelligencePoolScalarDeletingDestructor",
         "CPools::PedAttractorPoolScalarDeletingDestructor", "CCover::ProcessedBuildingsIsMemberOfList", "CReplay::StoreVehicleBlock", "CReplay::StoreBmxBlock", "CReplay::StoreBikeBlock",
         "CReplay::ExtractVehicleBlock", "CReplay::ExtractBmxBlock", "CReplay::ExtractBikeBlock"]
ENTRIES = (0x550BD0, 0x550CA0, 0x550D70, 0x550E40, 0x550C30, 0x550D00, 0x550DD0, 0x550EA0,
           0x5512F0, 0x551350, 0x5513B0, 0x551410, 0x551470, 0x5514D0, 0x551530, 0x551590, 0x5515F0, 0x551650, 0x5516B0, 0x551710, 0x551770, 0x5517D0, 0x551830, 0x551890, 0x5518F0,
           0x5521D0, 0x45B730, 0x45BF90, 0x45C070, 0x45BA90, 0x45BFF0, 0x45C0D0)
ELEM = (0x24, 0x20, 0x294, 0xC4)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_BIG = 0x32000000
(CTOR, DTOR, SCALAR, ISMEMBER, STORECAR, STOREBMX, STOREBIKE, EXTRACTVEH, EXTRACTBMX, EXTRACTBIKE) = 'ctor dtor scalar ismember storecar storebmx storebike extractveh extractbmx extractbike'.split()
KIND = [CTOR] * 4 + [DTOR] * 4 + [SCALAR] * 17 + [ISMEMBER, STORECAR, STOREBMX, STOREBIKE, EXTRACTVEH, EXTRACTBMX, EXTRACTBIKE]
RET_AL = {ISMEMBER}                                                              # los llamadores del EXE solo leen AL
RET_TAG = {CTOR, SCALAR}
STACK_TAG = 0x7F000000
EV_NEWARR, EV_DELARR, EV_DEL = 0x821195, 0x8213AE, 0x8214BD
EV_RIDE = 0xF00000BC

# Constantes float del EXE (bits)
F_1000, F_QUARTER, F_100, F_4, F_M4, F_8000 = 0x447A0000, 0x3E800000, 0x42C80000, 0x40800000, 0xC0800000, 0x45FA0000
F_HEAD, F_50, F_DOOR_STORE = 0x42230EAC, 0x42480000, 0x41A1C88F          # 40.764328, 50.0, 20.222929
F_002, F_HD, F_SPD, F_DOOR, F_001 = 0x3CA3D70A, 0x3CC8F5C3, 0x3903126F, 0x3D4A8AD9, 0x3C23D70A
F_ONE = 0x3F800000


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


# ------------------------------------------------------------------------------------------------ aritmetica de float
def fval(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def is_inf(bits):
    return (bits & 0x7FFFFFFF) == 0x7F800000


def frac(bits):
    """valor exacto de un float finito"""
    s = -1 if bits >> 31 else 1
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0:
        return s * Fraction(m, 1 << 149)
    return s * Fraction(m | 0x800000, 1 << 23) * (Fraction(2) ** (e - 127))


def round24(x):
    """redondea un racional a 24 bits de mantisa (al par, exponente ilimitado)"""
    if x == 0:
        return x
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if a < Fraction(2) ** e:
        e -= 1
    scaled = a / (Fraction(2) ** (e - 23))            # en [2^23, 2^24)
    fl = scaled.numerator // scaled.denominator
    rem = scaled - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    return s * Fraction(fl) * (Fraction(2) ** (e - 23))


def to_float_bits(x):
    """racional ya a 24 bits (normal) -> bits de float"""
    if x == 0:
        return 0
    return struct.unpack('<I', struct.pack('<f', float(x)))[0]


def ftol_low8(bits_a, bits_b):
    """byte bajo de _ftol2(fmul(a, b)) con el producto a 24 bits (fuera de rango o NaN: 0x8000000000000000 -> 0)"""
    if is_nan(bits_a) or is_nan(bits_b) or is_inf(bits_a) or is_inf(bits_b):
        return 0
    p = round24(frac(bits_a) * frac(bits_b))
    if abs(p) >= (1 << 63):
        return 0
    return int(p) & 0xFF                               # int() trunca hacia cero


def mul_bits(int_value, const_bits):
    """fild (entero) * constante, producto a 24 bits y fstp dword -> bits"""
    return to_float_bits(round24(Fraction(int_value) * frac(const_bits)))


def i8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def clamp_speed(bits):
    """if (4.0 < s) 4.0 else if (-4.0 > s) -4.0 else s (NaN pasa)"""
    if is_nan(bits):
        return bits
    v = fval(bits)
    if 4.0 < v:
        return F_4
    if -4.0 > v:
        return F_M4
    return bits


def one_minus(t):
    """fld 1.0; fsub dword [t]; fstp dword: 1 - t a 24 bits"""
    if is_nan(t):
        return t | 0x400000
    if is_inf(t):
        return 0x7F800000 if (t >> 31) else 0xFF800000
    return to_float_bits(round24(1 - frac(t)))


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

    def __init__(self, c):
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.big = c.get('big') or DEFAULT_BIG
        sd = c['sd']
        fill = padded(c)[2]
        self.mem = tuple(Mem(fill, sd[i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


def padded(c):
    a = [x & M32 for x in c['a']]
    return a + [0] * (64 - len(a))


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_STORE = ['type0', 'typeN', 'rhino', 'nonrhino', 'speed_hi', 'speed_lo', 'speed_nan', 'health_clamp', 'fp_self', 'fp_other', 'doors_bits']
_EXTRACT = ['type0', 'typeN', 'rhino', 'nonrhino', 'door_lf', 'door_rf', 'doors_removed', 'engine_on', 'engine_off', 't_nan', 't_inf', 't_normal']
_EXTRA = {
    CTOR: ['cap_neg', 'cap_zero', 'cap_pos', 'cap_big', 'new1_null'],
    DTOR: ['size_pos_owner', 'size_pos_noowner', 'size_le0'],
    SCALAR: ['size_pos_owner', 'size_pos_noowner', 'size_le0', 'flag_set', 'flag_clear'],
    ISMEMBER: ['empty', 'hit_head', 'hit_mid', 'hit_tail', 'absent', 'item_null'],
    STORECAR: _STORE, STOREBMX: _STORE, STOREBIKE: _STORE,
    EXTRACTVEH: _EXTRACT + ['null_veh'], EXTRACTBMX: _EXTRACT, EXTRACTBIKE: _EXTRACT,
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def store_expected(W, a, fn, k, marks, events):
    """0x45B730 (y las de BMX y moto): escribe el paquete (B) desde el vehiculo (A)"""
    A, B, Cc, D = W.mem
    o = a[0]
    this = W.raw(0, o)
    p = a[4]
    pkt = W.raw(1, p)
    mptr = 0 if a[6] == 0 else (W.raw(2, a[7]) if a[6] == 1 else a[8])
    bike = k in (STOREBMX, STOREBIKE)
    typ = a[11]
    A.wr(o + 0x14, 4, mptr)
    A.wr(o + 0x4C0, 4, a[9]); A.wr(o + 0x49C, 4, a[10]); A.wr(o + 0x590, 4, typ); A.wr(o + 0x5B4, 4, a[12])
    A.wr(o + 0x44, 4, a[13]); A.wr(o + 0x48, 4, a[14]); A.wr(o + 0x4C, 4, a[15])
    A.wr(o + 0x22, 2, a[16] & 0xFFFF); A.wr(o + 0x434, 1, a[17] & 0xFF); A.wr(o + 0x435, 1, a[18] & 0xFF)
    A.wr(o + 0x1C, 4, a[19]); A.wr(o + 0x40, 4, a[20]); A.wr(o + 0x94C, 4, a[21]); A.wr(o + 0x494, 4, a[22])
    for i in range(4):
        A.wr(o + 0x7D4 + 4 * i, 4, a[23 + i]); A.wr(o + 0x828 + 4 * i, 4, a[27 + i])
    A.wr(o + 0x5F4, 4, a[31]); A.wr(o + 0x60C, 4, a[32])
    for i in range(6):
        A.wr(o + 0x5A9 + i, 1, a[33 + i] & 0xFF)
    A.wr(o + 0x594, 1, a[39] & 0xFF)
    player = this if a[40] == 0 else (a[46] if a[40] == 1 else 0)
    firstperson = a[41] & 0xFF
    if bike:
        D.wr(a[43] + 4, 4, a[45]); D.wr(a[43] + 8, 4, a[44])
    W.snap()
    # ---- 0x45B730
    if bike:
        B.wr(p, 1, 0x0F if k == STOREBMX else 2)
    B.wr(p + 1, 1, a[5] & 0xFF)
    events.append([0x59BAD0, W.tagv((pkt + 4) & M32), W.tagv(mptr)])
    h = a[9]
    if not is_nan(h) and fval(F_1000) < fval(h):
        h = F_1000
        marks.append('health_clamp')
    B.wr(p + 2, 1, ftol_low8(h, F_QUARTER))
    B.wr(p + 3, 1, ftol_low8(a[10], F_100))
    B.wr(p + 0x1C, 4, a[12] if typ == 0 else 0)
    for i, off in enumerate((0x20, 0x21, 0x22)):
        sbits = a[13 + i]
        if is_nan(sbits):
            marks.append('speed_nan')
        elif fval(sbits) > 4.0:
            marks.append('speed_hi')
        elif fval(sbits) < -4.0:
            marks.append('speed_lo')
        B.wr(p + off, 1, ftol_low8(clamp_speed(sbits), F_8000))
    B.wr(p + 0x1A, 2, a[16] & 0xFFFF)
    B.wr(p + 0x2D, 1, a[17] & 0xFF); B.wr(p + 0x2E, 1, a[18] & 0xFF)
    ef = a[19]
    bl = B.rd(p + 0x2F, 1)
    cl = (bl & ~2) | (((ef >> 7) & 1) << 1)
    cl = (cl & ~4) | (((ef >> 30) & 1) << 2)
    cl = (cl & ~8) | (((ef >> 31) & 1) << 3)
    B.wr(p + 0x2F, 1, cl)
    events.append([0x56E0D0, 0xFFFFFFFF, 0])
    if this == player and firstperson != 0:
        B.wr(p + 0x2F, 1, B.rd(p + 0x2F, 1) & 0xFD)
        marks.append('fp_self')
    elif this == player:
        marks.append('fp_other')
    rhino = (a[16] & 0xFFFF) == 0x1B0
    marks.append('rhino' if rhino else 'nonrhino')
    B.wr(p + 0x23, 1, ftol_low8(a[21], F_HEAD) if rhino else ftol_low8(a[22], F_50))
    if typ == 0:
        marks.append('type0')
        B.wr(p + 0x24, 1, ftol_low8(a[23], F_50)); B.wr(p + 0x28, 1, ftol_low8(a[27], F_HEAD))
        B.wr(p + 0x25, 1, ftol_low8(a[24], F_50)); B.wr(p + 0x29, 1, ftol_low8(a[28], F_HEAD))
        B.wr(p + 0x26, 1, ftol_low8(a[25], F_50)); B.wr(p + 0x2A, 1, ftol_low8(a[29], F_HEAD))
        B.wr(p + 0x27, 1, ftol_low8(a[26], F_50)); B.wr(p + 0x2B, 1, ftol_low8(a[30], F_HEAD))
        B.wr(p + 0x18, 1, ftol_low8(a[31], F_DOOR_STORE)); B.wr(p + 0x19, 1, ftol_low8(a[32], F_DOOR_STORE))
        doors = 0
        for i in range(6):
            if (a[33 + i] & 0xFF) == 4:
                doors |= 1 << i
        B.wr(p + 0x2C, 1, doors)
        if doors:
            marks.append('doors_bits')
    else:
        marks.append('typeN')
    b2f = B.rd(p + 0x2F, 1)
    B.wr(p + 0x2F, 1, (b2f & ~1) | ((a[20] >> 29) & 1))
    B.wr(p + 0x30, 1, a[39] & 0xFF)
    if bike:
        events.append([EV_RIDE, W.tagv(this)])
        B.wr(p + 0x34, 1, ftol_low8(a[44], F_50))
        events.append([EV_RIDE, W.tagv(this)])
        B.wr(p + 0x35, 1, ftol_low8(a[45], F_50))


def extract_expected(W, a, fn, k, marks, events):
    """0x45BA90 (y las de BMX y moto): lee el paquete (B) y escribe el vehiculo (A)"""
    A, B, Cc, D = W.mem
    o = a[0]
    this = W.raw(0, o)
    p = a[4]
    pkt = W.raw(1, p)
    mptr = 0 if a[6] == 0 else (W.raw(2, a[7]) if a[6] == 1 else a[8])
    t = a[5]
    typ = a[30]
    bike = k in (EXTRACTBMX, EXTRACTBIKE)
    A.wr(o + 0x14, 4, mptr); A.wr(o + 0x590, 4, typ); A.wr(o + 0x22, 2, a[31] & 0xFFFF)
    B.wr(p + 2, 1, a[9] & 0xFF); B.wr(p + 3, 1, a[10] & 0xFF); B.wr(p + 0x1C, 4, a[11])
    B.wr(p + 0x20, 1, a[12] & 0xFF); B.wr(p + 0x21, 1, a[13] & 0xFF); B.wr(p + 0x22, 1, a[14] & 0xFF)
    B.wr(p + 0x2F, 1, a[15] & 0xFF); B.wr(p + 0x23, 1, a[16] & 0xFF)
    for i in range(4):
        B.wr(p + 0x24 + i, 1, a[17 + i] & 0xFF); B.wr(p + 0x28 + i, 1, a[21 + i] & 0xFF)
    B.wr(p + 0x18, 1, a[25] & 0xFF); B.wr(p + 0x19, 1, a[26] & 0xFF); B.wr(p + 0x2C, 1, a[27] & 0xFF); B.wr(p + 0x34, 1, a[28] & 0xFF); B.wr(p + 0x35, 1, a[29] & 0xFF)
    curArea = a[32]
    doorRet = [a[33 + i] for i in range(6)]
    if bike:
        D.wr(a[41] + 4, 4, 0); D.wr(a[41] + 8, 4, 0)
    W.snap()
    marks.append('t_nan' if is_nan(t) else ('t_inf' if is_inf(t) else 't_normal'))
    if k == EXTRACTVEH and a[42]:
        marks.append('null_veh')
        return

    def pb(off):
        return B.rd(p + off, 1)
    # matrices (CMatrix locales: en la pila)
    events.append([0x59B9F0, W.tagv((pkt + 4) & M32), STACK_TAG])
    events.append([0x59BCF0, STACK_TAG, W.tagv(mptr)])
    events.append([0x45AF40, STACK_TAG, one_minus(t)])
    events.append([0x45AF40, STACK_TAG, t])
    events.append([0x59ADF0, STACK_TAG, STACK_TAG])
    events.append([0x59BBC0, W.tagv(mptr), STACK_TAG])
    A.wr(o + 0x50, 4, 0); A.wr(o + 0x54, 4, 0); A.wr(o + 0x58, 4, 0)
    A.wr(o + 0x4C0, 4, mul_bits(pb(2) << 2, F_ONE))
    A.wr(o + 0x49C, 4, mul_bits(pb(3), F_001))
    if typ == 0:
        marks.append('type0')
        events.append([0x59CA10, B.rd(p + 0x1C, 4), W.tagv(this), 1])
    else:
        marks.append('typeN')
    A.wr(o + 0x44, 4, mul_bits(i8(pb(0x20)), F_SPD))
    A.wr(o + 0x48, 4, mul_bits(i8(pb(0x21)), F_SPD))
    A.wr(o + 0x4C, 4, mul_bits(i8(pb(0x22)), F_SPD))
    pf = pb(0x2F)
    e = A.rd(o + 0x1C, 4)
    e = e ^ ((((pf << 6) & M32) ^ e) & 0x80)
    e = e ^ ((((pf << 28) & M32) ^ e) & 0x40000000)
    x = (pf << 28) & M32
    e = ((((x ^ e) & M32) & 0x7FFFFFFF) ^ x) & M32
    A.wr(o + 0x1C, 4, e)
    rhino = (a[31] & 0xFFFF) == 0x1B0
    marks.append('rhino' if rhino else 'nonrhino')
    if rhino:
        A.wr(o + 0x494, 4, 0)
        A.wr(o + 0x94C, 4, mul_bits(i8(pb(0x23)), F_HD))
    else:
        A.wr(o + 0x494, 4, mul_bits(i8(pb(0x23)), F_002))
    if typ == 0:
        for i in range(4):
            A.wr(o + 0x7D4 + 4 * i, 4, mul_bits(i8(pb(0x24 + i)), F_002))
            A.wr(o + 0x828 + 4 * i, 4, mul_bits(i8(pb(0x28 + i)), F_HD))
        lf = mul_bits(i8(pb(0x18)), F_DOOR)
        A.wr(o + 0x5F4, 4, lf); A.wr(o + 0x5F8, 4, lf)
        rf = mul_bits(i8(pb(0x19)), F_DOOR)
        A.wr(o + 0x60C, 4, rf); A.wr(o + 0x610, 4, rf)
        if pb(0x18) != 0:
            A.wr(o + 0x5AB, 1, 1)
            marks.append('door_lf')
        if pb(0x19) != 0:
            A.wr(o + 0x5AC, 1, 1)
            marks.append('door_rf')
        mask = pb(0x2C)
        for i in range(6):
            if mask & (1 << i):
                events.append([0x6C2230, W.tagv((this + 0x5A0) & M32), i])
                if doorRet[i] != 4:
                    events.append([0x6C21C0, W.tagv((this + 0x5A0) & M32), i, 4])
                    events.append([0x6B1600, W.tagv(this), i, 1])
                    if 'doors_removed' not in marks:
                        marks.append('doors_removed')
    b428 = A.rd(o + 0x428, 1)
    if A.rd(o + 0x42D, 1) & 2:
        b428 &= 0xEF
        marks.append('engine_off')
    else:
        b428 |= 0x10
        marks.append('engine_on')
    A.wr(o + 0x428, 1, b428)
    if typ == 0:
        A.wr(o + 0x961, 1, 4)
    A.wr(o + 0x2F, 1, curArea & 0xFF)
    events.append([0x563280, W.tagv(this)])
    events.append([0x563220, W.tagv(this)])
    pfl = A.rd(o + 0x40, 4)
    A.wr(o + 0x40, 4, pfl ^ ((((pf << 29) & M32) ^ pfl) & 0x20000000))
    events.append([0x59ACD0, STACK_TAG])
    events.append([0x59ACD0, STACK_TAG])
    if bike:
        bar = mul_bits(i8(pb(0x35)), F_002)
        events.append([EV_RIDE, W.tagv(this)])
        D.wr(a[41] + 4, 4, bar)
        lean = mul_bits(i8(pb(0x34)), F_002)
        events.append([EV_RIDE, W.tagv(this)])
        D.wr(a[41] + 8, 4, lean)
        A.wr(o + 0x5C8, 1, 0)
        events.append([0x6B7150, W.tagv(this)])


def expected(c):
    a = padded(c)
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, threw, big = [], None, False, []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tg = W.tagv
    if k == CTOR:
        cap = a[4]
        scap = cap - (1 << 32) if cap >= (1 << 31) else cap
        elem = ELEM[fn]
        new1 = a[5]
        new2 = (W.big + a[6]) & M32 if scap > 0 else a[7]
        W.snap()
        events.append([EV_NEWARR, (cap * elem) & M32])
        events.append([EV_NEWARR, cap])
        A.wr(o, 4, new1); A.wr(o + 4, 4, new2); A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, M32)
        if scap > 0:
            big = [[a[6], a[6] + scap, 0x80]]
            marks.append('cap_big' if scap >= 0x20000 else 'cap_pos')
        else:
            marks.append('cap_zero' if scap == 0 else 'cap_neg')
        if new1 == 0:
            marks.append('new1_null')
        ret = tg(this)
    elif k in (DTOR, SCALAR):
        A.wr(o + 8, 4, a[4]); A.wr(o + 0x10, 4, a[5]); A.wr(o, 4, a[6]); A.wr(o + 4, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        W.snap()
        size = a[4] - (1 << 32) if a[4] >= (1 << 31) else a[4]
        if size > 0:
            if a[5] & 0xFF:
                events.append([EV_DELARR, tg(a[6])])
                events.append([EV_DELARR, tg(a[7])])
                marks.append('size_pos_owner')
            else:
                marks.append('size_pos_noowner')
            A.wr(o, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        else:
            marks.append('size_le0')
        if k == SCALAR:
            if a[9] & 1:
                events.append([EV_DEL, tg(this)])
                marks.append('flag_set')
            else:
                marks.append('flag_clear')
            ret = tg(this)
    elif k == ISMEMBER:
        n = a[4]
        A.wr(o, 4, W.raw(1, a[5]) if n else 0)
        for i in range(n):
            B.wr(a[5 + i], 4, a[13 + i])
            B.wr(a[5 + i] + 4, 4, W.raw(1, a[5 + i + 1]) if i + 1 < n else 0)
        item = a[13 + a[22]] if (a[21] == 0 and n) else (0 if a[21] == 2 else a[23])
        W.snap()
        found = -1
        for i in range(n):
            if a[13 + i] == item:
                found = i
                break
        ret = 1 if found >= 0 else 0
        if n == 0:
            marks.append('empty')
        elif found == 0:
            marks.append('hit_head')
        elif found == n - 1 and found > 0:
            marks.append('hit_tail')
        elif found > 0:
            marks.append('hit_mid')
        else:
            marks.append('absent')
        if item == 0:
            marks.append('item_null')
    elif k in (STORECAR, STOREBMX, STOREBIKE):
        store_expected(W, a, fn, k, marks, events)
    else:
        extract_expected(W, a, fn, k, marks, events)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'big': big, 'gchanges': [], 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('big') != e['big']:
        return 'big: %s != %s' % (str(o.get('big'))[:200], str(e['big'])[:200])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL"""
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
