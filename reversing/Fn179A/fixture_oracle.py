"""Oraculo independiente (Python) de Fn179A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 179A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (mantisa de 24 bits, exponente ampliado) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x410610  CPool<ColDef>::Flush: esi = ecx; si [esi+8] > 0 (con signo): si BYTE [esi+10h] != 0: delete[] [esi] (0x8213AE), delete[] [esi+4]; [esi], [esi+4] = 0 (y con la capacidad positiva) [esi+8] = [esi+0Ch] = 0
   1  0x410F80  CPool<ColDef>::CPool(cap, nombre) `ret 8`: [esi] = new[] (cap * 2Ch) (0x821195); [esi+4] = new[] (cap); BYTE [esi+10h] = 1; [esi+8] = cap; [esi+0Ch] = -1; si cap > 0 (con signo): para i < cap:
            BYTE [[esi+4] + i] = ([..] | 80h) & 80h; eax = esi
   2  0x410FE0  segunda copia de Flush
   3  0x411030  CPool<ColDef>::New: edx = capacidad; bucle: [ecx+0Ch] += 1; si == capacidad: [ecx+0Ch] = 0 y, si ya habia dado la vuelta, devuelve 0; la segunda pasada pone la bandera; si BYTE [[ecx+4] + [ecx+0Ch]] & 80h:
            b = byte & 7Fh; byte = b; byte = (b + 1) & 7Fh; eax = [ecx] + [ecx+0Ch] * 2Ch
   4  0x4110E0  destructor con borrado: cuerpo de Flush (ya con los cuatro campos a cero) y, con BYTE [esp+8] & 1, operator delete (0x8214BD) de this; eax = this; `ret 4`
   5  0x609270  mov eax,4Ch
   6  0x609280  CloneEditable: 4B5620 (0x1C); sin memoria devuelve 0; 4B23D0 (ECX = memoria, 2, [this+18h]); [memoria] = 86D120h; eax = memoria
   7  0x6092E0  destructor con borrado: call 609300; con BYTE [esp+8] & 1: CEvent::operator delete 4B5630; eax = this; `ret 4`
   8  0x609300  [ecx] = 86D120h; jmp 4B2470
   9  0x609310  mov eax,2Ah          10  0x609320  mov eax,2Bh          11  0x609330  mov eax,2Dh
  12  0x6093B0  [this] = 86D168h; para +48h y luego +4Ch de [this+480h]: si el campo != 0: RemoveAnimBlockRef (4D3FD0) con ((ms_aAnimAssocGroups (0xB4EA34)[campo * 20] - 0B5D4A0h) >> 5), [this+480h] releido, campo = 0;
            si [this+478h] & 200000h == 0: RemoveGroup (5FB870) con [[this+480h] + 38h] (releido); ~CPed 5E8620
  13  0x65EBA0  mov eax,4B7h
  14  0x65ED40  CalculateOffsetPosition(destino) `ret 4`: ped = [this+10h]; estado = [ped+534h]; mover = estado en {4, 6, 7}; pos = [ped+14h] ? [ped+14h]+30h : ped+4; d = pos - [this+14h] (x87, 24 bits);
            mover: FromMultiply3x3 (59C6D0) sobre this+20h con la matriz [ped+14h] y (MovingOffsets[idx] (0xC19728 + 8 * idx), 0); flags &= 0EFh; si no: suma = ((dz*dz) + (dy*dy)) + (dx*dx); si suma > 9.0 (ordenado) o
            falta la bandera 10h en BYTE [this+3Dh]: [this+14h..1Ch] = pos (dwords); [this+20h..28h] = (Offsets[idx] (0xC196E8 + 8 * idx), 0); flags |= 10h; copia this+20h (3 dwords) al destino
  15  0x6634E0  destructor con borrado: call 65EEF0; BYTE [esp+8] & 1: CTask::operator delete (61A5B0); eax = this
  16-19, 21  destructores con borrado de los asignadores: call <cuerpo>; con el bit 0: pool = [0B744BCh]; idx = (this - [pool]) >> 5 (aritmetico); BYTE [[pool+4] + idx] |= 80h; si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx; eax = this
  20  0x69DAC0  [this] = 870E30h SIEMPRE; con el bit 0: la misma liberacion
  22  0x69DB40  [ecx] = 870E78h; jmp 69C2D0
  23  0x72A620  [this+10h] = 0; [this] = 872A74h; BYTE [this+7Ch] = 0; eax = this
  24  0x72A640  UpdateWeatherRegion(pos) cdecl: p = pos ? *pos : (matriz [0B6F03Ch] ? [0B6F03Ch]+30h : 0B6F02Ch) (3 floats); region 3 si x > 1000 y y > 910; 4 si x > -850, x < 1000 e y > 1280; 2 si x < -1430, y > -580 e y < 1430;
            1 si x > 250, x < 3000, y > -3000 e y < -850; 0 si no; WORD [0C81314h] = region (solo 2 bytes). Con NaN las comparaciones dan falso.

Los falsos anotan: [0x8213AE, argumento (etiqueta o crudo)]; [0x821195, tamano]; [0x8214BD / 0x4B5630 / 0x61A5B0, argumento (etiqueta), dword de [argumento]]; [0x4B5620, tamano];
[cuerpos 0x609300, 0x4B2470, 0x5E8620, 0x65EEF0, 0x69BBF0, 0x69C2D0, 0x69C780, 0x69CB60, 0x69DB40: ECX (etiqueta), dword de [ECX] al entrar]; [0x4B23D0, ECX, primer argumento, segundo argumento];
[0x4D3FD0, indice]; [0x5FB870, grupo]; [0x59C6D0, ECX (etiqueta), matriz (etiqueta), tres dwords del vector].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CPool<ColDef>::Flush", "CPool<ColDef>::Constructor", "CPool<ColDef>::FlushSecondCopy", "CPool<ColDef>::New", "CPool<ColDef>::ScalarDeletingDestructor",
         "CEventPlayerCommandToGroupGather::GetEventType", "CEventPlayerCommandToGroupGather::CloneEditable", "CEventPlayerCommandToGroupGather::ScalarDeletingDestructor",
         "CEventPlayerCommandToGroupGather::DestructorBody", "CEventNewGangMember::GetEventPriority", "CEventDontJoinPlayerGroup::GetEventPriority",
         "CEventPlayerCommandToGroupGather::GetEventPriority", "CPlayerPed::DestructorBody", "CTaskComplexGangFollower::GetTaskType", "CTaskComplexGangFollower::CalculateOffsetPosition",
         "CTaskSimpleGoToPointFine::ScalarDeletingDestructor", "CTaskAllocatorKillOnFoot::ScalarDeletingDestructor", "CTaskAllocatorAttack::ScalarDeletingDestructor",
         "CTaskAllocatorKillThreatsBasic::ScalarDeletingDestructor", "CTaskAllocatorKillThreatsDriveby::ScalarDeletingDestructor", "CTaskAllocator::ScalarDeletingDestructor",
         "CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor", "CTaskAllocatorPlayerCommandAttack::DestructorBody", "CAEWeatherAudioEntity::Constructor", "CWeather::UpdateWeatherRegion"]
ENTRIES = (0x410610, 0x410F80, 0x410FE0, 0x411030, 0x4110E0, 0x609270, 0x609280, 0x6092E0, 0x609300, 0x609310, 0x609320, 0x609330, 0x6093B0, 0x65EBA0, 0x65ED40, 0x6634E0, 0x69D9C0, 0x69DA00,
           0x69DA40, 0x69DA80, 0x69DAC0, 0x69DB00, 0x69DB40, 0x72A620, 0x72A640)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CONST, FLUSH, PCTOR, PNEW, PSDD, CLONE, SDD, EVBODY, PLAYER, GANG, ALLOC, ALLOCBASE, WCTOR, WREGION = \
    'const flush pctor pnew psdd clone sdd evbody player gang alloc allocbase wctor wregion'.split()
KIND = [FLUSH, PCTOR, FLUSH, PNEW, PSDD, CONST, CLONE, SDD, EVBODY, CONST, CONST, CONST, PLAYER, CONST, GANG, SDD, ALLOC, ALLOC, ALLOC, ALLOC, ALLOCBASE, ALLOC, EVBODY, WCTOR, WREGION]
CONST_VALUE = {5: 0x4C, 9: 0x2A, 10: 0x2B, 11: 0x2D, 13: 0x4B7}
SDD_BODY = {7: 0x609300, 15: 0x65EEF0, 16: 0x69BBF0, 17: 0x69C2D0, 18: 0x69C780, 19: 0x69CB60, 21: 0x69DB40}
SDD_DELETE = {7: 0x4B5630, 15: 0x61A5B0}
EVBODY_INFO = {8: (0x86D120, 0x4B2470), 22: (0x870E78, 0x69C2D0)}
GLOBAL_ALLOC_POOL, GLOBAL_ANIM_GROUPS, GLOBAL_OFFSETS, GLOBAL_MOVING, GLOBAL_CAMERA, GLOBAL_WEATHER = 0xB744BC, 0xB4EA34, 0xC196E8, 0xC19728, 0xB6F028, 0xC81314


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta; memoria global del juego aparte"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))
        self.glob = {}
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
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

    def getg(self, addr, n=4):
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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (FLUSH, PSDD):
        _k += ['cap_pos', 'cap_nonpos', 'owns_set', 'owns_clear', 'storage_null', 'storage_arena']
        if _kind == PSDD:
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits']
    elif _kind == PCTOR:
        _k += ['cap_pos', 'cap_zero', 'cap_neg', 'size_wraps']
    elif _kind == PNEW:
        _k += ['first_try', 'wrapped', 'full', 'counter_wrap', 'ret_null', 'cursor_last']
    elif _kind == CLONE:
        _k += ['mem_null', 'mem_in_b', 'ctor_ret_mem', 'ctor_ret_mem4', 'ctor_ret_zero', 'ctor_ret_raw']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _kind == EVBODY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == PLAYER:
        _k += ['group1_set', 'group1_zero', 'group2_set', 'group2_zero', 'replay_flag', 'no_replay_flag', 'swap1', 'swap2', 'noswap', 'negative_group']
    elif _kind == GANG:
        _k += ['moving', 'still', 'far', 'near', 'recalc_flag_clear', 'skip_recalc', 'matrix_used', 'matrix_null', 'nan_sum']
    elif _kind in (ALLOC, ALLOCBASE):
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'lowers_cursor', 'keeps_cursor', 'first_slot', 'last_slot']
    elif _kind == WCTOR:
        _k += ['prev_zero', 'prev_other']
    elif _kind == WREGION:
        _k += ['pos_given', 'pos_camera', 'camera_matrix', 'camera_placement', 'region0', 'region1', 'region2', 'region3', 'region4', 'nan_input']
    KEYS[_fn] = _k


# ------------------------------------------------------------------ aritmetica x87 con mantisa de 24 bits y exponente ampliado (fracciones exactas)
NAN = ('n',)


def fval(bits):
    bits &= M32
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    sign = -1 if bits >> 31 else 1
    if e == 0xFF:
        return NAN if m else ('i', sign)
    if e == 0:
        return ('f', sign * Fraction(m, 1 << 149))
    return ('f', sign * Fraction((1 << 23) | m, 1) * Fraction(2) ** (e - 150))


def r24(v):
    if v == 0:
        return Fraction(0)
    sign = -1 if v < 0 else 1
    m = abs(v)
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if m < Fraction(2) ** e:
        e -= 1
    q = m / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return sign * Fraction(n) * Fraction(2) ** (e - 23)


def x_sub(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN if a[1] == b[1] else ('i', a[1])
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return ('i', -b[1])
    return ('f', r24(a[1] - b[1]))


def x_add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN if a[1] != b[1] else a
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    return ('f', r24(a[1] + b[1]))


def x_mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        other = b if a[0] == 'i' else a
        if other[0] == 'f' and other[1] == 0:
            return NAN
        sa = a[1] if a[0] == 'i' else (1 if a[1] > 0 else -1)
        sb = b[1] if b[0] == 'i' else (1 if b[1] > 0 else -1)
        return ('i', sa * sb)
    return ('f', r24(a[1] * b[1]))


def x_gt(a, c):
    """a > c (ordenado; NaN da falso); c es una Fraction"""
    if a == NAN:
        return False
    if a[0] == 'i':
        return a[1] > 0
    return a[1] > c


def squared_distance_greater_than_9(pos, last):
    dx = x_sub(fval(pos[0]), fval(last[0]))
    dy = x_sub(fval(pos[1]), fval(last[1]))
    dz = x_sub(fval(pos[2]), fval(last[2]))
    s = x_add(x_mul(dz, dz), x_mul(dy, dy))
    s = x_add(s, x_mul(dx, dx))
    return x_gt(s, Fraction(9)), s == NAN or (s[0] == 'i')


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    if k not in (CLONE, WREGION):
        A.wr(o, 4, a[3])

    def ptr(mode, arena_addr, raw):
        return 0 if mode == 0 else (arena_addr if mode == 1 else raw)

    if k == CONST:
        W.snap()
        ret = CONST_VALUE[fn]
    elif k in (FLUSH, PSDD):
        storage, flags = ptr(a[4], W.raw(1, a[5]), a[6]), ptr(a[7], W.raw(2, a[8]), a[9])
        A.wr(o, 4, storage); A.wr(o + 4, 4, flags); A.wr(o + 8, 4, a[10]); A.wr(o + 0xC, 4, a[11]); A.wr(o + 0x10, 1, a[12] & 0xFF)
        W.snap()
        cap = s32(a[10])
        marks.append('cap_pos' if cap > 0 else 'cap_nonpos')
        marks.append('owns_set' if a[12] & 0xFF else 'owns_clear')
        marks.append('storage_null' if storage == 0 else ('storage_arena' if a[4] == 1 else 'storage_raw'))
        if cap > 0:
            if a[12] & 0xFF:
                events.append([0x8213AE, W.tagv(storage)])
                events.append([0x8213AE, W.tagv(flags)])
            A.wr(o, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        if k == PSDD:
            fl = a[13] & 0xFF
            marks.append('flag_bit0_set' if fl & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[13] >> 8 else 'flag_byte_only')
            if fl & 1:
                events.append([0x8214BD, W.tagv(this), A.rd(o, 4)])
            ret = W.tagv(this)
    elif k == PCTOR:
        p1, p2 = ptr(a[4], W.raw(1, a[5]), a[6]), ptr(a[7], W.raw(2, a[8]), a[9])
        W.snap()
        cap = s32(a[10])
        events.append([0x821195, (a[10] * 0x2C) & M32])
        events.append([0x821195, a[10] & M32])
        A.wr(o, 4, p1); A.wr(o + 4, 4, p2); A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, a[10]); A.wr(o + 0xC, 4, M32)
        if cap > 0:
            for i in range(cap):
                W.wr(p2 + i, 1, 0x80)
        marks.append('cap_pos' if cap > 0 else ('cap_zero' if cap == 0 else 'cap_neg'))
        if a[10] * 0x2C > M32:
            marks.append('size_wraps')
        ret = W.tagv(this)
    elif k == PNEW:
        cap = a[6]
        A.wr(o + 0, 4, a[4]); A.wr(o + 4, 4, W.raw(3, a[5])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 1, a[8] & 0xFF)
        for i in range(cap):
            counter = 0 if a[11] == 0 else (0x7F if a[11] == 1 else rnd(a[63], 1000 + i) & 0x7F)
            if a[9] == 0:
                free = False
            elif a[9] == 1:
                free = i == a[10]
            elif a[9] == 2:
                free = rnd(a[63], 5000 + i) % 10 == 0
            elif a[9] == 3:
                free = (rnd(a[63], 6000 + i) & 1) != 0
            else:
                free = True
            D.wr(a[5] + i, 1, counter | (0x80 if free else 0))
        W.snap()
        last = s32(a[7])
        wrapped = False
        steps = 0
        ret = 0
        while True:
            last += 1
            steps += 1
            if last == cap:
                last = 0
                if wrapped:
                    marks.append('ret_null')
                    marks.append('full')
                    ret = 0
                    break
                wrapped = True
                marks.append('wrapped')
            byte = D.rd(a[5] + last, 1)
            if byte & 0x80:
                b = byte & 0x7F
                if b == 0x7F:
                    marks.append('counter_wrap')
                D.wr(a[5] + last, 1, (b + 1) & 0x7F)
                ret = W.tagv((a[4] + last * 0x2C) & M32)
                if steps == 1 and not wrapped:
                    marks.append('first_try')
                if last == cap - 1:
                    marks.append('cursor_last')
                break
        A.wr(o + 0xC, 4, last & M32)
    elif k == CLONE:
        A.wr(o + 0x18, 4, a[8])
        W.snap()
        events.append([0x4B5620, 0x1C])
        mem = 0 if a[3] == 0 else W.raw(1, a[4])
        sel = a[7]
        ctor_eax = mem if sel == 0 else ((mem + 4) & M32 if sel == 1 else (0 if sel == 2 else a[6]))
        marks.append('mem_null' if mem == 0 else 'mem_in_b')
        marks.append(['ctor_ret_mem', 'ctor_ret_mem4', 'ctor_ret_zero', 'ctor_ret_raw'][sel])
        if mem == 0:
            ret = 0
        else:
            events.append([0x4B23D0, W.tagv(mem), 2, a[8]])
            W.wr(mem, 4, a[5])            # el falso del constructor deja gDtorWrite en [ECX]
            W.wr(mem, 4, 0x86D120)        # y la funcion escribe despues su vtable
            ret = W.tagv(mem)
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), a[3]])
        A.wr(o, 4, a[5])                                     # el falso del cuerpo deja gDtorWrite en [this]
        flags = a[6] & 0xFF                                  # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([SDD_DELETE[fn], W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        ret = W.tagv(this)
    elif k == EVBODY:
        W.snap()
        vt, body = EVBODY_INFO[fn]
        A.wr(o, 4, vt)
        events.append([body, W.tagv(this), vt])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == PLAYER:
        base = a[9]
        A.wr(o + 0x478, 4, a[5]); A.wr(o + 0x480, 4, W.raw(1, a[4]))
        B.wr(a[4] + 0x48, 4, a[6]); B.wr(a[4] + 0x4C, 4, a[7]); B.wr(a[4] + 0x38, 4, a[8])
        for g_, blk in ((s32(a[6]), a[10]), (s32(a[7]), a[11])):
            if g_ != 0:
                D.wr(s32(base) + g_ * 20, 4, blk)
        B.wr(a[13] + 0x48, 4, a[15]); B.wr(a[13] + 0x4C, 4, a[16]); B.wr(a[13] + 0x38, 4, a[17])
        for g_, blk in ((s32(a[15]), a[18]), (s32(a[16]), a[19])):
            if g_ != 0:
                D.wr(s32(base) + g_ * 20, 4, blk)
        groups = W.raw(3, base)
        W.setg(GLOBAL_ANIM_GROUPS, 4, groups)
        gorder = [(GLOBAL_ANIM_GROUPS, 4)]
        W.snap()
        A.wr(o, 4, 0x86D168)
        calls = [0]

        def release(field):
            data = A.rd(o + 0x480, 4)
            group = s32(W.rd(data + field, 4))
            if group != 0:
                blk = W.rd(W.getg(GLOBAL_ANIM_GROUPS, 4) + group * 20, 4)
                arg = ((s32(blk - 0xB5D4A0) >> 5) & M32)
                events.append([0x4D3FD0, arg])
                calls[0] += 1
                if calls[0] == a[12]:
                    A.wr(o + 0x480, 4, W.raw(1, a[13]))
                data = A.rd(o + 0x480, 4)
                W.wr(data + field, 4, 0)
                return group
            return 0
        g1 = release(0x48)
        g2 = release(0x4C)
        marks.append('group1_set' if g1 else 'group1_zero')
        marks.append('group2_set' if g2 else 'group2_zero')
        if g1 < 0 or g2 < 0:
            marks.append('negative_group')
        marks.append('swap1' if a[12] == 1 else ('swap2' if a[12] == 2 else 'noswap'))
        if not (A.rd(o + 0x478, 4) & 0x200000):
            data = A.rd(o + 0x480, 4)
            events.append([0x5FB870, W.rd(data + 0x38, 4)])
            marks.append('no_replay_flag')
        else:
            marks.append('replay_flag')
        events.append([0x5E8620, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[14])
    elif k == GANG:
        ped = W.raw(1, a[4])
        matrix = W.raw(2, a[6]) if a[5] else 0
        A.wr(o + 0x10, 4, ped)
        for i in range(3):
            A.wr(o + 0x14 + 4 * i, 4, a[14 + i]); A.wr(o + 0x20 + 4 * i, 4, a[17 + i])
        A.wr(o + 0x3C, 1, a[20]); A.wr(o + 0x3D, 1, a[21])
        for i in range(3):
            B.wr(a[4] + 4 + 4 * i, 4, a[8 + i])
        B.wr(a[4] + 0x14, 4, matrix)
        B.wr(a[4] + 0x534, 4, a[7])
        if a[5]:
            for i in range(3):
                Cc.wr(a[6] + 0x30 + 4 * i, 4, a[11 + i])
        for i in range(8):
            W.setg(GLOBAL_OFFSETS + 4 * i, 4, a[24 + i]); W.setg(GLOBAL_MOVING + 4 * i, 4, a[32 + i])
        gorder = [(GLOBAL_OFFSETS, 0x20), (GLOBAL_MOVING, 0x20)]
        out = W.raw(3, a[22])
        W.snap()
        state = a[7]
        moving = state in (4, 6, 7)
        pos = [W.rd((matrix + 0x30 + 4 * i) if matrix else (ped + 4 + 4 * i), 4) for i in range(3)]
        last = [A.rd(o + 0x14 + 4 * i, 4) for i in range(3)]
        far, nan_like = squared_distance_greater_than_9(pos, last)
        idx = A.rd(o + 0x3C, 1)
        flags = A.rd(o + 0x3D, 1)
        marks.append('moving' if moving else 'still')
        marks.append('matrix_used' if matrix else 'matrix_null')
        if nan_like:
            marks.append('nan_sum')
        if moving:
            vx, vy = W.getg(GLOBAL_MOVING + 8 * idx, 4), W.getg(GLOBAL_MOVING + 8 * idx + 4, 4)
            events.append([0x59C6D0, W.tagv(this + 0x20), W.tagv(matrix), vx, vy, 0])
            A.wr(o + 0x20, 4, vx ^ 0x11111111); A.wr(o + 0x24, 4, vy ^ 0x22222222); A.wr(o + 0x28, 4, 0x33333333)
            A.wr(o + 0x3D, 1, flags & 0xEF)
        else:
            marks.append('far' if far else 'near')
            if far or not (flags & 0x10):
                marks.append('recalc_flag_clear' if not (flags & 0x10) else 'far_recalc')
                for i in range(3):
                    A.wr(o + 0x14 + 4 * i, 4, pos[i])
                A.wr(o + 0x20, 4, W.getg(GLOBAL_OFFSETS + 8 * idx, 4)); A.wr(o + 0x24, 4, W.getg(GLOBAL_OFFSETS + 8 * idx + 4, 4)); A.wr(o + 0x28, 4, 0)
                A.wr(o + 0x3D, 1, flags | 0x10)
            else:
                marks.append('skip_recalc')
        for i in range(3):
            W.wr(out + 4 * i, 4, A.rd(o + 0x20 + 4 * i, 4))
    elif k in (ALLOC, ALLOCBASE):
        idx = a[9]
        storage = (this - 32 * idx) & M32
        B.wr(a[7] + 0, 4, storage); B.wr(a[7] + 4, 4, W.raw(3, a[10])); B.wr(a[7] + 8, 4, a[8]); B.wr(a[7] + 0xC, 4, a[11]); B.wr(a[7] + 0x10, 1, 1)
        D.wr(a[10] + idx, 1, a[12] & 0x7F)
        W.setg(GLOBAL_ALLOC_POOL, 4, W.raw(1, a[7]))
        gorder = [(GLOBAL_ALLOC_POOL, 4)]
        W.snap()
        if k == ALLOC:
            events.append([SDD_BODY[fn], W.tagv(this), a[3]])
            A.wr(o, 4, a[5])
        else:
            A.wr(o, 4, 0x870E30)
        flags = a[6] & 0xFF
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        if flags & 1:
            pool = W.getg(GLOBAL_ALLOC_POOL, 4)
            index = s32(this - W.rd(pool, 4)) >> 5
            state = W.rd(pool + 4, 4)
            W.wr(state + index, 1, W.rd(state + index, 1) | 0x80)
            last = s32(W.rd(pool + 0xC, 4))
            if index < last:
                W.wr(pool + 0xC, 4, index & M32)
                marks.append('lowers_cursor')
            else:
                marks.append('keeps_cursor')
            if index == 0:
                marks.append('first_slot')
            if index == a[8] - 1:
                marks.append('last_slot')
        ret = W.tagv(this)
    elif k == WCTOR:
        W.snap()
        A.wr(o + 0x10, 4, 0)
        A.wr(o, 4, 0x872A74)
        A.wr(o + 0x7C, 1, 0)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == WREGION:
        if a[4]:
            A.wr(o, 4, a[5]); A.wr(o + 4, 4, a[6]); A.wr(o + 8, 4, a[7])
        for i in range(3):
            W.setg(GLOBAL_CAMERA + 4 + 4 * i, 4, a[10 + i])
        matrix = W.raw(2, a[9]) if a[8] else 0
        W.setg(GLOBAL_CAMERA + 0x14, 4, matrix)
        if a[8]:
            for i in range(3):
                Cc.wr(a[9] + 0x30 + 4 * i, 4, a[13 + i])
        W.setg(GLOBAL_WEATHER, 4, a[16])
        gorder = [(GLOBAL_CAMERA, 0x18), (GLOBAL_WEATHER, 4)]
        W.snap()
        if a[4]:
            x, y = a[5], a[6]
            marks.append('pos_given')
        else:
            marks.append('pos_camera')
            if a[8]:
                x, y = a[13], a[14]
                marks.append('camera_matrix')
            else:
                x, y = a[10], a[11]
                marks.append('camera_placement')
        fx, fy = f32(x), f32(y)
        if math.isnan(fx) or math.isnan(fy):
            marks.append('nan_input')
        if fx > 1000.0 and fy > 910.0:
            region = 3
        elif fx > -850.0 and fx < 1000.0 and fy > 1280.0:
            region = 4
        elif fx < -1430.0 and fy > -580.0 and fy < 1430.0:
            region = 2
        elif fx > 250.0 and fx < 3000.0 and fy > -3000.0 and fy < -850.0:
            region = 1
        else:
            region = 0
        marks.append('region%d' % region)
        W.setg(GLOBAL_WEATHER, 2, region)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


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
