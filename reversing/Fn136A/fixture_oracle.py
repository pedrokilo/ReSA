"""Oraculo independiente (Python) de Fn136A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las dos funciones del lote 136A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp)
para conocer su imagen inicial. Las dos tablas (PLATE, SIGN) se decodificaron del gta-sa.exe canonico con exe_tables.py (que las comprueba con --check).

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x6FD7C0  GetCharacterPositionInCharSet (__usercall: AL = caracter; pila: [esp+4] = uint32* columna, [esp+8] = uint32* fila; `ret`)
       movzx eax,al; add eax,-0x20; cmp eax,0x5A; ja default; movzx eax,byte [eax + 0x6FDB98]; jmp [eax*4 + 0x6FDB00].  Cada caso: mov edx,[esp+4]; <eax = col>; mov [edx],eax;
       mov eax,[esp+8]; <ecx = fila>; mov [eax],ecx; ret (primero la columna y despues la fila: con punteros iguales o solapados gana la fila). Por defecto (0, 9).
       Digitos '0'..'9': (2,6)(3,6)(0,7)(1,7)(2,7)(3,7)(0,8)(1,8)(2,8)(3,8); letras A..Z y a..z comparten caso: A (0,0) .. X (3,5), Y (0,6), Z (1,6).
  1  0x6FE260  RoadsignGetLineAndRow (mismo contrato)
       movzx eax,al; add eax,-0x21; cmp eax,0x5D; ja default; jmp [eax*4 + 0x6FE9F4].  Mismos casos (primero la columna, despues la fila). Por defecto (0, 26) en 0x6FE9DF.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['GetCharacterPositionInCharSetExe', 'RoadsignGetLineAndRowExe']
ENTRIES = (0x6FD7C0, 0x6FE260)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
# Tablas (columna, fila) por caracter 0..255 decodificadas del EXE (exe_tables.py --check las verifica)
PLATE = [(0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (2, 6), (3, 6), (0, 7), (1, 7), (2, 7), (3, 7), (0, 8), (1, 8), (2, 8), (3, 8), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 0), (1, 0), (2, 0), (3, 0), (0, 1), (1, 1), (2, 1), (3, 1), (0, 2), (1, 2), (2, 2), (3, 2), (0, 3), (1, 3), (2, 3), (3, 3), (0, 4), (1, 4), (2, 4), (3, 4), (0, 5), (1, 5), (2, 5), (3, 5), (0, 6), (1, 6), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 0), (1, 0), (2, 0), (3, 0), (0, 1), (1, 1), (2, 1), (3, 1), (0, 2), (1, 2), (2, 2), (3, 2), (0, 3), (1, 3), (2, 3), (3, 3), (0, 4), (1, 4), (2, 4), (3, 4), (0, 5), (1, 5), (2, 5), (3, 5), (0, 6), (1, 6), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9), (0, 9)]
SIGN = [(0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 0), (0, 26), (3, 21), (0, 22), (2, 21), (2, 0), (0, 26), (0, 1), (1, 1), (1, 22), (2, 1), (0, 26), (0, 2), (1, 2), (0, 26), (3, 2), (0, 3), (1, 3), (2, 3), (3, 3), (0, 4), (1, 4), (2, 4), (3, 4), (0, 5), (2, 5), (1, 5), (2, 20), (0, 26), (3, 20), (3, 5), (2, 22), (0, 6), (1, 6), (2, 6), (3, 6), (0, 7), (1, 7), (2, 7), (3, 7), (0, 8), (1, 8), (2, 8), (3, 8), (0, 9), (1, 9), (2, 9), (3, 9), (0, 10), (1, 10), (2, 10), (3, 10), (0, 11), (1, 11), (2, 11), (3, 11), (0, 12), (1, 12), (2, 12), (0, 26), (0, 13), (0, 21), (0, 26), (0, 26), (1, 13), (2, 13), (3, 13), (0, 14), (1, 14), (2, 14), (3, 14), (0, 15), (1, 15), (2, 15), (3, 15), (0, 16), (1, 16), (2, 16), (3, 16), (0, 17), (1, 17), (2, 17), (3, 17), (0, 18), (1, 18), (2, 18), (3, 18), (0, 19), (1, 19), (2, 19), (0, 23), (3, 22), (2, 23), (1, 21), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26), (0, 26)]


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


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'ch_digit', 'ch_upper', 'ch_lower', 'ch_default', 'ch_ctrl', 'ch_high', 'ch_punct', 'alias', 'overlap', 'writes_guard', 'unaligned'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'ch_digit', 'ch_upper', 'ch_lower', 'ch_default', 'ch_ctrl', 'ch_high', 'ch_punct', 'ch_space', 'ch_del', 'alias', 'overlap', 'writes_guard', 'unaligned'],
}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    kind, fill = a[1], a[2]
    ch = a[5]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    A.snap(); B.snap(); C.snap()
    col, row = (PLATE if fn == 0 else SIGN)[ch]
    second = A if a[4] else B
    A.wr(a[0], 4, col)          # primero la columna ...
    second.wr(a[3], 4, row)     # ... y despues la fila (con punteros iguales o solapados gana la fila)
    isdig, isup, islow = 0x30 <= ch <= 0x39, 0x41 <= ch <= 0x5A, 0x61 <= ch <= 0x7A
    marks.append('ch_digit' if isdig else 'ch_upper' if isup else 'ch_lower' if islow else 'ch_other')
    if (col, row) == ((0, 9) if fn == 0 else (0, 26)):
        marks.append('ch_default')
    if ch < 0x20:
        marks.append('ch_ctrl')
    elif ch >= 0x80:
        marks.append('ch_high')
    elif not (isdig or isup or islow):
        marks.append('ch_punct')
    if fn == 1 and ch == 0x20:
        marks.append('ch_space')
    if fn == 1 and ch == 0x7F:
        marks.append('ch_del')
    if a[4] and a[0] == a[3]:
        marks.append('alias')
    elif a[4] and abs(a[0] - a[3]) < 4:
        marks.append('overlap')
    if a[0] + 4 > WIN or a[3] + 4 > WIN or a[0] < 0 or a[3] < 0:
        marks.append('writes_guard')
    if a[0] % 4 or a[3] % 4:
        marks.append('unaligned')
    return {'events': [], 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': None, 'marks': marks}


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
