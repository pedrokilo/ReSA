"""Oraculo independiente (Python) de Fn169A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 169A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x48E220  CEventLeaderEnteredCarAsDriver cuerpo del destructor (marco SEH): [esi]=85A198h; si [esi+14h]: call 571A00 (CleanUpOldReference, ecx = [esi+14h], arg = esi+14h); call 4AC480 (cuerpo de
                ~CEventEditableResponse, ecx = this); ret
   1  0x48E280  CEventLeaderEnteredCarAsDriver::GetEventType (vtable 0x85A198, ranura 1): mov eax,36h; ret
   2  0x48E290  CEventLeaderEnteredCarAsDriver::GetLifeTime (ranura 3): xor eax,eax; ret
   3  0x48E2A0  CEventLeaderEnteredCarAsDriver::CloneEditable (ranura 16, marco SEH): eax = 4B5620(18h) (cdecl); si no es nulo: ecx = eax; push [this+14h]; call 48E1C0 (constructor, thiscall `ret 4`) y devuelve
                su EAX; si es nulo, devuelve 0
   4  0x48E300  CEventLeaderEnteredCarAsDriver::AffectsPed (ranura 5): xor al,al; ret 4
   5  0x48E310  CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent (ranura 15): mov al,1; ret
   6  0x48E320  CEventLeaderEnteredCarAsDriver destructor con borrado (ranura 0): cuerpo 48E220; CEvent::operator delete 4B5630
   7  0x48E340  CEventLeaderEnteredCarAsDriver::GetEventPriority (ranura 2): mov eax,2Eh; ret
   8  0x4B0F70  CEventLeaderExitedCarAsDriver::TakesPriorityOver (vtable 0x85B898, ranura 11): mov al,1; ret 4
   9  0x4ABFE0  CEvent::AffectsPed (vtable 0x85AB38, ranura 5): mov al,1; ret 4
  10  0x4ABFF0  CEvent::AffectsPedGroup (ranura 6): mov al,1; ret 4
  11  0x4AC000  CEvent::CanBeInterruptedBySameEvent (ranura 15): xor al,al; ret
  12  0x4AC020  CEvent::IsValid (ranura 14): si el byte [this+8] no es cero devuelve 1; si no, eax = [[this]+0Ch] (GetLifeTime, ecx = this); devuelve ([this+4] <= eax) con signo (jle); ret 4
  13  0x4AC0F0  elemento de CInformFriendsEvent (ped +0, evento +4, instante +8), destructor: si [this]: 571A00 (ecx = ped, arg = this); [this] = 0; si [this+4]: virtual [vtable+0] con (ecx = evento, 1); [this+4] = 0;
                [this+8] = -1
  14  0x4AC130  constructor con argumentos (ped, evento, retardo) `ret 0Ch`: [this] = ped; si ped: 571B70 (ecx = ped, arg = this); [this+4] = evento; [this+8] = [B7CB84h] + retardo
  15  0x4AC160  Process de un elemento: si [this]: si [this+8] >= [B7CB84h] (sin signo) sale; si no: ecx = [[ped+47Ch]] + 68h; 4AB420(ecx, evento, 0) (`ret 8`); si [this]: 571A00 (ecx = ped, arg = this);
                [this] = 0; despues (tambien sin ped) como 13 para el evento y el instante
  16  0x4B2950  igual que 13
  17  0x421190  CTaskComplex::GetSubTask (vtable 0x86D4CC, ranura 2): mov eax,[ecx+8]; ret
  18  0x4211A0  CTaskComplex::IsSimple (ranura 3): xor al,al; ret
  19  0x4211B0  CTaskComplex::MakeAbortable (ranura 6): mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h] (los tres argumentos de pila pasan tal cual)
  20  0x61A620  CTaskComplex destructor con borrado (ranura 0): call 61A3D0; con bit 0 del BYTE de banderas la liberacion en linea del pool de tareas [0B744A8h]: indice = (this - [pool]) >> 7 (sar), [[pool+4]+indice] |= 80h,
                si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; devuelve this
  21  0x61A660  CTask destructor con borrado (vtable 0x86D48C, ranura 0): [this] = 86D48Ch siempre; con bit 0 la liberacion en linea del pool de tareas; devuelve this
  22  0x61A6F0  CTaskSimpleAnim cuerpo del destructor (marco SEH): [esi]=86D504h; si [esi+8]: 4CEBE0 (SetFinishCallback, ecx = anim, args 4D1390h y 0); word [anim+2Eh] |= 4; si el bit 2 del byte [esi+0Ch] es 0
                y float [anim+18h] > 0 (ordenado) y float [anim+1Ch] >= 0 (ordenado): [anim+1Ch] = C0800000h; [esi+8] = 0; call 61A3A0 (~CTask, ecx = this); ret

Los falsos de los colaboradores anotan: cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero];
CleanUpOldReference y RegisterReference [direccion, ECX, argumento (etiquetas)]; CEvent::operator new [direccion, tamano]; constructor 48E1C0 [direccion, ECX, argumento]; CEventGroup::Add 4AB420 [direccion, ECX,
evento, bandera]; SetFinishCallback 4CEBE0 [direccion, ECX, funcion (el DefaultAnimCB de la DLL se normaliza al del EXE), dato]; el GetLifeTime virtual falso [identificador, ECX]; el destructor virtual falso de un
evento [identificador, ECX, banderas]; el MakeAbortable virtual falso [identificador, ECX, ped, prioridad, evento]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventLeaderEnteredCarAsDriver::DestructorBody', 'CEventLeaderEnteredCarAsDriver::GetEventType', 'CEventLeaderEnteredCarAsDriver::GetLifeTime',
         'CEventLeaderEnteredCarAsDriver::CloneEditable', 'CEventLeaderEnteredCarAsDriver::AffectsPed', 'CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent',
         'CEventLeaderEnteredCarAsDriver::ScalarDeletingDestructor', 'CEventLeaderEnteredCarAsDriver::GetEventPriority', 'CEventLeaderExitedCarAsDriver::TakesPriorityOver',
         'CEvent::AffectsPed', 'CEvent::AffectsPedGroup', 'CEvent::CanBeInterruptedBySameEvent', 'CEvent::IsValid', 'CInformFriendsEvent::Destructor-2', 'CInformFriendsEvent::Constructor',
         'CInformFriendsEvent::Process', 'CInformFriendsEvent::Destructor', 'CTaskComplex::GetSubTask', 'CTaskComplex::IsSimple', 'CTaskComplex::MakeAbortable',
         'CTaskComplex::ScalarDeletingDestructor', 'CTask::ScalarDeletingDestructor', 'CTaskSimpleAnim::DestructorBody']
ENTRIES = (0x48E220, 0x48E280, 0x48E290, 0x48E2A0, 0x48E300, 0x48E310, 0x48E320, 0x48E340, 0x4B0F70, 0x4ABFE0, 0x4ABFF0, 0x4AC000, 0x4AC020, 0x4AC0F0, 0x4AC130, 0x4AC160,
           0x4B2950, 0x421190, 0x4211A0, 0x4211B0, 0x61A620, 0x61A660, 0x61A6F0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CLEANUP, REGREF, ADD, SETFINISH, NEWEV, CTOREV, EDITDTOR, EV_LIFE, EV_DTOR, EV_ABORT = 0x571A00, 0x571B70, 0x4AB420, 0x4CEBE0, 0x4B5620, 0x48E1C0, 0x4AC480, 0x7E1EA5E0, 0x7E1EA5E1, 0x7E1EA5E2
DEFCB = 0x4D1390
FAKEVT = 0xF00

(CONST, CONSTAL0, CONSTAL1, ENTBODY, CLONE, SDD, SDDPOOL, ISVALID, FLUSH, SET, PROC, READ8, MAKEABORT, ANIMDTOR) = 'const constal0 constal1 entbody clone sdd sddpool isvalid flush set proc read8 makeabort animdtor'.split()
KIND = [ENTBODY, CONST, CONST, CLONE, CONSTAL1, CONSTAL0, SDD, CONST, CONSTAL1, CONSTAL1, CONSTAL1, CONSTAL0, ISVALID, FLUSH, SET, PROC, FLUSH, READ8, CONSTAL0, MAKEABORT, SDDPOOL, SDDPOOL, ANIMDTOR]
CONSTANT = {1: 0x36, 2: 0, 7: 0x2E}
CONSTANT_AL = {4: 0, 5: 1, 8: 1, 9: 1, 10: 1, 11: 0, 18: 0}
# SDD / SDDPOOL: direccion (EXE) del cuerpo que llama (sin entrada = ninguno) y de la liberacion
BODY = {6: 0x48E220, 20: 0x61A3D0}
DELETE = {6: 0x4B5630}
VTABLE_WORD = {21: 0x86D48C}
SPAN = {ENTBODY: 0x18, CLONE: 0x18, ISVALID: 0xC, FLUSH: 0xC, SET: 0xC, PROC: 0xC, READ8: 0xC, MAKEABORT: 0xC, ANIMDTOR: 0x10}


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


KEYS = {}
for _fn in range(23):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, CONSTAL0, CONSTAL1, READ8):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    elif _kind == ENTBODY:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'vehicle_set', 'vehicle_null', 'vehicle_in_arena']
    elif _kind == CLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok', 'vehicle_in_arena', 'vehicle_raw']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == SDDPOOL:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only',
               'index_negative', 'index_zero', 'index_positive', 'cursor_lowered', 'cursor_kept']
    elif _kind == ISVALID:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'valid_flag_set', 'valid_flag_clear', 'life_enough', 'life_expired']
    elif _kind == FLUSH:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'ped_set', 'ped_null', 'event_set', 'event_null', 'ped_in_arena', 'ped_raw']
    elif _kind == SET:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'ped_set', 'ped_null', 'ped_in_arena', 'ped_raw']
    elif _kind == PROC:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'ped_set', 'ped_null', 'expired', 'not_expired', 'event_set', 'event_null']
    elif _kind == MAKEABORT:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'result_true', 'result_false']
    elif _kind == ANIMDTOR:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'anim_set', 'anim_null', 'hold_set', 'hold_clear', 'blend_positive', 'delta_set', 'delta_kept']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN.get(k, 4) > WIN:
        marks.append('spill')
    return marks


def pool_free(W, B, Cc, d, cursor, cell_c, pool_b, marks):
    """liberacion de la ranura en el pool falso de tareas: indice = (obj - almacenamiento) >> 7 (aritmetico); estados[indice] |= 0x80; cursor = min(cursor, indice) con signo"""
    idx = s32(d) >> 7
    flag = Cc.rd(cell_c, 1)
    Cc.wr(cell_c, 1, flag | 0x80)
    if idx < s32(cursor):
        B.wr(pool_b + 0xC, 4, idx & M32)
        marks.append('cursor_lowered')
    else:
        marks.append('cursor_kept')
    marks.append('index_negative' if idx < 0 else 'index_zero' if idx == 0 else 'index_positive')


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k in (CONSTAL0, CONSTAL1):
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT_AL[fn]
    elif k == READ8:
        A.wr(o + 8, 4, a[3])
        W.snap()
        ret = a[3]
    elif k == ENTBODY:
        veh = {0: 0, 1: W.raw(1, a[7]), 2: a[8]}[a[6]]
        A.wr(o, 4, a[3])
        A.wr(o + 0x14, 4, veh)
        W.snap()
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        if veh:
            events.append([CLEANUP, W.tagv(veh), tag(0, o + 0x14)])
            marks.append('vehicle_set')
            if a[6] == 1:
                marks.append('vehicle_in_arena')
        else:
            marks.append('vehicle_null')
        events.append([EDITDTOR, tag(0, o), 0x85A198])
        A.wr(o, 4, a[5])
    elif k == CLONE:
        veh = W.raw(2, a[5]) if a[4] else a[3]
        A.wr(o + 0x14, 4, veh)
        newp = W.raw(1, a[7]) if a[6] else 0
        W.snap()
        marks.append('vehicle_in_arena' if a[4] else 'vehicle_raw')
        events.append([NEWEV, 0x18])
        if newp:
            events.append([CTOREV, W.tagv(newp), W.tagv(veh)])
            W.wr(newp, 4, a[8])
            marks.append('new_ok')
        else:
            marks.append('new_null')
        ret = W.tagv(newp)
    elif k in (SDD, SDDPOOL):
        A.wr(o, 4, a[3])
        if k == SDDPOOL:       # pool falso (lo escribe Setup antes de la instantanea): +0 almacenamiento = this - d, +4 estados (celda en C - indice), +8 relleno, +0xC cursor
            obj = W.raw(0, o)
            d, cursor, cell, pool = a[7], a[8], a[9], a[10]
            B.wr(pool + 0, 4, (obj - d) & M32)
            B.wr(pool + 4, 4, (W.raw(2, cell) - (s32(d) >> 7)) & M32)
            B.wr(pool + 8, 4, a[11])
            B.wr(pool + 0xC, 4, cursor)
        W.snap()
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if k == SDD:
            events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
            A.wr(o, 4, a[5])
            if flags & 1:
                events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        else:
            if fn in BODY:
                events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
                A.wr(o, 4, a[5])
            else:
                A.wr(o, 4, VTABLE_WORD[fn])
            if flags & 1:
                pool_free(W, B, Cc, a[7], a[8], a[9], a[10], marks)
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ISVALID:
        A.wr(o + 0, 4, W.raw(2, a[7]))
        A.wr(o + 4, 4, a[4])
        A.wr(o + 8, 4, a[5])
        W.snap()
        if a[5] & 0xFF:
            ret = 1
            marks.append('valid_flag_set')
        else:
            marks.append('valid_flag_clear')
            events.append([EV_LIFE, tag(0, o)])
            ret = 1 if s32(a[4]) <= s32(a[3]) else 0
            marks.append('life_enough' if ret else 'life_expired')
    elif k == FLUSH:
        ped = {0: 0, 1: W.raw(1, a[4]), 2: a[5]}[a[3]]
        ev = W.raw(2, a[7]) if a[6] else 0
        A.wr(o, 4, ped)
        A.wr(o + 4, 4, ev)
        A.wr(o + 8, 4, a[8])
        W.snap()
        if ped:
            events.append([CLEANUP, W.tagv(ped), tag(0, o)])
            A.wr(o, 4, 0)
            marks.append('ped_set')
            marks.append('ped_in_arena' if a[3] == 1 else 'ped_raw')
        else:
            marks.append('ped_null')
        if ev:
            events.append([EV_DTOR, W.tagv(ev), 1])
            A.wr(o + 4, 4, 0)
            marks.append('event_set')
        else:
            marks.append('event_null')
        A.wr(o + 8, 4, M32)
    elif k == SET:
        ped = {0: 0, 1: W.raw(1, a[4]), 2: a[5]}[a[3]]
        A.wr(o + 8, 4, a[9])
        W.snap()
        if ped:
            events.append([REGREF, W.tagv(ped), tag(0, o)])
            marks.append('ped_set')
            marks.append('ped_in_arena' if a[3] == 1 else 'ped_raw')
        else:
            marks.append('ped_null')
        A.wr(o, 4, ped)
        A.wr(o + 4, 4, a[6])
        A.wr(o + 8, 4, (a[8] + a[7]) & M32)
    elif k == PROC:
        ped = W.raw(1, a[4]) if a[3] else 0
        intel = W.raw(2, a[5])
        ev = W.raw(2, a[7]) if a[6] else 0
        A.wr(o, 4, ped)
        A.wr(o + 4, 4, ev)
        A.wr(o + 8, 4, a[8])
        if ped:
            B.wr(a[4] + 0x47C, 4, intel)
        W.snap()
        done = False
        if ped:
            marks.append('ped_set')
            if a[8] >= a[9]:                                # sin signo: no ha vencido, no hace nada
                marks.append('not_expired')
                done = True
            else:
                marks.append('expired')
                events.append([ADD, W.tagv((intel + 0x68) & M32), W.tagv(ev), 0])
                events.append([CLEANUP, W.tagv(ped), tag(0, o)])
                A.wr(o, 4, 0)
        else:
            marks.append('ped_null')
        if not done:
            if ev:
                events.append([EV_DTOR, W.tagv(ev), 1])
                A.wr(o + 4, 4, 0)
                marks.append('event_set')
            else:
                marks.append('event_null')
            A.wr(o + 8, 4, M32)
    elif k == MAKEABORT:
        sub = W.raw(2, a[6])
        A.wr(o + 8, 4, sub)
        W.snap()
        events.append([EV_ABORT, W.tagv(sub), a[3], a[4], a[5]])
        ret = a[7] & 0xFF
        marks.append('result_true' if ret else 'result_false')
    elif k == ANIMDTOR:
        anim = W.raw(1, a[7]) if a[6] else 0
        A.wr(o, 4, a[3])
        A.wr(o + 8, 4, anim)
        A.wr(o + 0xC, 4, a[4])
        if anim:
            B.wr(a[7] + 0x18, 4, a[8])
            B.wr(a[7] + 0x1C, 4, a[9])
            B.wr(a[7] + 0x2C, 4, a[10])
        W.snap()
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        hold = bool(a[4] & 4)
        marks.append('hold_set' if hold else 'hold_clear')
        if anim:
            marks.append('anim_set')
            events.append([SETFINISH, W.tagv(anim), DEFCB, 0])
            B.wr(a[7] + 0x2E, 2, B.rd(a[7] + 0x2E, 2) | 4)
            if not hold:
                blend = f32(a[8])
                if blend > 0.0:
                    marks.append('blend_positive')
                    if f32(a[9]) >= 0.0:
                        B.wr(a[7] + 0x1C, 4, 0xC0800000)
                        marks.append('delta_set')
                    else:
                        marks.append('delta_kept')
            A.wr(o + 8, 4, 0)
        else:
            marks.append('anim_null')
        events.append([0x61A3A0, tag(0, o), 0x86D504])
        A.wr(o, 4, a[5])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
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
