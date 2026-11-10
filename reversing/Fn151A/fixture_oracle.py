"""Oraculo independiente (Python) de Fn151A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 151A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x661F10  CTaskComplexSeekEntity<XYOffset> (vtable 0x86FBB4) destructor con borrado: push esi; mov esi,ecx; call 661F30; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x665DC0  GetSeekPos: sub esp,0Ch; push esi; ecx=this+40h; eax=[this+0Ch]; esi=&tmp; push esi; push eax; push [ped]; call [[this+40h]] (ranura 0 del calculador); copia tmp (3 dwords) a la salida; eax = salida; ret 8
   2  0x4AC370  CInformGroupEvent (copia de 4B29A0): si [this]: 571A00(ped, &[this]); [this]=0; si [this+8]: call [vtable] con 1; [this+8]=0; [this+0Ch]=-1
   3  0x4AC3B0  CInformGroupEvent (cuatro argumentos): [this]=ped; si ped: 571B70(ped, &[this]); [this+4]=grupo; [this+8]=evento; [this+0Ch]=[0B7CB84h]+retardo; eax = ese valor; ret 10h
   4  0x684750  CTaskComplexPartnerShove cuerpo: mov [ecx],870800h; jmp 683A40
   5  0x684760  CTaskComplexWalkAlongsidePed (vtable 0x8706CC) destructor con borrado: cuerpo 6832C0
   6  0x684780  CTaskComplexWaitForPed (vtable 0x8706F8): cuerpo 6833D0 (sin hook)
   7  0x6847A0  CTaskComplexBeInCouple (vtable 0x870724): cuerpo 683780
   8  0x690CB0  CTaskSimpleFacial cuerpo: mov [ecx],870AB8h; jmp 61A3A0
   9  0x692F90  CTaskComplexFacial (vtable 0x870ADC): cuerpo 690D90
  10  0x692FB0  CTaskSimpleAffectSecondaryBehaviour (vtable 0x870B08): cuerpo 6912C0
  11  0x692FD0  CTaskSimpleHoldEntity (vtable 0x870B2C): cuerpo 691630 (sin hook)
  12  0x5F65A0  CTaskSimpleChat: push 0; push 86C7B4h; push 1A0h; push duracion; push C0800000h; push 40800000h; push 0Ch; push 0; call 61ABE0; mov [esi],86C78Ch; eax = this; ret 4
  13  0x5F7200  CPedGroupDefaultTaskAllocators::Get: tipo sin signo > 5: 0; si no tabla {C09908, C0990C, C09910, C09914, C0991C, C09918}; solo escribe EAX y ECX
  14  0x691A40  CTaskComplexGoPickUpEntity::GetTaskType: mov eax,136h
  15  0x692C80  CTaskComplexGoPickUpEntity::Clone: push 34h; call 61A5A0; si 0: eax = 0; si no: push [this+2Ch]; push [this+0Ch]; ecx = memoria; call 6919C0; eax = el del constructor
  16  0x602BE0  CEventEscalator (vtable 0x86CD00) destructor con borrado: call 5FF860 (jmp 4AC010); test flags; push esi; call 4B5630; ...
  17  0x602B90  CEventEscalator::Clone: push 0Ch; call 4B5620; si 0: eax = 0; si no: ecx = memoria; call 4ABFC0; [memoria] = 86CD00h; eax = memoria
  18  0x61A350  mov dword [ecx],86D48Ch; ret (EAX, ECX y EDX intactos)
  19  0x61A430  CTaskComplex::SetSubTask: si [this+8] == arg: nada; si [this+8]: call [vtable] con 1; [this+8] = arg; si arg: [arg+4] = this; ret 4
  20  0x61A2E0  GetPlayerRelationshipRecorder: si [0C17084h]: eax = ese valor; si no: push 80h; call 82119A; si eax: 16 entradas de 8 bytes: dword +0 = 0 y byte +4 = 0 (el resto no se toca); [0C17084h] = eax
  21  0x48EA90  CTaskSimpleLookAbout cuerpo: jmp 61A6F0
  22  0x48EA40  CTaskSimpleHandsUp (vtable 0x85A29C): cuerpo 48EA60
  23  0x48EA70  CTaskSimpleLookAbout (vtable 0x85A164): cuerpo 48EA90

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor, ~CTask y constructores de CEvent); operator delete y las liberaciones anotan
[direccion, puntero (etiqueta), dword del puntero] (prueba que se borra DESPUES de destruir); operator new anota [direccion, tamano]; CleanUpOldReference y RegisterReference [direccion, ECX, argumento]; el constructor
de RunTimedAnim [direccion, ECX, grupo, anim, mezcla, sin uso, duracion, tipo, FNV del nombre, ultimo]; los destructores virtuales de los objetos de las arenas [0xFA11D001, ECX, argumento]; el calculador
[0xFA11CA1C, ECX, peaton, entidad, salida (etiqueta o 0x7F000000 si es la pila)]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexSeekEntityXYOffset::ScalarDeletingDestructor", "CTaskComplexSeekEntityXYOffset::GetSeekPos", "CInformGroupEvent::Destructor-2", "CInformGroupEvent::Constructor-2",
         "CTaskComplexPartnerShove::DestructorBody", "CTaskComplexWalkAlongsidePed::ScalarDeletingDestructor", "CTaskComplexWaitForPed::ScalarDeletingDestructor",
         "CTaskComplexBeInCouple::ScalarDeletingDestructor", "CTaskSimpleFacial::DestructorBody", "CTaskComplexFacial::ScalarDeletingDestructor",
         "CTaskSimpleAffectSecondaryBehaviour::ScalarDeletingDestructor", "CTaskSimpleHoldEntity::ScalarDeletingDestructor", "CTaskSimpleChat::Constructor",
         "CPedGroupDefaultTaskAllocators::Get", "CTaskComplexGoPickUpEntity::GetTaskType", "CTaskComplexGoPickUpEntity::Clone", "CEventEscalator::ScalarDeletingDestructor",
         "CEventEscalator::Clone", "CTask::Destructor-2", "CTaskComplex::SetSubTask", "GetPlayerRelationshipRecorder", "CTaskSimpleLookAbout::DestructorBody",
         "CTaskSimpleHandsUp::ScalarDeletingDestructor", "CTaskSimpleLookAbout::ScalarDeletingDestructor"]
ENTRIES = (0x661F10, 0x665DC0, 0x4AC370, 0x4AC3B0, 0x684750, 0x684760, 0x684780, 0x6847A0, 0x690CB0, 0x692F90, 0x692FB0, 0x692FD0,
           0x5F65A0, 0x5F7200, 0x691A40, 0x692C80, 0x602BE0, 0x602B90, 0x61A350, 0x61A430, 0x61A2E0, 0x48EA90, 0x48EA40, 0x48EA70)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_FAKES = (0x30300000, 0x30300010)                   # idem: vtables falsas (destructor virtual, calculador)
VDEL_EVENT, CALC_EVENT = 0xFA11D001, 0xFA11CA1C
CLEANUP, REGREF, RUNTIMED, PICKCTOR, TASKNEW, EVNEW, EVCTOR, CRTNEW = 0x571A00, 0x571B70, 0x61ABE0, 0x6919C0, 0x61A5A0, 0x4B5620, 0x4ABFC0, 0x82119A

# forma de cada funcion
SDD, GETSEEK, INFDTOR, INFCTOR, VTBODY, CHAT, ALLOC, CONST, CLONEPICK, ESCCLONE, TASKVT, SETSUB, RELREC = 'sdd getseek infdtor infctor vtbody chat alloc const clonepick escclone taskvt setsub relrec'.split()
KIND = [SDD, GETSEEK, INFDTOR, INFCTOR, VTBODY, SDD, SDD, SDD, VTBODY, SDD, SDD, SDD, CHAT, ALLOC, CONST, CLONEPICK, SDD, ESCCLONE, TASKVT, SETSUB, RELREC, VTBODY, SDD, SDD]
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {0: 0x661F30, 5: 0x6832C0, 6: 0x6833D0, 7: 0x683780, 9: 0x690D90, 10: 0x6912C0, 11: 0x691630, 16: 0x5FF860, 22: 0x48EA60, 23: 0x48EA90}
DELETE = {0: 0x61A5B0, 5: 0x61A5B0, 6: 0x61A5B0, 7: 0x61A5B0, 9: 0x61A5B0, 10: 0x61A5B0, 11: 0x61A5B0, 16: 0x4B5630, 22: 0x61A5B0, 23: 0x61A5B0}
# cuerpos con vtable (0 = ninguna) y colaborador que llaman
VTBODY_INFO = {4: (0x870800, 0x683A40), 8: (0x870AB8, 0x61A3A0), 21: (0, 0x61A6F0)}
ALLOC_TABLE = {0: 0xC09908, 1: 0xC0990C, 2: 0xC09910, 3: 0xC09914, 4: 0xC0991C, 5: 0xC09918}
SEED_ALIAS = (0x00, 0x08, 0x0C, 0x38, 0x40)


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


CHAT_NAME_FNV = fnv(b'Chat')


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


KEYS = {}
for _fn in range(24):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (SDD, VTBODY, CHAT, CONST, TASKVT, INFDTOR, INFCTOR, GETSEEK, CLONEPICK, ESCCLONE, SETSUB):
        _k += ['this_aligned', 'this_unaligned']
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == VTBODY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == GETSEEK:
        _k += ['out_in_b', 'out_alias', 'snan_component', 'nan_component', 'entity_null', 'ped_null']
    elif _kind == INFDTOR:
        _k += ['ped_null', 'ped_set', 'event_null', 'event_set']
    elif _kind == INFCTOR:
        _k += ['ped_null', 'ped_set', 'time_wraps']
    elif _kind == CHAT:
        _k += ['prev_zero', 'prev_other']
    elif _kind == ALLOC:
        _k += ['type_0', 'type_1', 'type_2', 'type_3', 'type_4', 'type_5', 'type_above', 'type_negative']
    elif _kind == CLONEPICK:
        _k += ['memory_null', 'memory_in_b', 'memory_in_a', 'eax_memory', 'eax_other']
    elif _kind == ESCCLONE:
        _k += ['memory_null', 'memory_in_b', 'eax_ignored']
    elif _kind == SETSUB:
        _k += ['cur_null', 'cur_set', 'arg_null', 'arg_other', 'arg_same', 'arg_this', 'delete_old']
    elif _kind == RELREC:
        _k += ['global_set', 'global_null', 'alloc_null', 'alloc_ok']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] in (SDD, VTBODY, CHAT, CONST, TASKVT, INFDTOR, INFCTOR, GETSEEK, CLONEPICK, ESCCLONE, SETSUB):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    fakes = c.get('fakes') or DEFAULT_FAKES
    vt_del, vt_calc = fakes[0] & M32, fakes[1] & M32
    events, ret, glob = [], None, None
    o = a[0]
    this = W.raw(0, o)
    if k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], W.tagv(this), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], W.tagv(this), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = W.tagv(this)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == GETSEEK:
        A.wr(o + 0xC, 4, a[3])
        A.wr(o + 0x40, 4, vt_calc)
        mode = a[8]
        out = W.raw(1, a[9]) if mode == 0 else W.raw(0, o + SEED_ALIAS[mode - 1])
        W.snap()
        marks.append('out_in_b' if mode == 0 else 'out_alias')
        events.append([CALC_EVENT, W.tagv(this + 0x40), a[4], a[3], 0x7F000000])
        for i in range(3):
            W.wr((out + 4 * i) & M32, 4, a[5 + i])
        ret = W.tagv(out)
        for i in range(3):
            w = a[5 + i]
            if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
                marks.append('snan_component' if not (w & 0x400000) else 'nan_component')
        if a[3] == 0:
            marks.append('entity_null')
        if a[4] == 0:
            marks.append('ped_null')
    elif k == INFDTOR:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 0xC, 4, a[7])
        ev = 0
        if a[5] == 1:
            B.wr(a[6], 4, vt_del)
            ev = W.raw(1, a[6])
        A.wr(o + 8, 4, ev)
        W.snap()
        if a[3]:
            events.append([CLEANUP, W.tagv(a[3]), W.tagv(this)])
            A.wr(o, 4, 0)
            marks.append('ped_set')
        else:
            marks.append('ped_null')
        if ev:
            events.append([VDEL_EVENT, W.tagv(ev), 1])
            A.wr(o + 8, 4, 0)
            marks.append('event_set')
        else:
            marks.append('event_null')
        A.wr(o + 0xC, 4, M32)
    elif k == INFCTOR:
        A.wr(o, 4, a[8]); A.wr(o + 4, 4, a[9]); A.wr(o + 8, 4, a[10]); A.wr(o + 0xC, 4, a[11])
        W.snap()
        A.wr(o, 4, a[3])
        if a[3]:
            events.append([REGREF, W.tagv(a[3]), W.tagv(this)])
            marks.append('ped_set')
        else:
            marks.append('ped_null')
        A.wr(o + 4, 4, a[4])
        A.wr(o + 8, 4, a[5])
        total = a[7] + a[6]
        if total > M32:
            marks.append('time_wraps')
        t = total & M32
        A.wr(o + 0xC, 4, t)
        ret = t
    elif k == VTBODY:
        vt, body = VTBODY_INFO[fn]
        A.wr(o, 4, a[3])
        W.snap()
        if vt:
            A.wr(o, 4, vt)
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CHAT:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([RUNTIMED, W.tagv(this), 0, 0xC, 0x40800000, 0xC0800000, a[6], 0x1A0, CHAT_NAME_FNV, 0])
        A.wr(o, 4, a[5])                                  # el falso del constructor base deja su dword
        A.wr(o, 4, 0x86C78C)                              # y la funcion deja la vtable del EXE
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ALLOC:
        W.snap()
        t = a[3]
        ret = ALLOC_TABLE.get(t, 0)
        marks.append('type_%d' % t if t <= 5 else ('type_negative' if t & 0x80000000 else 'type_above'))
    elif k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 0x136
    elif k == CLONEPICK:
        A.wr(o + 0xC, 4, a[3]); A.wr(o + 0x2C, 4, a[4])
        W.snap()
        events.append([TASKNEW, 0x34])
        mem = 0 if a[5] == 0 else (W.raw(1, a[6]) if a[5] == 1 else W.raw(0, a[6]))
        if mem == 0:
            ret = 0
            marks.append('memory_null')
        else:
            events.append([PICKCTOR, W.tagv(mem), a[3], a[4]])
            W.wr(mem, 4, a[8])
            sel = a[7]
            eax = mem if sel == 0 else ((mem + 4) & M32 if sel == 1 else (0 if sel == 2 else a[9]))
            ret = W.tagv(eax)
            marks.append('memory_in_b' if a[5] == 1 else 'memory_in_a')
            marks.append('eax_memory' if sel == 0 else 'eax_other')
    elif k == ESCCLONE:
        W.snap()
        events.append([EVNEW, 0xC])
        mem = 0 if a[3] == 0 else W.raw(1, a[4])
        if mem == 0:
            ret = 0
            marks.append('memory_null')
        else:
            events.append([EVCTOR, W.tagv(mem), W.rd(mem, 4)])
            W.wr(mem, 4, a[5])
            W.wr(mem, 4, 0x86CD00)
            ret = W.tagv(mem)
            marks.append('memory_in_b')
            marks.append('eax_ignored')
    elif k == TASKVT:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x86D48C)
    elif k == SETSUB:
        cur = W.raw(1, a[5]) if a[3] == 1 else 0
        A.wr(o, 4, a[8]); A.wr(o + 4, 4, a[7]); A.wr(o + 8, 4, cur)
        if a[3] == 1:
            B.wr(a[5], 4, vt_del)
        mode = a[4]
        arg = 0 if mode == 0 else (cur if mode == 2 else (this if mode == 3 else W.raw(2, a[6])))
        W.snap()
        marks.append('cur_set' if cur else 'cur_null')
        marks.append('arg_null' if arg == 0 else 'arg_other')
        if mode == 2:
            marks.append('arg_same')
        if mode == 3:
            marks.append('arg_this')
        if cur != arg:
            if cur:
                events.append([VDEL_EVENT, W.tagv(cur), 1])
                marks.append('delete_old')
            A.wr(o + 8, 4, arg)
            if arg:
                W.wr((arg + 4) & M32, 4, this)
    elif k == RELREC:
        g = 0 if a[3] == 0 else W.raw(2, a[4])
        W.snap()
        if g:
            ret = W.tagv(g)
            glob = ret
            marks.append('global_set')
        else:
            marks.append('global_null')
            events.append([CRTNEW, 0x80])
            mem = 0 if a[5] == 0 else W.raw(1, a[6])
            if mem == 0:
                ret = 0
                glob = 0
                marks.append('alloc_null')
            else:
                for i in range(16):
                    W.wr(mem + 8 * i, 4, 0)
                    W.wr(mem + 8 * i + 4, 1, 0)
                ret = W.tagv(mem)
                glob = ret
                marks.append('alloc_ok')
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}
    if glob is not None:
        out['glob'] = glob
    return out


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if 'glob' in e and o.get('glob') != e['glob']:
        return 'glob %s != %s' % (o.get('glob'), e['glob'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """contratos de registros del EXE que los llamadores usan: 0x61A350 conserva EAX, ECX y EDX; 0x5F7200 conserva EDX (su llamador lo guarda ahi)"""
    fn = c['fn']
    if fn == 18:
        this = (c.get('arena') or DEFAULT_ARENAS)[0] + GUARD + (c['a'][0] & M32)
        if (residue['eax'], residue['ecx'], residue['edx']) != (c['eax'] & M32, this & M32, c['edx'] & M32):
            return 'EAX/ECX/EDX no conservados %s' % residue
    elif fn == 13:
        if residue['edx'] != c['edx'] & M32:
            return 'EDX no conservado %s' % residue
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
