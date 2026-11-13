"""Oraculo independiente (Python) de Fn184A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 184A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables y datos leidos del propio EXE).
   0  0x460CD0  CTaskComplexWander::GetTaskType: mov eax,390h; ret
   1  0x460CE0  CTaskComplexWanderCop::Clone: new(50h) 61A5A0; si no es nulo: ctor 460C80 (ecx = mem; pila: [this+0Ch], BYTE [this+10h] ampliado a cero); eax = lo que deja el constructor; reserva nula: 0
   2  0x460D50  GetWanderType: mov eax,1; ret
   3  0x460D60  destructor con borrado: call 460D80; si el bit 0 del BYTE de banderas: delete 61A5B0 (cdecl); eax = this
   4  0x460D80  cuerpo del destructor: [this] = 859C80h; si [+28h] != 0: llamada virtual de la ranura 0 con 1 (delete); cuerpo 669D20 (ecx = this)
   5  0x49EA80  FxEntitySystem constructor: call 4A8DB0 (ListItem_c: [+0] = [+4] = 0); eax = this
   6  0x49EA90  Fx_c::Init: 4A98E0 (ecx = 0A9AE80h); 5C2420 (ecx = 0A9AE80h; "models\\effects.fxp"); 4A93E0 (ecx = 0A9AE80h; pila: 0C813E0h, 0C812F0h); 49E660 (ecx = this); [+50h] = 0; al = 1
   7  0x49EAE0  Fx_c::Reset: 4A98B0 (ecx = 0A9AE80h); salto a 49E660 (ecx = this)
   8  0x4A11C0  destructor de FxEntitySystem con borrado: call 49EA70 (ret); si el bit 0: delete 8214BD (cdecl); eax = this
   9  0x4A11E0  Fx_c::CreateEntityFx(entidad, nombre, pos*, matriz*) `ret 10h`: 4A9BE0 (ecx = 0A9AE80h; pila: nombre, pos, matriz, 1); si el sistema es nulo: fin; si no: new(10h) 82119A; ctor del nodo 4A8DB0;
            [nodo+8] = sistema; [nodo+0Ch] = entidad; AddItem 4A8DF0 (ecx = this+44h; nodo; el nodo pasa a ser el primero); Play 4AA2F0 (ecx = sistema)
  10  0x4A1280  Fx_c::DestroyEntityFx(entidad) `ret 4`: recorre la lista desde el primero; siguiente = [nodo+4] (leido antes); si [nodo+0Ch] == entidad: RemoveItem 4A8E30 (ecx = this+44h); Kill 4AA3F0 (ecx = [nodo+8]); 49EA70; delete 8214BD
  11  0x4A12D0  Fx_c::ExitEntitySystems: recorre la lista; siguiente = [nodo+4]; RemoveItem 4A8E30; DestroyFxSystem 4A9810 (ecx = 0A9AE80h; [nodo+8]); 49EA70; delete 8214BD
  12  0x4A1320  Fx_c::Exit: 4A12D0 (ecx = this); 49E850 (ecx = this); salto a 4A9A10 (ecx = 0A9AE80h)
  13  0x4B1ED0  CEventPotentialWalkIntoFire::GetEventType: mov eax,2Eh; ret
  14  0x4B1EE0  GetLifeTime: xor eax,eax; ret
  15  0x4B1EF0  GetEventPriority: mov eax,1Fh; ret
  16  0x4B1F90  CEventFireNearby::AffectsPed(ped) `ret 4`: eax = [ped+47Ch]; FindActiveTaskByType 681740 (ecx = eax+4; 25Ch); si el resultado no es nulo: al = 0; si no: CPed::IsAlive 5E0170 (ecx = ped); al = su resultado
  17-21  destructores con borrado de eventos: call <cuerpo> (4B2070, 4B2130, 4B2210, 4B2470, 4B26A0); si el bit 0 del BYTE de banderas: pool = [0B744ACh]; indice = (this - [pool]) / 44h (con signo, truncado hacia cero);
            byte [[pool+4] + indice] |= 80h; si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; eax = this
  22  0x674EA0  CEventInteriorUseInfo::Constructor(info, interior, animTime, loop) `ret 10h`: CEvent 4ABFC0 (ecx = this); [+0Ch] = info; BYTE [+18h] = loop; [this] = 8701A8h; [+10h] = interior; [+14h] = animTime; eax = this
  23  0x674ED0  cuerpo del destructor: [this] = 8701A8h; salto a 4AC010 (ecx = this)
  24  0x674F00  Clone: new(1Ch) 4B5620; si no es nulo: lee +10h, BYTE +18h, +14h y +0Ch del origen; ctor de CEvent 4ABFC0 (ecx = mem); [mem+14h] = animTime; BYTE [mem+18h] = loop; [mem] = 8701A8h;
            [mem+0Ch] = info; [mem+10h] = interior; eax = mem; si no, 0
  25  0x674F90  destructor con borrado: call 674ED0; si el bit 0: delete 4B5630 (cdecl); eax = this

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (crudo), dword del puntero]; operator new [direccion, tamano]; metodos de otros objetos
[direccion, ECX, argumentos de pila]; delete virtual [0xD7E1, objeto, bandera, dword de [this]].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexWander::GetTaskType", "CTaskComplexWanderCop::Clone", "CTaskComplexWanderCop::GetWanderType", "CTaskComplexWanderCop::ScalarDeletingDestructor", "CTaskComplexWanderCop::DestructorBody",
         "FxEntitySystem::Constructor", "Fx_c::Init", "Fx_c::Reset", "FxEntitySystem::ScalarDeletingDestructor", "Fx_c::CreateEntityFx", "Fx_c::DestroyEntityFx", "Fx_c::ExitEntitySystems", "Fx_c::Exit",
         "CEventPotentialWalkIntoFire::GetEventType", "CEventPotentialWalkIntoFire::GetLifeTime", "CEventPotentialWalkIntoFire::GetEventPriority", "CEventFireNearby::AffectsPed",
         "CEventGunShotWhizzedBy::ScalarDeletingDestructor", "CEventSeenPanickedPed::ScalarDeletingDestructor", "CEventAreaCodes::ScalarDeletingDestructor", "CEventPlayerCommandToGroup::ScalarDeletingDestructor",
         "CEventDanger::ScalarDeletingDestructor", "CEventInteriorUseInfo::Constructor", "CEventInteriorUseInfo::DestructorBody", "CEventInteriorUseInfo::Clone", "CEventInteriorUseInfo::ScalarDeletingDestructor"]
ENTRIES = (0x460CD0, 0x460CE0, 0x460D50, 0x460D60, 0x460D80, 0x49EA80, 0x49EA90, 0x49EAE0, 0x4A11C0, 0x4A11E0, 0x4A1280, 0x4A12D0, 0x4A1320, 0x4B1ED0, 0x4B1EE0, 0x4B1EF0,
           0x4B1F90, 0x4B6990, 0x4B69E0, 0x4B6A30, 0x4B6A80, 0x4B6AD0, 0x674EA0, 0x674ED0, 0x674F00, 0x674F90)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SEED = 60                                                        # a[60..63] = semillas de las arenas
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
HW = 0xA9AE80                                                    # g_fxMan
WINDDIR, WIND = 0xC813E0, 0xC812F0
FXPROJECT = b'models\\effects.fxp'
VDTOR_EV = 0xD7E1

CONST, WCLONE, SDD, SDDNB, WDTOR, FXCTOR, FXINIT, FXRESET, FXCREATE, FXDESTROY, FXEXITENT, FXEXIT, AFFECTS, EVTSDD, IUICTOR, IUIBODY, IUICLONE = \
    'const wclone sdd sddnb wdtor fxctor fxinit fxreset fxcreate fxdestroy fxexitent fxexit affects evtsdd iuictor iuibody iuiclone'.split()
KIND = [CONST, WCLONE, CONST, SDD, WDTOR, FXCTOR, FXINIT, FXRESET, SDDNB, FXCREATE, FXDESTROY, FXEXITENT, FXEXIT, CONST, CONST, CONST, AFFECTS, EVTSDD, EVTSDD, EVTSDD, EVTSDD, EVTSDD,
        IUICTOR, IUIBODY, IUICLONE, SDD]
CONSTS = {0: 0x390, 2: 1, 13: 0x2E, 14: 0, 15: 0x1F}
SDD_BODY = {3: (0x460D80, 0x61A5B0), 25: (0x674ED0, 0x4B5630)}
EVT_BODY = {17: 0x4B2070, 18: 0x4B2130, 19: 0x4B2210, 20: 0x4B2470, 21: 0x4B26A0}
NEWADDR, WANDERCTOR, EVENTCTOR, DELETE_CRT, NEW_CRT = 0x61A5A0, 0x460C80, 0x4ABFC0, 0x8214BD, 0x82119A
LISTOFF = 0x44
MAXNODES = 5


def signed32(v):
    v &= M32
    return v - 0x100000000 if v & 0x80000000 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


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


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def sx16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


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
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[SEED + i]) for i in range(4))

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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_FLAGS = ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
_EXTRA = {
    CONST: [],
    WCLONE: ['mem_null', 'mem_set', 'dir_hi_bits', 'dir_byte_only'],
    SDD: _FLAGS,
    SDDNB: _FLAGS,
    WDTOR: ['child_null', 'child_set'],
    FXCTOR: [],
    FXINIT: ['prev_zero', 'prev_other'],
    FXRESET: [],
    FXCREATE: ['list_empty', 'list_nonempty', 'system_null', 'system_set', 'matrix_null', 'matrix_raw', 'matrix_arena'],
    FXDESTROY: ['list_empty', 'list_nonempty', 'match_none', 'match_some', 'match_all'],
    FXEXITENT: ['list_empty', 'list_nonempty', 'list_many'],
    FXEXIT: [],
    AFFECTS: ['task_found', 'task_missing', 'alive_true', 'alive_false', 'found_low_byte_zero'],
    EVTSDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'idx_negative', 'idx_zero', 'idx_positive', 'cursor_lowered', 'cursor_kept', 'state_was_set', 'extra_offset'],
    IUICTOR: ['loop_hi_bits', 'loop_byte_only'],
    IUIBODY: [],
    IUICLONE: ['mem_null', 'mem_set'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, gch = [], None, []
    o = a[0]
    this = W.raw(0, o)
    tag = W.tagv

    def build_list(destroy):
        """lista de Fx_c (+44h: primero, ultimo, recuento) con los nodos en la arena B; devuelve las direcciones de los nodos y sus entidades"""
        n = a[3]
        ptr = [W.raw(1, a[4 + i]) for i in range(MAXNODES)]
        ents = []
        for i in range(n):
            ent = a[9] if destroy and (a[10] >> i) & 1 else a[16 + i]
            ents.append(ent)
            B.wr(a[4 + i] + 0, 4, ptr[i - 1] if i > 0 else 0)
            B.wr(a[4 + i] + 4, 4, ptr[i + 1] if i + 1 < n else 0)
            B.wr(a[4 + i] + 8, 4, a[11 + i])
            B.wr(a[4 + i] + 0xC, 4, ent)
        A.wr(o + LISTOFF + 0, 4, ptr[0] if n else 0)
        A.wr(o + LISTOFF + 4, 4, ptr[n - 1] if n else 0)
        A.wr(o + LISTOFF + 8, 4, (n + a[21]) & M32)
        return ptr[:n], ents

    def remove_item(node):
        """RemoveItem 4A8E30 (ecx = this+44h)"""
        nxt = W.rd(node + 4, 4)
        prv = W.rd(node, 4)
        if nxt:
            W.wr(nxt, 4, prv)
        else:
            A.wr(o + LISTOFF + 4, 4, prv)
        if prv:
            W.wr(prv + 4, 4, nxt)
        else:
            A.wr(o + LISTOFF, 4, W.rd(node + 4, 4))
        A.wr(o + LISTOFF + 8, 4, (A.rd(o + LISTOFF + 8, 4) - 1) & M32)

    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == WCLONE:
        A.wr(o + 0xC, 4, a[3]); A.wr(o + 0x10, 4, a[4])
        W.snap()
        events.append([NEWADDR, 0x50])
        if a[5] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[6])
            events.append([WANDERCTOR, tag(mem), a[3], a[4] & 0xFF])
            B.wr(a[6], 4, a[8])
            ret = tag(a[7])
        marks.append('dir_hi_bits' if a[4] >> 8 else 'dir_byte_only')
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        body, delete = SDD_BODY[fn]
        events.append([body, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([delete, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    elif k == SDDNB:
        A.wr(o, 4, a[3])
        W.snap()
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE_CRT, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    elif k == WDTOR:
        A.wr(o, 4, a[3])
        child = W.raw(1, a[5]) if a[4] else 0
        A.wr(o + 0x28, 4, child)
        W.snap()
        A.wr(o, 4, 0x859C80)
        if child:
            events.append([VDTOR_EV, tag(child), 1, A.rd(o, 4)])
            marks.append('child_set')
        else:
            marks.append('child_null')
        events.append([0x669D20, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[7])
    elif k == FXCTOR:
        W.snap()
        A.wr(o, 4, 0); A.wr(o + 4, 4, 0)
        ret = tag(this)
    elif k == FXINIT:
        A.wr(o + 0x50, 4, a[3])
        W.snap()
        events.append([0x4A98E0, HW])
        events.append([0x5C2420, HW, fnv(FXPROJECT), len(FXPROJECT)])
        events.append([0x4A93E0, HW, WINDDIR, WIND])
        events.append([0x49E660, tag(this)])
        A.wr(o + 0x50, 4, 0)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == FXRESET:
        W.snap()
        events.append([0x4A98B0, HW])
        events.append([0x49E660, tag(this)])
    elif k == FXEXIT:
        W.snap()
        events.append([0x4A12D0, tag(this)])
        events.append([0x49E850, tag(this)])
        events.append([0x4A9A10, HW])
    elif k == FXCREATE:
        nodes, _ = build_list(False)
        name, pos = W.raw(3, a[23]), W.raw(3, a[24])
        matrix = 0 if a[25] == 0 else (a[26] if a[25] == 1 else W.raw(3, a[26]))
        W.snap()
        events.append([0x4A9BE0, HW, tag(name), tag(pos), D.rd(a[24], 4), D.rd(a[24] + 4, 4), D.rd(a[24] + 8, 4), tag(matrix), 1])
        marks.append('list_empty' if not nodes else 'list_nonempty')
        marks.append('matrix_null' if a[25] == 0 else ('matrix_raw' if a[25] == 1 else 'matrix_arena'))
        if a[27] == 0:
            marks.append('system_null')
        else:
            marks.append('system_set')
            mem = W.raw(2, a[28])
            events.append([NEW_CRT, 0x10])
            old_head = A.rd(o + LISTOFF, 4)
            Cc.wr(a[28] + 0, 4, 0)
            Cc.wr(a[28] + 4, 4, old_head)
            Cc.wr(a[28] + 8, 4, a[27])
            Cc.wr(a[28] + 0xC, 4, a[22])
            A.wr(o + LISTOFF, 4, mem)
            if old_head:
                W.wr(old_head, 4, mem)
            else:
                A.wr(o + LISTOFF + 4, 4, mem)
            A.wr(o + LISTOFF + 8, 4, (A.rd(o + LISTOFF + 8, 4) + 1) & M32)
            events.append([0x4AA2F0, tag(a[27])])
    elif k == FXDESTROY or k == FXEXITENT:
        destroy = k == FXDESTROY
        nodes, ents = build_list(destroy)
        W.snap()
        hits = 0
        for i, node in enumerate(nodes):
            if destroy and ents[i] != a[9]:
                continue
            hits += 1
            system = a[11 + i]
            remove_item(node)
            if destroy:
                events.append([0x4AA3F0, tag(system)])
            else:
                events.append([0x4A9810, HW, system])
            events.append([DELETE_CRT, tag(node), W.rd(node, 4)])
        marks.append('list_empty' if not nodes else 'list_nonempty')
        if destroy:
            if nodes:
                marks.append('match_none' if hits == 0 else ('match_all' if hits == len(nodes) else 'match_some'))
        elif len(nodes) >= 3:
            marks.append('list_many')
    elif k == AFFECTS:
        ped, intelligence = W.raw(1, a[3]), W.raw(2, a[4])
        B.wr(a[3] + 0x47C, 4, intelligence)
        W.snap()
        events.append([0x681740, tag((intelligence + 4) & M32), 0x25C])
        if a[5] != 0:
            marks.append('task_found')
            if (a[5] & 0xFF) == 0:
                marks.append('found_low_byte_zero')
            ret = 0
        else:
            marks.append('task_missing')
            events.append([0x5E0170, tag(ped)])
            ret = a[6] & 0xFF
            marks.append('alive_true' if ret else 'alive_false')
    elif k == EVTSDD:
        delta = signed32(a[5])
        idx0 = abs(delta) // 0x44 * (1 if delta >= 0 else -1)
        B.wr(a[4] + 0, 4, (this - a[5]) & M32); B.wr(a[4] + 4, 4, W.raw(3, a[6])); B.wr(a[4] + 8, 4, a[12]); B.wr(a[4] + 0xC, 4, a[10])
        D.wr(a[6] + idx0, 1, a[11])
        A.wr(o, 4, a[3])
        W.snap()
        events.append([EVT_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
        flags = a[7] & 0xFF
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[7] >> 8 else 'flag_byte_only')
        if flags & 1:
            storage = B.rd(a[4], 4)
            diff = signed32((this - storage) & M32)
            prod = diff * 0x78787879
            edx = (prod >> 32) >> 5            # sar: aritmetico (Python desplaza con signo)
            idx = signed32(edx + ((edx & M32) >> 31))
            state_ptr = B.rd(a[4] + 4, 4)
            st = W.rd((state_ptr + idx) & M32, 1)
            marks.append('state_was_set' if st & 0x80 else 'state_clear')
            W.wr((state_ptr + idx) & M32, 1, st | 0x80)
            last = signed32(B.rd(a[4] + 0xC, 4))
            if idx < last:
                B.wr(a[4] + 0xC, 4, idx & M32)
                marks.append('cursor_lowered')
            else:
                marks.append('cursor_kept')
            marks.append('idx_negative' if idx < 0 else ('idx_zero' if idx == 0 else 'idx_positive'))
            if signed32(a[5]) % 0x44:
                marks.append('extra_offset')
        ret = tag(this)
    elif k == IUICTOR:
        W.snap()
        events.append([EVENTCTOR, tag(this), A.rd(o + 0xC, 4), A.rd(o + 0x10, 4), A.rd(o + 0x14, 4), A.rd(o + 0x18, 4)])
        for i in range(7):
            A.wr(o + 4 * i, 4, a[8 + i])
        A.wr(o + 0xC, 4, a[3])
        A.wr(o + 0x18, 1, a[6] & 0xFF)
        A.wr(o, 4, 0x8701A8)
        A.wr(o + 0x10, 4, a[4])
        A.wr(o + 0x14, 4, a[5])
        ret = tag(this)
        marks.append('loop_hi_bits' if a[6] >> 8 else 'loop_byte_only')
    elif k == IUIBODY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x8701A8)
        events.append([0x4AC010, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == IUICLONE:
        A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[6]); A.wr(o + 0x14, 4, a[7]); A.wr(o + 0x18, 4, a[8])
        W.snap()
        events.append([0x4B5620, 0x1C])
        if a[3] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[4])
            events.append([EVENTCTOR, tag(mem), B.rd(a[4] + 0xC, 4), B.rd(a[4] + 0x10, 4), B.rd(a[4] + 0x14, 4), B.rd(a[4] + 0x18, 4)])
            for i in range(7):
                B.wr(a[4] + 4 * i, 4, a[9 + i])
            B.wr(a[4] + 0x14, 4, a[7])
            B.wr(a[4] + 0x18, 1, a[8] & 0xFF)
            B.wr(a[4], 4, 0x8701A8)
            B.wr(a[4] + 0xC, 4, a[5])
            B.wr(a[4] + 0x10, 4, a[6])
            ret = tag(mem)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


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
