"""Oraculo independiente (Python) de Fn132A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las nueve funciones del lote 132A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica de la x87 (24 bits de mantisa, exponente ampliado) con racionales exactos.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x41B660  CPatrolRoutePool::New (thiscall, `ret`)
       edx = [this+8] (tamano); al = 0; bucle: edi = [this+0xC] + 1; [this+0xC] = edi; si edi == edx: [this+0xC] = 0; si al != 0 -> devuelve 0; al = 1 (sin volver a incrementar); esi = [this+4]; edi = [this+0xC];
       si el byte [esi+edi] tiene el bit 7 -> sale del bucle (libre), si no repite. Libre: b = byte & 0x7F; byte = (b + 1) & 0x7F (el bit 7 queda a 0); devuelve [this] + [this+0xC] * 0x1A4.
  1  0x611F20  GetFormationOffset (thiscall, dx y dy en pila, `ret 8`): dx == -1: dy == -1 -> 5; dy == 0 -> 3; (dy == 1 u otro -> 0); dx == 0: dy == -1 -> 6; dy == 1 -> 1; otro -> 0;
       dx == 1: dy == -1 -> 7; dy == 0 -> 4; dy == 1 -> 2; otro -> 0; cualquier otro dx -> 0. Devuelve this + indice * 8.
  2  0x6F9EB0  CScriptedEffectPairs::Reset (thiscall, `ret`): para p = 0..3 (base = this + 0x24 p): dwords -1 en base + 4, 8, 0x10, 0x14, 0xC, 0x18 y 0x20; byte 0 en base + 0x24; despues [this] = 0.
       El PartnerTask del segundo efecto (base + 0x1C) y el relleno no se tocan. Sale con EAX = -1, ECX = this (el bucle de Init, 0x6FA740, hace `add ecx, 0x94` tras la llamada) y EDX = 0.
  3  0x5D0570  JPegScanlineToRaster (cdecl, raster, scanline, first, last; `ret`): para i = first mientras i < last (sin signo): r, g, b = bytes [0xBD0170 + 4 i + 0..2]; rgba en la pila = (r, g, b, 0xFF);
       formato = (byte [raster + 0x23]) << 8; eax = RwRGBAToPixel(&rgba, formato) (0x803740, cdecl); dword [[0xBD0160]] = eax; [0xBD0160] += 4.
  4  0x6FBA20  SunReflectionDistanceFactor (cdecl, x, y, z, radio floats; `ret`; resultado en ST0)
       camara = [0xB6F03C] ? [0xB6F03C] + 0x30 : 0xB6F02C (x, y); dx = cx - x; dy = cy - y (x87, 24 bits de mantisa, exponente ampliado); d = sqrt(dy*dy + dx*dx) guardado como float;
       si !(radio + 100.0f > d) -> 1.0; si !(d <= radio) -> (d - radio) * 0.01f; si no -> 0.0.
  5  0x6007A0  CDecision::Choices::Pick (thiscall, seek, slot (int16*), index (int32*); `ret 0Ch`)
       [slot] = 200 (word); [index] = -1; si seek != -1: para i en 0..[this+0x30) (con signo): si seek == dword [this + 4 i] -> [slot] = word bajo; [index] = i; fin. Si el word [slot] != 200 termina; si no,
       roll = (float)(rand() de 0x821B1E) * 0x38000100 (guardado como float); para i en 0..[this+0x30) (releido): si roll <= float [this + 0x18 + 4 i] (ordenado) -> [slot] = word bajo de la tarea i; [index] = i; fin.
  6  0x683950  CTaskComplexWaitForPed (Clone): eax = operator new(0x2C) (0x61A5A0); si 0 -> devuelve 0; si no, ctor 0x683340 (thiscall sobre eax; pila: [this+0xC], [this+0x10], [this+0x14], byte [this+0x18] sin signo) y devuelve
       el EAX del constructor.
  7  0x684D10  CTaskComplexPartnerDeal (Clone): operator new(0x74); si 0 -> 0; si no, ctor 0x684190 (pila: this+0x14 (direccion), [this+0x38], byte [this+0x58] sin signo, [this+0x3C], punto [this+0x40..0x4B] por valor) y devuelve su EAX.
  8  0x636180  CTaskSimpleTired (Clone): operator new(0x1C); si 0 -> 0; si no: duracion = [this+8] (leida antes de llamar); CTaskSimple::CTaskSimple (0x61A390, sobre mem: +4 = 0 y vtable 0x86D4A8; en el banco un falso que no escribe);
       [mem+8] = duracion; [mem] = 0x86DED0; [mem+0xC] = 0; [mem+0x10] = 0; byte [mem+0x14] = 0; byte [mem+0x15] = 0; [mem+0x18] = 0; devuelve mem.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CPatrolRoutePool::New', 'CTaskComplexFollowLeaderInFormation::GetFormationOffset', 'CScripted2dEffects::ResetEffectPairs', 'JPegPlugin::JPegScanlineToRaster', 'CCoronas::SunReflectionDistanceFactor',
         'CDecision::Pick', 'CTaskComplexWaitForPed::CloneBody', 'CTaskComplexPartnerDeal::Clone', 'CTaskSimpleTired::Clone']
ENTRIES = (0x41B660, 0x611F20, 0x6F9EB0, 0x5D0570, 0x6FBA20, 0x6007A0, 0x683950, 0x684D10, 0x636180)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SCAN_BYTES = 0x940
INF = float('inf')


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


def fill_kind(nbytes, kind, seed):
    if kind == 0:
        return fill_words(nbytes, seed)
    return bytearray([0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)]) * nbytes


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return (0x10000000 + (arena << 20) + GUARD + off) & M32


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


def f2i(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def f32_round(x):
    """redondea un Fraction al float de 32 bits mas cercano (par en empate, con denormales); devuelve un Fraction o float('inf')"""
    if x == 0:
        return Fraction(0)
    s = 1 if x > 0 else -1
    q = abs(x)
    if q >= Fraction(2) ** 128 - Fraction(2) ** 103:   # a partir del punto medio entre FLT_MAX y 2^128 se redondea a infinito
        return INF * s
    e = q.numerator.bit_length() - q.denominator.bit_length()
    while Fraction(2) ** e > q:
        e -= 1
    while Fraction(2) ** (e + 1) <= q:
        e += 1
    unit = Fraction(2) ** (max(e, -126) - 23)
    m = q / unit
    fl = m.numerator // m.denominator
    rem = m - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2):
        fl += 1
    return s * fl * unit


def round24(q):
    """redondea un Fraction a 24 bits significativos (par en empate), exponente sin limite (la x87 con el control del juego)"""
    if q == 0:
        return Fraction(0)
    s = 1 if q > 0 else -1
    q = abs(q)
    e = q.numerator.bit_length() - q.denominator.bit_length() - 24
    while q / Fraction(2) ** e >= 2 ** 24:
        e += 1
    while q / Fraction(2) ** e < 2 ** 23:
        e -= 1
    m = q / Fraction(2) ** e
    fl = m.numerator // m.denominator
    rem = m - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2):
        fl += 1
    return s * Fraction(fl) * Fraction(2) ** e


def sqrt24(q):
    """raiz cuadrada correctamente redondeada a 24 bits de un Fraction >= 0"""
    if q == 0:
        return Fraction(0)
    k = 0
    while q * 4 ** k < 4 ** 28:   # la raiz entera tiene al menos 28 bits
        k += 1
    n = q * 4 ** k
    r = math.isqrt(n.numerator // n.denominator)
    exact = Fraction(r * r) == n
    v = Fraction(r) + (0 if exact else Fraction(1, 4))
    return round24(v / Fraction(2) ** k)


def to_exact(bits):
    """float de 32 bits -> Fraction exacto, o float (nan o infinito) si no es finito"""
    v = f2i(bits)
    if v != v or v in (INF, -INF):
        return v
    return Fraction(v)


def ext80(value):
    """(10 octetos del registro, etiqueta de la x87) de un Fraction que es un float de 24 bits con exponente ampliado"""
    if value == 0:
        return bytes(10), 1
    s = 0x8000 if value < 0 else 0
    q = abs(value)
    e = q.numerator.bit_length() - q.denominator.bit_length()
    while Fraction(2) ** e > q:
        e -= 1
    while Fraction(2) ** (e + 1) <= q:
        e += 1
    mant = q / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return struct.pack('<QH', int(mant), s | (e + 16383)), 0


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'found', 'full', 'wrapped', 'cursor_neg1', 'cursor_last', 'ref_wrap', 'size_1', 'writes_guard'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'c_m1_m1', 'c_m1_0', 'c_m1_1', 'c_0_m1', 'c_0_0', 'c_0_1', 'c_1_m1', 'c_1_0', 'c_1_1', 'other_dx', 'other_dy'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill'],
    3: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'real_full', 'empty', 'reversed', 'cnt_1', 'cnt_2_8', 'cnt_9_40', 'cnt_big', 'writes_guard', 'cursor_unaligned', 'fmt_zero', 'fmt_high'],
    4: ['kind0', 'kind1', 'kind2', 'kind3', 'ret_one_far', 'ret_one_special', 'ret_zero', 'formula', 'cam_matrix', 'cam_placement', 'boundary_reach', 'boundary_radius', 'ext_range'],
    5: ['kind0', 'kind1', 'kind2', 'kind3', 'seek_found', 'seek_miss', 'seek_none', 'roll_pick', 'roll_none', 'count0', 'count_neg', 'count6', 'count_gt6', 'eq_boundary', 'nan_cum', 'slot_unaligned'],
    6: ['kind0', 'kind1', 'kind2', 'kind3', 'alloc_null', 'alloc_b', 'alloc_a_overlap', 'ret_mem', 'ret_other', 'ret_zero', 'rotate_high', 'this_unaligned', 'spill'],
    7: ['kind0', 'kind1', 'kind2', 'kind3', 'alloc_null', 'alloc_b', 'alloc_a_overlap', 'ret_mem', 'ret_other', 'ret_zero', 'lead_high', 'this_unaligned', 'spill'],
    8: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'alloc_null', 'alloc_b', 'alloc_a_overlap', 'overlap_this', 'dur_zero', 'this_unaligned', 'spill'],
}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    s = bytes.fromhex(c['s'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    events, gch, ret = [], [], None
    extra = {}
    st0 = None   # fn 4: (10 octetos de ST0, etiqueta)

    if fn == 0:
        o, so, size = a[0], a[3], a[4]
        A.wr(o + 0, 4, a[6]); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, size); A.wr(o + 0xC, 4, a[5])
        for i in range(size):
            B.wr(so + i, 1, s[i])
        A.snap(); B.snap(); C.snap()
        cursor = s32(a[5])
        wrapped = False
        res = None
        while True:
            cursor = s32(cursor + 1)
            A.wr(o + 0xC, 4, cursor & M32)
            if cursor == size:
                cursor = 0
                A.wr(o + 0xC, 4, 0)
                if wrapped:
                    res = 0
                    marks.append('full')
                    break
                wrapped = True
            stv = B.rd(so + cursor, 1)
            if stv & 0x80:
                B.wr(so + cursor, 1, ((stv & 0x7F) + 1) & 0x7F)
                res = (a[6] + cursor * 0x1A4) & M32
                marks.append('found')
                if wrapped:
                    marks.append('wrapped')
                if stv & 0x7F == 0x7F:
                    marks.append('ref_wrap')
                break
        if a[5] == M32:
            marks.append('cursor_neg1')
        if a[5] == size - 1:
            marks.append('cursor_last')
        if size == 1:
            marks.append('size_1')
        if so + size > WIN:
            marks.append('writes_guard')
        ret = res
    elif fn == 1:
        A.snap(); B.snap(); C.snap()
        dx, dy = s32(a[3]), s32(a[4])
        idx = 0
        if dx == -1:
            idx = {-1: 5, 0: 3}.get(dy, 0)
        elif dx == 0:
            idx = {-1: 6, 1: 1}.get(dy, 0)
        elif dx == 1:
            idx = {-1: 7, 0: 4, 1: 2}.get(dy, 0)
        names = {-1: 'm1', 0: '0', 1: '1'}
        if dx in names and dy in names:
            marks.append('c_%s_%s' % (names[dx], names[dy]))
        if dx not in names:
            marks.append('other_dx')
        if dy not in names:
            marks.append('other_dy')
        ret = (c['ecx'] + idx * 8) & M32
    elif fn == 2:
        A.snap(); B.snap(); C.snap()
        o = a[0]
        for p in range(4):
            base = o + 0x24 * p
            for off in (0x4, 0x8, 0x10, 0x14, 0xC, 0x18, 0x20):
                A.wr(base + off, 4, M32)
            A.wr(base + 0x24, 1, 0)
        A.wr(o, 4, 0)
        extra = {'residue': [M32, tag(0, o), 0]}   # EAX = -1, ECX = this, EDX = 0 (el bucle de Init del EXE conserva ECX)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + 0x94 > WIN:
            marks.append('spill')
    elif fn == 3:
        o, first, last, curoff, fmt = a[0], a[3], a[4], a[5], a[7] & 0xFF
        A.wr(o + 0x23, 1, fmt)
        A.snap(); B.snap(); C.snap()
        scan = fill_kind(SCAN_BYTES, fill, a[15])
        count = 0
        h = 2166136261
        evs = []
        i = first
        while i < last:
            r, g_, b_ = scan[4 * i], scan[4 * i + 1], scan[4 * i + 2]
            rgba = r | (g_ << 8) | (b_ << 16) | (0xFF << 24)
            f = fmt << 8
            px = rnd(rgba, f)
            if count < 16:
                evs.append([0x803740, rgba, f])
            for byte in struct.pack('<II', rgba, f):
                h = ((h ^ byte) * 16777619) & M32
            B.wr(curoff + 4 * count, 4, px)
            count += 1
            i += 1
        events = evs
        extra = {'calls': [count, h], 'cursor': tag(1, curoff + 4 * count)}
        if first == 0x40 and last == 0x240:
            marks.append('real_full')
        if first == last:
            marks.append('empty')
        if first > last:
            marks.append('reversed')
        marks.append('cnt_1' if count == 1 else 'cnt_2_8' if 2 <= count <= 8 else 'cnt_9_40' if 9 <= count <= 40 else 'cnt_big' if count > 40 else 'cnt_0')
        if curoff + 4 * count > WIN:
            marks.append('writes_guard')
        if curoff % 4:
            marks.append('cursor_unaligned')
        marks.append('fmt_zero' if fmt == 0 else 'fmt_high' if fmt >= 0x80 else 'fmt_low')
    elif fn == 4:
        A.snap(); B.snap(); C.snap()
        x, y, radius = to_exact(a[3]), to_exact(a[4]), to_exact(a[6])
        cx, cy = to_exact(a[7]), to_exact(a[8])
        marks.append('cam_matrix' if a[9] else 'cam_placement')
        finite = all(not isinstance(v, float) for v in (x, y, radius, cx, cy))
        if not finite:
            # al menos un operando no finito: se sigue con floats de Python (NaN e infinitos se propagan igual en la x87); en esta rama el resultado siempre es 1.0 o 0.0
            fx, fy, fr, fcx, fcy = (float(v) for v in (x, y, radius, cx, cy))
            dxf, dyf = fcx - fx, fcy - fy
            dist = math.sqrt(dyf * dyf + dxf * dxf)
            try:   # la distancia se guarda como float (fstp dword): fuera de rango pasa a infinito
                dist = struct.unpack('<f', struct.pack('<f', dist))[0]
            except OverflowError:
                dist = INF
            if not (fr + 100.0 > dist):
                value = Fraction(1)
                marks.append('ret_one_special')
            elif dist <= fr:
                value = Fraction(0)
                marks.append('ret_zero')
            else:
                raise AssertionError('formula con operandos no finitos')
        else:
            dx = round24(cx - x)
            dy = round24(cy - y)
            sq = round24(round24(dy * dy) + round24(dx * dx))
            dist = f32_round(sqrt24(sq))
            reach = round24(radius + 100)
            if max(abs(dx), abs(dy), sq) > Fraction(2) ** 128 or (sq != 0 and sq < Fraction(2) ** -126):
                marks.append('ext_range')
            if isinstance(dist, float):
                value = Fraction(1)
                marks.append('ret_one_far')
            else:
                if reach == dist:
                    marks.append('boundary_reach')
                if dist == radius:
                    marks.append('boundary_radius')
                if not (reach > dist):
                    value = Fraction(1)
                    marks.append('ret_one_far')
                elif dist <= radius:
                    value = Fraction(0)
                    marks.append('ret_zero')
                else:
                    value = round24(round24(dist - radius) * Fraction(f2i(0x3C23D70A)))
                    marks.append('formula')
        st0 = ext80(value)
    elif fn == 5:
        o, count, seek, r = a[0], a[3], s32(a[4]), a[5]
        for i in range(6):
            A.wr(o + 4 * i, 4, struct.unpack('<I', s[4 * i:4 * i + 4])[0])
            A.wr(o + 0x18 + 4 * i, 4, struct.unpack('<I', s[24 + 4 * i:28 + 4 * i])[0])
        A.wr(o + 0x30, 4, count)
        A.snap(); B.snap(); C.snap()
        B.wr(a[6], 2, 200); C.wr(a[7], 4, M32)
        cnt = s32(count)
        marks.append('count0' if cnt == 0 else 'count_neg' if cnt < 0 else 'count6' if cnt == 6 else 'count_gt6' if cnt > 6 else 'count_few')
        if seek != -1:
            found = False
            for i in range(cnt):
                if seek == s32(A.rd(o + 4 * i, 4)):
                    B.wr(a[6], 2, A.rd(o + 4 * i, 2))
                    C.wr(a[7], 4, i)
                    found = True
                    break
            marks.append('seek_found' if found else 'seek_miss')
        else:
            marks.append('seek_none')
        if B.rd(a[6], 2) == 200:
            events = [[0x821B1E]]
            roll = struct.unpack('<f', struct.pack('<f', r * f2i(0x38000100)))[0]
            cnt = s32(A.rd(o + 0x30, 4))
            picked = False
            for i in range(cnt):
                cum = f2i(A.rd(o + 0x18 + 4 * i, 4))
                if cum != cum:
                    marks.append('nan_cum')
                if roll <= cum:
                    B.wr(a[6], 2, A.rd(o + 4 * i, 2))
                    C.wr(a[7], 4, i)
                    picked = True
                    if roll == cum:
                        marks.append('eq_boundary')
                    break
            marks.append('roll_pick' if picked else 'roll_none')
        if a[6] % 2:
            marks.append('slot_unaligned')
    else:
        o = a[0]
        memsz = {6: 0x2C, 7: 0x74, 8: 0x1C}[fn]
        if fn == 6:
            A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[8]); A.wr(o + 0x14, 4, a[9]); A.wr(o + 0x18, 1, a[16] & 0xFF)
        elif fn == 7:
            A.wr(o + 0x38, 4, a[7]); A.wr(o + 0x3C, 4, a[8]); A.wr(o + 0x40, 4, a[9]); A.wr(o + 0x44, 4, a[10]); A.wr(o + 0x48, 4, a[11]); A.wr(o + 0x58, 1, a[16] & 0xFF)
        else:
            A.wr(o + 8, 4, a[7])
        A.snap(); B.snap(); C.snap()
        events = [[0x61A5A0, memsz]]
        if a[3] == 0:
            mem = None
            marks.append('alloc_null')
        elif a[3] == 1:
            mem = (1, a[4])
            marks.append('alloc_b')
        else:
            mem = (0, o + s32(a[4]))
            marks.append('alloc_a_overlap')
        memtag = None if mem is None else tag(*mem)
        if mem is None:
            ret = 0
            marks.append('ret_zero')
        elif fn == 8:
            M = A if mem[0] == 0 else B
            mo = mem[1]
            dur = A.rd(o + 8, 4)
            events.append([0x61A390, memtag, M.rd(mo, 4)])   # el falso del constructor base no escribe: ve la vtable previa de la memoria
            M.wr(mo + 8, 4, dur); M.wr(mo, 4, 0x86DED0)
            M.wr(mo + 0xC, 4, 0); M.wr(mo + 0x10, 4, 0); M.wr(mo + 0x14, 1, 0); M.wr(mo + 0x15, 1, 0); M.wr(mo + 0x18, 4, 0)
            ret = memtag
            if mem[0] == 0 and mo < o + 0xC and o < mo + 0x1C:
                marks.append('overlap_this')
            if dur == 0:
                marks.append('dur_zero')
        else:
            rm = a[5]
            eax = memtag if rm == 0 else a[6] if rm == 1 else 0 if rm == 2 else (memtag + 4) & M32
            if fn == 6:
                events.append([0x683340, memtag, a[7], a[8], a[9], a[16] & 0xFF])
                if a[16] & 0xFF > 1:
                    marks.append('rotate_high')
            else:
                events.append([0x684190, memtag, tag(0, o + 0x14), a[7], a[16] & 0xFF, a[8], a[9], a[10], a[11]])
                if a[16] & 0xFF > 1:
                    marks.append('lead_high')
            ret = eax
            marks.append('ret_mem' if rm == 0 else 'ret_zero' if rm == 2 else 'ret_other')
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + {6: 0x19, 7: 0x59, 8: 0xC}[fn] > WIN:
            marks.append('spill')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks, 'extra': extra, 'st0': st0}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    for k, v in e['extra'].items():
        if o.get(k) != v:
            return '%s %s != %s' % (k, o.get(k), v)
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if e['st0'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
        return None
    fsw = int.from_bytes(img[4:6], 'little')
    top = (fsw >> 11) & 7
    want = (0xFFFF & ~(3 << (2 * top))) | (e['st0'][1] << (2 * top))
    if ftw != want:
        return 'etiquetas x87 %04x != %04x' % (ftw, want)
    if img[28:38] != e['st0'][0]:
        return 'ST0 %s != %s' % (img[28:38].hex(), e['st0'][0].hex())
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
