"""Oraculo independiente (Python) de Fn149A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 149A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x48E800  CTaskSimplePause::GetTaskType (vtable 0x85A278, ranura 4)   mov eax,0CAh; ret
   1  0x48E8A0  CTaskSimplePause::Destructor (cuerpo)   mov [ecx],85A278h; jmp 61A3A0 (~CTask, que deja la vtable de CTask)
   2  0x48E880  CTaskSimplePause (vtable 0x85A278, ranura 0) destructor con borrado: push esi; mov esi,ecx; call 48E8A0; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   3  0x48E8E0  CTaskSimpleCower (vtable 0x85A0D0) destructor con borrado, cuerpo 48E900 (jmp 61A6F0)
   4  0x744D50  cmp word [B733F6],0; je L; cmp word [B73186],0; je R1; L: cmp word [B733F8],0; je R0; cmp word [B73188],0; jne R0; R1: mov eax,1; ret; R0: xor eax,eax; ret
   5  0x744D90  cmp word [B733E6],0; je R0; cmp word [B73176],0; jne R0; mov eax,1; ret; R0: xor eax,eax; ret
   6  0x63D0D0  CTaskComplexEnterAnyCarAsDriver::Constructor  push esi; mov esi,ecx; call 61A3B0; mov [esi],86EA14h; mov eax,esi; pop esi; ret
   7  0x6434F0  CTaskComplexEnterAnyCarAsDriver (vtable 0x86EA14) destructor con borrado, cuerpo 63D100
   8  0x643A40  CTaskComplexEnterCar (vtable 0x86E6F0) destructor con borrado, cuerpo 63DFA0
   9  0x6389B0  CTaskSimpleWaitForBus (vtable 0x86E1B8) destructor con borrado, cuerpo 632A90
  10  0x6389D0  CTaskSimpleWaitForPizza (vtable 0x86E1DC) destructor con borrado, cuerpo 632B70
  11  0x4ACFB0  CEventBuildingCollision::GetEventType (vtable 0x85ACE8, ranura 1)       mov eax,6; ret
  12  0x4ACFC0  CEventBuildingCollision::GetLifeTime (ranura 3)                         xor eax,eax; ret
  13  0x4ACFD0  CEventBuildingCollision::CanBeInterruptedBySameEvent (ranura 15)        mov al,1; ret
  14  0x4ACFE0  CEventBuildingCollision::GetEventPriority (ranura 2)                    mov eax,3Bh; ret
  15  0x4AD050  CEventBuildingCollision::TakesPriorityOver (ranura 11)  push esi; mov esi,ecx; mov ecx,[esp+8]; mov eax,[ecx]; push edi; call [eax+4]; mov edx,[esi]; mov ecx,esi; mov edi,eax; call [edx+4]; cmp edi,eax; pop edi; setne al; pop esi; ret 4
  16  0x4AD3A0  CEventDraggedOutCar::AffectsPed (vtable 0x85AD30, ranura 5)  push esi; mov esi,ecx; mov ecx,[esp+8]; call 5E0170; test al,al; jne +; pop esi; ret 4; mov eax,[esi+14h]; test eax,eax; setne al; pop esi; ret 4
  17  0x404180  CBuilding::DestructorBody   jmp 535E90 (~CEntity)
  18  0x404130  CBuilding (vtable 0x8585C8) destructor con borrado: push esi; push edi; mov edi,ecx; call 404180; test byte [esp+0Ch],1; je +; mov esi,[B74498]; edx=[esi]; ecx=edi-edx; edx=ecx/56 (imul 92492493h ...);
            or byte [[esi+4]+edx],80h; cmp edx,[esi+0Ch]; mov eax,edi; jge +; mov [esi+0Ch],edx; ...; ret 4
  19  0x5F6510  CPedGroupDefaultTaskAllocatorRandom::Constructor   mov eax,ecx; mov [eax],86C77Ch; ret
  20  0x4D7990  CAEAudioChannel::IsBufferPlaying   mov eax,[ecx+5Ch]; and eax,1; ret
  21  0x4D7C00  CAEAudioChannel (vtable 0x85F03C) destructor con borrado: push esi; mov esi,ecx; mov eax,[esi+8]; test eax,eax; mov [esi],85F03Ch; je +; dec [B5F898]; mov eax,[esi+8]; test; je +;
            mov ecx,[eax]; push eax; call [ecx+8]; mov eax,[esi+0Ch]; test eax,eax; mov [esi+8],0; je +; mov edx,[eax]; push eax; call [edx+8]; test byte [esp+8],1; mov [esi+0Ch],0; je +; push esi; call 8214BD;
            add esp,4; mov eax,esi; pop esi; ret 4
  22  0x5F8510  CPedGroupIntelligence::SetEventResponseTask  (6 dwords, ret 18h): si [arg2 byte] SetTask(ped, tarea, this+0Ch, -1, 0); si [arg4 byte] SetTask(ped, tarea2, this+0ACh, slot, 0)

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos y constructores base) y dejan un dword del caso en [ECX]; operator delete anota
[direccion, puntero (etiqueta), dword de [puntero]]: prueba que se borra DESPUES de destruir. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskSimplePause::GetTaskType', 'CTaskSimplePause::Destructor', 'CTaskSimplePause::ScalarDeletingDestructor', 'CTaskSimpleCower::ScalarDeletingDestructor', 'SecuromAnyPairPending',
         'SecuromFirstPairPending', 'CTaskComplexEnterAnyCarAsDriver::Constructor', 'CTaskComplexEnterAnyCarAsDriver::ScalarDeletingDestructor', 'CTaskComplexEnterCar::ScalarDeletingDestructor',
         'CTaskSimpleWaitForBus::ScalarDeletingDestructor', 'CTaskSimpleWaitForPizza::ScalarDeletingDestructor', 'CEventBuildingCollision::GetEventType', 'CEventBuildingCollision::GetLifeTime',
         'CEventBuildingCollision::CanBeInterruptedBySameEvent', 'CEventBuildingCollision::GetEventPriority', 'CEventBuildingCollision::TakesPriorityOver', 'CEventDraggedOutCar::AffectsPed',
         'CBuilding::DestructorBody', 'CBuilding::ScalarDeletingDestructor', 'CPedGroupDefaultTaskAllocatorRandom::Constructor', 'CAEAudioChannel::IsBufferPlaying',
         'CAEAudioChannel::ScalarDeletingDestructor', 'CPedGroupIntelligence::SetEventResponseTask']
ENTRIES = (0x48E800, 0x48E8A0, 0x48E880, 0x48E8E0, 0x744D50, 0x744D90, 0x63D0D0, 0x6434F0, 0x643A40, 0x6389B0, 0x6389D0, 0x4ACFB0, 0x4ACFC0, 0x4ACFD0, 0x4ACFE0, 0x4AD050, 0x4AD3A0, 0x404180,
           0x404130, 0x5F6510, 0x4D7990, 0x4D7C00, 0x5F8510)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
CRT_DELETE = 0x8214BD
TASK_DELETE = 0x61A5B0
EV_REL, EV_TYPE_REF, EV_TYPE_OWN = 0x7E1EA5E0, 0x7E1EA5E1, 0x7E1EA5E2
SETTASK = 0x5F7540
ISALIVE = 0x5E0170
COUNTER = 0xB5F898

# forma de cada funcion
CONST, CONSTAL, BODY, SDD, SDDB, CTOR, CTORNC, SEC, STATUS, PRIO, AFF, CHAN, SETRESP = 'const constal body sdd sddb ctor ctornc sec status prio aff chan setresp'.split()
KIND = [CONST, BODY, SDD, SDD, SEC, SEC, CTOR, SDD, SDD, SDD, SDD, CONST, CONST, CONSTAL, CONST, PRIO, AFF, BODY, SDDB, CTORNC, STATUS, CHAN, SETRESP]
# direccion (EXE) del colaborador que llama la funcion (cuerpo del destructor, constructor base, ~CEntity)
CALLEE = {1: 0x61A3A0, 2: 0x48E8A0, 3: 0x48E900, 6: 0x61A3B0, 7: 0x63D100, 8: 0x63DFA0, 9: 0x632A90, 10: 0x632B70, 17: 0x535E90, 18: 0x404180}
# vtable que el cuerpo deja en [this] ANTES de llamar al colaborador
VT_BEFORE = {1: 0x85A278}
# vtable que deja DESPUES (constructores)
VT_AFTER = {6: 0x86EA14, 19: 0x86C77C}
CONSTANT = {0: 0xCA, 11: 6, 12: 0, 14: 0x3B}
CHAN_VT = 0x85F03C


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


def trunc_div(d, n):
    """division con signo truncada hacia cero (d con signo)"""
    q = abs(d) // n
    return -q if d < 0 else q


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


KEYS = {}
for _fn in range(len(NAMES)):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (BODY, SDD, SDDB, CTOR):
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other']
        if _kind in (SDD, SDDB):
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
        if _kind == SDDB:
            _k += ['idx_neg', 'idx_zero', 'idx_pos', 'cursor_lowered', 'cursor_equal', 'cursor_kept', 'rem_nonzero']
    elif _kind in (CONST, CONSTAL, CTORNC):
        _k += ['this_aligned', 'this_unaligned']
    elif _kind == STATUS:
        _k += ['this_aligned', 'this_unaligned', 'bit0_set', 'bit0_clear']
    elif _kind == SEC:
        _k += ['ret_zero', 'ret_one', 'word_zero_high_set', 'word_nonzero']
        _k += ['branch1_true', 'branch2_true'] if _fn == 4 else ['pair_true']
    elif _kind == PRIO:
        _k += ['this_aligned', 'this_unaligned', 'ret_zero', 'ret_one', 'alias', 'distinct']
    elif _kind == AFF:
        _k += ['this_aligned', 'this_unaligned', 'alive', 'dead', 'jacker_set', 'jacker_zero', 'ret_zero', 'ret_one']
    elif _kind == CHAN:
        _k += ['this_aligned', 'this_unaligned', 'buf1_null', 'buf1_obj', 'buf3_null', 'buf3_obj', 'alias', 'flag_bit0_set', 'flag_bit0_clear', 'counter_borrow']
    elif _kind == SETRESP:
        _k += ['this_aligned', 'this_unaligned', 'main_called', 'main_skipped', 'sec_called', 'sec_skipped', 'flag_high_only']
    KEYS[_fn] = _k

SPAN = {20: 0x60, 21: 0x10, 22: 0xB0, 16: 0x18}


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] not in (SEC,):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        if a[0] + SPAN.get(fn, 4) > WIN:
            marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret, gch = [], None, []
    o = a[0]
    if k in (BODY, SDD, SDDB, CTOR):
        A.wr(o, 4, a[3])
        if k == SDDB:
            B.wr(a[10] + 0xC, 4, a[8])
        A.snap(); B.snap(); C.snap()
        callee = CALLEE[fn]
        if fn in VT_BEFORE:
            A.wr(o, 4, VT_BEFORE[fn])
        # el colaborador anota [direccion, this, dword de [this] al entrar] y deja el dword del caso en [this]
        events.append([callee, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        if k in (SDD, SDDB):
            flags = a[6] & 0xFF                                  # solo el byte bajo de la ranura de pila
            if flags & 1:
                if k == SDD:
                    events.append([TASK_DELETE, tag(0, o), a[5]])   # operator delete ve lo que dejo el colaborador
                else:                                            # operator delete de CBuilding en linea: ranura del pool falso
                    d = s32(a[7])
                    t = trunc_div(d, 56)
                    C.m[GUARD + a[9]] = C.get(GUARD + a[9]) | 0x80
                    if t < s32(a[8]):
                        B.wr(a[10] + 0xC, 4, t & M32)
            ret = tag(0, o)
            marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
            if k == SDDB:
                d = s32(a[7])
                t = trunc_div(d, 56)
                cur = s32(a[8])
                marks.append('idx_neg' if t < 0 else 'idx_zero' if t == 0 else 'idx_pos')
                marks.append('cursor_lowered' if t < cur else 'cursor_equal' if t == cur else 'cursor_kept')
                if d - t * 56 != 0:
                    marks.append('rem_nonzero')
        if k == CTOR:
            A.wr(o, 4, VT_AFTER[fn])
            ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CTORNC:
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        A.wr(o, 4, VT_AFTER[fn])
        ret = tag(0, o)
    elif k == CONST:
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif k == CONSTAL:
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        ret = 1
    elif k == STATUS:
        A.wr(o + 0x5C, 4, a[3]); A.snap(); B.snap(); C.snap()
        ret = a[3] & 1
        marks.append('bit0_set' if a[3] & 1 else 'bit0_clear')
    elif k == SEC:
        A.snap(); B.snap(); C.snap()
        w = [(x & 0xFFFF) for x in a[3:9]]
        if fn == 4:
            b1 = bool(w[0]) and not w[1]
            b2 = bool(w[2]) and not w[3]
            ret = int(b1 or b2)
            if b1:
                marks.append('branch1_true')
            if b2:
                marks.append('branch2_true')
        else:
            ret = int(bool(w[4]) and not w[5])
            if ret:
                marks.append('pair_true')
        for x in a[3:9]:
            marks.append('word_zero_high_set' if (x & 0xFFFF) == 0 and x else 'word_nonzero')
        marks.append('ret_one' if ret else 'ret_zero')
    elif k == PRIO:
        A.snap(); B.snap(); C.snap()
        if a[5] & 1:                                   # el argumento es el propio objeto: las dos llamadas van al falso del tipo propio
            events.append([EV_TYPE_OWN, tag(0, o)])
            events.append([EV_TYPE_OWN, tag(0, o)])
            ref_t = own_t = a[4]
            marks.append('alias')
        else:
            events.append([EV_TYPE_REF, tag(1, a[6])])
            events.append([EV_TYPE_OWN, tag(0, o)])
            ref_t, own_t = a[3], a[4]
            marks.append('distinct')
        ret = int(ref_t != own_t)
        marks.append('ret_one' if ret else 'ret_zero')
    elif k == AFF:
        A.wr(o + 0x14, 4, a[4]); A.snap(); B.snap(); C.snap()
        ped = tag(1, a[6]) if a[5] & 1 else a[7]
        events.append([ISALIVE, ped])
        alive = (a[3] & 0xFF) != 0
        ret = int(alive and a[4] != 0)
        marks += ['alive' if alive else 'dead', 'jacker_set' if a[4] else 'jacker_zero', 'ret_one' if ret else 'ret_zero']
    elif k == CHAN:
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        objs = [0, tag(1, a[8]), tag(1, a[9])]
        p1 = objs[a[4] % 3]
        p3 = objs[2] if a[5] == 1 else (objs[1] if a[5] == 2 else 0)
        counter = a[7]
        A.wr(o, 4, CHAN_VT)
        if p1:
            counter = (counter - 1) & M32
            events.append([EV_REL, p1, CHAN_VT, p1, p3, counter])
        if p3:
            events.append([EV_REL, p3, CHAN_VT, 0, p3, counter])
        if a[6] & 1:
            events.append([CRT_DELETE, tag(0, o), CHAN_VT])
        for i in range(4):
            ob, nb = (a[7] >> (8 * i)) & 0xFF, (counter >> (8 * i)) & 0xFF
            if ob != nb:
                gch.append([COUNTER + i, nb])
        ret = tag(0, o)
        marks += ['buf1_obj' if p1 else 'buf1_null', 'buf3_obj' if p3 else 'buf3_null', 'flag_bit0_set' if a[6] & 1 else 'flag_bit0_clear']
        if p1 and p1 == p3:
            marks.append('alias')
        if p1 and (a[7] & 0xFF) == 0:
            marks.append('counter_borrow')
    elif k == SETRESP:
        A.snap(); B.snap(); C.snap()
        self_t = tag(0, o)
        main_on, sec_on = (a[4] & 0xFF) != 0, (a[6] & 0xFF) != 0
        if main_on:
            events.append([SETTASK, self_t, a[3], a[5], tag(0, o + 0xC), 0xFFFFFFFF, 0])
        if sec_on:
            events.append([SETTASK, self_t, a[3], a[7], tag(0, o + 0xAC), a[8], 0])
        marks += ['main_called' if main_on else 'main_skipped', 'sec_called' if sec_on else 'sec_skipped']
        if (a[4] and not a[4] & 0xFF) or (a[6] and not a[6] & 0xFF):
            marks.append('flag_high_only')
    obs = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}
    if k == CHAN:
        obs['fields'] = [0, 0]
    return obs


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if 'fields' in e and o.get('fields') != e['fields']:
        return 'fields %s != %s' % (o.get('fields'), e['fields'])
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
