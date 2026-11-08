"""Oraculo independiente (Python) de Fn121A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las seis funciones del lote 121A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables (leidas del PE y de los llamadores):
  0x69BB40  C7 01 30 0E 87 00 C3           mov dword ptr [ecx],870E30h; ret          cuerpo de ~CTaskAllocator (jmp desde ocho funclets de limpieza SEH, 0x847B43..0x847D13)
  0x46A610  C7 01 C4 9D 85 00 C3           mov dword ptr [ecx],859DC4h; ret          cuerpo de ~CEntitySeekPosCalculator (jmp desde 0x837E3E, 0x837E5E y 0x83E28B)
  0x5F6480  8B C1 C7 00 64 C7 86 00 C3     mov eax,ecx; mov [eax],86C764h; ret       ctor de FollowLimited (GetType, ranura 1 = 0x5F64A0, devuelve 1; llamada desde 0x851295)
  0x5F64B0  8B C1 C7 00 6C C7 86 00 C3     mov eax,ecx; mov [eax],86C76Ch; ret       ctor de StandStill    (GetType 0x5F64D0 devuelve 2; llamada desde 0x8512B5)
  0x5F64E0  8B C1 C7 00 74 C7 86 00 C3     mov eax,ecx; mov [eax],86C774h; ret       ctor de Chat          (GetType 0x5F6500 devuelve 3; llamada desde 0x8512D5)
  0x5F6540  8B C1 C7 00 84 C7 86 00 C3     mov eax,ecx; mov [eax],86C784h; ret       ctor de SitInLeaderCar (GetType 0x5F6560 devuelve 4; llamada desde 0x851315)
Cada una escribe un dword (la vtable) en [this] y vuelve: no lee memoria, no llama a nada, no toca la pila. Los constructores devuelven this en EAX; en los cuerpos de destructor EAX no esta definido
(el EXE lo deja como entro; la DLL puede dejarlo distinto: no se compara). Se comprueba el dword escrito, los bytes cambiados (solo los que difieren de la imagen previa, incluidos los que caen en la
guarda posterior de la arena) y, en los constructores, `this` devuelto como etiqueta de arena (0x10000000 + 0x40 de guarda + desplazamiento).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskAllocator::Destructor', 'CEntitySeekPosCalculator::Destructor', 'CPedGroupDefaultTaskAllocatorFollowLimited::Constructor', 'CPedGroupDefaultTaskAllocatorStandStill::Constructor',
         'CPedGroupDefaultTaskAllocatorChat::Constructor', 'CPedGroupDefaultTaskAllocatorSitInLeaderCar::Constructor']
ENTRIES = (0x69BB40, 0x46A610, 0x5F6480, 0x5F64B0, 0x5F64E0, 0x5F6540)
VTABLE = (0x870E30, 0x859DC4, 0x86C764, 0x86C76C, 0x86C774, 0x86C784)
RETURNS_THIS = (False, False, True, True, True, True)
GUARD = 0x40
OBJ_WINDOW = 0x100


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    off, kind, fill, prev = a[0], a[1], a[2], a[6]
    vt = VTABLE[fn]
    new = [(vt >> (8 * k)) & 0xFF for k in range(4)]
    old = [(prev >> (8 * k)) & 0xFF for k in range(4)]
    changes = [[0, off + k, new[k]] for k in range(4) if new[k] != old[k]]
    marks = ['ctor' if RETURNS_THIS[fn] else 'dtor', 'fill%d' % fill, 'kind%d' % kind, 'changes%d' % len(changes)]
    marks.append('this_unaligned' if off % 4 else 'this_aligned')
    if off + 4 > OBJ_WINDOW:
        marks.append('this_spill')
    if prev == vt:
        marks.append('prev_same')
    elif prev == 0:
        marks.append('prev_zero')
    elif prev == M32:
        marks.append('prev_ones')
    elif prev == 0xDEADBEEF:
        marks.append('prev_deadbeef')
    elif prev in VTABLE:
        marks.append('prev_other_vt')
    elif 0 < len(changes) < 4:
        marks.append('prev_partial')
    else:
        marks.append('prev_other')
    ret = 0x10000000 + GUARD + off if RETURNS_THIS[fn] else None
    return {'events': [], 'marks': marks, 'changes': changes, 'ret': ret, 'vptr': vt}


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
        return 'eventos %s != %s' % (o['events'][:2], e['events'][:2])
    if o.get('vptr') != e['vptr']:
        return 'vtable escrita %s != %s' % (o.get('vptr'), e['vptr'])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
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
