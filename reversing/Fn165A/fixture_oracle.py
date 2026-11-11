"""Oraculo independiente (Python) de Fn165A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 165A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x4F2930  CAETwinLoopSoundEntity::StopSound (thiscall, `ret`): esi = this; ecx = [esi+0A0h]; si no es nulo: call 4EF1C0 (CAESound::StopSound); ecx = [esi+0A4h] (leido DESPUES de la primera llamada);
                si no es nulo: jmp 4EF1C0 (llamada de cola)
   1  0x4F2B80  CAETwinLoopSoundEntity cuerpo del destructor (thiscall, `ret`, marco SEH): [esi] = 85F438h; si WORD [esi+88h] != 0: para +0A0 y despues +0A4: ecx = [esi+off]; si no es nulo: call 4EF850
                (StopSoundAndForget) y [esi+off] = 0; WORD [esi+88h] = 0; [esi] = 859988h; ecx = esi+8; call 4EF660 (~CAESound sobre m_tempSound)
   2  0x5DEA00  CEventSoundQuiet cuerpo del destructor (vtable 0x86C2B0, marco SEH): [esi] = 86C2B0h; ecx = [esi+14h]; si no es nulo: call 571A00 (CleanUpOldReference, ecx = la entidad, argumento = esi+14h);
                ecx = esi; call 4AC480 (cuerpo de ~CEventEditableResponse)
   3  0x5DEA60  CEventSoundQuiet::GetEventType (ranura 1): mov eax,3Eh; ret
   4  0x5DEA70  CEventSoundQuiet::GetLifeTime (ranura 3): xor eax,eax; ret
   5  0x5DEA80  CEventSoundQuiet::GetSourceEntity: mov eax,[ecx+14h]; ret
   6  0x5DEA90  CEventSoundQuiet::GetLocalSoundLevel: fld dword [ecx+18h]; ret (el resultado queda en ST0)
   7  0x5DEAA0  CEventSoundQuiet::CanBeInterruptedBySameEvent: mov al,1; ret
   8  0x5DEAB0  CEventSoundQuiet destructor con borrado (ranura 0): cuerpo 5DEA00 (funcion 2); CEvent::operator delete 4B5630
   9  0x5DEAD0  CEventSoundQuiet::GetEventPriority (ranura 2): mov eax,8; ret
  10  0x5DEB90  CPedIntelligence destructor con borrado: push esi; mov esi,ecx; call 607300; test byte [esp+8],1; je +; push esi; call 6074E0; add esp,4; mov eax,esi; pop esi; ret 4
  11  0x4CEB60  CAnimBlendAssociation::GetNode (thiscall(indice) `ret 4`): eax = [esp+4]; ecx = [ecx+10h]; eax = eax*3; eax = ecx + eax*8
  12  0x4CEC00  CAnimBlendStaticAssociation cuerpo del destructor (vtable 0x85C6CC): [ecx] = 85C6CCh; ecx = [ecx+0Ch]; si no es nulo: push ecx; call 72F430 (CMemoryMgr::Free, cdecl); pop ecx; ret
  13  0x4CEF30  CAnimBlendStaticAssociation destructor con borrado: eax = [esi+0Ch]; [esi] = 85C6CCh; si eax no es nulo: push eax; call 72F430; add esp,4; test byte [esp+8],1; je +; push esi; call 8214BD;
                add esp,4; eax = esi; ret 4
  14  0x4D1570  RpAnimBlendNodeUpdateKeyFrames (EAX = contexto; pila: frames, n; `ret`): para cada array de nodos [contexto+4+4k] hasta un nulo: ebp = [array+14h] (asociacion del primer nodo); si n (con signo) > 0:
                para i en 0..n-1: si byte [frames+18h*i] & 8 y [[0B4EA0Ch]+0Ch] != 0: siguiente; nodo = array+18h*i; si [nodo+10h] == 0: siguiente; call 4D0240 (thiscall, ecx = nodo, el float [ebp+20h] en la pila)
  15  0x63A2E0  CTaskUtilityLineUpPedWithCar destructor con borrado: cuerpo 64FC00; operator delete 8214BD (CRT)
  16  0x643C60  CTaskComplexEnterCarAsDriver (vtable 0x86EAAC) destructor con borrado: cuerpo 640330; CTask::operator delete 61A5B0
  17  0x643C80  CTaskComplexEnterCarAsPassenger (0x86EADC): cuerpo 640380; 61A5B0
  18  0x643CA0  CTaskComplexOpenDriverDoor (0x86EB0C): cuerpo 6403D0; 61A5B0
  19  0x643CC0  CTaskComplexOpenPassengerDoor (0x86EB3C): cuerpo 640420; 61A5B0
  20  0x643CE0  CTaskComplexDragPedFromCar (0x86EB6C): cuerpo 6404D0; 61A5B0
  21  0x590DE0  FurnitureGroup_c::FurnitureGroup_c: call 4A8DD0 (ecx = this: tres dwords a cero en +0, +4, +8); eax = this
  22  0x590E00  ListItem_c<FurnitureSubGroup_c> constructor: call 4A8DB0 (dos dwords a cero en +0 y +4); eax = this
  23  0x590E20  FurnitureSubGroup_c::FurnitureSubGroup_c (marco SEH): 4A8DB0 sobre this (+0, +4) y 4A8DD0 sobre this+0Ch (+0Ch, +10h, +14h) a cero; eax = this
  24  0x590E70  FurnitureSubGroup_c::~FurnitureSubGroup_c (marco SEH): call 4A8DE0 (ret) sobre this+0Ch y call 4A8DC0 (ret) sobre this: no escribe nada
  25  0x590EC0  Furniture_c::Furniture_c: call 4A8DB0 (+0, +4 a cero); eax = this

Los falsos de los colaboradores anotan: cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero];
CleanUpOldReference [direccion, ECX, argumento (etiquetas)]; StopSound [direccion, ECX]; StopSoundAndForget [direccion, ECX, +0A0, +0A4, +88, [this]]; ~CAESound [direccion, ECX, [ECX], [this], +0A0, +0A4, +88];
CMemoryMgr::Free [direccion, puntero (etiqueta), dword del puntero si cae en una arena, si no 0]; FindKeyFrame [direccion, ECX (etiqueta del nodo), bits del float]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CAETwinLoopSoundEntity::StopSound', 'CAETwinLoopSoundEntity::Destructor', 'CEventSoundQuiet::DestructorBody', 'CEventSoundQuiet::GetEventType', 'CEventSoundQuiet::GetLifeTime',
         'CEventSoundQuiet::GetSourceEntity', 'CEventSoundQuiet::GetLocalSoundLevel', 'CEventSoundQuiet::CanBeInterruptedBySameEvent', 'CEventSoundQuiet::ScalarDeletingDestructor',
         'CEventSoundQuiet::GetEventPriority', 'CPedIntelligence::ScalarDeletingDestructorExe', 'CAnimBlendAssociation::GetNode', 'CAnimBlendStaticAssociation::DestructorBody',
         'CAnimBlendStaticAssociation::ScalarDeletingDestructor', 'RpAnimBlendNodeUpdateKeyFramesExe', 'CTaskUtilityLineUpPedWithCar::ScalarDeletingDestructorExe',
         'CTaskComplexEnterCarAsDriver::ScalarDeletingDestructor', 'CTaskComplexEnterCarAsPassenger::ScalarDeletingDestructor', 'CTaskComplexOpenDriverDoor::ScalarDeletingDestructorExe',
         'CTaskComplexOpenPassengerDoor::ScalarDeletingDestructorExe', 'CTaskComplexDragPedFromCar::ScalarDeletingDestructor', 'FurnitureGroup_c::Constructor',
         'FurnitureSubGroup_c::ListItemConstructor', 'FurnitureSubGroup_c::Constructor', 'FurnitureSubGroup_c::Destructor', 'Furniture_c::Constructor']
ENTRIES = (0x4F2930, 0x4F2B80, 0x5DEA00, 0x5DEA60, 0x5DEA70, 0x5DEA80, 0x5DEA90, 0x5DEAA0, 0x5DEAB0, 0x5DEAD0, 0x5DEB90, 0x4CEB60, 0x4CEC00, 0x4CEF30, 0x4D1570, 0x63A2E0,
           0x643C60, 0x643C80, 0x643CA0, 0x643CC0, 0x643CE0, 0x590DE0, 0x590E00, 0x590E20, 0x590E70, 0x590EC0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

STOP_SOUND, FORGET, SOUND_DTOR, CLEANUP, EDITABLE_BODY, FREE, FINDKF = 0x4EF1C0, 0x4EF850, 0x4EF660, 0x571A00, 0x4AC480, 0x72F430, 0x4D0240

(CONST, CONSTAL, LOAD, FLOAT, SDD, BODYREF, TWINSTOP, TWINDTOR, GETNODE, STATICBODY, STATICSDD, UPDATEKF, CTOR, NOP) = 'const constal load float sdd bodyref twinstop twindtor getnode staticbody staticsdd updatekf ctor nop'.split()
KIND = [TWINSTOP, TWINDTOR, BODYREF, CONST, CONST, LOAD, FLOAT, CONSTAL, SDD, CONST, SDD, GETNODE, STATICBODY, STATICSDD, UPDATEKF, SDD, SDD, SDD, SDD, SDD, SDD, CTOR, CTOR, CTOR, NOP, CTOR]
CONSTANT = {3: 0x3E, 4: 0, 9: 8}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {8: 0x5DEA00, 10: 0x607300, 15: 0x64FC00, 16: 0x640330, 17: 0x640380, 18: 0x6403D0, 19: 0x640420, 20: 0x6404D0}
DELETE = {8: 0x4B5630, 10: 0x6074E0, 15: 0x8214BD, 16: 0x61A5B0, 17: 0x61A5B0, 18: 0x61A5B0, 19: 0x61A5B0, 20: 0x61A5B0}
CTOR_WRITES = {21: (0, 4, 8), 22: (0, 4), 23: (0, 4, 0xC, 0x10, 0x14), 25: (0, 4)}
SPAN = [0xA8, 0xA8, 0x18, 4, 4, 0x18, 0x1C, 4, 4, 4, 4, 0x14, 0x10, 0x10, 0x40, 4, 4, 4, 4, 4, 4, 0xC, 8, 0x18, 4, 8]


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

    def inside(self, addr):
        return any(b <= addr < b + N for b in self.bases)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


KEYS = {}
for _fn in range(26):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, CONSTAL, LOAD, FLOAT, CTOR, NOP, GETNODE):
        _k += ['this_aligned', 'this_unaligned', 'spill']
        if _kind == FLOAT:
            _k += ['float_zero', 'float_nan', 'float_inf', 'float_denormal', 'float_normal', 'float_negative']
        if _kind == GETNODE:
            _k += ['index_zero', 'index_negative', 'index_large', 'index_small']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == BODYREF:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'entity_set', 'entity_null', 'entity_in_arena']
    elif _kind == TWINSTOP:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'voice0_set', 'voice0_null', 'voice1_set', 'voice1_null', 'voice_both', 'voice_same', 'mutate_second', 'voice_in_arena']
    elif _kind == TWINDTOR:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'inuse_zero', 'inuse_set', 'inuse_high_only', 'voice0_set', 'voice0_null', 'voice1_set', 'voice1_null', 'voice_same', 'prev_zero', 'prev_other']
    elif _kind in (STATICBODY, STATICSDD):
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'seqs_null', 'seqs_in_arena', 'seqs_raw', 'seqs_is_this']
        if _kind == STATICSDD:
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == UPDATEKF:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'n_positive', 'n_zero', 'n_negative', 'arrays_0', 'arrays_1', 'arrays_2', 'arrays_3', 'pedpos_null', 'pedpos_set', 'velocity_skip', 'velocity_kept', 'seq_null', 'seq_call', 'time_nan', 'time_normal']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN[fn] > WIN:
        marks.append('spill')
    return marks


def ext80(bits):
    """imagen de 10 bytes del float `bits` cargado con fld (x87 de 80 bits): un NaN senalizante se convierte en silencioso, un denormal se normaliza"""
    sign, exp, frac = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if exp == 0xFF:
        e, m = 0x7FFF, (1 << 63) if frac == 0 else (1 << 63) | (1 << 62) | (frac << 40)
    elif exp == 0:
        if frac == 0:
            e, m = 0, 0
        else:
            length = frac.bit_length()
            e, m = (length - 1) - 149 + 16383, frac << (64 - length)
    else:
        e, m = exp - 127 + 16383, (1 << 63) | (frac << 40)
    return m.to_bytes(8, 'little') + ((sign << 15) | e).to_bytes(2, 'little')


def voice(W, mode, a_off, a_raw, other=0):
    return {0: 0, 1: W.raw(1, a_off), 2: a_raw, 3: other}[mode]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret, st0 = [], None, None
    o = a[0]
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k == CONSTAL:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1
    elif k == LOAD:
        A.wr(o + 0x14, 4, a[3])
        A.wr(o + 0x18, 4, a[4])
        W.snap()
        ret = a[3]
    elif k == FLOAT:
        A.wr(o + 0x18, 4, a[3])
        W.snap()
        st0 = ext80(a[3])
        e = (a[3] >> 23) & 0xFF
        marks.append('float_negative' if a[3] >> 31 else 'float_normal')
        marks.append('float_nan' if e == 0xFF and a[3] & 0x7FFFFF else 'float_inf' if e == 0xFF else 'float_denormal' if e == 0 and a[3] & 0x7FFFFF else 'float_zero' if e == 0 else 'float_normal')
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BODYREF:
        entity = {0: 0, 1: W.raw(1, a[7]), 2: a[8]}[a[6]]
        A.wr(o, 4, a[3])
        A.wr(o + 0x14, 4, entity)
        W.snap()
        A.wr(o, 4, 0x86C2B0)
        if entity:
            events.append([CLEANUP, W.tagv(entity), tag(0, o + 0x14)])
            marks.append('entity_set')
            if a[6] == 1:
                marks.append('entity_in_arena')
        else:
            marks.append('entity_null')
        events.append([EDITABLE_BODY, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == TWINSTOP:
        v0 = voice(W, a[3], a[7], a[8])
        v1 = voice(W, a[4], a[9], a[10], v0)
        A.wr(o + 0xA0, 4, v0)
        A.wr(o + 0xA4, 4, v1)
        A.wr(o + 0x88, 4, a[11])
        W.snap()
        calls = 0
        for off in (0xA0, 0xA4):
            s = A.rd(o + off, 4)
            if s:
                events.append([STOP_SOUND, W.tagv(s)])
                calls += 1
                if a[6] == 1 and calls == 1:
                    A.wr(o + 0xA4, 4, a[5])
        marks.append('voice0_set' if v0 else 'voice0_null')
        marks.append('voice1_set' if v1 else 'voice1_null')
        if v0 and v1:
            marks.append('voice_both')
        if a[4] == 3 and v0:
            marks.append('voice_same')
        if a[6] == 1:
            marks.append('mutate_second')
        if a[3] == 1 or a[4] == 1:
            marks.append('voice_in_arena')
    elif k == TWINDTOR:
        v0 = voice(W, a[4], a[7], a[8])
        v1 = voice(W, a[5], a[9], a[10], v0)
        A.wr(o, 4, a[6])
        A.wr(o + 0x88, 4, a[3])
        A.wr(o + 0xA0, 4, v0)
        A.wr(o + 0xA4, 4, v1)
        W.snap()
        A.wr(o, 4, 0x85F438)
        marks.append('prev_zero' if a[6] == 0 else 'prev_other')
        marks.append('voice0_set' if v0 else 'voice0_null')
        marks.append('voice1_set' if v1 else 'voice1_null')
        if a[5] == 3 and v0:
            marks.append('voice_same')
        if A.rd(o + 0x88, 2) != 0:
            marks.append('inuse_set')
            for off in (0xA0, 0xA4):
                s = A.rd(o + off, 4)
                if s:
                    events.append([FORGET, W.tagv(s), A.rd(o + 0xA0, 4), A.rd(o + 0xA4, 4), A.rd(o + 0x88, 4), A.rd(o, 4)])
                    A.wr(o + off, 4, 0)
            A.wr(o + 0x88, 2, 0)
        else:
            marks.append('inuse_high_only' if a[3] & 0xFFFF0000 else 'inuse_zero')
        A.wr(o, 4, 0x859988)
        events.append([SOUND_DTOR, tag(0, o + 8), A.rd(o + 8, 4), A.rd(o, 4), A.rd(o + 0xA0, 4), A.rd(o + 0xA4, 4), A.rd(o + 0x88, 4)])
    elif k == GETNODE:
        A.wr(o + 0x10, 4, a[3])
        W.snap()
        ret = (a[3] + 0x18 * a[4]) & M32
        idx = a[4] - (1 << 32) if a[4] >= 1 << 31 else a[4]
        marks.append('index_zero' if idx == 0 else 'index_negative' if idx < 0 else 'index_large' if idx > 64 else 'index_small')
    elif k in (STATICBODY, STATICSDD):
        p = {0: 0, 1: W.raw(1, a[7]), 2: a[8], 3: W.raw(0, o)}[a[6]]
        A.wr(o, 4, a[3])
        A.wr(o + 0xC, 4, p)
        W.snap()
        A.wr(o, 4, 0x85C6CC)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append({0: 'seqs_null', 1: 'seqs_in_arena', 2: 'seqs_raw', 3: 'seqs_is_this'}[a[6]] if p or a[6] == 0 else 'seqs_null')
        if p:
            events.append([FREE, W.tagv(p), W.rd(p, 4) if W.inside(p) else 0])
        if k == STATICSDD:
            flags = a[9] & 0xFF
            if flags & 1:
                events.append([0x8214BD, tag(0, o), A.rd(o, 4)])
            ret = tag(0, o)
            marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[9] >> 8 else 'flag_byte_only')
    elif k == UPDATEKF:
        narr = a[4]
        n = a[3] - (1 << 32) if a[3] >= 1 << 31 else a[3]
        pedpos = a[16] if a[5] else 0
        ctx = W.raw(0, o)
        A.wr(o, 4, rnd(a[12], 5))
        arrays = []
        for kk in range(narr):
            arrays.append(W.raw(2, a[11] + 0x130 * kk))
            A.wr(o + 4 + 4 * kk, 4, arrays[-1])
        A.wr(o + 4 + 4 * narr, 4, 0)
        B.wr(a[7] + 0xC, 4, pedpos)
        hv = [0, 0xFFF, rnd(a[23], 98) & 0xFFF, 0x555][rnd(a[23], 99) % 4]
        flags = []
        for i in range(12):
            fl = (rnd(a[23], i) & 0xF7) | (8 if (hv >> i) & 1 else 0)
            flags.append(fl)
            B.wr(a[6] + 0x18 * i, 1, fl)
        assoc = []
        for kk in range(3):
            assoc.append(W.raw(1, a[8 + kk]))
            B.wr(a[8 + kk] + 0x20, 4, a[17 + kk])
        seqs = []
        for kk in range(3):
            row = []
            for i in range(12):
                sv = (rnd(a[22], 100 + kk * 12 + i) | 1) if (a[20 + kk] >> i) & 1 else 0
                row.append(sv)
                Cc.wr(a[11] + 0x130 * kk + 0x18 * i + 0x10, 4, sv)
                Cc.wr(a[11] + 0x130 * kk + 0x18 * i + 0x14, 4, rnd(a[22], 200 + kk * 12 + i))
            seqs.append(row)
            Cc.wr(a[11] + 0x130 * kk + 0x14, 4, assoc[kk])
        W.snap()
        marks.append('arrays_%d' % narr)
        marks.append('n_positive' if n > 0 else 'n_zero' if n == 0 else 'n_negative')
        marks.append('pedpos_set' if pedpos else 'pedpos_null')
        for kk in range(narr):
            tm = a[17 + kk]
            for i in range(n if n > 0 else 0):
                if (flags[i] & 8) and pedpos:
                    marks.append('velocity_skip')
                    continue
                marks.append('velocity_kept') if flags[i] & 8 else None
                if seqs[kk][i] == 0:
                    marks.append('seq_null')
                    continue
                events.append([FINDKF, W.tagv(arrays[kk] + 0x18 * i), tm])
                marks.append('seq_call')
                marks.append('time_nan' if (tm >> 23) & 0xFF == 0xFF and tm & 0x7FFFFF else 'time_normal')
        marks = [m for m in marks if m]
    elif k == CTOR:
        W.snap()
        for off in CTOR_WRITES[fn]:
            A.wr(o + off, 4, 0)
        ret = tag(0, o)
    elif k == NOP:
        W.snap()
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'st0': st0, 'marks': marks}


def x87_state(side_row):
    img = bytes.fromhex(side_row['post_floating_state'])
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    return img, (fsw >> 11) & 7, ftw


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img, top, ftw = x87_state(side_row)
    if e['st0'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
    else:
        tags = [(ftw >> (2 * r)) & 3 for r in range(8)]
        if tags[top] == 3 or any(tags[r] != 3 for r in range(8) if r != top):
            return 'x87: se esperaba exactamente ST0 valido (etiquetas %s, TOP %d)' % (tags, top)
        if img[28:38] != e['st0']:
            return 'ST0 %s != %s' % (img[28:38].hex(), e['st0'].hex())
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
