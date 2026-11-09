"""Oraculo independiente (Python) de Fn139A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 139A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y el formato de 80 bits de la x87 para el resultado en ST0.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4AE390  CEventPotentialWalkIntoVehicle (vtable 0x85AED0, ranura 1, GetEventType)         mov eax,38h; ret
   1  0x4AE3A0  CEventPotentialWalkIntoVehicle (ranura 3, GetLifeTime)                            xor eax,eax; ret
   2  0x4AE3B0  CEventPotentialWalkIntoVehicle (ranura 2, GetEventPriority)                       mov eax,1Dh; ret
   3  0x4AE5C0  CEventPotentialWalkIntoVehicle (ranura 11, TakesPriorityOver)                     mov al,1; ret 4
   4  0x4B0D70  CEventVehicleDied (vtable 0x85B400, ranura 1)                                     mov eax,23h; ret
   5  0x4B0D80  CEventVehicleDied (ranura 3)                                                      xor eax,eax; ret
   6  0x4B0D90  CEventVehicleDied (ranura 2)                                                      mov eax,48h; ret
   7  0x4B0EE0  CEventLeaderEnteredCarAsDriver (vtable 0x85A198, ranura 11, TakesPriorityOver)    mov al,1; ret 4
   8  0x56F200  cuerpo de ~CWanted                                                                add ecx,21Ch; jmp 0x4E6E00 (~CAEPoliceScannerAudioEntity)
   9  0x571970  stub de script (clase desconocida)                                                xor al,al; ret 0Ch
  10  0x571980  stub de script                                                                    xor al,al; ret 0Ch
  11  0x6300B0  CTaskComplexDie (vtable 0x86DE78, ranura 4, GetTaskType)                          mov eax,0D9h; ret
  12  0x6300C0  cuerpo de ~CTaskComplexDie                                                        mov dword [ecx],86DE78h; jmp 0x61A3D0
  13  0x630580  CTaskComplexDie (ranura 10, ControlSubTask)                                       mov eax,[ecx+8]; ret 4
  14  0x4B1960  CEventVehicleDamage (vtable 0x85B588, ranura 3, GetLifeTime)                      xor eax,eax; ret
  15  0x4B1970  CEventVehicleDamage (ranura 12, GetLocalSoundLevel)                               fld dword [85ADBCh] (= 55.0f); ret
  16  0x4B1980  CEventVehicleDamage (ranura 2, GetEventPriority)                                  mov eax,31h; ret
  17  0x4B1B50  cuerpo de ~CEventSpecial (llamador 0x4B6750 usa ECX despues)                      mov dword [ecx],85AB38h; ret
  18  0x4C56E0  cuerpo de ~CLodTimeModelInfo                                                      mov dword [ecx],85BB9Ch; ret
  19  0x4C56F0  constructor de CClumpModelInfo                                                    push esi; mov esi,ecx; call 4C4A60 (word [ecx+8]=0, word [ecx+0Ah]=FFFFh); mov [esi],85BD30h; mov eax,esi; pop esi; ret
  20  0x4B2030  CEventGunShotWhizzedBy (vtable 0x85B730, ranura 1, GetEventType)                  mov eax,31h; ret
  21  0x4B2040  CEventGunShotWhizzedBy (ranura 3)                                                 xor eax,eax; ret
  22  0x4B2050  CEventGunShotWhizzedBy (ranura 15, CanBeInterruptedBySameEvent)                   mov al,1; ret
  23  0x4B2060  CEventGunShotWhizzedBy (ranura 2)                                                 mov eax,24h; ret
  24  0x4B2070  cuerpo de ~CEventGunShotWhizzedBy                                                 mov dword [ecx],85B730h; jmp 0x4AC720

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventPotentialWalkIntoVehicle::GetEventType', 'CEventPotentialWalkIntoVehicle::GetLifeTime', 'CEventPotentialWalkIntoVehicle::GetEventPriority', 'CEventPotentialWalkIntoVehicle::TakesPriorityOver', 'CEventVehicleDied::GetEventType', 'CEventVehicleDied::GetLifeTime', 'CEventVehicleDied::GetEventPriority', 'CEventLeaderEnteredCarAsDriver::TakesPriorityOver', 'CWanted::DestructorBody', 'CPlayerInfo::ScriptStub_571970', 'CPlayerInfo::ScriptStub_571980', 'CTaskComplexDie::GetTaskType', 'CTaskComplexDie::Destructor', 'CTaskComplexDie::ControlSubTask', 'CEventVehicleDamage::GetLifeTime', 'CEventVehicleDamage::GetLocalSoundLevel', 'CEventVehicleDamage::GetEventPriority', 'CEventSpecial::Destructor', 'CLodTimeModelInfo::Destructor', 'CClumpModelInfo::Constructor', 'CEventGunShotWhizzedBy::GetEventType', 'CEventGunShotWhizzedBy::GetLifeTime', 'CEventGunShotWhizzedBy::CanBeInterruptedBySameEvent', 'CEventGunShotWhizzedBy::GetEventPriority', 'CEventGunShotWhizzedBy::Destructor']
ENTRIES = (0x4AE390, 0x4AE3A0, 0x4AE3B0, 0x4AE5C0, 0x4B0D70, 0x4B0D80, 0x4B0D90, 0x4B0EE0, 0x56F200, 0x571970, 0x571980, 0x6300B0, 0x6300C0, 0x630580, 0x4B1960,
           0x4B1970, 0x4B1980, 0x4B1B50, 0x4C56E0, 0x4C56F0, 0x4B2030, 0x4B2040, 0x4B2050, 0x4B2060, 0x4B2070)
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
CONSTANT = {0: 0x38, 1: 0, 2: 0x1D, 4: 0x23, 5: 0, 6: 0x48, 11: 0xD9, 14: 0, 16: 0x31, 20: 0x31, 21: 0, 23: 0x24}
BOOL_TRUE = (3, 7, 22)          # `mov al,1`
STUBS = (9, 10)                 # `xor al,al; ret 0Ch`
# cuerpos de destructor que dejan una vtable en [ecx] (y, salvo 17 y 18, saltan a un colaborador con hook)
DTOR_VTABLE = {12: 0x86DE78, 17: 0x85AB38, 18: 0x85BB9C, 24: 0x85B730}
DTOR_COLLAB = {12: 0x61A3D0, 24: 0x4AC720}
SPAN = {8: 0x220, 13: 0x0C, 19: 0x0C}
PREV_VTABLES = {12: 0x86DE78, 17: 0x85B5D0, 18: 0x85BCF0, 24: 0x85B730}
CLUMP_VTABLE = 0x85BD30
SOUND_LEVEL = 55.0


def float_to_ext(bits):
    """imagen de 10 bytes (mantisa de 64 bits y signo+exponente de 16) que deja `fld dword` en un registro x87 (solo valores normales aqui)"""
    sign = (bits >> 31) & 1
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    assert 0 < exp < 0xFF
    e = exp - 127 + 16383
    m = (1 << 63) | (man << 40)
    return m.to_bytes(8, 'little') + (e | (sign << 15)).to_bytes(2, 'little')


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
for _fn in range(25):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill']
    if _fn in (3, 7):
        _k += ['arg_zero', 'arg_ffff', 'arg_exe', 'arg_other']
    elif _fn == 8:
        _k += ['gret_zero', 'gret_other']
    elif _fn in STUBS:
        _k += ['ecx_arena', 'ecx_raw', 'flag_one', 'flag_other', 'id_16bit', 'id_wide', 'str_known', 'str_other']
    elif _fn in DTOR_VTABLE:
        _k += ['prev_same', 'prev_other']
        if _fn in DTOR_COLLAB:
            _k += ['gret_zero', 'gret_other']
    elif _fn == 13:
        _k += ['sub_zero', 'sub_ffff', 'sub_exe', 'sub_other']
    elif _fn == 19:
        _k += ['prev0_same', 'prev0_other', 'prev8_zero', 'prev8_ffff0000', 'prev8_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret, x87, ecx_after = [], None, None, None
    o = a[0]
    if fn in CONSTANT:
        ret = CONSTANT[fn]
        A.snap(); B.snap(); C.snap()
    elif fn in BOOL_TRUE:
        ret = 1
        if fn != 22:
            marks.append(ptr_mark('arg', a[3]))
        A.snap(); B.snap(); C.snap()
    elif fn in STUBS:
        ret = 0
        marks.append('ecx_raw' if a[6] else 'ecx_arena')
        marks.append('flag_one' if a[4] == 1 else 'flag_other')
        marks.append('id_16bit' if a[3] < 0x10000 else 'id_wide')
        marks.append('str_known' if a[5] == 0x859E40 else 'str_other')
        A.snap(); B.snap(); C.snap()
    elif fn == 8:
        A.snap(); B.snap(); C.snap()
        events = [[0x4E6E00, tag(0, o + 0x21C)]]
        marks.append('gret_zero' if a[4] == 0 else 'gret_other')
    elif fn in DTOR_VTABLE:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, DTOR_VTABLE[fn])
        if fn in DTOR_COLLAB:
            events = [[DTOR_COLLAB[fn], tag(0, o), DTOR_VTABLE[fn]]]
            marks.append('gret_zero' if a[4] == 0 else 'gret_other')
        marks.append('prev_same' if a[3] == PREV_VTABLES[fn] else 'prev_other')
        if fn == 17:
            ecx_after = tag(0, o)
    elif fn == 13:
        A.wr(o, 4, a[5]); A.wr(o + 8, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = a[3]
        marks.append(ptr_mark('sub', a[3]))
    elif fn == 15:
        A.snap(); B.snap(); C.snap()
        import struct
        x87 = float_to_ext(struct.unpack('<I', struct.pack('<f', SOUND_LEVEL))[0]).hex()
    elif fn == 19:
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, a[4])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, CLUMP_VTABLE)
        A.wr(o + 8, 2, 0)
        A.wr(o + 0xA, 2, 0xFFFF)
        ret = tag(0, o)
        marks.append('prev0_same' if a[3] == CLUMP_VTABLE else 'prev0_other')
        marks.append('prev8_zero' if a[4] == 0 else 'prev8_ffff0000' if a[4] == 0xFFFF0000 else 'prev8_other')
    else:
        raise AssertionError(fn)
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'x87': x87, 'marks': marks}
    if ecx_after is not None:
        out['ecx_after'] = ecx_after
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
    if o.get('ecx_after') != e.get('ecx_after'):
        return 'ecx tras la llamada %s != %s' % (o.get('ecx_after'), e.get('ecx_after'))
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    top = (int.from_bytes(img[4:6], 'little') >> 11) & 7
    if e['x87'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
    else:
        # una sola entrada (TOP = 7 tras el fld sobre la pila vacia); el resto vacio (etiqueta 11); registro 7 valido (etiqueta 00)
        if ftw != 0x3FFF or top != 7:
            return 'x87: ftw %04x top %d (se esperaba 3fff / 7)' % (ftw, top)
        st0 = img[28:38].hex()
        if st0 != e['x87']:
            return 'ST0 %s != %s' % (st0, e['x87'])
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
