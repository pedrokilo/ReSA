"""Oraculo independiente (Python) de Fn142A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 142A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x658830  CTaskComplexWanderMedic (vtable 0x86F48C, ranura 0)   destructor con borrado, 30 B: push esi; mov esi,ecx; call 658850; test byte [esp+8],1; je ..; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x658850  cuerpo de ~CTaskComplexWanderMedic                    mov dword [ecx],86F48Ch; jmp 669D20 (cuerpo de ~CTaskComplexWander)
   2  0x48E5E0  CTaskComplexWanderStandard (vtable 0x85A200, ranura 0) idem; cuerpo 48E600, delete 61A5B0
   3  0x48E600  cuerpo de ~CTaskComplexWanderStandard                 mov dword [ecx],85A200h; jmp 669D20
   4  0x4636C0  CTaskSimpleFinishBrain (vtable 0x859D20, ranura 0)    idem; cuerpo 4636E0, delete 61A5B0
   5  0x4636E0  cuerpo de ~CTaskSimpleFinishBrain                     mov dword [ecx],859CFCh; jmp 61A3A0 (cuerpo de ~CTask)
   6  0x492D40  cuerpo de ~CTaskSimpleTogglePedThreatScanner          mov dword [ecx],85A334h; jmp 61A3A0
   7  0x492DF0  CTaskSimpleTogglePedThreatScanner (vtable 0x85A334, ranura 0)  idem; cuerpo 492D40, delete 61A5B0
   8  0x4C5680  cuerpo de ~CTimeModelInfo                             mov dword [ecx],85BB9Ch; ret
   9  0x4C5690  constructor de CLodTimeModelInfo (vtable 0x85BCF0)    push esi; mov esi,ecx; call 4C4A60 (word [ecx+8]=0, word [ecx+0Ah]=FFFFh); word [esi+20h]=0; word [esi+22h]=0; mov [esi],85BCF0h; word [esi+26h]=FFFFh; mov eax,esi; pop esi; ret
  10  0x5A40E0  cuerpo de ~CProjectile                                mov dword [ecx],867030h; jmp 59F660 (cuerpo de ~CObject)
  11  0x5A40F0  CProjectile (vtable 0x867030, ranura 0)               idem; cuerpo 5A40E0, delete 5A1F20 (CObject::operator delete)
  12  0x4F1020  CAEStaticChannel (vtable 0x85F3CC, ranura 0)          27 B: push esi; mov esi,ecx; call 4F0B50; test byte [esp+8],1; je ..; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
  13  0x4F11D0  constructor de CAEStreamThread                        mov eax,ecx; ret
  14  0x4F11E0  destructor de CAEStreamThread                         add ecx,38h; push ecx; call [8580E0h] (DeleteCriticalSection); ret
  15  0x697EF0  CTaskComplexWaitForBackup (vtable 0x870DA4, ranura 0) idem; cuerpo 697330, delete 61A5B0
  16  0x6986E0  constructor de CPointList                             push esi; mov esi,ecx; call 699F00 (dword [ecx]=0; 6 dwords a cero en +124h); mov eax,esi; pop esi; ret
  17  0x5F6880  CTaskSimpleWaitUntilLeaderAreaCodesMatch (vtable 0x86C7E8, ranura 0)  idem; cuerpo 5F67E0, delete 61A5B0
  18  0x5F68A0  constructor de CPedList                               push esi; mov esi,ecx; call 699DB0 (CPedList::Empty); mov eax,esi; pop esi; ret
  19  0x4B2670  CEventDanger (vtable 0x85B848, ranura 1, GetEventType)     mov eax,4Bh; ret
  20  0x4B2680  CEventDanger (ranura 3, GetLifeTime)                       xor eax,eax; ret
  21  0x4B2690  CEventDanger (ranura 2, GetEventPriority)                  mov eax,14h; ret
  22  0x4B27B0  CEventVehicleHitAndRun (vtable 0x85AFA8, ranura 7, IsCriminalEvent)   mov eax,[ecx+10h]; mov ecx,[eax+460h]; test ecx,ecx; je ..; call 5DF8F0 (CPed::IsPlayer); test al,al; je ..; mov al,1; ret; xor al,al; ret

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskComplexWanderMedic::ScalarDeletingDestructor', 'CTaskComplexWanderMedic::Destructor', 'CTaskComplexWanderStandard::ScalarDeletingDestructor', 'CTaskComplexWanderStandard::Destructor',
         'CTaskSimpleFinishBrain::ScalarDeletingDestructor', 'CTaskSimpleFinishBrain::Destructor', 'CTaskSimpleTogglePedThreatScanner::Destructor', 'CTaskSimpleTogglePedThreatScanner::ScalarDeletingDestructor',
         'CTimeModelInfo::Destructor', 'CLodTimeModelInfo::Constructor', 'CProjectile::Destructor', 'CProjectile::ScalarDeletingDestructor', 'CAEStaticChannel::ScalarDeletingDestructor',
         'CAEStreamThread::Constructor', 'CAEStreamThread::Destructor', 'CTaskComplexWaitForBackup::ScalarDeletingDestructor', 'CPointList::Constructor',
         'CTaskSimpleWaitUntilLeaderAreaCodesMatch::ScalarDeletingDestructor', 'CPedList::Constructor', 'CEventDanger::GetEventType', 'CEventDanger::GetLifeTime', 'CEventDanger::GetEventPriority',
         'CEventVehicleHitAndRun::IsCriminalEvent']
ENTRIES = (0x658830, 0x658850, 0x48E5E0, 0x48E600, 0x4636C0, 0x4636E0, 0x492D40, 0x492DF0, 0x4C5680, 0x4C5690, 0x5A40E0, 0x5A40F0, 0x4F1020, 0x4F11D0, 0x4F11E0,
           0x697EF0, 0x6986E0, 0x5F6880, 0x5F68A0, 0x4B2670, 0x4B2680, 0x4B2690, 0x4B27B0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


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


# destructores con borrado: indice -> (cuerpo con el que se destruye, operator delete, vtable propia de la clase)
SDD = {0: (0x658850, 0x61A5B0, 0x86F48C), 2: (0x48E600, 0x61A5B0, 0x85A200), 4: (0x4636E0, 0x61A5B0, 0x859D20), 7: (0x492D40, 0x61A5B0, 0x85A334), 11: (0x5A40E0, 0x5A1F20, 0x867030),
       12: (0x4F0B50, 0x8214BD, 0x85F3CC), 15: (0x697330, 0x61A5B0, 0x870DA4), 17: (0x5F67E0, 0x61A5B0, 0x86C7E8)}
# cuerpos de destructor con un colaborador: indice -> (vtable que escribe, colaborador al que salta)
BODY = {1: (0x86F48C, 0x669D20), 3: (0x85A200, 0x669D20), 5: (0x859CFC, 0x61A3A0), 6: (0x85A334, 0x61A3A0), 10: (0x867030, 0x59F660)}
TIME_VTABLE = 0x85BB9C            # 8: `mov [ecx],85BB9Ch; ret`
LOD_VTABLE = 0x85BCF0             # 9
CONSTANT = {19: 0x4B, 20: 0, 21: 0x14}
SPAN = {9: 0x28, 14: 0x3C, 16: 0x13C, 22: 0x14}
PREV_OWN = {0: 0x86F48C, 2: 0x85A200, 4: 0x859D20, 7: 0x85A334, 11: 0x867030, 12: 0x85F3CC, 15: 0x870DA4, 17: 0x86C7E8, 1: 0x86F48C, 3: 0x85A200, 5: 0x859CFC, 6: 0x85A334, 10: 0x867030}


def ptr_mark(prefix, v):
    return prefix + ('_zero' if v == 0 else '_ffff' if v == M32 else '_exe' if 0x400000 <= v < 0xC00000 else '_other')


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
for _fn in range(23):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_delete', 'flag_keep', 'flag_high_bits', 'prev_same', 'prev_other', 'gret_zero', 'gret_other', 'write_zero', 'write_other']
    elif _fn in BODY:
        _k += ['prev_same', 'prev_other', 'gret_zero', 'gret_other']
    elif _fn == 8:
        _k += ['prev_same', 'prev_other']
    elif _fn == 9:
        _k += ['prev0_same', 'prev0_other', 'prev8_zero', 'prev8_ffff0000', 'prev8_other', 'prev20_zero', 'prev20_other', 'prev24_ffff', 'prev24_other']
    elif _fn == 14:
        _k += ['cs_zero', 'cs_ffff', 'cs_exe', 'cs_other']
    elif _fn == 16:
        _k += ['prev0_zero', 'prev0_other', 'prev124_zero', 'prev124_other']
    elif _fn == 18:
        _k += ['prev0_zero', 'prev0_other']
    elif _fn == 22:
        _k += ['driver_null', 'driver_arena', 'driver_raw', 'player_true', 'player_false', 'veh_spill']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret = [], None
    o = a[0]
    if fn in SDD:
        body, delete, own = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, a[5])                       # el falso del cuerpo deja su dword en [this]
        events = [[body, tag(0, o), a[3]]]
        if a[4] & 1:                           # solo el bit 0 del BYTE bajo de las banderas manda
            events.append([delete, tag(0, o), a[5]])
        ret = tag(0, o)
        marks.append('flag_delete' if a[4] & 1 else 'flag_keep')
        if a[4] > 0xFF:
            marks.append('flag_high_bits')
        marks.append('prev_same' if a[3] == own else 'prev_other')
        marks.append('gret_zero' if a[6] == 0 else 'gret_other')
        marks.append('write_zero' if a[5] == 0 else 'write_other')
    elif fn in BODY:
        vtable, collab = BODY[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, vtable)
        events = [[collab, tag(0, o), vtable]]
        marks.append('prev_same' if a[3] == vtable else 'prev_other')
        marks.append('gret_zero' if a[4] == 0 else 'gret_other')
    elif fn == 8:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, TIME_VTABLE)
        marks.append('prev_same' if a[3] == TIME_VTABLE else 'prev_other')
    elif fn == 9:
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, a[4]); A.wr(o + 0x20, 4, a[5]); A.wr(o + 0x24, 4, a[6])
        A.snap(); B.snap(); C.snap()
        A.wr(o + 8, 2, 0)                      # CBaseModelInfo (0x4C4A60): m_nRefCount = 0, m_nTxdIndex = -1 (word en +0xA)
        A.wr(o + 0xA, 2, 0xFFFF)
        A.wr(o + 0x20, 2, 0)                   # m_numChildren, m_numChildrenRendered
        A.wr(o + 0x22, 2, 0)
        A.wr(o, 4, LOD_VTABLE)
        A.wr(o + 0x26, 2, 0xFFFF)              # m_lodTimeInfo.m_nOtherTimeModel
        ret = tag(0, o)
        marks.append('prev0_same' if a[3] == LOD_VTABLE else 'prev0_other')
        marks.append('prev8_zero' if a[4] == 0 else 'prev8_ffff0000' if a[4] == 0xFFFF0000 else 'prev8_other')
        marks.append('prev20_zero' if a[5] == 0 else 'prev20_other')
        marks.append('prev24_ffff' if (a[6] >> 16) == 0xFFFF else 'prev24_other')
    elif fn == 13:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o)
    elif fn == 14:
        A.wr(o + 0x38, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[0x8580E0, tag(0, o + 0x38), a[3]]]
        marks.append(ptr_mark('cs', a[3]))
    elif fn == 16:
        A.wr(o, 4, a[3]); A.wr(o + 0x124, 4, a[4])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0)
        for k in range(6):
            A.wr(o + 0x124 + 4 * k, 4, 0)
        ret = tag(0, o)
        marks.append('prev0_zero' if a[3] == 0 else 'prev0_other')
        marks.append('prev124_zero' if a[4] == 0 else 'prev124_other')
    elif fn == 18:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events = [[0x699DB0, tag(0, o), a[3]]]
        ret = tag(0, o)
        marks.append('prev0_zero' if a[3] == 0 else 'prev0_other')
    elif fn in CONSTANT:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif fn == 22:
        A.snap(); B.snap(); C.snap()
        if a[4] == 0:
            driver_tag = 0
            marks.append('driver_null')
        elif a[4] == 1:
            driver_tag = tag(2, a[5])
            marks.append('driver_arena')
        else:
            driver_tag = a[6]
            marks.append('driver_raw')
        if driver_tag:
            events = [[0x5DF8F0, driver_tag]]
        player = driver_tag != 0 and (a[7] & 0xFF) != 0
        ret = 1 if player else 0
        marks.append('player_true' if player else 'player_false')
        if a[3] + 0x464 > WIN:
            marks.append('veh_spill')
    else:
        raise AssertionError(fn)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
