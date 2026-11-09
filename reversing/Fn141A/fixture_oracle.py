"""Oraculo independiente (Python) de Fn141A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 141A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de la arena (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable; las clases del listado de pendientes eran inexactas):
   0  0x5D6830  CCustomBuildingDNPipeline (avance del indice de trabajo)  mov eax,[C02C14]; mov ecx,[esp+4]; inc eax; and eax,0Fh; mov [C02C14],eax; mov [C02C18],ecx; ret   (cdecl)
   1  0x5D6E90  CCustomBuildingDNPipeline (NightColors de la geometria)  mov eax,[esp+4]; mov ecx,[8D12BC]; mov eax,[ecx+eax]; ret                                   (cdecl; LEE el dword)
   2  0x532540  CDummy (cuerpo del constructor)  push esi; mov esi,ecx; call 532A90; mov al,[esi+36h]; and al,0FDh; or al,5; mov [esi+36h],al; mov [esi],8638C0h; mov eax,esi; pop esi; ret
   3  0x532560  CDummy (cuerpo del destructor)   mov [ecx],8638C0h; jmp 535E90
   4  0x608F00  CWeaponInfo::GetAnimLoopStart    mov al,[esp+4]; test al,al; je ..; fld dword [ecx+4Ch]; ret 4;  ..: fld dword [ecx+40h]; ret 4
   5  0x608F20  CWeaponInfo::GetAnimLoopEnd      igual con +50h / +44h
   6  0x631910  CTaskComplexObserveTrafficLightsAndAchieveHeading (constructor, vtable 0x86DFC4)  push esi; mov esi,ecx; call 61A3B0; mov eax,[esp+8]; mov ecx,[esp+0Ch]; mov [esi+0Ch],eax; mov [esi],86DFC4h; mov [esi+10h],ecx; mov eax,esi; pop esi; ret 8
   7  0x631940  la misma clase (ranura 4, GetTaskType)  mov eax,0E1h; ret
   8  0x6CE440  CQuadBike (vtable 0x871AE8, ranura 0)  push esi; mov esi,ecx; call 6CDC30; test byte [esp+8],1; je ..; push esi; call 6E2D90; add esp,4; mov eax,esi; pop esi; ret 4
   9  0x6CED10  cuerpo de ~CTrailer (vtable 0x871C28; ranura 0 = 0x6CF180)  mov [ecx],871C28h; jmp 6A61E0
  10  0x5F6900  CTaskAllocatorKillThreatsBasicRandomGroup (vtable 0x86C80C, ranura 0)  ... call 5F6920 ...; call 69D990 ...; ret 4
  11  0x5F6920  cuerpo de su destructor  mov [ecx],86C80Ch; jmp 69C780
  12  0x633290  cuerpo de ~CTaskComplexAttractorPartnerWait (vtable 0x86E284)  mov [ecx],86E284h; jmp 61A3D0
  13  0x638FD0  CTaskComplexUseAttractorPartner (vtable 0x86E2B0, ranura 0)  ... call 633300 ...; call 61A5B0 ...; ret 4
  14  0x63CE70  cuerpo de ~CTaskComplexDriveToPoint (vtable 0x86E9DC)  mov [ecx],86E9DCh; jmp 63CA40
  15  0x645350  CTaskComplexDrivePointRoute (vtable 0x86EBF8, ranura 0)  ... call 643490 ...; call 61A5B0 ...; ret 4
  16  0x63D100  cuerpo de ~CTaskComplexEnterAnyCarAsDriver (vtable 0x86EA14)  mov [ecx],86EA14h; jmp 61A3D0
  17  0x6436E0  CTaskComplexShuffleSeats (vtable 0x86EA40, ranura 0)  ... call 63D1A0 ...; call 61A5B0 ...; ret 4
  18  0x668630  cuerpo de ~CTaskComplexGoToPointAndStandStill (vtable 0x86FDA8)  mov [ecx],86FDA8h; jmp 61A3D0
  19  0x66DD50  CTaskComplexGoToPointAiming (vtable 0x86FE00, ranura 0)  ... call 668870 ...; call 61A5B0 ...; ret 4
  20  0x636830  CTaskComplexUseEffectRunning (vtable 0x86E594, ranura 0)  ... call 636850 ...; call 61A5B0 ...; ret 4
  21  0x636850  cuerpo de su destructor  mov [ecx],86E594h; jmp 632270
  22  0x636910  CTaskComplexUseEffectSprinting (vtable 0x86E5C0, ranura 0)  ... call 636930 ...; call 61A5B0 ...; ret 4
  23  0x636930  cuerpo de su destructor  mov [ecx],86E5C0h; jmp 632270

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from pathlib import Path

M32 = 0xFFFFFFFF
NAMES = json.loads((Path(__file__).resolve().parent / 'names.json').read_text(encoding='utf-8'))
ENTRIES = (0x5D6830, 0x5D6E90, 0x532540, 0x532560, 0x608F00, 0x608F20, 0x631910, 0x631940, 0x6CE440, 0x6CED10, 0x5F6900, 0x5F6920, 0x633290, 0x638FD0, 0x63CE70, 0x645350,
           0x63D100, 0x6436E0, 0x668630, 0x66DD50, 0x636830, 0x636850, 0x636910, 0x636930)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
# forma de cada funcion (ver la cabecera), vtable del EXE que escribe, colaboradores y operator delete (leidos del desensamblado)
SHAPE = {0: 'G0', 1: 'G1', 2: 'DC', 3: 'BODY', 4: 'F', 5: 'F', 6: 'OC', 7: 'CONST'}
for _i in (8, 10, 13, 15, 17, 19, 20, 22):
    SHAPE[_i] = 'SC'
for _i in (9, 11, 12, 14, 16, 18, 21, 23):
    SHAPE[_i] = 'BODY'
VTABLE = {2: 0x8638C0, 3: 0x8638C0, 6: 0x86DFC4, 8: 0x871AE8, 9: 0x871C28, 10: 0x86C80C, 11: 0x86C80C, 12: 0x86E284, 13: 0x86E2B0, 14: 0x86E9DC, 15: 0x86EBF8, 16: 0x86EA14, 17: 0x86EA40,
          18: 0x86FDA8, 19: 0x86FE00, 20: 0x86E594, 21: 0x86E594, 22: 0x86E5C0, 23: 0x86E5C0}
COLLAB = {2: 0x532A90, 3: 0x535E90, 6: 0x61A3B0, 8: 0x6CDC30, 9: 0x6A61E0, 10: 0x5F6920, 11: 0x69C780, 12: 0x61A3D0, 13: 0x633300, 14: 0x63CA40, 15: 0x643490, 16: 0x61A3D0, 17: 0x63D1A0,
          18: 0x61A3D0, 19: 0x668870, 20: 0x636850, 21: 0x632270, 22: 0x636930, 23: 0x632270}
DELETE = {8: 0x6E2D90, 10: 0x69D990, 13: 0x61A5B0, 15: 0x61A5B0, 17: 0x61A5B0, 19: 0x61A5B0, 20: 0x61A5B0, 22: 0x61A5B0}
SPAN = {2: 0x38, 4: 0x54, 5: 0x54, 6: 0x14}


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
    """etiqueta de un puntero dentro de la arena `arena` (0 = A): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
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

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


def f32_to_ext(b):
    """float de 32 bits -> (exponente con signo, mantisa de 64 bits) del x87 tras `fld dword`: los SNaN salen como QNaN, los denormales se normalizan"""
    sign, exp, man = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    if exp == 0xFF:
        if man == 0:
            return (sign << 15) | 0x7FFF, 0x8000000000000000
        return (sign << 15) | 0x7FFF, 0xC000000000000000 | (man << 40)
    if exp == 0:
        if man == 0:
            return sign << 15, 0
        n = man.bit_length()
        return (sign << 15) | ((n - 1) - 149 + 16383), man << (64 - n)
    return (sign << 15) | (exp - 127 + 16383), (0x800000 | man) << 40


def float_class(b):
    exp, man = (b >> 23) & 0xFF, b & 0x7FFFFF
    if exp == 0xFF:
        return 'inf' if man == 0 else ('qnan' if man & 0x400000 else 'snan')
    if exp == 0:
        return 'zero' if man == 0 else 'denormal'
    return 'normal'


def ptr_mark(prefix, v):
    return prefix + ('_zero' if v == 0 else '_ffff' if v == M32 else '_exe' if 0x400000 <= v < 0xC00000 else '_other')


KEYS = {}
for _fn in range(24):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _s = SHAPE[_fn]
    if _s != 'G0':
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _s == 'G0':
        _k += ['idx_wrap', 'idx_inc', 'flag_zero', 'flag_other']
    elif _s == 'G1':
        _k += ['off_zero', 'off_neg', 'off_pos', 'val_zero', 'val_ffff', 'val_exe', 'val_other']
    elif _s == 'DC':
        _k += ['byte_bit1_set', 'byte_bit1_clear', 'base_vt_zero', 'base_vt_other', 'gret_zero', 'gret_other']
    elif _s == 'BODY':
        _k += ['prev_same', 'prev_other', 'prev_zero', 'base_write_zero', 'base_write_other', 'gret_zero', 'gret_other']
    elif _s == 'F':
        _k += ['sel_first', 'sel_second', 'arg_low_zero_high_set', 'arg_low_set', 'cls_normal', 'cls_zero', 'cls_inf', 'cls_qnan', 'cls_snan', 'cls_denormal']
    elif _s == 'OC':
        _k += ['dur_zero', 'dur_other', 'heading_zero', 'heading_other', 'base_write_zero', 'base_write_other']
    elif _s == 'CONST':
        _k += ['dword_zero', 'dword_other']
    elif _s == 'SC':
        _k += ['delete', 'keep', 'flags_zero', 'flags_one', 'flags_upper_set_bit0', 'flags_upper_only', 'prev_own', 'prev_other', 'prev_zero']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    shape = SHAPE[fn]
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A = Mem(fill, a[12])
    events, ret, gchanges, st0 = [], None, [], None
    o = a[0]
    if shape != 'G0':
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + SPAN.get(fn, 4) > WIN:
            marks.append('spill')
    if shape == 'G0':
        before = struct.pack('<II', a[4], a[5])
        g0 = ((a[4] + 1) & 0xF)
        after = struct.pack('<II', g0, a[3])
        gchanges = [[0, i, after[i]] for i in range(8) if after[i] != before[i]]
        A.snap()
        marks += ['idx_wrap' if a[4] & 0xF == 0xF else 'idx_inc', 'flag_zero' if a[3] == 0 else 'flag_other']
    elif shape == 'G1':
        A.wr(o, 4, a[4])
        A.snap()
        ret = a[4]
        marks += ['off_zero' if a[3] == 0 else 'off_neg' if a[3] & 0x80000000 else 'off_pos', ptr_mark('val', a[4])]
    elif shape == 'DC':
        A.snap()
        d0, b0 = A.rd(o, 4), A.rd(o + 0x36, 1)
        events = [[COLLAB[fn], tag(0, o), d0, b0]]
        A.wr(o, 4, a[3])
        A.wr(o + 0x36, 1, a[4] & 0xFF)
        b = A.rd(o + 0x36, 1)
        A.wr(o + 0x36, 1, (b & 0xFD) | 5)
        A.wr(o, 4, VTABLE[fn])
        ret = tag(0, o)
        marks += ['byte_bit1_set' if a[4] & 2 else 'byte_bit1_clear', 'base_vt_zero' if a[3] == 0 else 'base_vt_other', 'gret_zero' if a[5] == 0 else 'gret_other']
    elif shape == 'BODY':
        A.wr(o, 4, a[3])
        A.snap()
        events = [[COLLAB[fn], tag(0, o), VTABLE[fn]]]
        A.wr(o, 4, VTABLE[fn])
        A.wr(o, 4, a[4])
        marks += ['prev_same' if a[3] == VTABLE[fn] else 'prev_other', 'prev_zero' if a[3] == 0 else 'prev_other', 'base_write_zero' if a[4] == 0 else 'base_write_other',
                  'gret_zero' if a[5] == 0 else 'gret_other']
    elif shape == 'F':
        A.wr(o + 0x40, 4, a[4]); A.wr(o + 0x44, 4, a[5]); A.wr(o + 0x4C, 4, a[6]); A.wr(o + 0x50, 4, a[7])
        A.snap()
        second = (a[3] & 0xFF) != 0           # solo el byte bajo del argumento decide
        bits = (a[6] if second else a[4]) if fn == 4 else (a[7] if second else a[5])
        st0 = f32_to_ext(bits)
        marks += ['sel_second' if second else 'sel_first', 'arg_low_zero_high_set' if not second and a[3] else 'arg_low_set' if second else 'arg_all_zero', 'cls_' + float_class(bits)]
    elif shape == 'OC':
        A.snap()
        events = [[COLLAB[fn], tag(0, o), A.rd(o, 4)]]
        A.wr(o, 4, a[5])
        A.wr(o + 0xC, 4, a[3])
        A.wr(o, 4, VTABLE[fn])
        A.wr(o + 0x10, 4, a[4])
        ret = tag(0, o)
        marks += ['dur_zero' if a[3] == 0 else 'dur_other', 'heading_zero' if a[4] == 0 else 'heading_other', 'base_write_zero' if a[5] == 0 else 'base_write_other']
    elif shape == 'CONST':
        A.wr(o, 4, a[3])
        A.snap()
        ret = 0xE1
        marks.append('dword_zero' if a[3] == 0 else 'dword_other')
    else:   # SC: destructor con borrado
        prev, flags, wr = a[6], a[7], a[8]
        A.wr(o, 4, prev)
        A.snap()
        events = [[COLLAB[fn], tag(0, o), prev]]
        A.wr(o, 4, wr)
        if flags & 1:
            events.append([DELETE[fn], tag(0, o), wr])
        ret = tag(0, o)
        marks.append('delete' if flags & 1 else 'keep')
        marks.append('flags_zero' if flags == 0 else 'flags_one' if flags == 1 else 'flags_upper_set_bit0' if (flags & 1) and flags & ~0xFF else 'flags_upper_only' if flags & ~0xFF and not flags & 0xFF else 'flags_other')
        marks.append('prev_own' if prev == VTABLE[fn] else 'prev_zero' if prev == 0 else 'prev_other')
    return {'events': events, 'changes': {'A': A.changes()}, 'gchanges': gchanges, 'ret': ret, 'marks': marks, 'st0': st0}


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
    if e['st0'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
    else:
        top = (int.from_bytes(img[4:6], 'little') >> 11) & 7
        exp, man = e['st0']
        # etiqueta de ST0 (registro fisico 7) que calcula FNSAVE: 00 valido, 01 cero, 10 especial (infinito o NaN); los demas registros vacios (11)
        t7 = 1 if man == 0 and exp & 0x7FFF == 0 else 2 if exp & 0x7FFF == 0x7FFF else 0
        if ftw != (0x3FFF | (t7 << 14)) or top != 7:
            return 'x87: se esperaba un solo valor en ST0 (ftw %04x top %d)' % (ftw, top)
        got = img[28:38]   # FNSAVE guarda los registros en orden ST(0)..ST(7)
        if struct.unpack('<QH', got) != (man, exp):
            return 'ST0 %s != %s' % (struct.unpack('<QH', got), (man, exp))
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
