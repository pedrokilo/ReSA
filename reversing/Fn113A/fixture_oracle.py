"""Oraculo independiente (Python) de Fn113A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 17 funciones del lote 113A y lo compara con las filas
de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Funcion 0, CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag (0x63AB70, thiscall, EAX): byte +0x10 bit 0 puesto y dword +0x14 == 8 -> 0xA; si no 0.
Funcion 1, CTaskComplexWander::RequestDir (0x669D30, thiscall, `ret 4`): si el dword del argumento != byte +0x10 (sin signo): byte +0x10 = al, byte +0x24 |= 2.
Funcion 2, CTaskComplexWander::RequestNodesFrom (0x669D50, thiscall, `ret 4`): si alguna de las palabras +0x18, +0x1A, +0x1C, +0x1E difiere de la de la otra tarea: copia los dwords +0x18 y +0x1C, el byte +0x10 y byte +0x24 |= 4.
Funcion 3, CTaskComplexWanderStandard::SetMinNextScanTime (0x66B150, thiscall, `ret 4`): dword +0x34 = argumento.
Funcion 4, CTaskComplexPartner::GetStateOfPartnerTask (0x6822B0, thiscall): FindActiveTaskByType (0x681740, ecx = inteligencia del companero + 4, tipo = dword +0x34); nulo -> 0; si no movsx del byte +0x59 de la tarea.
Funciones 5 y 6, GangDrinkAnimOfPed (0x65E6F0) / GangSmokeAnimOfPed (0x65E710) (cdecl): palabra +0x22 del ped (con signo) == 0x67 o 0x69 -> 0x12D / 0x12E; si no 0x12B / 0x12C.
Funciones 7 y 9, operator new de CTaskAllocator (0x69D980; pool 0xB744BC) y de CPedIntelligence (0x6074A0; pool 0xB744C0): jmp a la New del pool (cursor +0xC; avanza con una sola vuelta; ranura libre = bit 7 del estado;
  la ocupa: id = (id + 1) & 0x7F, bit 7 a cero); lleno -> nulo con el cursor a 0.
Funciones 8 y 10, operator delete de los mismos pools: indice = (obj - almacenamiento) / tamano (0x20 / 0x294); estado |= 0x80; si indice < cursor (con signo) cursor = indice.
Funcion 11, CPlayerRelationshipRecorder::ClearRelationshipWithPlayer (0x61A150): la PRIMERA entrada (16 x {ped, relacion}) con ped == argumento: ped = 0 y solo el byte bajo de la relacion = 0.
Funcion 12, AddRelationship (0x61A180, `ret 8`): si el ped de la entrada 0 != 0: ped = argumento y byte bajo de la relacion = byte bajo del valor.
Funcion 13, GetRelationshipWithPlayer (0x61A1A0, AL): byte bajo de la relacion de la primera entrada con ped == argumento; 0 si no hay.
Funcion 14, Flush (0x61A2A0): las 16 entradas: ped = 0 y byte bajo de la relacion = 0.
Funcion 15, CEventScanner::ScanForEventsNow (0x6053D0, `ret 8`): dword del argumento != 0 -> nada; si no, si el byte +0xC (temporizador del escaner de vehiculos en +4) != 0: inicio = tiempo, intervalo = -1, byte = 1;
  y siempre llama a 0x603720 (ecx = escaner + 4; ped; entidades = inteligencia + 0xE0; 16).
Funcion 16, CTaskTimer::GetTimeLeft (0x62EB80, EAX): byte +8 != 0 -> (intervalo - tiempo) + inicio (32 bits); si no 0.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
(A_OBJ, A_OTHER, A_PED, A_INTEL, A_PED2, A_INTEL2, A_PTASK, A_POOL, A_STO, A_ST) = range(10)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 113001) & M32)


class Img:
    """imagen de las arenas: bytes explicitos del banco y el relleno pseudoaleatorio por defecto; escrituras registradas"""

    def __init__(self, seed):
        self.base = (seed * 13 + 50) & M32
        self.explicit = {}
        self.writes = {}

    def fill(self, arena, off):
        idx = off + GUARD
        w = rand((self.base + arena) & M32, idx // 4)
        return (w >> (8 * (idx & 3))) & 0xFF

    def put(self, arena, off, val, n):
        for j in range(n):
            self.explicit[(arena, off + j)] = (val >> (8 * j)) & 0xFF

    def initial(self, arena, off):
        k = (arena, off)
        return self.explicit[k] if k in self.explicit else self.fill(arena, off)

    def write(self, arena, off, val, n):
        for j in range(n):
            self.writes[(arena, off + j)] = (val >> (8 * j)) & 0xFF

    def changes(self):
        out = []
        for (arena, off) in sorted(self.writes):
            if self.writes[(arena, off)] != self.initial(arena, off):
                out.append([arena, off, self.writes[(arena, off)]])
        return out


def res(img=None, **kw):
    r = {'events': [], 'marks': [], 'changes': img.changes() if img else [], 'ret': None}
    r.update(kw)
    return r


def expected0(c, o):
    a = c['a']
    drv = bool(a[0] & 1)
    door8 = a[1] == 8
    marks = ['driver' if drv else 'passenger', 'door8' if door8 else 'other_door', 'result_10' if drv and door8 else 'result_0']
    return res(marks=marks, ret=10 if drv and door8 else 0)


def expected1(c, o):
    a = c['a']
    img = Img(c['seed'])
    img.put(A_OBJ, 0x10, a[0], 1)
    img.put(A_OBJ, 0x24, a[2], 1)
    marks = []
    if a[1] != a[0]:
        img.write(A_OBJ, 0x10, a[1] & 0xFF, 1)
        img.write(A_OBJ, 0x24, (a[2] | 2) & 0xFF, 1)
        marks.append('different')
        if (a[1] & 0xFF) == a[0]:
            marks.append('different_high_bits_only')
        if a[2] & 2:
            marks.append('flag_already_set')
    else:
        marks.append('equal')
    return res(img, marks=marks)


def expected2(c, o):
    a = c['a']
    img = Img(c['seed'])
    for k in range(4):
        img.put(A_OBJ, 0x18 + 2 * k, a[k], 2)
        img.put(A_OTHER, 0x18 + 2 * k, a[4 + k], 2)
    img.put(A_OBJ, 0x10, a[8], 1)
    img.put(A_OTHER, 0x10, a[9], 1)
    img.put(A_OBJ, 0x24, a[10], 1)
    diff = [a[k] != a[4 + k] for k in range(4)]
    marks = []
    if any(diff):
        for k in range(4):
            img.write(A_OBJ, 0x18 + 2 * k, a[4 + k], 2)
            if diff[k]:
                marks.append('diff_w%d' % k)
        img.write(A_OBJ, 0x10, a[9], 1)
        img.write(A_OBJ, 0x24, (a[10] | 4) & 0xFF, 1)
        if a[10] & 4:
            marks.append('flag_already_set')
        if sum(diff) == 4:
            marks.append('all_differ')
    else:
        marks.append('same')
    return res(img, marks=marks)


def expected3(c, o):
    a = c['a']
    img = Img(c['seed'])
    img.put(A_OBJ, 0x34, a[0], 4)
    img.write(A_OBJ, 0x34, a[1], 4)
    marks = ['same_value' if a[0] == a[1] else 'new_value']
    return res(img, marks=marks)


def expected4(c, o):
    a = c['a']
    ev = [[50, tag(A_INTEL2, 4), a[0]]]
    if a[1] == 0:
        return res(events=ev, marks=['no_partner_task'], ret=0)
    st = s8(a[2])
    marks = ['partner_task', 'negative_state' if st < 0 else 'state_%s' % ('ge5' if st >= 5 else 'lt5')]
    return res(events=ev, marks=marks, ret=st & M32)


def expected56(c, o):
    a = c['a']
    female = (a[0] & 0xFFFF) in (0x67, 0x69)
    base = 299 if c['fn'] == 5 else 300
    return res(marks=['female_model' if female else 'other_model'], ret=base + (2 if female else 0))


def pool_new(c, o):
    a = c['a']
    cap, last = a[0], s32(a[1])
    size = 0x20 if c['fn'] == 7 else 0x294
    img = Img(c['seed'])
    states = [a[2 + k] & 0xFF for k in range(8)]
    for k in range(8):
        img.put(A_ST, k, states[k], 1)
    img.put(A_POOL, 0xC, a[1], 4)
    wrapped = False
    ptr = 0
    marks = []
    while True:
        last += 1
        if last == cap:
            last = 0
            if wrapped:
                ptr = 0
                break
            wrapped = True
        if states[last] & 0x80:
            img.write(A_ST, last, ((states[last] & 0x7F) + 1) & 0x7F, 1)
            ptr = tag(A_STO, last * size)
            break
    img.write(A_POOL, 0xC, last & M32, 4)
    marks.append('allocated' if ptr else 'full')
    if wrapped:
        marks.append('wrapped')
    if s32(a[1]) == -1:
        marks.append('cursor_minus1')
    if ptr and (states[last] & 0x7F) == 0x7F:
        marks.append('id_wraps')
    return res(img, marks=marks, ret=ptr)


def pool_delete(c, o):
    a = c['a']
    last = s32(a[1])
    slot = a[10]
    img = Img(c['seed'])
    states = [a[2 + k] & 0xFF for k in range(8)]
    for k in range(8):
        img.put(A_ST, k, states[k], 1)
    img.put(A_POOL, 0xC, a[1], 4)
    img.write(A_ST, slot, states[slot] | 0x80, 1)
    marks = ['delete']
    if slot > last:
        marks.append('cursor_kept_below')
    elif slot == last:
        marks.append('cursor_equal')
    else:
        marks.append('cursor_lowered')
        img.write(A_POOL, 0xC, slot, 4)
    if a[11]:
        marks.append('interior')
    return res(img, marks=marks)


def rec_entries(a):
    ped = []
    rel = []
    for j in range(16):
        ped.append(a[0] if (a[1] >> j) & 1 else (0x20000000 | ((mix((a[2] + j) & M32) & 0xFFFF) << 8)))
        rel.append(mix((a[2] + 0x100 + j) & M32))
    if a[4]:
        ped[0] = 0
    return ped, rel


def rec_image(c):
    a = c['a']
    img = Img(c['seed'])
    ped, rel = rec_entries(a)
    for j in range(16):
        img.put(A_OBJ, 8 * j, ped[j], 4)
        img.put(A_OBJ, 8 * j + 4, rel[j], 4)
    return img, ped, rel


def first_match(ped, arg):
    for j in range(16):
        if ped[j] == arg:
            return j
    return None


def rec_marks(a, j):
    marks = ['null_arg' if a[0] == 0 else 'arg']
    marks.append('no_match' if j is None else ('match_first' if j == 0 else 'match_later'))
    if a[4]:
        marks.append('entry0_null')
    return marks


def expected11(c, o):
    a = c['a']
    img, ped, rel = rec_image(c)
    j = first_match(ped, a[0])
    if j is not None:
        img.write(A_OBJ, 8 * j, 0, 4)
        img.write(A_OBJ, 8 * j + 4, rel[j] & 0xFFFFFF00, 4)
    return res(img, marks=rec_marks(a, j))


def expected12(c, o):
    a = c['a']
    img, ped, rel = rec_image(c)
    marks = ['entry0_used' if ped[0] != 0 else 'entry0_empty']
    if ped[0] != 0:
        img.write(A_OBJ, 0, a[0], 4)
        img.write(A_OBJ, 4, (rel[0] & 0xFFFFFF00) | (a[3] & 0xFF), 4)
    if a[3] > 0xFF:
        marks.append('value_high_bits')
    return res(img, marks=marks)


def expected13(c, o):
    a = c['a']
    img, ped, rel = rec_image(c)
    j = first_match(ped, a[0])
    return res(img, marks=rec_marks(a, j), ret=0 if j is None else (rel[j] & 0xFF))


def expected14(c, o):
    a = c['a']
    img, ped, rel = rec_image(c)
    for j in range(16):
        img.write(A_OBJ, 8 * j, 0, 4)
        img.write(A_OBJ, 8 * j + 4, rel[j] & 0xFFFFFF00, 4)
    return res(img, marks=['flush'])


def expected15(c, o):
    a = c['a']
    img = Img(c['seed'])
    img.put(A_OBJ, 4, a[1], 4)
    img.put(A_OBJ, 8, a[2], 4)
    img.put(A_OBJ, 0xC, a[3], 1)
    if a[4] != 0:
        marks = ['dont_scan']
        if a[4] > 1:
            marks.append('dont_scan_garbage')
        return res(img, marks=marks)
    marks = ['scan']
    if a[3] & 0xFF:
        img.write(A_OBJ, 4, a[0], 4)
        img.write(A_OBJ, 8, M32, 4)
        img.write(A_OBJ, 0xC, 1, 1)
        marks.append('timer_started')
        if (a[3] & 0xFF) > 1:
            marks.append('started_garbage')
    else:
        marks.append('timer_stopped')
    ev = [[60, tag(A_OBJ, 4), tag(A_PED, 0), tag(A_INTEL, 0xE0), 16]]
    return res(img, events=ev, marks=marks)


def expected16(c, o):
    a = c['a']
    if a[2] & 0xFF:
        left = (a[1] - a[4] + a[0]) & M32
        marks = ['started', 'negative' if left & 0x80000000 else 'positive']
        if left == 0:
            marks.append('zero')
        if a[3] & 0xFF:
            marks.append('stopped_flag')
        return res(marks=marks, ret=left)
    return res(marks=['not_started'], ret=0)


def expected(c, o=None):
    fn = c['fn']
    if fn in (7, 9):
        return pool_new(c, o)
    if fn in (8, 10):
        return pool_delete(c, o)
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected56, 6: expected56, 11: expected11, 12: expected12, 13: expected13, 14: expected14,
            15: expected15, 16: expected16}[fn](c, o)


def fp_stack(hexstr):
    """(TOP, lista de (st, 10 bytes)) de las posiciones validas de una imagen fnsave normalizada"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return top, valid


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if e['ret'] is not None and o.get('ret') != e['ret']:
        return 'retorno %s != %s' % (o.get('ret'), e['ret'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    return None


def check_row(row):
    c = row['input']
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        e = expected(c, o)
        why = check_side(c, o, e, row[side])
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
