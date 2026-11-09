"""Oraculo independiente (Python) de Fn137A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diecinueve funciones del lote 137A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y el formato de 80 bits de la x87 para el resultado en ST0.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico; cada funcion es un cuerpo de 3 a 7 bytes, seguido de relleno):
  0  0x406450  CdStreamGetLastPosn (cdecl)                          mov eax, dword ptr ds:[008E4898h]; ret
  1  0x406D10  CMaths::AbsFloat (cdecl, un float en la pila)         fld dword ptr [esp+4]; fabs; ret          (ST0 = |x| en 80 bits; un NaN senalizante sale silencioso)
  2  0x420E90  CEvent::IsCriminalEvent (ranura 7)                    xor al, al; ret
  3  0x420EA0  CEvent::ReportCriminalEvent (ranura 8)                ret 4
  4  0x420EB0  CEvent::GetLocalSoundLevel (ranura 12)                fld dword ptr ds:[00858B50h] (0.0f); ret  (ST0 = +0.0)
  5  0x420EC0  CEvent::DoInformVehicleOccupants (ranura 13)          xor al, al; ret 4
  6  0x4ACA40  CEventPedCollisionWithPed::GetEventType (ranura 1)    mov eax, 2; ret
  7  0x4ACA50  CEventPedCollisionWithPed::GetLifeTime (ranura 3)     xor eax, eax; ret
  8  0x4ACA60  CEventPedCollisionWithPed::GetEventPriority (ranura 2) mov eax, 3Ch; ret
  9  0x4AE760  CEventPotentialWalkIntoPed::GetEventType              mov eax, 0Dh; ret
 10  0x4AE770  CEventPotentialWalkIntoPed::GetLifeTime               xor eax, eax; ret
 11  0x4AE780  CEventPotentialWalkIntoPed::GetSourceEntity (ranura 10) mov eax, dword ptr [ecx+20h]; ret
 12  0x4AE790  CEventPotentialWalkIntoPed::GetEventPriority          mov eax, 1Ch; ret
 13  0x4AF6D0  CEventVehicleToSteal::GetEventType                    mov eax, 1Ch; ret
 14  0x4AF6E0  CEventVehicleToSteal::GetLifeTime                     xor eax, eax; ret
 15  0x4AF6F0  CEventVehicleToSteal::GetEventPriority                mov eax, 7; ret
 16  0x4B0CD0  CEventInAir::GetEventType                             mov eax, 21h; ret
 17  0x4B0CE0  CEventInAir::GetLifeTime                              xor eax, eax; ret
 18  0x4B0CF0  CEventInAir::GetEventPriority                         mov eax, 3Dh; ret

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CdStreamGetLastPosn', 'CMaths::AbsFloat', 'CEvent::IsCriminalEvent', 'CEvent::ReportCriminalEvent', 'CEvent::GetLocalSoundLevel', 'CEvent::DoInformVehicleOccupants',
         'CEventPedCollisionWithPed::GetEventType', 'CEventPedCollisionWithPed::GetLifeTime', 'CEventPedCollisionWithPed::GetEventPriority',
         'CEventPotentialWalkIntoPed::GetEventType', 'CEventPotentialWalkIntoPed::GetLifeTime', 'CEventPotentialWalkIntoPed::GetSourceEntity', 'CEventPotentialWalkIntoPed::GetEventPriority',
         'CEventVehicleToSteal::GetEventType', 'CEventVehicleToSteal::GetLifeTime', 'CEventVehicleToSteal::GetEventPriority',
         'CEventInAir::GetEventType', 'CEventInAir::GetLifeTime', 'CEventInAir::GetEventPriority']
ENTRIES = (0x406450, 0x406D10, 0x420E90, 0x420EA0, 0x420EB0, 0x420EC0, 0x4ACA40, 0x4ACA50, 0x4ACA60, 0x4AE760, 0x4AE770, 0x4AE780, 0x4AE790, 0x4AF6D0, 0x4AF6E0, 0x4AF6F0, 0x4B0CD0, 0x4B0CE0, 0x4B0CF0)
# valor que el EXE deja en EAX (funciones de constante); None = no aplica (x87, void o puntero leido)
CONSTANT_EAX = {2: 0, 5: 0, 6: 2, 7: 0, 8: 0x3C, 9: 0xD, 10: 0, 12: 0x1C, 13: 0x1C, 14: 0, 15: 7, 16: 0x21, 17: 0, 18: 0x3D}
AL_ONLY = (2, 5)          # bool en AL: solo se observa el byte bajo
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


def float_to_ext(bits, absolute):
    """imagen de 10 bytes (mantisa de 64 bits y signo+exponente de 16) que deja `fld dword` (y `fabs`) en un registro x87: un NaN senalizante se vuelve silencioso (bit 62 de la mantisa), la carga util se
    conserva desplazada, los denormales se normalizan, el infinito lleva la parte entera a uno"""
    sign = (bits >> 31) & 1
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        e = 0x7FFF
        m = (1 << 63) | ((man << 40) | (1 << 62) if man else 0)
    elif exp == 0:
        if man == 0:
            e, m = 0, 0
        else:
            bl = man.bit_length()
            e = 16383 + (bl - 1) - 149
            m = man << (64 - bl)
    else:
        e = exp - 127 + 16383
        m = (1 << 63) | (man << 40)
    if absolute:
        sign = 0
    return m.to_bytes(8, 'little') + (e | (sign << 15)).to_bytes(2, 'little')


def float_class(bits):
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man == 0:
            return 'inf'
        return 'nan_quiet' if man & 0x400000 else 'nan_signaling'
    if exp == 0:
        return 'zero' if man == 0 else 'denormal'
    return 'normal'


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'val_zero', 'val_high_bit', 'val_fileid', 'val_all_ones'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'cls_zero', 'cls_denormal', 'cls_normal', 'cls_inf', 'cls_nan_quiet', 'cls_nan_signaling', 'sign_set', 'sign_clear', 'payload_nonzero'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_arena', 'this_raw', 'this_unaligned'],
    3: ['kind0', 'kind1', 'kind2', 'kind3', 'this_arena', 'this_raw', 'mode0', 'mode1', 'mode2', 'mode3', 'this_unaligned'],
    4: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_arena', 'this_raw', 'this_unaligned'],
    5: ['kind0', 'kind1', 'kind2', 'kind3', 'this_arena', 'this_raw', 'mode0', 'mode1', 'mode2', 'mode3', 'this_unaligned'],
    11: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'mode0', 'mode1', 'mode2', 'mode3', 'this_unaligned', 'spill'],
}
for _i in (6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18):
    KEYS[_i] = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_arena', 'this_raw', 'this_unaligned']


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    ret = None
    x87 = None            # None = pila vacia; si no, los 10 bytes de ST0 (hex)
    this_raw = kind == 2 and fn != 11
    marks.append('this_raw' if this_raw else 'this_arena')
    if not this_raw:
        if a[0] % 4:
            marks.append('this_unaligned')
        if a[0] + 0x24 > WIN:
            marks.append('spill')

    def mode_value():
        m = a[4]
        marks.append('mode%d' % m)
        if m == 0:
            return a[3]
        if m == 1:
            return tag(1, a[5])
        return 0 if m == 2 else M32

    if fn == 0:
        ret = a[3]
        marks.append('val_zero' if a[3] == 0 else 'val_all_ones' if a[3] == M32 else 'val_high_bit' if a[3] & 0x80000000 else 'val_other')
        if a[3] >> 24:
            marks.append('val_fileid')
    elif fn == 1:
        x87 = float_to_ext(a[3], True).hex()
        marks.append('cls_' + float_class(a[3]))
        marks.append('sign_set' if a[3] >> 31 else 'sign_clear')
        if (a[3] & 0x7FFFFF) and (a[3] >> 23) & 0xFF == 0xFF:
            marks.append('payload_nonzero')
    elif fn == 4:
        x87 = float_to_ext(0, False).hex()
    elif fn == 3:
        mode_value()
    elif fn == 5:
        mode_value()
        ret = 0
    if fn == 11:
        # GetSourceEntity devuelve el dword que el banco planta en this+0x20 (antes de la imagen de referencia, asi que no hay cambios): crudo (modo 0), puntero a la arena B (1, etiqueta), nulo (2) o -1 (3)
        ret = mode_value()
    elif fn in CONSTANT_EAX:
        ret = CONSTANT_EAX[fn]
    A.snap(); B.snap(); C.snap()
    return {'events': [], 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'x87': x87, 'marks': marks}


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
    top = (int.from_bytes(img[4:6], 'little') >> 11) & 7
    if e['x87'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
    else:
        # una sola entrada (TOP = 7 tras el fld sobre la pila vacia); el resto vacio (etiqueta 11). Etiqueta del registro 7 (bits 15..14 de la palabra): 00 valido (un denormal de float ya es un extendido
        # normalizado), 01 cero, 10 especial (NaN e infinito)
        st = bytes.fromhex(e['x87'])
        ext_exp = int.from_bytes(st[8:10], 'little') & 0x7FFF
        mant = int.from_bytes(st[0:8], 'little')
        tagbits = 2 if ext_exp == 0x7FFF else (1 if ext_exp == 0 and mant == 0 else 0)
        want = (tagbits << 14) | 0x3FFF
        if ftw != want or top != 7:
            return 'x87: ftw %04x top %d (se esperaba %04x / 7)' % (ftw, top, want)
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
