"""Oraculo independiente (Python) de Fn163A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los cuatro cuerpos de destructor del lote 163A y lo compara con
las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
  0  0x634500  CTaskSimpleTriggerLookAt (marco SEH): esi = ecx; [esi] = 86E3CCh; ecx = [esi+8]; si no es nulo: push esi+8; call 571A00 (CleanUpOldReference, ecx = entidad, un argumento de pila `ret 4`);
               ecx = esi; call 61A3A0 (~CTask); ret
  1  0x631980  CTaskComplexObserveTrafficLightsAndAchieveHeading: mov [ecx],86DFC4h; jmp 61A3D0 (~CTaskComplex, thiscall sin argumentos)
  2  0x62FCF0  CTaskComplexDieInCar: mov [ecx],86DE04h; jmp 61A3D0
  3  0x688A00  CTaskSimpleSwim (marco SEH): esi = ecx; [esi] = 870920h; si BYTE [esi+9] != 0: eax = [0B4EA34h] (ms_aAnimAssocGroups); eax = [eax+58Ch] (bloque del grupo de nado, ANIM_GROUP_SWIM = 71 por 14h);
               eax = (eax - 0B5D4A0h) sar 5 (indice con signo; ms_aAnimBlocks = 0B5D4A0h, bloques de 20h); push eax; call 4D3FD0 (RemoveAnimBlockRef, cdecl); add esp,4;
               ecx = [esi+38h]: si no es nulo: push esi+38h; call 571A00; ecx = [esi+20h]: si no es nulo: push esi+20h; call 571A00; ecx = esi; call 61A3A0; ret

Los falsos de los colaboradores anotan: ~CTask / ~CTaskComplex [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); CleanUpOldReference [direccion, ECX, argumento (etiquetas)];
RemoveAnimBlockRef [direccion, indice crudo].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskSimpleTriggerLookAt::DestructorBody", "CTaskComplexObserveTrafficLightsAndAchieveHeading::DestructorBody", "CTaskComplexDieInCar::DestructorBody", "CTaskSimpleSwim::DestructorBody"]
ENTRIES = (0x634500, 0x631980, 0x62FCF0, 0x688A00)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CLEANUP, REMOVEREF, TASKDTOR, COMPLEXDTOR = 0x571A00, 0x4D3FD0, 0x61A3A0, 0x61A3D0
ANIMBLOCKS = 0xB5D4A0
GROUPOFF = 0x58C

TL, CC, SWIM = 'tl cc swim'.split()
KIND = [TL, CC, CC, SWIM]
VTABLE = (0x86E3CC, 0x86DFC4, 0x86DE04, 0x870920)
SPAN = {TL: 0x30, CC: 4, SWIM: 0x64}


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
for _fn in range(4):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other']
    if KIND[_fn] == TL:
        _k += ['entity_set', 'entity_null', 'entity_in_arena']
    elif KIND[_fn] == SWIM:
        _k += ['entity_set', 'entity_null', 'ped_set', 'ped_null', 'entity_in_arena', 'ped_in_arena', 'flag_set', 'flag_clear', 'flag_not_one', 'index_negative', 'index_positive', 'index_unaligned']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN[KIND[fn]] > WIN:
        marks.append('spill')
    marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    return marks


def pointer(W, mode, off, raw):
    return {0: 0, 1: W.raw(1, off), 2: raw}[mode]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events = []
    o = a[0]
    vt = VTABLE[fn]
    A.wr(o, 4, a[3])
    if k == TL:
        entity = pointer(W, a[6], a[7], a[8])
        A.wr(o + 8, 4, entity)
        W.snap()
        A.wr(o, 4, vt)                                                       # mov [esi],86E3CCh
        if entity:
            events.append([CLEANUP, W.tagv(entity), tag(0, o + 8)])
            marks.append('entity_set')
            if a[6] == 1:
                marks.append('entity_in_arena')
        else:
            marks.append('entity_null')
        events.append([TASKDTOR, tag(0, o), A.rd(o, 4)])                     # ~CTask ve la vtable de la clase
    elif k == CC:
        W.snap()
        A.wr(o, 4, vt)
        events.append([COMPLEXDTOR, tag(0, o), A.rd(o, 4)])                  # el cuerpo de ~CTaskComplex ve la vtable de la clase
    else:
        ped = pointer(W, a[6], a[7], a[8])
        entity = pointer(W, a[9], a[10], a[11])
        A.wr(o + 9, 1, a[16] & 0xFF)
        A.wr(o + 0x20, 4, ped)
        A.wr(o + 0x38, 4, entity)
        Cc.wr(a[17] + GROUPOFF, 4, a[18])
        W.snap()
        A.wr(o, 4, vt)
        if a[16] & 0xFF:
            diff = (a[18] - ANIMBLOCKS) & M32
            idx = diff >> 5
            if diff & 0x80000000:
                idx |= 0xF8000000                                            # sar 5 de un valor negativo
            events.append([REMOVEREF, idx])
            marks.append('flag_set')
            marks.append('index_negative' if diff & 0x80000000 else 'index_positive')
            if diff & 31:
                marks.append('index_unaligned')
            if a[16] & 0xFF != 1:
                marks.append('flag_not_one')
        else:
            marks.append('flag_clear')
        if entity:
            events.append([CLEANUP, W.tagv(entity), tag(0, o + 0x38)])
            marks.append('entity_set')
            if a[9] == 1:
                marks.append('entity_in_arena')
        else:
            marks.append('entity_null')
        if ped:
            events.append([CLEANUP, W.tagv(ped), tag(0, o + 0x20)])
            marks.append('ped_set')
            if a[6] == 1:
                marks.append('ped_in_arena')
        else:
            marks.append('ped_null')
        events.append([TASKDTOR, tag(0, o), A.rd(o, 4)])
    A.wr(o, 4, a[5])                                                         # el falso del cuerpo final deja un dword del caso en [this]
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': None, 'marks': marks}


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
