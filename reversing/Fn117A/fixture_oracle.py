"""Oraculo independiente (Python) de Fn117A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 14 funciones del lote 117A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion ni de Fixtures.hpp: la tabla de funciones (vtables, desplazamientos, argumentos y
colaborador) se reescribe aqui desde el desensamblado (dumpbin /disasm del EXE canonico).

Constructores (0x4AD250, 0x4B0700, 0x4ADEA0, 0x4B1B60, 0x4AF130, 0x4AF240, 0x4AE1A0; thiscall, ECX = evento, argumentos en la pila, `ret 4` / `ret 0Ch`, EAX = this): marco SEH que solo desenrolla;
  [+4] = 0, byte [+8] = 0 y, en los CEventEditableResponse, byte [+0xC] = 1, palabra [+0xE] = 0xC8, palabra [+0x10] = 0xFFFF; [+0] = vtable propia; los argumentos en sus campos (punteros, byte bajo o dword);
  para cada referencia, en el orden del EXE, si el puntero no es nulo 0x571B70(puntero, &campo) (CEntity::RegisterReference). No escribe el relleno. EAX = this.
  GetEventType / GetEventPriority / GetLifeTime de la vtable que queda escrita: (7, 40, 0), (31, 50, 0), (11, 15, 0), (43, 37, 0), (25, 1, 0), (26, 2, 0), (12, 51, 0).
Cuerpo de los destructores (0x4AD330, 0x4B07B0, 0x4ADF70, 0x4B1C10, 0x4AF1C0, 0x4AF2D0, 0x4AE270; thiscall, ECX = evento, `ret`): [+0] = vtable propia; para cada puntero de la lista (en orden), si no es nulo,
  0x571A00(puntero, &campo) (CEntity::CleanUpOldReference); [+0] = 0x85AB38 (vtable de CEvent).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
A_EV, A_ENTA, A_ENTB = range(3)
RAW1, RAW2 = 0x00BADC00, 0x00BADC40
CEVENT = 0x85AB38
PRE_VT = 0xDEADBEEF

NAMES = [
    'CEventDraggedOutCar::Constructor', 'CEventDraggedOutCar::Destructor', 'CEventGunAimedAt::Constructor', 'CEventGunAimedAt::Destructor', 'CEventDeadPed::Constructor', 'CEventDeadPed::Destructor',
    'CEventGotKnockedOverByCar::Constructor', 'CEventGotKnockedOverByCar::Destructor', 'CEventPedToChase::Constructor', 'CEventPedToChase::Destructor', 'CEventPedToFlee::Constructor',
    'CEventPedToFlee::Destructor', 'CEventPotentialGetRunOver::Constructor', 'CEventPotentialGetRunOver::Destructor']
ENTRIES = [0x4AD250, 0x4AD330, 0x4B0700, 0x4B07B0, 0x4ADEA0, 0x4ADF70, 0x4B1B60, 0x4B1C10, 0x4AF130, 0x4AF1C0, 0x4AF240, 0x4AF2D0, 0x4AE1A0, 0x4AE270]
CLEAN, REG = 0x571A00, 0x571B70
# (vtable propia, [(desplazamiento del puntero, colaborador)]) leidos de `mov [esi], vtable` y `lea eax, [esi+off]` del desensamblado de cada destructor
DTORS = {
    0x4AD330: (0x85AD30, [(0x18, CLEAN), (0x14, CLEAN)]),
    0x4B07B0: (0x85B330, [(0x14, CLEAN)]),
    0x4ADF70: (0x85AE00, [(0x14, CLEAN)]),
    0x4B1C10: (0x85B618, [(0x14, CLEAN)]),
    0x4AF1C0: (0x85B138, [(0x0C, CLEAN)]),
    0x4AF2D0: (0x85B178, [(0x0C, CLEAN)]),
    0x4AE270: (0x85AE88, [(0x14, CLEAN)]),
}
# constructores: vtable propia, editable, argumentos de pila en orden [(tipo, desplazamiento)] con tipo 'p' puntero, 'b' byte bajo, 'd' dword; indices de argumento registrados en orden; (tipo, prioridad, vida) de la vtable
CTORS = {
    0x4AD250: (0x85AD30, True, [('p', 0x18), ('p', 0x14), ('b', 0x1C)], [0, 1], (7, 40, 0)),
    0x4B0700: (0x85B330, True, [('p', 0x14)], [0], (31, 50, 0)),
    0x4ADEA0: (0x85AE00, True, [('p', 0x14), ('b', 0x18), ('d', 0x1C)], [0], (11, 15, 0)),
    0x4B1B60: (0x85B618, True, [('p', 0x14)], [0], (43, 37, 0)),
    0x4AF130: (0x85B138, False, [('p', 0x0C)], [0], (25, 1, 0)),
    0x4AF240: (0x85B178, False, [('p', 0x0C)], [0], (26, 2, 0)),
    0x4AE1A0: (0x85AE88, True, [('p', 0x14)], [0], (12, 51, 0)),
}


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def le32(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def changes_from(pre_hex, arena, new):
    """[[arena, desplazamiento, byte nuevo]] de los bytes que difieren de la imagen previa; `new` = {desplazamiento: byte}"""
    pre = bytes.fromhex(pre_hex)
    return [[arena, off, b] for off, b in sorted(new.items()) if pre[off] != b]


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


def ptr_value(i, mode):
    return [tag(A_ENTA if i == 0 else A_ENTB, 0), 0, RAW1 if i == 0 else RAW2][mode]


def expected_dtor(c, pre):
    a = c['a']
    vt, refs = DTORS[ENTRIES[c['fn']]]
    modes = [a[0], a[1]]
    marks = []
    ev = []
    for i, (off, coll) in enumerate(refs):
        p = ptr_value(i, modes[i])
        marks.append('ref%d_%s' % (i + 1, ['arena', 'null', 'raw'][modes[i]]))
        if p:
            ev.append([20, p, tag(A_EV, off), vt])
    if len(refs) == 2 and (modes[0] != 1) and (modes[1] != 1):
        marks.append('both_refs')
    if len(refs) == 2 and modes[0] == 1 and modes[1] == 1:
        marks.append('no_refs')
    marks.append('vt_own' if a[2] == vt else ('vt_base' if a[2] == CEVENT else 'vt_other'))
    new = {j: b for j, b in enumerate(le32(CEVENT))}
    return {'events': ev, 'marks': marks, 'changes': changes_from(pre, A_EV, new)}


def expected_ctor(c, pre):
    a = c['a']
    vt, editable, args, regs, vtab = CTORS[ENTRIES[c['fn']]]
    new = {}
    for j in range(4, 8):
        new[j] = 0
    new[8] = 0
    if editable:
        new[0xC] = 1
        new[0xE], new[0xF] = 0xC8, 0
        new[0x10], new[0x11] = 0xFF, 0xFF
    marks = ['editable' if editable else 'plain']
    ptrs = []
    vals = []
    for i, (kind, off) in enumerate(args):
        if kind == 'p':
            p = ptr_value(i, a[i])
            vals.append(p)
            ptrs.append(p)
            marks.append('arg%d_%s' % (i + 1, ['arena', 'null', 'raw'][a[i]]))
        elif kind == 'b':
            new[off] = a[i] & 0xFF
            vals.append(a[i] & 0xFF)
            marks.append('byte_zero' if a[i] & 0xFF == 0 else ('byte_one' if a[i] & 0xFF == 1 else 'byte_other'))
            if a[i] & ~0xFF & M32:
                marks.append('byte_dirty_high')
        else:
            for j, b in enumerate(le32(a[i])):
                new[off + j] = b
            vals.append(a[i])
            marks.append('dword_zero' if a[i] == 0 else 'dword_other')
    ev = []
    for i in regs:
        p = vals[i]
        if p:
            ev.append([11, p, tag(A_EV, args[i][1]), 1])
    ch = changes_from(pre, A_EV, new)
    ptr_offs = [off for (kind, off) in args if kind == 'p']
    # los bytes de los punteros y de la vtable no se comparan (se informan aparte)
    ch = [x for x in ch if x[1] >= 4 and not any(po <= x[1] < po + 4 for po in ptr_offs)]
    return {'events': ev, 'marks': marks, 'changes': ch, 'ret': tag(A_EV, 0), 'ptrs': ptrs, 'vt': list(vtab), 'vptr_changed': True}


def expected(c, obs=None):
    kind = ENTRIES[c['fn']]
    pre = obs['pre'] if obs else '00' * 0x40
    if kind in CTORS:
        return expected_ctor(c, pre)
    return expected_dtor(c, pre)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    for k in ('ret', 'ptrs', 'vt', 'vptr_changed'):
        if k in e and o.get(k) != e[k]:
            return '%s: %s != %s' % (k, o.get(k), e[k])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    return None


def check_row(row):
    c = row['input']
    for side in ('original', 'candidate'):
        obs = row[side]['observed']
        e = expected(c, obs)
        why = check_side(c, obs, e, row[side])
        if why:
            return side, why
    return None


def marks_of(row):
    return expected(row['input'], row['original']['observed'])['marks']


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 12:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
