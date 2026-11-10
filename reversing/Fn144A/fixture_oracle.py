"""Oraculo independiente (Python) de Fn144A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 144A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de la region global
(Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x69FFA0  cuerpo de destructor de CKeyArray (listado: CText)   push esi; mov esi,ecx; mov eax,[esi]; test eax,eax; je ..; push eax; call 8213AE (operator delete[]); add esp,4; mov [esi],0; mov [esi+4],0; pop esi; ret
   1  0x69FFD0  cuerpo de destructor de CData                        idem (byte a byte)
   2  0x4D8670  CAEBankLoader (destructor con borrado, 27 B)         push esi; mov esi,ecx; call 4DFB20; test byte [esp+8],1; je ..; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   3  0x4D8690  CAEMP3TrackLoader (destructor con borrado)           idem; cuerpo 4E0940
   4  0x47C0E0  CCarGenerator::SwitchOnWithCount                     push esi; mov esi,ecx; call 6F32C0; mov ax,[esp+8]; mov [esi+1Ah],ax; pop esi; ret 4
   5  0x47E050  CColPoint::GetSurfaceTypeB                           mov al,[ecx+23h]; ret
   6  0x47E070  CCutsceneMgr::SetCutsceneOffset                      mov eax,[esp+4]; mov ecx,[eax]; mov [0BC4034h],ecx; mov edx,[eax+4]; mov [0BC4038h],edx; mov eax,[eax+8]; mov [0BC403Ch],eax; ret
   7  0x4A9110  Particle_c::Constructor                              push esi; mov esi,ecx; call 4A8DB0 ([ecx] = [ecx+4] = 0); mov eax,esi; pop esi; ret
   8  0x4A9340  FxSystem_c (destructor con borrado)                  idem destructor con borrado; cuerpo 4AA260
   9  0x4A94E0  FxEmitterPrt_c::Constructor                          idem 7
  10  0x4A9C10  FxMemoryPool_c::Constructor                          mov eax,ecx; ret
  11  0x4A9C20  FxMemoryPool_c::Destructor                           ret
  12  0x4B21F0  CEventAreaCodes::GetEventType (vtable 0x85B7C0, ranura 1)       mov eax,43h; ret
  13  0x4B2200  CEventAreaCodes::GetLifeTime (ranura 3)                         xor eax,eax; ret
  14  0x4B2390  CEventLeaderEntryExit::TakesPriorityOver (vtable 0x859540, ranura 11)   mov al,1; ret 4
  15  0x4B23A0  CEventLeaderEntryExit::AffectsPedGroup (ranura 6)      push esi; mov esi,ecx; mov eax,[esi+0Ch]; test eax,eax; je ..; mov ecx,[esp+8]; add ecx,8; call 5F69A0; cmp eax,[esi+0Ch]; jne ..; mov eax,1; pop esi; ret 4; xor eax,eax; pop esi; ret 4
  16  0x4E3350  CAEPedSpeechSlot::Reset                              xor eax,eax; or edx,-1; word [ecx]=0; [ecx+4]=0; [ecx+8]=0; word [ecx+0Ch]=-1; word [ecx+0Eh]=-1; [ecx+10h]=0; word [ecx+14h]=-1; word [ecx+16h]=-1; byte [ecx+19h]=0; byte [ecx+1Ah]=0; byte [ecx+18h]=0; ret
  17  0x4E34C0  CAEPedSpeechAudioEntity::Destructor (cuerpo)        mov [ecx],859988h; add ecx,8; jmp 4EF660
  18  0x630F50  CTaskSimpleTired::GetTaskType (vtable 0x86DED0, ranura 4)       mov eax,0DBh; ret
  19  0x630FF0  CTaskSimpleTired::MakeAbortable (ranura 6)           mov eax,[ecx+18h]; test eax,eax; push esi; je ..; mov esi,eax; mov eax,[esp+8]; mov ecx,[eax+18h]; push 0Ah; push ecx; call 4D68B0; add esp,8; cmp esi,eax; mov al,1; jne ..; mov dword [esi+1Ch],0C0800000h; pop esi; ret 0Ch
  20  0x5BA020  CDecisionMaker::From                                 41 veces (0x29): ecx = destino + i*3Ch; push origen + i*3Ch; call 6006B0; ret 4
  21  0x6E3E30  WaterCreature_c::Constructor                         push esi; mov esi,ecx; call 4A8DB0; mov [esi+8],0; mov eax,esi; pop esi; ret
  22  0x506DC0  CAEGlobalWeaponAudioEntity (destructor con borrado)  idem destructor con borrado; cuerpo 506C50
  23  0x507400  CAudioEngine::StopPoliceScanner                      mov eax,[esp+4]; push eax; call 4E6DC0; pop ecx; ret 4
  24  0x5074B0  CAEExplosionAudioEntity::Constructor                 mov eax,ecx; xor ecx,ecx; [eax+10h]=ecx; [eax]=862E60h; [eax+7Ch]=cl; ret

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CKeyArray::Destructor', 'CData::Destructor', 'CAEBankLoader::ScalarDeletingDestructor', 'CAEMP3TrackLoader::ScalarDeletingDestructor', 'CCarGenerator::SwitchOnWithCount',
         'CColPoint::GetSurfaceTypeB', 'CCutsceneMgr::SetCutsceneOffset', 'Particle_c::Constructor', 'FxSystem_c::ScalarDeletingDestructor', 'FxEmitterPrt_c::Constructor',
         'FxMemoryPool_c::Constructor', 'FxMemoryPool_c::Destructor', 'CEventAreaCodes::GetEventType', 'CEventAreaCodes::GetLifeTime', 'CEventLeaderEntryExit::TakesPriorityOver',
         'CEventLeaderEntryExit::AffectsPedGroup', 'CAEPedSpeechSlot::Reset', 'CAEPedSpeechAudioEntity::Destructor', 'CTaskSimpleTired::GetTaskType', 'CTaskSimpleTired::MakeAbortable',
         'CDecisionMaker::From', 'WaterCreature_c::Constructor', 'CAEGlobalWeaponAudioEntity::ScalarDeletingDestructor', 'CAudioEngine::StopPoliceScanner', 'CAEExplosionAudioEntity::Constructor']
ENTRIES = (0x69FFA0, 0x69FFD0, 0x4D8670, 0x4D8690, 0x47C0E0, 0x47E050, 0x47E070, 0x4A9110, 0x4A9340, 0x4A94E0, 0x4A9C10, 0x4A9C20, 0x4B21F0, 0x4B2200, 0x4B2390, 0x4B23A0,
           0x4E3350, 0x4E34C0, 0x630F50, 0x630FF0, 0x5BA020, 0x6E3E30, 0x506DC0, 0x507400, 0x5074B0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
REGION_AT = 0xBC4020
REGION_BYTES = 0x30
OFFSET_AT = 0xBC4034
DELETE_ARRAY = 0x8213AE
DELETE = 0x8214BD


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


# destructores con borrado: indice -> cuerpo con el que se destruye (el operator delete es siempre 0x8214BD)
SDD = {2: 0x4DFB20, 3: 0x4E0940, 8: 0x4AA260, 22: 0x506C50}
CONSTANT = {12: 0x43, 13: 0, 18: 0xDB}
SPAN = {0: 8, 1: 8, 4: 0x1C, 5: 0x24, 6: 0xC, 7: 8, 9: 8, 10: 0xC, 11: 0xC, 15: 0x10, 16: 0x1C, 17: 0x10, 19: 0x1C, 20: 0x99C, 21: 0xC, 24: 0x80}


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
for _fn in range(25):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_delete', 'flag_keep', 'flag_high_bits', 'prev_zero', 'prev_other', 'gret_zero', 'gret_other', 'write_zero', 'write_other']
    elif _fn in (0, 1):
        _k += ['ptr_zero', 'ptr_nonzero', 'ptr_exe', 'size_zero', 'size_other']
    elif _fn == 4:
        _k += ['cnt_zero', 'cnt_le100', 'cnt_gt100', 'cnt_ffff', 'cnt_high_bits', 'stamp_ffff', 'stamp_other']
    elif _fn == 5:
        _k += ['surf_zero', 'surf_ff', 'surf_other']
    elif _fn == 6:
        _k += ['src_arena', 'src_self', 'src_p4', 'src_m4', 'src_p8', 'src_m8', 'src_p12', 'src_m12', 'vec_nan', 'vec_zero']
    elif _fn in (7, 9, 21):
        _k += ['prev0_zero', 'prev0_other', 'prev4_zero', 'prev4_other']
    elif _fn in (10, 11):
        _k += ['prev0_zero', 'prev0_other']
    elif _fn == 15:
        _k += ['ped_null', 'ped_raw', 'ped_arena', 'leader_same', 'leader_other', 'leader_null', 'group_arena', 'group_raw']
    elif _fn == 16:
        _k += ['prev14_ffff', 'prev14_other', 'prev18_zero', 'prev18_other']
    elif _fn == 17:
        _k += ['prev_same', 'prev_other']
    elif _fn == 19:
        _k += ['anim_null', 'anim_arena', 'anim_raw', 'assoc_same', 'assoc_other', 'assoc_null']
    elif _fn == 20:
        _k += ['src_arena', 'src_raw']
    elif _fn == 23:
        _k += ['arg_zero', 'arg_nonzero', 'arg_zero_low_byte_nonzero_high']
    elif _fn == 24:
        _k += ['prev0_same', 'prev0_other', 'prev10_zero', 'prev10_other', 'prev7c_zero', 'prev7c_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret, gchanges = [], None, []
    o = a[0]
    if fn in SDD:
        body = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, a[5])                       # el falso del cuerpo deja su dword en [this]
        events = [[body, tag(0, o), a[3]]]
        if a[4] & 1:                           # solo el bit 0 del BYTE bajo de las banderas manda
            events.append([DELETE, tag(0, o), a[5]])
        ret = tag(0, o)
        marks.append('flag_delete' if a[4] & 1 else 'flag_keep')
        if a[4] > 0xFF:
            marks.append('flag_high_bits')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append('gret_zero' if a[6] == 0 else 'gret_other')
        marks.append('write_zero' if a[5] == 0 else 'write_other')
    elif fn in (0, 1):
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4])
        A.snap(); B.snap(); C.snap()
        if a[3] != 0:
            events = [[DELETE_ARRAY, a[3]]]
            A.wr(o, 4, 0)
        A.wr(o + 4, 4, 0)
        marks.append('ptr_zero' if a[3] == 0 else 'ptr_nonzero')
        if 0x400000 <= a[3] < 0xC00000:
            marks.append('ptr_exe')
        marks.append('size_zero' if a[4] == 0 else 'size_other')
    elif fn == 4:
        A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x18, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[0x6F32C0, tag(0, o), a[3]]]
        A.wr(o + 0x1A, 2, a[5] & 0xFFFF)       # el falso de SwitchOn deja su palabra
        A.wr(o + 0x1A, 2, a[4] & 0xFFFF)       # despues el contador (palabra baja del argumento)
        cnt = a[4] & 0xFFFF
        marks.append('cnt_zero' if cnt == 0 else 'cnt_le100' if cnt <= 100 else 'cnt_gt100')
        if cnt == 0xFFFF:
            marks.append('cnt_ffff')
        if a[4] > 0xFFFF:
            marks.append('cnt_high_bits')
        marks.append('stamp_ffff' if (a[5] & 0xFFFF) == 0xFFFF else 'stamp_other')
    elif fn == 5:
        A.wr(o + 0x20, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = (a[3] >> 24) & 0xFF
        marks.append('surf_zero' if ret == 0 else 'surf_ff' if ret == 0xFF else 'surf_other')
    elif fn == 6:
        reg = {}
        for k in range(REGION_BYTES // 4):
            w = rnd(a[15], k)
            for b in range(4):
                reg[4 * k + b] = (w >> (8 * b)) & 0xFF
        ref = dict(reg)
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, a[5]); A.wr(o + 8, 4, a[6])
        A.snap(); B.snap(); C.snap()
        delta = {0: None, 1: 0, 2: 4, 3: -4, 4: 8, 5: -8, 6: 12, 7: -12}[a[3]]
        names = {0: 'src_arena', 1: 'src_self', 2: 'src_p4', 3: 'src_m4', 4: 'src_p8', 5: 'src_m8', 6: 'src_p12', 7: 'src_m12'}
        marks.append(names[a[3]])
        dst = OFFSET_AT - REGION_AT
        for k in range(3):
            if delta is None:
                v = A.rd(o + 4 * k, 4)
            else:
                at = dst + delta + 4 * k
                v = sum(reg[at + b] << (8 * b) for b in range(4))
            for b in range(4):
                reg[dst + 4 * k + b] = (v >> (8 * b)) & 0xFF
        gchanges = [[i, reg[i]] for i in sorted(reg) if reg[i] != ref[i]]
        vec = a[4:7]
        if any((w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) for w in vec):
            marks.append('vec_nan')
        if all((w & 0x7FFFFFFF) == 0 for w in vec):
            marks.append('vec_zero')
    elif fn in (7, 9, 21):
        n = 3 if fn == 21 else 2
        for k in range(n):
            A.wr(o + 4 * k, 4, a[3 + k])
        A.snap(); B.snap(); C.snap()
        for k in range(n):
            A.wr(o + 4 * k, 4, 0)
        ret = tag(0, o)
        marks.append('prev0_zero' if a[3] == 0 else 'prev0_other')
        marks.append('prev4_zero' if a[4] == 0 else 'prev4_other')
    elif fn in (10, 11):
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5])
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o) if fn == 10 else None
        marks.append('prev0_zero' if a[3] == 0 else 'prev0_other')
    elif fn in CONSTANT:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif fn == 14:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = 1
    elif fn == 15:
        A.snap(); B.snap(); C.snap()
        if a[3] == 0:
            ped = 0
            marks.append('ped_null')
        elif a[3] == 1:
            ped = a[4]
            marks.append('ped_raw')
        else:
            ped = tag(2, a[5])
            marks.append('ped_arena')
        if a[6] == 0:
            group_ecx = tag(1, a[7] + 8)
            marks.append('group_arena')
        else:
            group_ecx = (a[8] + 8) & M32
            marks.append('group_raw')
        leader = {0: ped, 1: tag(2, a[10]), 2: 0, 3: a[11]}[a[9]]
        if ped == 0:
            ret = 0
        else:
            events = [[0x5F69A0, group_ecx]]
            ret = 1 if leader == ped else 0
            marks.append('leader_same' if leader == ped else 'leader_other')
            if leader == 0:
                marks.append('leader_null')
    elif fn == 16:
        A.wr(o + 0x14, 4, a[3]); A.wr(o + 0x18, 4, a[4])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 2, 0)
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        A.wr(o + 0xC, 2, 0xFFFF)
        A.wr(o + 0xE, 2, 0xFFFF)
        A.wr(o + 0x10, 4, 0)
        A.wr(o + 0x14, 2, 0xFFFF)
        A.wr(o + 0x16, 2, 0xFFFF)
        A.wr(o + 0x19, 1, 0)
        A.wr(o + 0x1A, 1, 0)
        A.wr(o + 0x18, 1, 0)
        marks.append('prev14_ffff' if a[3] == 0xFFFFFFFF else 'prev14_other')
        marks.append('prev18_zero' if a[4] == 0 else 'prev18_other')
    elif fn == 17:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x859988)
        events = [[0x4EF660, tag(0, o + 8), 0x859988]]
        marks.append('prev_same' if a[3] == 0x859988 else 'prev_other')
    elif fn == 19:
        A.snap(); B.snap(); C.snap()
        if a[3] == 0:
            anim = 0
            marks.append('anim_null')
        elif a[3] == 1:
            anim = tag(2, a[4])
            marks.append('anim_arena')
        else:
            anim = a[5]
            marks.append('anim_raw')
        assoc = {0: anim if a[3] == 1 else 0, 1: tag(2, a[10]), 2: 0, 3: a[9]}[a[8]]
        ret = 1
        if anim != 0:
            events = [[0x4D68B0, a[7], 10]]
            if assoc == anim:
                C.wr(a[4] + 0x1C, 4, 0xC0800000)
                marks.append('assoc_same')
            else:
                marks.append('assoc_other')
            if assoc == 0:
                marks.append('assoc_null')
    elif fn == 20:
        A.snap(); B.snap(); C.snap()
        for i in range(41):
            if a[3] == 0:
                src = tag(1, a[4] + 0x3C * i)
            else:
                src = (a[5] + 0x3C * i) & M32
            events.append([0x6006B0, tag(0, o + 0x3C * i), src])
        marks.append('src_arena' if a[3] == 0 else 'src_raw')
    elif fn == 23:
        A.snap(); B.snap(); C.snap()
        events = [[0x4E6DC0, 1 if (a[3] & 0xFF) else 0]]
        marks.append('arg_zero' if a[3] == 0 else 'arg_nonzero')
        if (a[3] & 0xFF) == 0 and a[3] != 0:
            marks.append('arg_zero_low_byte_nonzero_high')
    elif fn == 24:
        A.wr(o, 4, a[3]); A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x7C, 4, a[5])
        A.snap(); B.snap(); C.snap()
        A.wr(o + 0x10, 4, 0)
        A.wr(o, 4, 0x862E60)
        A.wr(o + 0x7C, 1, 0)
        ret = tag(0, o)
        marks.append('prev0_same' if a[3] == 0x862E60 else 'prev0_other')
        marks.append('prev10_zero' if a[4] == 0 else 'prev10_other')
        marks.append('prev7c_zero' if (a[5] & 0xFF) == 0 else 'prev7c_other')
    else:
        raise AssertionError(fn)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gchanges, 'ret': ret, 'marks': marks}


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
