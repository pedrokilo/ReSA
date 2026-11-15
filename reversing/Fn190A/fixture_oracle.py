"""Oraculo independiente (Python) de Fn190A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 190A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x420F00  CEventAcquaintancePedHate::GetLifeTime: xor eax,eax; ret
   1  0x420F10  GetSourceEntity: mov eax,[ecx+14h]; ret
   2  0x420F20  CanBeInterruptedBySameEvent: mov al,1; ret
   3  0x420F30  cuerpo del destructor: mov [ecx],858E68h; jmp 4AF890
   4  0x420F40  GetEventType: mov eax,24h; ret
   5  0x420F50  CloneEditable: new(18h) (4B5620); si no es nulo: ctor 4AF820 (ecx = mem; pila: [this+14h]); [mem] = 858E68h; eax = mem (nulo si no hubo memoria)
   6  0x420FB0  destructor con borrado: push esi; mov esi,ecx; call 420F30; test byte [esp+8],1; je +; push esi; call 4B5630; add esp,4; mov eax,esi; pop esi; ret 4
   7  0x420FD0  GetEventPriority: mov eax,1Ah; ret
   8..13  0x4B58E0, 0x4B5930, 0x4B5980, 0x4B59D0, 0x4B5A20, 0x4B5A70  destructores con borrado de CEventGunShot, CEventVehicleCollision, CEventPedCollisionWithPed, CEventObjectCollision,
            CEventBuildingCollision y CEventDraggedOutCar: push esi; push edi; mov edi,ecx; call <cuerpo 4AC720, 4AC920, 4ACA70, 4ACDD0, 4ACFF0, 4AD330>; test byte [esp+0Ch],1; je fin;
            esi = [0B744ACh] (pool de eventos); ecx = this - [esi]; indice = (ecx * 78787879h >> 32) >> 5 con signo, mas el bit de signo (division con signo entre 44h = 68, truncada hacia cero);
            or byte [[esi+4] + indice],80h; si indice < [esi+0Ch] (con signo): [esi+0Ch] = indice; eax = this; ret 4
  14  0x622180  CTaskComplexKillAllThreats constructor: push esi; mov esi,ecx; call 61A3B0 (base: [esi+4]=0, [esi]=86D4CCh, [esi+8]=0); [esi+0Ch]=arg1; [esi]=86DA1Ch; [esi+10h]=arg2; [esi+14h]=arg3; eax=esi; ret 0Ch
  15  0x6221B0  GetTaskType: mov eax,3F6h
  16  0x6221C0  cuerpo del destructor: mov [ecx],86DA1Ch; jmp 61A3D0 (base: [esi]=86D4CCh; si [esi+8]: push 1; call [[esi+8]] (destructor con borrado de la subtarea); [esi+8]=0; [esi]=86D48Ch)
  17  0x6221D0  ScanForThreat(ped): CPedAcquaintanceScanner local (dwords +0 y +4 a cero, bytes +8 y +9 a cero, +0Ch = 1, +0Dh y +0Eh a cero), salida A = 0; ScanEntities 607A90 (ecx = escaner; pila: ped, 4,
            [ped+47Ch] + 130h, 16, &salidaA, &salidaB (el hueco del argumento ped)); eax = salida A; ret 4
  18  0x622230  ControlSubTask: mov eax,[ecx+8]; ret 4
  19  0x6293A0  destructor con borrado: como 6 con el cuerpo 6221C0 y CTask::operator delete 61A5B0
  20  0x6293C0  CreateNextSubTask(ped): tipo = [[this+8]] ranura 4 (+10h) con ecx = subtarea; si es 0xCB o no es 0x3E8: 0; si no ScanForThreat 6221D0 (ecx = this; pila: ped); sin amenaza 0; new(38h) (61A5A0);
            sin memoria 0; ctor 620E30 (ecx = mem; pila: amenaza, -1, [this+0Ch], [this+10h], [this+14h], 1); eax = el del constructor
  21  0x629480  CreateFirstSubTask(ped): ScanForThreat; con amenaza como 20; sin amenaza new(20h); sin memoria 0; ctor 62F310 (ecx = mem; pila: 0, 0, 0, 41000000h); eax = el del constructor
  22  0x629550  KillPedGroupOnFoot destructor con borrado: como 6 con el cuerpo 6222B0 y CTask::operator delete 61A5B0
  23  0x710BA0  FindForOwner(propietario): eax = [0C6A16Ch]; mientras eax != 0: si [eax] == propietario devuelve eax; eax = [eax+14h]; 0
  24  0x711280  CStencilShadowObject::Create(entidad, tipo): bl = BYTE tipo; si bl == 0 o bl > 2: 0; colmodelo = 535300 (ecx = entidad); datos = [colmodelo+2Ch]; nulo: 0; si no [datos+7] & 4: 0; n = [datos+20h] (con signo); n <= 0: 0;
            [esi+4] = n (word); [esi+8] = n*15; [esi] = entidad; [esi+6] = bl; [esi+0Ch] = 0; Malloc 72F420 (n*180, 0); [esi+10h] = eax; RegisterReference 571B70 (ecx = entidad; pila: esi);
            UpdateHierarchy 710BC0 (0C6A168h, 0C6A16Ch, esi); eax = esi

Los falsos anotan: cuerpos y constructores [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (los de un argumento anaden el argumento etiquetado; los de cuatro y seis, los valores); operator delete [direccion, puntero (etiqueta),
dword del puntero]; operator new [direccion, tamano]; ScanForThreat [direccion, ECX, ped]; ScanEntities [direccion, escaner, ped, prioridad, lista, cantidad, salidas, campos del escaner]; GetColModel [direccion, ECX crudo]; Malloc
[direccion, tamano, pista, objeto]; RegisterReference [direccion, ECX crudo, argumento, objeto]; UpdateHierarchy [direccion, a, b, c, objeto]; la subtarea sintetica: destructor con borrado [0xD1000000, ECX, banderas], GetTaskType [0xD1000004, ECX].
La vtable que deja un constructor / destructor real se clasifica: 1 = la de la clase, 2 = la base final, 3 = sin cambios.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventAcquaintancePedHate::GetLifeTime", "CEventAcquaintancePedHate::GetSourceEntity", "CEventAcquaintancePedHate::CanBeInterruptedBySameEvent", "CEventAcquaintancePedHate::DestructorBody",
         "CEventAcquaintancePedHate::GetEventType", "CEventAcquaintancePedHate::CloneEditable", "CEventAcquaintancePedHate::ScalarDeletingDestructor", "CEventAcquaintancePedHate::GetEventPriority",
         "CEventGunShot::ScalarDeletingDestructor", "CEventVehicleCollision::ScalarDeletingDestructor", "CEventPedCollisionWithPed::ScalarDeletingDestructor", "CEventObjectCollision::ScalarDeletingDestructor",
         "CEventBuildingCollision::ScalarDeletingDestructor", "CEventDraggedOutCar::ScalarDeletingDestructor", "CTaskComplexKillAllThreats::Constructor", "CTaskComplexKillAllThreats::GetTaskType",
         "CTaskComplexKillAllThreats::Destructor", "CTaskComplexKillAllThreats::ScanForThreat", "CTaskComplexKillAllThreats::ControlSubTask", "CTaskComplexKillAllThreats::ScalarDeletingDestructor",
         "CTaskComplexKillAllThreats::CreateNextSubTask", "CTaskComplexKillAllThreats::CreateFirstSubTask", "CTaskComplexKillPedGroupOnFoot::ScalarDeletingDestructor", "CStencilShadows::FindForOwner",
         "CStencilShadowObject::Create"]
ENTRIES = (0x420F00, 0x420F10, 0x420F20, 0x420F30, 0x420F40, 0x420F50, 0x420FB0, 0x420FD0, 0x4B58E0, 0x4B5930, 0x4B5980, 0x4B59D0, 0x4B5A20, 0x4B5A70,
           0x622180, 0x6221B0, 0x6221C0, 0x6221D0, 0x622230, 0x6293A0, 0x6293C0, 0x629480, 0x629550, 0x710BA0, 0x711280)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000

CONST, CONSTAL, LOADFIELD, HATEBODY, HATECLONE, SDD, EVSDD, KATCTOR, KATBODY, SCAN, CTRLSUB, NEXT, FIRST, FIND, CREATE = \
    'const constal loadfield hatebody hateclone sdd evsdd katctor katbody scan ctrlsub next first find create'.split()
KIND = [CONST, LOADFIELD, CONSTAL, HATEBODY, CONST, HATECLONE, SDD, CONST, EVSDD, EVSDD, EVSDD, EVSDD, EVSDD, EVSDD, KATCTOR, CONST, KATBODY, SCAN, CTRLSUB, SDD, NEXT, FIRST, SDD, FIND, CREATE]
CONSTS = {0: 0, 4: 0x24, 7: 0x1A, 15: 0x3F6}
SDD_BODY = {6: 0x420F30, 19: 0x6221C0, 22: 0x6222B0}
SDD_DEL = {6: 0x4B5630, 19: 0x61A5B0, 22: 0x61A5B0}
EV_BODY = {8: 0x4AC720, 9: 0x4AC920, 10: 0x4ACA70, 11: 0x4ACDD0, 12: 0x4ACFF0, 13: 0x4AD330}
HATE_VT = 0x858E68
NEWADDR_EVENT, EVENT_CTOR, SCANFOR, SCANENT, NEWADDR_TASK, KILLCTOR, STANDCTOR = 0x4B5620, 0x4AF820, 0x6221D0, 0x607A90, 0x61A5A0, 0x620E30, 0x62F310
HATE_BODY_CALLEE = 0x4AF890
GETCOL, MALLOC, REGREF, UPDHIER = 0x535300, 0x72F420, 0x571B70, 0x710BC0
SUB_DTOR, SUB_TYPE = 0xD1000000, 0xD1000004


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


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


def tdiv(a, b):
    """division con signo truncada hacia cero"""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


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
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

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
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == LOADFIELD:
        _k += ['field_zero', 'field_other']
    elif _kind in (HATEBODY,):
        _k += ['prev_zero', 'prev_other']
    elif _kind == HATECLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == EVSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'index_negative', 'index_zero', 'index_positive', 'cursor_lowered', 'cursor_kept', 'map_in_guard']
    elif _kind == KATCTOR:
        _k += ['prev_vt_class', 'prev_other']
    elif _kind == KATBODY:
        _k += ['sub_null', 'sub_set']
    elif _kind == SCAN:
        _k += ['intel_raw', 'intel_arena', 'found_null', 'found_set']
    elif _kind == NEXT:
        _k += ['type_3e8', 'type_cb', 'type_other', 'threat_null', 'threat_set', 'mem_null', 'mem_set']
    elif _kind == FIRST:
        _k += ['threat_null', 'threat_set', 'mem_null', 'mem_set', 'standstill']
    elif _kind == FIND:
        _k += ['count_zero', 'hit_first', 'hit_later', 'hit_none', 'long_chain']
    elif _kind == CREATE:
        _k += ['type_zero', 'type_high', 'type_ok', 'col_null', 'flag_clear', 'tris_nonpositive', 'tris_big', 'tris_word_overflow', 'success']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, vt = [], None, None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == CONSTAL:
        W.snap()
        ret = 1
    elif k == LOADFIELD:
        A.wr(o + 0x14, 4, a[4])
        W.snap()
        ret = a[4]
        marks.append('field_zero' if a[4] == 0 else 'field_other')
    elif k == HATEBODY:
        W.snap()
        A.wr(o, 4, HATE_VT)
        events.append([HATE_BODY_CALLEE, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == HATECLONE:
        A.wr(o + 0x14, 4, a[8])
        W.snap()
        events.append([NEWADDR_EVENT, 0x18])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([EVENT_CTOR, W.tagv(mem), W.tagv(a[8])])
            W.wr(mem, 4, a[7])
            W.wr(mem, 4, HATE_VT)
            ret = W.tagv(mem)
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([SDD_DEL[fn], W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == EVSDD:
        delta = signed(a[7])
        D.wr(a[10] + 0, 4, (this - a[7]) & M32)
        D.wr(a[10] + 4, 4, W.raw(2, a[8]))
        D.wr(a[10] + 8, 4, 0x4000)
        D.wr(a[10] + 0xC, 4, a[9])
        W.snap()
        events.append([EV_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        idx = tdiv(delta, 68)
        at = a[8] + idx
        marks.append('index_negative' if idx < 0 else ('index_zero' if idx == 0 else 'index_positive'))
        if at < 0 or at >= WIN:
            marks.append('map_in_guard')
        if flags & 1:
            marks.append('flag_bit0_set')
            Cc.wr(at, 1, Cc.rd(at, 1) | 0x80)
            if idx < signed(a[9]):
                D.wr(a[10] + 0xC, 4, idx & M32)
                marks.append('cursor_lowered')
            else:
                marks.append('cursor_kept')
        else:
            marks.append('flag_bit0_clear')
        ret = W.tagv(this)
    elif k == KATCTOR:
        W.snap()
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        A.wr(o + 0xC, 4, a[4])
        A.wr(o + 0x10, 4, a[5])
        A.wr(o + 0x14, 4, a[6])
        vt = 1
        ret = W.tagv(this)
        marks.append('prev_vt_class' if a[3] in (0x86DA1C, 0x86D4CC, 0x86D48C) else 'prev_other')
    elif k == KATBODY:
        sub = 0
        if a[4] != 0:
            sub = W.raw(1, a[5])
            A.wr(o + 8, 4, sub)
            B.wr(a[5], 4, W.raw(3, a[6]))
        else:
            A.wr(o + 8, 4, 0)
        W.snap()
        if sub:
            events.append([SUB_DTOR, W.tagv(sub), 1])
            marks.append('sub_set')
        else:
            marks.append('sub_null')
        A.wr(o + 8, 4, 0)
        vt = 2
    elif k == SCAN:
        ped = W.raw(1, a[4])
        intel = a[7] if a[5] == 0 else W.raw(2, a[6])
        B.wr(a[4] + 0x47C, 4, intel)
        W.snap()
        ents = (intel + 0x130) & M32
        events.append([SCANENT, STACK_TAG, W.tagv(ped), 4, W.tagv(ents), 16, STACK_TAG, STACK_TAG, 0, 0, 0, 0, 1, 0, 0])
        ret = W.tagv(a[8])
        marks.append('intel_raw' if a[5] == 0 else 'intel_arena')
        marks.append('found_null' if a[8] == 0 else 'found_set')
    elif k == CTRLSUB:
        A.wr(o + 8, 4, a[4])
        W.snap()
        ret = W.tagv(a[4])
    elif k in (NEXT, FIRST):
        sub = W.raw(1, a[4])
        A.wr(o + 8, 4, sub)
        B.wr(a[4], 4, W.raw(3, a[5]))
        A.wr(o + 0xC, 4, a[13]); A.wr(o + 0x10, 4, a[14]); A.wr(o + 0x14, 4, a[15])
        W.snap()
        ret = 0
        threat = a[7]
        proceed = True
        if k == NEXT:
            events.append([SUB_TYPE, W.tagv(sub)])
            marks.append('type_3e8' if a[6] == 0x3E8 else ('type_cb' if a[6] == 0xCB else 'type_other'))
            if a[6] != 0x3E8:
                proceed = False
        if proceed:
            events.append([SCANFOR, W.tagv(this), W.tagv(a[12])])
            marks.append('threat_null' if threat == 0 else 'threat_set')
            if threat:
                events.append([NEWADDR_TASK, 0x38])
                if a[8] == 0:
                    marks.append('mem_null')
                else:
                    marks.append('mem_set')
                    mem = W.raw(1, a[9])
                    events.append([KILLCTOR, W.tagv(mem), W.tagv(threat), 0xFFFFFFFF, a[13], a[14], a[15], 1])
                    B.wr(a[9], 4, a[11])
                    ret = W.tagv(a[10])
            elif k == FIRST:
                marks.append('standstill')
                events.append([NEWADDR_TASK, 0x20])
                if a[8] == 0:
                    marks.append('mem_null')
                else:
                    marks.append('mem_set')
                    mem = W.raw(1, a[9])
                    events.append([STANDCTOR, W.tagv(mem), 0, 0, 0, 0x41000000])
                    B.wr(a[9], 4, a[11])
                    ret = W.tagv(a[10])
    elif k == FIND:
        cnt = a[4]
        nodes = []
        for i in range(cnt):
            arena = 1 + (a[19 + i] >> 16)
            nodes.append((arena, a[19 + i] & 0xFFFF))
        for i in range(cnt):
            arena, off = nodes[i]
            W.mem[arena].wr(off, 4, a[7 + i])
            W.mem[arena].wr(off + 0x14, 4, W.raw(*nodes[i + 1]) if i + 1 < cnt else 0)
        W.snap()
        found = 0
        pos = None
        for i in range(cnt):
            if a[7 + i] == a[6]:
                found = W.raw(*nodes[i])
                pos = i
                break
        ret = W.tagv(found)
        if cnt == 0:
            marks.append('count_zero')
        if cnt >= 8:
            marks.append('long_chain')
        if pos is None:
            marks.append('hit_none')
        elif pos == 0:
            marks.append('hit_first')
        else:
            marks.append('hit_later')
    elif k == CREATE:
        col_off = a[6]
        colData = W.raw(3, a[8]) if a[7] else 0
        Cc.wr(col_off + 0x2C, 4, colData)
        if colData:
            D.wr(a[8] + 7, 1, a[9] & 0xFF)
            D.wr(a[8] + 0x20, 4, a[10])
        W.snap()
        tb = a[4] & 0xFF
        ret = 0
        if tb == 0:
            marks.append('type_zero')
        elif tb > 2:
            marks.append('type_high')
        else:
            marks.append('type_ok')
            events.append([GETCOL, a[5]])
            n = signed(a[10])
            if not colData:
                marks.append('col_null')
            elif not ((a[9] & 0xFF) & 4):
                marks.append('flag_clear')
            elif n <= 0:
                marks.append('tris_nonpositive')
            else:
                if n >= 0x10000:
                    marks.append('tris_word_overflow')
                if n * 15 >= 0x80000000:
                    marks.append('tris_big')
                marks.append('success')
                A.wr(o + 4, 2, n & 0xFFFF)
                A.wr(o + 8, 4, (n * 15) & M32)
                A.wr(o, 4, a[5])
                A.wr(o + 6, 1, tb)
                A.wr(o + 0xC, 4, 0)
                snap7 = [A.rd(o + 4 * i, 4) for i in range(7)]
                events.append([MALLOC, (n * 180) & M32, 0] + snap7)
                A.wr(o + 0x10, 4, a[11])
                snap7 = [A.rd(o + 4 * i, 4) for i in range(7)]
                events.append([REGREF, a[5], W.tagv(this)] + snap7)
                events.append([UPDHIER, 0xC6A168, 0xC6A16C, W.tagv(this)] + snap7)
                ret = W.tagv(this)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'vt': vt, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if o.get('vt') != e['vt']:
        return 'vtable %s != %s' % (o.get('vt'), e['vt'])
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
