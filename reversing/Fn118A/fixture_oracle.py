"""Oraculo independiente (Python) de Fn118A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las ocho funciones del lote 118A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Funcion 0, CMemoryMgr::Malloc (0x72F420; cdecl, (tamano, hint)): `mov eax,[esp+4]; push eax; call 0x824257; add esp,4; ret`: malloc del CRT del EXE con el tamano tal cual; devuelve lo que devuelve (EAX). El hint se ignora.
Funcion 1, CMemoryMgr::Realloc (0x72F440; cdecl, (puntero, tamano, hint)): `push eax(tamano); push ecx(puntero); call 0x824269; add esp,8; ret`: realloc del CRT con (puntero, tamano) tal cual (nulo incluido); devuelve EAX.
Funcion 2, CMemoryMgr::Calloc (0x72F460; cdecl, (num, size, hint)): `push [esp+8]; push [esp+4]; call 0x824416`: calloc del CRT con (num, size) tal cual; devuelve EAX.
Funcion 3, CMemoryMgr::MallocAlign (0x72F4C0; cdecl, (tamano, align, hint)): malloc(tamano + align) (suma de 32 bits con desborde); r = (base + align) & ~(align - 1) (dec/not/and sobre ESI = align); [r - 4] = base; devuelve r.
Funcion 4, CMemoryMgr::FreeAlign (0x72F4F0; cdecl, (puntero)): ecx = [puntero - 4]; [esp+4] = ecx; `jmp 0x82413F` (free del CRT): libera el puntero guardado, no el recibido.
Funcion 5, CDecision::CDecision (0x6040C0; thiscall): `call 0x600530` (SetDefault: seis dwords de tareas a -1 en +0..0x17, seis dwords de probabilidades a 0 en +0x18..0x2F, seis words de banderas a 0 en +0x30..0x3B);
  `mov eax,ecx; ret`: devuelve this.
Funcion 6, CTaskComplexLeaveCar::ComputeTargetDoor (0x63BAB0; thiscall, `ret 4`, (ped)): si [this+0x10] (puerta) == 0 -> [this+0x10] = ComputeTargetDoorToExit (0x64F110, cdecl) con (vehiculo = [this+0xC], ped); si no, no hace nada.
Funcion 7, CWaterLevel::CalculateWavesOnlyForCoordinate2 (0x6E8550; cdecl, (x, y, altura*, grandes, pequenas)): llama a 0x6E7210 (cdecl) con (x, y, grandes, pequenas, altura*) y vuelve; no toca memoria por si misma.

Eventos de los falsos (todos con todos sus argumentos): 40 free [puntero]; 50 malloc [tamano]; 51 realloc [puntero, tamano]; 52 calloc [num, size]; 60 ComputeTargetDoorToExit [vehiculo, ped];
70 CalculateWavesOnlyForCoordinate2Native [x, y, grandes, pequenas, altura*].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
A_OBJ, A_A, A_B = range(3)
M32 = 0xFFFFFFFF


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def ptr(kind, raw, arena, off=0):
    """valor que registran los falsos para un puntero: etiqueta de arena, 0 o el valor crudo"""
    return tag(arena, off) if kind == 0 else (0 if kind == 1 else raw & M32)


def byte_changes(arena, base_off, old_words, new_words):
    """cambios de bytes [arena, desplazamiento, byte nuevo] entre dos listas de dwords consecutivos (desde base_off)"""
    out = []
    for k, (o, n) in enumerate(zip(old_words, new_words)):
        for j in range(4):
            ob, nb = (o >> (8 * j)) & 0xFF, (n >> (8 * j)) & 0xFF
            if ob != nb:
                out.append([arena, base_off + 4 * k + j, nb])
    return out


KIND = ('arena', 'null', 'raw')


def size_marks(prefix, v, marks):
    if v == 0:
        marks.append(prefix + '0')
    if v >= 0x80000000:
        marks.append(prefix + '_big')


def expected0(c):
    a = [x & M32 for x in c['a']]
    marks = ['ret_' + KIND[a[2]]]
    size_marks('size', a[0], marks)
    return {'events': [[50, a[0]]], 'marks': marks, 'changes': [], 'ret': ptr(a[2], a[3], A_B, a[4])}


def expected1(c):
    a = [x & M32 for x in c['a']]
    marks = ['ptr_' + KIND[a[0]], 'ret_' + KIND[a[5]]]
    size_marks('size', a[3], marks)
    return {'events': [[51, ptr(a[0], a[1], A_A, a[2]), a[3]]], 'marks': marks, 'changes': [], 'ret': ptr(a[5], a[6], A_B, a[7])}


def expected2(c):
    a = [x & M32 for x in c['a']]
    marks = ['ret_' + KIND[a[3]]]
    if a[0] == 0:
        marks.append('num0')
    if a[1] == 0:
        marks.append('size0')
    if a[0] * a[1] >= 1 << 32:
        marks.append('overflow')
    return {'events': [[52, a[0], a[1]]], 'marks': marks, 'changes': [], 'ret': ptr(a[3], a[4], A_B, a[5])}


def expected3(c):
    a = [x & M32 for x in c['a']]
    size, align, base_off = a[0], a[1], a[3]
    pos = GUARD + base_off
    res = ((pos + align) & M32) & (~(align - 1) & M32)
    marks = ['align_pow2' if align & (align - 1) == 0 else 'align_odd']
    if align == 1:
        marks.append('align1')
    if align >= 0x100:
        marks.append('align_big')
    if base_off == 0:
        marks.append('base0')
    if size + align >= 1 << 32:
        marks.append('wrap')
    if size == 0:
        marks.append('size0')
    # los cuatro bytes de [r - 4] contienen la direccion absoluta de la base (no se comparan como bytes: se compara su etiqueta en `ptrs`)
    return {'events': [[50, (size + align) & M32]], 'marks': marks, 'changes': [], 'ret': 0x10000000 + (A_OBJ << 20) + res, 'ptrs': [tag(A_OBJ, base_off)]}


def expected4(c):
    a = [x & M32 for x in c['a']]
    marks = ['val_' + KIND[a[1]]]
    if a[0] == 4:
        marks.append('ptr4')
    if a[0] >= 0x80:
        marks.append('ptr_far')
    return {'events': [[40, ptr(a[1], a[2], A_A, a[3])]], 'marks': marks, 'changes': [], 'ret': None}


def expected5(c):
    a = [x & M32 for x in c['a']]
    off = a[0]
    old = a[2:17]
    new = [M32] * 6 + [0] * 9
    changes = byte_changes(A_OBJ, off, old, new)
    marks = ['off0' if off == 0 else ('off_unaligned' if off % 4 else 'off_aligned'), 'fill%d' % a[1]]
    if not changes:
        marks.append('noop')
    elif any(o == n for o, n in zip(old, new)):
        marks.append('partial')
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': tag(A_OBJ, off)}


def expected6(c):
    a = [x & M32 for x in c['a']]
    off, door, r = a[0], a[5], a[6]
    veh = ptr(a[1], a[2], A_A)
    ped = ptr(a[3], a[4], A_B)
    marks = ['veh_' + KIND[a[1]], 'ped_' + KIND[a[3]]]
    events, changes = [], []
    if door == 0:
        marks.append('door0')
        events = [[60, veh, ped]]
        changes = byte_changes(A_OBJ, off + 0x10, [0], [r])
        if r == 0:
            marks.append('ret0')
        if r >= 0x80000000:
            marks.append('ret_neg')
    else:
        marks.append('door_set')
    return {'events': events, 'marks': marks, 'changes': changes, 'ret': None}


def expected7(c):
    a = [x & M32 for x in c['a']]
    marks = ['h_' + KIND[a[2]]]
    if a[0] >= 0x80000000:
        marks.append('x_neg')
    if a[1] >= 0x80000000:
        marks.append('y_neg')
    if a[0] == 0x80000000:
        marks.append('x_min')
    for name, v in (('big', a[5]), ('small', a[6])):
        if (v & 0x7F800000) == 0x7F800000 and (v & 0x7FFFFF):
            marks.append(name + '_nan')
    return {'events': [[70, a[0], a[1], a[5], a[6], ptr(a[2], a[3], A_A, a[4])]], 'marks': marks, 'changes': [], 'ret': None}


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected5, 6: expected6, 7: expected7}[c['fn']](c)


def fp_stack_valid(hexstr):
    """posiciones validas de una imagen fnsave normalizada (la pila x87 debe quedar vacia)"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return valid


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    if 'ptrs' in e and o.get('ptrs') != e['ptrs']:
        return 'ptrs %s != %s' % (o.get('ptrs'), e['ptrs'])
    valid = fp_stack_valid(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
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
