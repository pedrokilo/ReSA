"""Oraculo independiente (Python) de Fn148A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 20 funciones del lote 148A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (capstone sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4A9F20  FxPrim_c::Constructor            mov eax,ecx; xor ecx,ecx; mov [eax],85A9E0h; mov [eax+4],ecx; mov [eax+8],ecx; mov byte [eax+0Ch],1; ret
   1  0x4A9F40  FxPrim_c::Destructor (cuerpo)    xor eax,eax; mov [ecx],85A9E0h; mov [ecx+4],eax; mov [ecx+8],eax; ret
   2  0x4A9F50  FxPrim_c::Enable                 mov al,[esp+4]; mov [ecx+0Ch],al; ret 4
   3  0x4A9F60  CAEAudioEntity::Destructor       mov [ecx],859988h; add ecx,8; jmp 4EF660 (CAESound::Destructor)
   4  0x4A9F70  FxSphere_c::Constructor          mov eax,ecx; mov dword [eax+10h],0; ret
   5  0x5F8580  CPedGroupIntelligence::SetDefaultTask   mov edx,[esp+8]; push 0; push -1; lea eax,[ecx+1ECh]; push eax; mov eax,[esp+10h]; push edx; push eax; call 5F7540; ret 8
   6  0x5F8750  CPedGroupIntelligence::ReportAllBarDefaultTasksFinished   push esi; mov esi,ecx; lea eax,[esi+0Ch]; push eax; call 5F7730; lea ecx,[esi+14Ch]; push ecx; mov ecx,esi; call 5F7730; lea edx,[esi+0ACh]; push edx; mov ecx,esi; call 5F7730; pop esi; ret
   7  0x63CB50  CTaskComplexCarDriveWander::Destructor (cuerpo)   mov dword [ecx],86E96Ch; jmp 63CA40
   8  0x643260  CTaskComplexCarDriveMission SDD      push esi; mov esi,ecx; call 63CCC0; test byte [esp+8],1; je ..; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   9  0x643280  CTaskComplexDriveToPoint SDD         idem; cuerpo 63CE70
  10  0x5F7FB0  CTaskSimpleChat SDD                  idem; cuerpo 5F7FD0
  11  0x5FB990  CPedGroupMembership::Destructor (cuerpo)   para i = 0..7: si dword [this+4+4i] != 0 -> RemoveMember (5F80D0, ecx = this, push i)
  12  0x5FE960  tUserList::IsPedTypeAllowed          mov al,[ecx+20h]; test al,al; jne ..; mov al,1; ret 4; ..: bucle de 4 entradas: [ecx+4i] == -2 y [ecx+4i+10h] == arg -> al = 1
  13  0x5FE9C0  IsPedTypeGang                        mov eax,[esp+4]; cmp eax,7; jl no; cmp eax,10h; jg no; mov eax,1; ret; no: xor eax,eax; ret
  14  0x4161D0  CLinkList<CCollisionData*>::Shutdown push esi; mov esi,ecx; push [esi+30h]; call 8213AE (delete[]); add esp,4; mov dword [esi+30h],0; pop esi; ret
  15  0x4161F0  CLinkList<CCollisionData*>::Insert   eax = [ecx+20h] (primer libre); si eax == ecx+24h devuelve 0; [eax] = *arg; desengancha eax de su lista (next.prev = prev; prev.next = next); lo pone tras la cabeza usada ([ecx+8]); devuelve eax
  16  0x678CB0  CTaskComplexInAirAndLand::GetTaskType (vtable 0x8704AC, ranura 4)   mov eax,0F0h; ret
  17  0x678CC0  CTaskComplexInAirAndLand::Destructor (cuerpo)   mov dword [ecx],8704ACh; jmp 61A3D0
  18  0x67D340  CTaskSimpleInAir SDD (vtable 0x8704D8 ranura 0)   idem 8; cuerpo 678D50
  19  0x67D360  CTaskSimpleLand SDD (vtable 0x8704FC ranura 0)    idem 8; cuerpo 678EE0

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['FxPrim_c::Constructor', 'FxPrim_c::Destructor', 'FxPrim_c::Enable', 'CAEAudioEntity::Destructor-2', 'FxSphere_c::Constructor', 'CPedGroupIntelligence::SetDefaultTask',
         'CPedGroupIntelligence::ReportAllBarDefaultTasksFinished', 'CTaskComplexCarDriveWander::Destructor', 'CTaskComplexCarDriveMission::ScalarDeletingDestructor',
         'CTaskComplexDriveToPoint::ScalarDeletingDestructor', 'CTaskSimpleChat::ScalarDeletingDestructor', 'CPedGroupMembership::Destructor', 'tUserList::IsPedTypeAllowed',
         'IsPedTypeGang', 'CLinkList<CCollisionData*>::Shutdown', 'CLinkList<CCollisionData*>::Insert', 'CTaskComplexInAirAndLand::GetTaskType',
         'CTaskComplexInAirAndLand::Destructor', 'CTaskSimpleInAir::ScalarDeletingDestructor', 'CTaskSimpleLand::ScalarDeletingDestructor']
ENTRIES = (0x4A9F20, 0x4A9F40, 0x4A9F50, 0x4A9F60, 0x4A9F70, 0x5F8580, 0x5F8750, 0x63CB50, 0x643260, 0x643280, 0x5F7FB0, 0x5FB990, 0x5FE960, 0x5FE9C0, 0x4161D0, 0x4161F0,
           0x678CB0, 0x678CC0, 0x67D340, 0x67D360)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DELETE_ARRAY = 0x8213AE
CTASK_DELETE = 0x61A5B0
SOUND_DTOR = 0x4EF660
SET_TASK = 0x5F7540
REPORT_ALL = 0x5F7730
REMOVE_MEMBER = 0x5F80D0
CAR_DRIVE_DTOR = 0x63CA40
COMPLEX_DTOR = 0x61A3D0


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


def untag(v):
    """(arena, desplazamiento respecto a la ventana) de una etiqueta"""
    return (v - 0x10000000) >> 20, (v & 0xFFFFF) - GUARD


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

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

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

    def wchanges(self):
        """cambios por dwords alineados de la arena: [desplazamiento respecto a la ventana, dword nuevo]"""
        out = []
        for w in sorted({i >> 2 for i in self.m}):
            cur = sum(self.get(4 * w + k) << (8 * k) for k in range(4))
            ref = sum((self.r.get(4 * w + k) if self.r.get(4 * w + k) is not None else self.base(4 * w + k)) << (8 * k) for k in range(4))
            if cur != ref:
                out.append([4 * w - GUARD, cur])
        return out


# destructores con borrado: indice -> cuerpo con el que se destruyen (el operator delete es CTask::operator delete, 0x61A5B0)
SDD = {8: 0x63CCC0, 9: 0x63CE70, 10: 0x5F7FD0, 18: 0x678D50, 19: 0x678EE0}
SPAN = {0: 0x10, 1: 0x10, 2: 0x10, 3: 0x10, 4: 0x14, 5: 0x2A0, 6: 0x2A0, 11: 0x28, 12: 0x24, 14: 0x34, 15: 0x34}


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    o = a[0]
    span = SPAN.get(fn, 4)
    marks.append('this_unaligned' if o % 4 else 'this_aligned')
    if o + span > WIN:
        marks.append('spill')
    return marks


KEYS = {}
for _fn in range(20):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_delete', 'flag_keep', 'flag_high_bits', 'prev_zero', 'prev_other', 'gret_zero', 'gret_other', 'write_zero', 'write_other']
    elif _fn == 0:
        _k += ['p0_same', 'p0_other', 'p4_zero', 'p4_other', 'pc_one', 'pc_other']
    elif _fn == 1:
        _k += ['p0_same', 'p0_other', 'p4_zero', 'p4_other', 'p8_zero', 'p8_other']
    elif _fn == 2:
        _k += ['arg_zero', 'arg_one', 'arg_other_byte', 'arg_low_zero_high_nonzero', 'same_byte', 'diff_byte']
    elif _fn == 3:
        _k += ['prev_same', 'prev_other']
    elif _fn == 4:
        _k += ['prev_zero', 'prev_other']
    elif _fn == 5:
        _k += ['ped_raw', 'ped_arena', 'task_raw', 'task_arena']
    elif _fn == 7:
        _k += ['prev_same', 'prev_other', 'write_zero', 'write_other']
    elif _fn == 11:
        _k += ['mask_all', 'mask_none', 'mask_some', 'mod_none', 'mod_skips', 'mod_adds', 'mod_noeffect']
    elif _fn == 12:
        _k += ['flag_zero', 'flag_nonzero', 'flag_high_only', 'match', 'nomatch', 'marker_no_ped', 'ped_no_marker']
    elif _fn == 13:
        _k += ['gang', 'not_gang', 'negative', 'below', 'above', 'huge', 'edge7', 'edge16', 'edge6', 'edge17']
    elif _fn == 14:
        _k += ['ptr_zero', 'ptr_nonzero', 'ptr_exe']
    elif _fn == 15:
        _k = [k for k in _k if k != 'this_unaligned'] + ['free0', 'free1', 'free2', 'free3', 'used0', 'used1', 'used2']
    elif _fn == 17:
        _k += ['prev_same', 'prev_other', 'write_zero', 'write_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[16]), Mem(a[2], a[17]), Mem(a[2], a[18])
    events, ret, gchanges, wch = [], None, [], None
    o = a[0]
    if fn in SDD:
        body = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, a[5])                       # el falso del cuerpo deja su dword en [this]
        events = [[body, tag(0, o), a[3]]]
        if a[4] & 1:                           # solo el bit 0 del BYTE bajo de las banderas manda
            events.append([CTASK_DELETE, tag(0, o), a[5]])
        ret = tag(0, o)
        marks.append('flag_delete' if a[4] & 1 else 'flag_keep')
        if a[4] > 0xFF:
            marks.append('flag_high_bits')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append('gret_zero' if a[6] == 0 else 'gret_other')
        marks.append('write_zero' if a[5] == 0 else 'write_other')
    elif fn == 0:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5]); A.wr(o + 0xC, 4, a[6])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x85A9E0)
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        A.wr(o + 0xC, 1, 1)
        ret = tag(0, o)
        marks.append('p0_same' if a[3] == 0x85A9E0 else 'p0_other')
        marks.append('p4_zero' if a[4] == 0 else 'p4_other')
        marks.append('pc_one' if (a[6] & 0xFF) == 1 else 'pc_other')
    elif fn == 1:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5]); A.wr(o + 0xC, 4, a[6])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x85A9E0)
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        marks.append('p0_same' if a[3] == 0x85A9E0 else 'p0_other')
        marks.append('p4_zero' if a[4] == 0 else 'p4_other')
        marks.append('p8_zero' if a[5] == 0 else 'p8_other')
    elif fn == 2:
        A.wr(o + 0xC, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o + 0xC, 1, a[4] & 0xFF)
        low = a[4] & 0xFF
        marks.append('arg_zero' if a[4] == 0 else 'arg_one' if a[4] == 1 else 'arg_low_zero_high_nonzero' if low == 0 else 'arg_other_byte')
        marks.append('same_byte' if (a[3] & 0xFF) == low else 'diff_byte')
    elif fn == 3:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x859988)
        events = [[SOUND_DTOR, tag(0, o + 8), 0x859988]]
        marks.append('prev_same' if a[3] == 0x859988 else 'prev_other')
    elif fn == 4:
        A.wr(o + 0x10, 4, a[3]); A.wr(o + 0xC, 4, a[4])
        A.snap(); B.snap(); C.snap()
        A.wr(o + 0x10, 4, 0)
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 5:
        A.snap(); B.snap(); C.snap()
        if a[3] == 0:
            ped = a[4]
            marks.append('ped_raw')
        else:
            ped = tag(2, a[5])
            marks.append('ped_arena')
        if a[6] == 0:
            task = a[7]
            marks.append('task_raw')
        else:
            task = tag(1, a[8])
            marks.append('task_arena')
        events = [[SET_TASK, tag(0, o), ped, task, tag(0, o + 0x1EC), 0xFFFFFFFF, 0]]
    elif fn == 6:
        A.snap(); B.snap(); C.snap()
        events = [[REPORT_ALL, tag(0, o), tag(0, o + 0xC)], [REPORT_ALL, tag(0, o), tag(0, o + 0x14C)], [REPORT_ALL, tag(0, o), tag(0, o + 0xAC)]]
    elif fn == 7:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[CAR_DRIVE_DTOR, tag(0, o), 0x86E96C]]
        A.wr(o, 4, a[4])
        marks.append('prev_same' if a[3] == 0x86E96C else 'prev_other')
        marks.append('write_zero' if a[4] == 0 else 'write_other')
    elif fn == 11:
        for i in range(8):
            v = a[8 + i] if (a[3] >> i) & 1 else 0
            A.wr(o + 4 + 4 * i, 4, v)
        A.snap(); B.snap(); C.snap()
        n = bin(a[3] & 0xFF).count('1')
        marks.append('mask_all' if n == 8 else 'mask_none' if n == 0 else 'mask_some')
        calls = 0
        mod = None
        for i in range(8):
            if A.rd(o + 4 + 4 * i, 4) != 0:
                events.append([REMOVE_MEMBER, tag(0, o), i])
                A.wr(o + 4 + 4 * i, 4, 0)
                if calls == a[4]:
                    s, v = a[5] & 7, a[6]
                    cur = A.rd(o + 4 + 4 * s, 4)
                    if s > i and v == 0 and cur != 0:
                        mod = 'mod_skips'
                    elif s > i and v != 0 and cur == 0:
                        mod = 'mod_adds'
                    else:
                        mod = 'mod_noeffect'
                    A.wr(o + 4 + 4 * s, 4, v)
                calls += 1
        marks.append(mod or 'mod_none')
    elif fn == 12:
        A.wr(o + 0x20, 4, a[3])
        for k in range(4):
            A.wr(o + 4 * k, 4, a[4 + k])
            A.wr(o + 0x10 + 4 * k, 4, a[8 + k])
        A.snap(); B.snap(); C.snap()
        if (a[3] & 0xFF) == 0:
            ret = 1
            marks.append('flag_high_only' if a[3] != 0 else 'flag_zero')
        else:
            marks.append('flag_nonzero')
            ret = 0
            for k in range(4):
                if a[4 + k] == 0xFFFFFFFE and a[8 + k] == a[12]:
                    ret = 1
            marks.append('match' if ret else 'nomatch')
            if any(a[4 + k] == 0xFFFFFFFE for k in range(4)) and not ret:
                marks.append('marker_no_ped')
            if any(a[8 + k] == a[12] and a[4 + k] != 0xFFFFFFFE for k in range(4)):
                marks.append('ped_no_marker')
    elif fn == 13:
        A.snap(); B.snap(); C.snap()
        v = a[3]
        sv = v - (1 << 32) if v & 0x80000000 else v
        ret = 1 if 7 <= sv <= 16 else 0
        marks.append('gang' if ret else 'not_gang')
        if sv < 0:
            marks.append('negative')
        if 0 <= sv < 7:
            marks.append('below')
        if sv > 16:
            marks.append('above')
        if sv > 40 or sv < 0:
            marks.append('huge')
        for e in (7, 16, 6, 17):
            if sv == e:
                marks.append('edge%d' % e)
    elif fn == 14:
        A.wr(o + 0x30, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[DELETE_ARRAY, a[3], a[3]]]       # [direccion, puntero (crudo), [this+0x30] al entrar: aun el puntero viejo]
        A.wr(o + 0x30, 4, 0)
        marks.append('ptr_zero' if a[3] == 0 else 'ptr_nonzero')
        if 0x400000 <= a[3] < 0xC00000:
            marks.append('ptr_exe')
    elif fn == 15:
        free, used, nb, dc, data = a[3], a[4], a[5], a[6], a[7]
        usedHead, usedTail, freeHead, freeTail = tag(0, o), tag(0, o + 0xC), tag(0, o + 0x18), tag(0, o + 0x24)
        node = [tag(1, nb + 0xC * j) for j in range(5)]
        uses = node[:used]
        frees = node[used:used + free]

        def put(addr, off, v):
            ar, base = untag(addr)
            [A, B, C][ar].wr(base + off, 4, v)

        def get(addr, off):
            ar, base = untag(addr)
            return [A, B, C][ar].rd(base + off, 4)
        for j in range(5):
            B.wr(nb + 0xC * j, 4, a[8 + j])                            # data de cada enlace
        # lista usada: cabeza.next = primer usado, cola.prev = ultimo usado
        put(usedHead, 8, uses[0] if uses else usedTail)
        put(usedTail, 4, uses[-1] if uses else usedHead)
        for j, nd in enumerate(uses):
            put(nd, 4, uses[j - 1] if j else usedHead)
            put(nd, 8, uses[j + 1] if j + 1 < len(uses) else usedTail)
        put(freeHead, 8, frees[0] if frees else freeTail)
        put(freeTail, 4, frees[-1] if frees else freeHead)
        for j, nd in enumerate(frees):
            put(nd, 4, frees[j - 1] if j else freeHead)
            put(nd, 8, frees[j + 1] if j + 1 < len(frees) else freeTail)
        C.wr(dc, 4, data)
        A.snap(); B.snap(); C.snap()
        marks += ['free%d' % free, 'used%d' % used]
        link = get(freeHead, 8)
        if link == freeTail:
            ret = 0
        else:
            put(link, 0, data)
            nxt, prv = get(link, 8), get(link, 4)
            put(nxt, 4, prv)                      # next.prev = prev
            put(prv, 8, nxt)                      # prev.next = next
            head = get(usedHead, 8)
            put(link, 8, head)                    # link.next = usedHead.next
            put(head, 4, link)                    # (usedHead.next).prev = link
            put(link, 4, usedHead)                # link.prev = usedHead
            put(usedHead, 8, link)                # usedHead.next = link
            ret = link
        wch = {'A': A.wchanges(), 'B': B.wchanges(), 'C': C.wchanges()}
    elif fn == 16:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = 0xF0
    elif fn == 17:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[COMPLEX_DTOR, tag(0, o), 0x8704AC]]
        A.wr(o, 4, a[4])
        marks.append('prev_same' if a[3] == 0x8704AC else 'prev_other')
        marks.append('write_zero' if a[4] == 0 else 'write_other')
    else:
        raise AssertionError(fn)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'wchanges': wch, 'gchanges': gchanges, 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if c['fn'] == 15:
        if o.get('wchanges') != e['wchanges']:
            return 'wchanges: %s != %s' % (str(o.get('wchanges'))[:300], str(e['wchanges'])[:300])
    elif o.get('changes') != e['changes']:
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
