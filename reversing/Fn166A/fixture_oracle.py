"""Oraculo independiente (Python) de Fn166A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 166A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.
Las formas de cada funcion estan en Fixtures.hpp (cabecera) y en README.md.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
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


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


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
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        addr &= M32
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        addr &= M32
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

    def snap(self):
        for m in self.mem:
            m.snap()


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con fracciones exactas
def f32_class(bits):
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return ('nan', bits) if m else ('inf', s)
    if e == 0:
        v = Fraction(m, 1 << 149)
    else:
        v = Fraction((m | 0x800000) << e, 1 << 150)
    return ('fin', -v if s else v, s)


def _exp2floor(m):
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    return e


def round_sig(fr, bits):
    """redondeo al par mas cercano de `fr` a `bits` bits de mantisa, sin limite de exponente"""
    if fr == 0:
        return fr
    sign = -1 if fr < 0 else 1
    m = abs(fr)
    e = _exp2floor(m)
    unit = Fraction(2) ** (e - (bits - 1))
    q = m / unit
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
        n += 1
    return sign * n * unit


def to_f32_bits(fr, zero_sign=0):
    """float de 32 bits (como fstp m32): redondeo al par, denormales y desbordamiento a infinito"""
    if fr == 0:
        return zero_sign << 31
    s = 1 if fr < 0 else 0
    m = abs(fr)
    e = _exp2floor(m)
    if e < -126:
        q = m * (1 << 149)                      # cuanto 2^-149
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
            n += 1
        return (s << 31) | n                     # n = 2^23 da el menor normal (exponente 1, mantisa 0)
    q = m / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
        n += 1
    if n == 1 << 24:
        n >>= 1
        e += 1
    if e > 127:
        return (s << 31) | 0x7F800000
    return (s << 31) | ((e + 127) << 23) | (n & 0x7FFFFF)


def x87_add(st0_bits, mem_bits):
    """fld [st0]; fadd [mem]; fstp float, con la x87 a 24 bits de mantisa (exponente ampliado). A lo sumo un operando NaN. Devuelve (bits, marcas)"""
    a, b = f32_class(st0_bits), f32_class(mem_bits)
    marks = []
    if a[0] == 'nan' or b[0] == 'nan':
        if a[0] == 'nan' and b[0] == 'nan':
            raise ValueError('dos NaN: fuera del modelo')
        nb = (a if a[0] == 'nan' else b)[1]
        marks.append('nan_operand')
        return (nb & 0x80000000) | 0x7FC00000 | (nb & 0x7FFFFF), marks   # QNaN con la misma carga y signo
    if a[0] == 'inf' or b[0] == 'inf':
        if a[0] == 'inf' and b[0] == 'inf' and a[1] != b[1]:
            marks.append('inf_minus_inf')
            return 0xFFC00000, marks                                      # indefinido real
        marks.append('inf_operand')
        s = a[1] if a[0] == 'inf' else b[1]
        return (s << 31) | 0x7F800000, marks
    s_sum = a[1] + b[1]
    if s_sum == 0:
        zs = 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0       # suma exacta cero: +0 salvo (-0) + (-0)
        marks.append('sum_zero')
        return zs << 31, marks
    total = round_sig(s_sum, 24)                   # PC = 24 bits
    if abs(s_sum) >= (1 << 128):
        marks.append('sum_beyond_float_range')
    bits = to_f32_bits(total)
    e = (bits >> 23) & 0xFF
    if e == 0xFF:
        marks.append('result_inf')
    elif e == 0:
        marks.append('result_denormal_or_zero')
    return bits, marks


NAMES = ['CTaskSimpleFight::Clone', 'CTaskSimpleFight::ScalarDeletingDestructor', 'CTaskSimpleFightingControl::ScalarDeletingDestructor', 'CTaskSimpleIKChain::ConstructorDefaultBody',
         'CTaskComplexFollowPedFootsteps::GetTaskType', 'CTaskComplexFollowPedFootsteps::SetMaxEntityDist2D', 'CTaskComplexSeekEntityAnyMeansXYOffset::ConstructorExe',
         'D3DResourceSystem::GrowableArrayPushBack', 'D3DResourceSystem::ScratchDirectoryInit', 'CTxdStore::PluginAttach', 'FxPrimBP_c::ConstructorBody', 'FxPrimBP_c::DestructorBody',
         'CEventHitByWaterCannon::GetEventType', 'CEventHitByWaterCannon::GetLifeTime', 'CEventHitByWaterCannon::GetLocalSoundLevel', 'CEventHitByWaterCannon::GetEventPriority',
         'CEventInWater::AffectsPed', 'CEventInWater::TakesPriorityOver', 'CTaskSimpleRunNamedAnim::StartAnim', 'CTaskSimpleRunTimedAnim::ScalarDeletingDestructor',
         'CTaskComplexSeekEntityStandard::ScalarDeletingDestructor', 'CMatrix::AddTranslation', 'CTheScripts::ScriptEntryListAdd', 'CTaskSimpleSetCharDecisionMaker::ScalarDeletingDestructor']
ENTRIES = (0x622E40, 0x6239D0, 0x624B30, 0x62EC10, 0x694EC0, 0x6955D0, 0x695600, 0x730B30, 0x7315A0, 0x731650, 0x4A9CF0, 0x4A9D20, 0x4B12E0, 0x4B12F0, 0x4B1300, 0x4B1310,
           0x4B13D0, 0x4B1420, 0x61BB10, 0x61BBC0, 0x46AD50, 0x46AE00, 0x46AE80, 0x46AEF0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)
CLONE, SDD, IKCTOR, CONST, FLOAT, SETDIST, ANYMEANS, PUSH, SCRATCH, PLUGIN, FXCTOR, FXDTOR, AFFECTS, TAKES, STARTANIM, MATADD, ENTRYADD = \
    'clone sdd ikctor const float setdist anymeans push scratch plugin fxctor fxdtor affects takes startanim matadd entryadd'.split()
KIND = [CLONE, SDD, SDD, IKCTOR, CONST, SETDIST, ANYMEANS, PUSH, SCRATCH, PLUGIN, FXCTOR, FXDTOR, CONST, CONST, FLOAT, CONST, AFFECTS, TAKES, STARTANIM, SDD, SDD, MATADD, ENTRYADD, SDD]
BODY = {1: 0x61C530, 2: 0x61DCA0, 3: 0x61A390, 6: 0x61A3B0, 19: 0x48E090, 20: 0x46AD70, 23: 0x46AF10}
CONSTVAL = {4: 0x3A8, 12: 0x40, 13: 0, 15: 0x40}
SPAN = [0x30, 4, 4, 4, 4, 0x34, 0x34, 0x10, 4, 4, 0x40, 0x40, 4, 4, 4, 4, 4, 0x10, 0x64, 4, 4, 0x44, 0x200, 4]
TASKNEW, FIGHTCTOR, TASKDEL, REGREF, RESIZE, NEWSCALAR, DIRCTOR, REGPLUGIN = 0x61A5A0, 0x61C470, 0x61A5B0, 0x571B70, 0x7304B0, 0x82119A, 0x5322A0, 0x7F3C10
TEXDESTROY, GETACTIVE, ISPLAYER, ISALIVE, BLEND, SETFINISH, SETDELETE = 0x7F3820, 0x681720, 0x5DF8F0, 0x5E0170, 0x4D4410, 0x4CEBE0, 0x4CEBC0
EVTASKTYPE, EVTYPE, EVPRIOREF, EVPRIOTHIS = 0xE0000001, 0xE0000002, 0xE0000003, 0xE0000004
PLUGINGLOBAL = 0xC88018
SDD_VT = {}

KEYS = {}
for _fn in range(24):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (SDD,):
        _k += ['this_aligned', 'this_unaligned', 'flags_bit0_set', 'flags_bit0_clear']
    elif _kind == CLONE:
        _k += ['this_aligned', 'this_unaligned', 'new_null', 'new_ok', 'cmd_negative', 'cmd_positive', 'ret_new', 'ret_other']
    elif _kind in (IKCTOR, FXCTOR):
        _k += ['this_aligned', 'this_unaligned']
    elif _kind == CONST:
        _k = _k[:8]
    elif _kind == FLOAT:
        _k = _k[:8]
    elif _kind == SETDIST:
        _k += ['this_aligned', 'this_unaligned', 'equal', 'different', 'nan_arg', 'nan_current']
    elif _kind == ANYMEANS:
        _k += ['this_aligned', 'this_unaligned', 'entity_null', 'entity_set']
    elif _kind == PUSH:
        _k += ['this_aligned', 'this_unaligned', 'no_grow', 'grow', 'grow_many', 'alias_header', 'count_negative']
    elif _kind == SCRATCH:
        _k = _k[:8] + ['dir_ctor_called', 'new_null']
    elif _kind == PLUGIN:
        _k = _k[:8] + ['ret_minus1', 'ret_ok']
    elif _kind == FXDTOR:
        _k += ['this_aligned', 'this_unaligned', 'tex_null', 'tex_set', 'fourth_set']
    elif _kind == AFFECTS:
        _k = _k[:8] + ['task_null', 'task_set', 'player', 'not_player', 'type_in_water', 'alive_called']
    elif _kind == TAKES:
        _k += ['this_aligned', 'this_unaligned', 'type_8', 'type_9', 'type_5d', 'type_other', 'accel_gt1', 'prio_compare']
    elif _kind == STARTANIM:
        _k += ['this_aligned', 'this_unaligned', 'timer_started', 'timer_skipped', 'offset_at_end', 'delete_cb', 'finish_cb']
    elif _kind == MATADD:
        _k += ['this_aligned', 'this_unaligned', 'vec_separate', 'vec_alias', 'nan_operand', 'inf_operand']
    elif _kind == ENTRYADD:
        _k += ['this_aligned', 'this_unaligned', 'added', 'full', 'long_string', 'text_in_list']
    KEYS[_fn] = _k


def sgn(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def common_marks(fn, a):
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    k = KIND[fn]
    if k in (CONST, FLOAT, SCRATCH, PLUGIN, AFFECTS):
        return marks
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    return marks


def nan_pair(a_bits, b_bits):
    """dos NaN: el de mayor significando (el QNaN gana al SNaN; con empate se queda el positivo)"""
    qa, qb = bool(a_bits & 0x400000), bool(b_bits & 0x400000)
    if qa != qb:
        w = a_bits if qa else b_bits
    else:
        sa, sb = a_bits & 0x7FFFFF, b_bits & 0x7FFFFF
        w = a_bits if (sa > sb or (sa == sb and not a_bits >> 31)) else b_bits
    return (w & 0x80000000) | 0x7FC00000 | (w & 0x7FFFFF)


def add_bits(st0, mem, marks):
    if (f32_class(st0)[0] == 'nan') and (f32_class(mem)[0] == 'nan'):
        marks.append('nan_operand')
        return nan_pair(st0, mem)
    bits, mk = x87_add(st0, mem)
    marks += mk
    return bits


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    gch = []
    o = a[0]
    this = W.raw(0, o)
    if k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('flags_bit0_set' if a[6] & 1 else 'flags_bit0_clear')
        if a[6] & 1:
            events.append([TASKDEL, tag(0, o), A.rd(o, 4)])
        ret = tag(0, o)
    elif k == IKCTOR:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x86DB48)
        ret = tag(0, o)
    elif k == CLONE:
        A.wr(o + 0x10, 4, a[3])
        ent = W.raw(1, a[10]) if a[11] else a[4]
        A.wr(o + 0x18, 4, ent)
        A.wr(o + 0x24, 4, a[5])
        newp = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else a[9])
        W.snap()
        events.append([TASKNEW, 0x28])
        if newp == 0:
            marks.append('new_null')
            ret = 0
        else:
            marks.append('new_ok')
            cmd = (a[5] >> 24) & 0xFF
            cmd32 = (cmd | 0xFFFFFF00) if cmd & 0x80 else cmd
            marks.append('cmd_negative' if cmd & 0x80 else 'cmd_positive')
            events.append([FIGHTCTOR, W.tagv(newp), W.tagv(ent), cmd32, a[3] & 0xFFFF])
            rv = newp if a[8] == 0 else (a[9] if a[8] == 1 else 0)
            marks.append('ret_new' if a[8] == 0 else 'ret_other')
            ret = W.tagv(rv)
    elif k == CONST:
        W.snap()
        ret = CONSTVAL[fn]
    elif k == FLOAT:
        W.snap()
    elif k == SETDIST:
        A.wr(o + 0x18, 4, a[3]); A.wr(o + 0x28, 4, a[5]); A.wr(o + 0x2C, 4, a[6]); A.wr(o + 0x30, 4, a[7])
        W.snap()
        cur, arg = f32(a[3]), f32(a[4])
        if IsNanBits(a[4]):
            marks.append('nan_arg')
        if IsNanBits(a[3]):
            marks.append('nan_current')
        if cur == arg:
            marks.append('equal')
        else:
            marks.append('different')
            A.wr(o + 0x18, 4, a[4]); A.wr(o + 0x28, 4, c.get('now', 0x12345)); A.wr(o + 0x2C, 4, 0); A.wr(o + 0x30, 1, 1)
    elif k == ANYMEANS:
        A.wr(o, 4, a[3])
        ent = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else a[8])
        W.snap()
        events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o + 0xC, 4, ent); A.wr(o + 0x10, 4, a[9]); A.wr(o + 0x14, 4, a[10]); A.wr(o, 4, 0x870CF0)
        A.wr(o + 0x18, 4, 0); A.wr(o + 0x1C, 4, 0); A.wr(o + 0x20, 1, 0); A.wr(o + 0x21, 1, 0)
        A.wr(o + 0x24, 4, 0x86F8F8); A.wr(o + 0x28, 4, 0); A.wr(o + 0x2C, 4, 0); A.wr(o + 0x30, 4, 0)
        if ent:
            marks.append('entity_set')
            events.append([REGREF, W.tagv(ent), tag(0, o + 0xC)])
        else:
            marks.append('entity_null')
        ret = tag(0, o)
    elif k == PUSH:
        cap, cnt = a[3], a[4]
        A.wr(o, 4, cap); A.wr(o + 4, 4, cnt)
        if a[6] == 0:
            store = W.raw(1, a[7])
        elif a[6] == 1:
            store = W.raw(2, a[7])
        else:
            store = (this + {2: 4, 3: 8, 4: 0, 5: 0xC}[a[6]]) & M32
        A.wr(o + 8, 4, (store - 4 * cnt) & M32)
        W.snap()
        loops = 0
        while sgn(A.rd(o + 4, 4)) >= sgn(A.rd(o, 4)):
            newcap = (A.rd(o, 4) << 1) & M32
            events.append([RESIZE, tag(0, o), newcap])
            A.wr(o, 4, newcap)
            loops += 1
            if loops > 40:
                raise ValueError('bucle sin fin')
        marks.append('grow_many' if loops > 2 else ('grow' if loops else 'no_grow'))
        if loops and loops <= 2:
            pass
        if a[6] >= 2:
            marks.append('alias_header')
        if sgn(cnt) < 0:
            marks.append('count_negative')
        d = A.rd(o + 8, 4)
        cn = A.rd(o + 4, 4)
        W.wr((d + cn * 4) & M32, 4, a[5])
        A.wr(o + 4, 4, (A.rd(o + 4, 4) + 1) & M32)
    elif k == SCRATCH:
        W.snap()
        events.append([NEWSCALAR, 0x800])
        events.append([NEWSCALAR, 0x10])
        p2 = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        if p2:
            events.append([DIRCTOR, W.tagv(p2), 0x1388])
            marks.append('dir_ctor_called')
        else:
            marks.append('new_null')
        ret = 1
    elif k == PLUGIN:
        W.snap()
        events.append([REGPLUGIN, 4, 0x253F2F5, 0x731600, 0x731620, 0x731630])
        new = a[3]
        for i in range(4):
            if ((a[4] >> (8 * i)) & 0xFF) != ((new >> (8 * i)) & 0xFF):
                gch.append([PLUGINGLOBAL + i, (new >> (8 * i)) & 0xFF])
        marks.append('ret_minus1' if new == M32 else 'ret_ok')
        ret = 0 if new == M32 else 1
    elif k == FXCTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x85A9C4)
        for off in (0x20, 0x24, 0x28, 0xC, 0x10, 0x14, 0x18):
            A.wr(o + off, 4, 0)
        ret = tag(0, o)
    elif k == FXDTOR:
        A.wr(o, 4, a[3])
        for j in range(4):
            A.wr(o + 0xC + 4 * j, 4, 0 if a[4 + j] == 0 else (W.raw(1, a[16 + j]) if a[4 + j] == 1 else a[20 + j]))
        W.snap()
        A.wr(o, 4, 0x85A9C4)
        for off in (0xC, 0x10, 0x14, 0x14):
            p = A.rd(o + off, 4)
            if p:
                events.append([TEXDESTROY, W.tagv(p)])
                A.wr(o + off, 4, 0)
                marks.append('tex_set')
        if 'tex_set' not in marks:
            marks.append('tex_null')
        if A.rd(o + 0x18, 4):
            marks.append('fourth_set')
    elif k == AFFECTS:
        ped = W.raw(1, a[8])
        intel = W.raw(2, a[9])
        B.wr(a[8] + 0x47C, 4, intel)
        if a[3]:
            Cc.wr(a[4], 4, 0)   # el contenido de la vtable falsa lo pone el banco: no es un cambio (se anota antes de la imagen de referencia)
        W.snap()
        task = W.raw(2, a[4]) if a[3] else 0
        events.append([GETACTIVE, W.tagv((intel + 4) & M32)])
        marks.append('task_set' if task else 'task_null')
        events.append([ISPLAYER, W.tagv(ped)])
        player = (a[5] & 0xFF) != 0
        marks.append('player' if player else 'not_player')
        res = None
        if not player and task:
            events.append([EVTASKTYPE, W.tagv(task)])
            if a[7] == 0x10C:
                res = 0
                marks.append('type_in_water')
        if res is None:
            events.append([ISALIVE, W.tagv(ped)])
            marks.append('alive_called')
            res = 1 if (a[6] & 0xFF) else 0
        ret = res
    elif k == TAKES:
        A.wr(o + 0xC, 4, a[3])
        W.snap()
        refp = W.raw(1, a[7])
        t = a[4]
        res = None
        for want in (8, 9, 0x5D):
            events.append([EVTYPE, W.tagv(refp)])
            if want == 0x5D:
                if t == 0x5D and f32(a[3]) > 1.0:
                    marks.append('accel_gt1')
                    res = 1
                break
            if t == want:
                res = 1
                break
        marks.append({8: 'type_8', 9: 'type_9', 0x5D: 'type_5d'}.get(t, 'type_other'))
        if res is None:
            events.append([EVPRIOREF, W.tagv(refp)])
            events.append([EVPRIOTHIS, tag(0, o)])
            res = 1 if sgn(a[5]) >= sgn(a[6]) else 0
            marks.append('prio_compare')
        ret = res
    elif k == STARTANIM:
        ped = W.raw(1, a[16])
        assoc = W.raw(2, a[17])
        A.wr(o + 0xC, 4, a[3]); A.wr(o + 0x38, 4, a[4]); A.wr(o + 0x3C, 4, a[5]); A.wr(o + 0x40, 4, a[6]); A.wr(o + 0x5C, 4, a[7])
        A.wr(o + 0x44, 4, a[10]); A.wr(o + 0x48, 4, a[11]); A.wr(o + 0x4C, 4, a[19]); A.wr(o + 0x8, 4, a[18]); A.wr(o + 0x60, 4, a[20])
        B.wr(a[16] + 0x18, 4, a[8])
        Cc.wr(a[17] + 0x2C, 4, a[9])
        W.snap()
        if sgn(a[6]) >= 0:
            A.wr(o + 0x44, 4, c.get('now', 0x12345)); A.wr(o + 0x48, 4, a[6]); A.wr(o + 0x4C, 1, 1)
            marks.append('timer_started')
        else:
            marks.append('timer_skipped')
        flags = A.rd(o + 0x5C, 4)
        if not flags & 8:
            A.wr(o + 0x5C, 4, flags | 4)
        events.append([BLEND, W.tagv(B.rd(a[16] + 0x18, 4)), A.rd(o + 0x3C, 4), A.rd(o + 0x5C, 4), A.rd(o + 0x38, 4)])
        A.wr(o + 0x8, 4, assoc)
        fl = A.rd(o + 0x5C, 4)
        b = A.rd(o + 0xC, 1)
        if b & 0x20:
            events.append([SETFINISH, W.tagv(assoc), 0x61AAA0, tag(0, o)])
            marks.append('offset_at_end')
        elif not (fl & 8) and not (b & 0x10):
            events.append([SETDELETE, W.tagv(assoc), 0x61A8A0, tag(0, o)])
            marks.append('delete_cb')
        else:
            events.append([SETFINISH, W.tagv(assoc), 0x61A8A0, tag(0, o)])
            marks.append('finish_cb')
        A.wr(o + 0x60, 2, Cc.rd(a[17] + 0x2C, 2))
    elif k == MATADD:
        A.wr(o + 0x28, 4, a[10]); A.wr(o + 0x2C, 4, a[11])
        for j in range(3):
            A.wr(o + 0x30 + 4 * j, 4, a[7 + j])
        A.wr(o + 0x3C, 4, a[20]); A.wr(o + 0x40, 4, a[21])
        for j in range(3):
            B.wr(a[6] + 4 * j, 4, a[3 + j])
        W.snap()
        mode = a[22]
        base = W.raw(1, a[6]) if mode == 0 else (this + 0x30 + {1: 0, 2: 4, 3: -4, 4: 8, 5: -8}[mode]) & M32
        marks.append('vec_separate' if mode == 0 else 'vec_alias')
        v = [W.rd((base + 4 * j) & M32, 4) for j in range(3)]
        p = [A.rd(o + 0x30 + 4 * j, 4) for j in range(3)]
        mk = []
        nx = add_bits(p[0], v[0], mk)
        ny = add_bits(v[1], p[1], mk)
        nz = add_bits(v[2], p[2], mk)
        marks += [m for m in mk if m in ('nan_operand', 'inf_operand')]
        A.wr(o + 0x30, 4, nx); A.wr(o + 0x34, 4, ny); A.wr(o + 0x38, 4, nz)
    elif k == ENTRYADD:
        A.wr(o, 4, a[3])
        text_in_a = a[9] != 0

        def put_text(off, v):
            if text_in_a:
                A.wr(o + a[10] + off, 1, v)
            else:
                Cc.wr(a[8] + off, 1, v)
        for i in range(a[16]):
            put_text(i, (a[20] + i % 5) & 0xFF)
        put_text(a[16], 0)
        for i in range(a[17]):
            put_text(0x18 + i, (a[20] + 7 + i % 5) & 0xFF)
        put_text(0x18 + a[17], 0)
        vec_in_a = a[11] != 0
        for j in range(3):
            if vec_in_a:
                A.wr(o + a[18] + 4 * j, 4, a[5 + j])
            else:
                B.wr(a[4] + 4 * j, 4, a[5 + j])
        W.snap()
        text = (this + a[10]) & M32 if text_in_a else W.raw(2, a[8])
        vec = (this + a[18]) & M32 if vec_in_a else W.raw(1, a[4])
        if text_in_a:
            marks.append('text_in_list')
        if sgn(A.rd(o, 4)) >= 8:
            marks.append('full')
            ret = 0
        else:
            marks.append('added')
            if a[16] > 24 or a[17] > 16:
                marks.append('long_string')
            idx = A.rd(o, 4)
            dest = (this + 0x144 + idx * 12) & M32
            for j in range(3):
                W.wr((dest + 4 * j) & M32, 4, W.rd((vec + 4 * j) & M32, 4))
            idx = A.rd(o, 4)
            entry = (this + 4 + idx * 40) & M32
            src, tgt = text, entry
            while True:
                ch = W.rd(src, 1)
                src = (src + 1) & M32
                W.wr(tgt, 1, ch)
                tgt = (tgt + 1) & M32
                if ch == 0:
                    break
            src, tgt = (text + 0x18) & M32, (entry + 0x18) & M32
            while True:
                ch = W.rd(src, 1)
                src = (src + 1) & M32
                W.wr(tgt, 1, ch)
                tgt = (tgt + 1) & M32
                if ch == 0:
                    break
            A.wr(o, 4, (A.rd(o, 4) + 1) & M32)
            ret = 1
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


def IsNanBits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


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
    if c['fn'] == 14:
        top = (int.from_bytes(img[4:6], 'little') >> 11) & 7
        if ftw != (0xFFFF & ~(3 << (2 * top))):
            return 'x87: etiquetas %04x top %d' % (ftw, top)
        if img[28:38] != bytes.fromhex('0000000000000000dc0440'):
            pass
        if img[28:38] != bytes.fromhex('00000000000000dc0440'):
            return 'x87: ST0 %s != 55.0' % img[28:38].hex()
    elif ftw != 0xFFFF:
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
