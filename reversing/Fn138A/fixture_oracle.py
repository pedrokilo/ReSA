"""Oraculo independiente (Python) de Fn138A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 138A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4B17C0  CEventCopCarBeingStolen (vtable 0x85B548, ranura 1, GetEventType)   mov eax,10h; ret
   1  0x4B17D0  CEventCopCarBeingStolen (ranura 3, GetLifeTime)                      xor eax,eax; ret
   2  0x4B17E0  CEventCopCarBeingStolen (ranura 2, GetEventPriority)                 mov eax,26h; ret
   3  0x4B1D20  CEventCarUpsideDown (vtable 0x85B660, ranura 1)                      mov eax,2Dh; ret
   4  0x4B1D30  CEventCarUpsideDown (ranura 3)                                       xor eax,eax; ret
   5  0x4B1D40  CEventCarUpsideDown (ranura 2)                                       mov eax,12h; ret
   6  0x4B20F0  CEventSeenPanickedPed (vtable 0x85B778, ranura 1)                    mov eax,41h; ret
   7  0x4B2100  CEventSeenPanickedPed (ranura 3)                                     xor eax,eax; ret
   8  0x4B2110  CEventSeenPanickedPed (ranura 10, GetSourceEntity)                   mov eax,[ecx+14h]; ret
   9  0x4B2120  CEventSeenPanickedPed (ranura 2)                                     mov eax,0Dh; ret
  10  0x4B2440  CEventPlayerCommandToGroup (vtable 0x85B800, ranura 3)               xor eax,eax; ret
  11  0x4B2450  CEventPlayerCommandToGroup (ranura 5, AffectsPed)                    xor al,al; ret 4
  12  0x4B2460  CEventPlayerCommandToGroup (ranura 10, GetSourceEntity)              mov eax,[ecx+18h]; ret
  13  0x4B2520  CEventPlayerCommandToGroup (ranura 11, TakesPriorityOver)            mov al,1; ret 4
  14  0x634F20  cuerpo de ~CTaskComplexUseGoggles                                    mov dword [ecx],86E4A8h; jmp 0x61A3D0
  15  0x634F30  CTaskComplexUseGoggles (vtable 0x86E4A8, ranura 6, MakeAbortable)    mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]   (los 3 argumentos de pila pasan tal cual; `ret 0Ch` del destino)
  16  0x678150  CTaskComplexGetUpAndStandStill (vtable 0x870404, ranura 4)           mov eax,0CEh; ret
  17  0x678160  cuerpo de ~CTaskComplexGetUpAndStandStill                            mov dword [ecx],870404h; jmp 0x61A3D0
  18  0x6790A0  CTaskComplexFallToDeath (vtable 0x870520, ranura 4)                  mov eax,115h; ret
  19  0x6790B0  cuerpo de ~CTaskComplexFallToDeath                                   mov dword [ecx],870520h; jmp 0x61A3D0
  20  0x72F4A0  CMemoryMgr::PushMemId  (sin argumentos en los llamadores)            ret
  21  0x72F4B0  CMemoryMgr::PopMemId                                                 ret
  22  0x72F500  CMemoryMgr::MoveMemory (cdecl, un puntero)                           mov eax,[esp+4]; ret

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventCopCarBeingStolen::GetEventType', 'CEventCopCarBeingStolen::GetLifeTime', 'CEventCopCarBeingStolen::GetEventPriority', 'CEventCarUpsideDown::GetEventType',
         'CEventCarUpsideDown::GetLifeTime', 'CEventCarUpsideDown::GetEventPriority', 'CEventSeenPanickedPed::GetEventType', 'CEventSeenPanickedPed::GetLifeTime',
         'CEventSeenPanickedPed::GetSourceEntity', 'CEventSeenPanickedPed::GetEventPriority', 'CEventPlayerCommandToGroup::GetLifeTime', 'CEventPlayerCommandToGroup::AffectsPed',
         'CEventPlayerCommandToGroup::GetSourceEntity', 'CEventPlayerCommandToGroup::TakesPriorityOver', 'CTaskComplexUseGoggles::Destructor', 'CTaskComplexUseGoggles::MakeAbortable',
         'CTaskComplexGetUpAndStandStill::GetTaskType', 'CTaskComplexGetUpAndStandStill::Destructor', 'CTaskComplexFallToDeath::GetTaskType', 'CTaskComplexFallToDeath::Destructor',
         'CMemoryMgr::PushMemId', 'CMemoryMgr::PopMemId', 'CMemoryMgr::MoveMemory']
ENTRIES = (0x4B17C0, 0x4B17D0, 0x4B17E0, 0x4B1D20, 0x4B1D30, 0x4B1D40, 0x4B20F0, 0x4B2100, 0x4B2110, 0x4B2120, 0x4B2440, 0x4B2450, 0x4B2460, 0x4B2520, 0x634F20, 0x634F30,
           0x678150, 0x678160, 0x6790A0, 0x6790B0, 0x72F4A0, 0x72F4B0, 0x72F500)
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


def fill_kind(nbytes, kind, seed):
    if kind == 0:
        return fill_words(nbytes, seed)
    return bytearray([0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)]) * nbytes


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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


# funciones que devuelven una constante en EAX (valor leido del desensamblado)
CONSTANT = {0: 0x10, 1: 0, 2: 0x26, 3: 0x2D, 4: 0, 5: 0x12, 6: 0x41, 7: 0, 9: 0x0D, 10: 0, 16: 0xCE, 18: 0x115}
# cuerpos de destructor: vtable que dejan en [ecx] antes de saltar a 0x61A3D0
DTOR_VTABLE = {14: 0x86E4A8, 17: 0x870404, 19: 0x870520}
SPAN = {8: 0x18, 12: 0x1C, 15: 0x0C}


def ptr_mark(prefix, v):
    return prefix + ('_zero' if v == 0 else '_ffff' if v == M32 else '_exe' if 0x400000 <= v < 0xC00000 else '_other')


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if fn < 20:
        o = a[0]
        span = SPAN.get(fn, 4)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + span > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(23):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    if _fn < 20:
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _fn in (8, 12):
        _k += ['src_zero', 'src_ffff', 'src_exe', 'src_other']
    elif _fn in (11, 13):
        _k += ['arg_zero', 'arg_ffff', 'arg_exe', 'arg_other']
    elif _fn in DTOR_VTABLE:
        _k += ['prev_same', 'prev_other', 'gret_zero', 'gret_other']
    elif _fn == 15:
        _k += ['ret_true', 'ret_false', 'ret_low_zero_high_set', 'prio_0', 'prio_other', 'task_unaligned', 'task_aligned']
    elif _fn in (20, 21):
        _k += ['args0', 'args1']
    elif _fn == 22:
        _k += ['ptr_zero', 'ptr_ffff', 'ptr_exe', 'ptr_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret = [], None
    if fn in CONSTANT:
        A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif fn in (8, 12):
        A.snap(); B.snap(); C.snap()
        ret = a[3]
        marks.append(ptr_mark('src', a[3]))
    elif fn in (11, 13):
        A.snap(); B.snap(); C.snap()
        ret = 0 if fn == 11 else 1
        marks.append(ptr_mark('arg', a[3]))
    elif fn in DTOR_VTABLE:
        o = a[0]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, DTOR_VTABLE[fn])
        events = [[0x61A3D0, tag(0, o), DTOR_VTABLE[fn]]]
        marks.append('prev_same' if a[3] == DTOR_VTABLE[fn] else 'prev_other')
        marks.append('gret_zero' if a[4] == 0 else 'gret_other')
    elif fn == 15:
        A.snap(); B.snap(); C.snap()
        # [this+8] apunta a la subtarea (arena B, desplazamiento a[6]); la ranura 6 de su vtable es el falso: recibe ECX = subtarea y los tres argumentos tal cual
        events = [[0xFAB6, tag(1, a[6]), a[3], a[4], a[5]]]
        ret = a[8] & 0xFF
        marks.append('ret_true' if ret else 'ret_false')
        if ret == 0 and a[8] > 0xFF:
            marks.append('ret_low_zero_high_set')
        marks.append('prio_0' if a[4] == 0 else 'prio_other')
        marks.append('task_unaligned' if a[6] % 4 else 'task_aligned')
    elif fn in (20, 21):
        A.snap(); B.snap(); C.snap()
        marks.append('args1' if a[3] else 'args0')
    else:   # 22: devuelve el puntero recibido
        A.snap(); B.snap(); C.snap()
        ret = a[3]
        marks.append(ptr_mark('ptr', a[3]))
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
