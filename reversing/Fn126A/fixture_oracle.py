"""Oraculo independiente (Python) de Fn126A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los trece destructores con borrado del lote 126A (clases de tarea, todas con el mismo operator delete CTask 0x61A5B0)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico). Las trece funciones (30 bytes) tienen la misma forma:
    push esi; mov esi,ecx; call BODY; test byte [esp+8],1; je +; push esi; call DELETE; add esp,4; mov eax,esi; pop esi; ret 4
  (BODY = cuerpo del destructor de la clase, thiscall sin argumentos; DELETE = operator delete de la clase, cdecl con `this`; el bit 0 del BYTE de banderas manda; devuelve this; `ret 4`.)
  idx  direccion  clase                                   BODY                      DELETE
   0   0x640590   CTaskComplexEnterCarAsDriverTimed      0x63AE00                  0x61A5B0
   1   0x640F10   CTaskComplexStealCar                   0x63B850                  0x61A5B0
   2   0x640640   CTaskComplexEnterCarAsPassengerTimed   0x63B0C0                  0x61A5B0
   3   0x640710   CTaskComplexEnterCarAsPassengerWait    0x63B3C0                  0x61A5B0
   4   0x640BC0   CTaskComplexLeaveCarAsPassengerWait    0x63B4A0                  0x61A5B0
   5   0x6560C0   CTaskComplexHitPedWithCar              0x653A30                  0x61A5B0
   6   0x697820   CTaskComplexInvestigateDeadPed         0x696E80                  0x61A5B0
   7   0x6629F0   CTaskComplexGangFollower               0x65EBB0                  0x61A5B0
   8   0x64ABE0   CTaskSimpleCarAlign                    0x645BE0                  0x61A5B0
   9   0x697B50   CTaskComplexReactToGunAimedAt          0x697230                  0x61A5B0
  10   0x68F9F0   CTaskComplexCopInCar                   0x68C8C0                  0x61A5B0
  11   0x6296B0   CTaskSimpleStealthKill                 0x622680                  0x61A5B0
  12   0x626FA0   CTaskComplexKillPedOnFootArmed         0x621250                  0x61A5B0

Banco: BODY y DELETE se sustituyen por falsos identicos en el EXE y en el cuerpo de la DLL. El falso de BODY registra [direccion de BODY, ECX (etiqueta de arena), dword que tenia [ECX] al entrar], deja
en [ECX] el dword del caso (a[8], la vtable de la base) y devuelve en EAX un valor del caso (a[9], que NO es this). El falso de DELETE registra [direccion de DELETE, argumento (etiqueta), dword de [argumento]
al entrar] (es decir, lo que dejo el cuerpo: el borrado va DESPUES de destruir) y no libera nada. Valor devuelto = this (la etiqueta de arena) siempre; sin tocar la x87.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskComplexEnterCarAsDriverTimed::ScalarDeletingDestructor', 'CTaskComplexStealCar::ScalarDeletingDestructor', 'CTaskComplexEnterCarAsPassengerTimed::ScalarDeletingDestructor', 'CTaskComplexEnterCarAsPassengerWait::ScalarDeletingDestructor', 'CTaskComplexLeaveCarAsPassengerWait::ScalarDeletingDestructor', 'CTaskComplexHitPedWithCar::ScalarDeletingDestructor', 'CTaskComplexInvestigateDeadPed::ScalarDeletingDestructor', 'CTaskComplexGangFollower::ScalarDeletingDestructor', 'CTaskSimpleCarAlign::ScalarDeletingDestructor', 'CTaskComplexReactToGunAimedAt::ScalarDeletingDestructor', 'CTaskComplexCopInCar::ScalarDeletingDestructor', 'CTaskSimpleStealthKill::ScalarDeletingDestructor', 'CTaskComplexKillPedOnFootArmed::ScalarDeletingDestructor']
ENTRIES = (0x640590, 0x640F10, 0x640640, 0x640710, 0x640BC0, 0x6560C0, 0x697820, 0x6629F0, 0x64ABE0, 0x697B50, 0x68F9F0, 0x6296B0, 0x626FA0)
BODY = (0x63AE00, 0x63B850, 0x63B0C0, 0x63B3C0, 0x63B4A0, 0x653A30, 0x696E80, 0x65EBB0, 0x645BE0, 0x697230, 0x68C8C0, 0x622680, 0x621250)
DELETE = (0x61A5B0,) * 13
VTABLE = (0x86E720, 0x86E7FC, 0x86E74C, 0x86E778, 0x86E7A4, 0x86F294, 0x870D3C, 0x86F938, 0x86EC78, 0x870D78, 0x870A2C, 0x86DA74, 0x86D918)
GUARD = 0x40
WIN = 0x1000
SPAN = 4


def t0(off):
    """etiqueta de un puntero dentro de la arena del objeto (arena 0): 0x10000000 + guarda + desplazamiento"""
    return 0x10000000 + GUARD + off


def dword_bytes(v):
    return [(v >> (8 * k)) & 0xFF for k in range(4)]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    kind, fill = a[1], a[2]
    marks = ['fill%d' % fill, 'kind%d' % kind]
    out = {'events': [], 'changes': [], 'ret': None, 'marks': marks}
    off = a[0]
    marks.append('this_unaligned' if off % 4 else 'this_aligned')
    if off + SPAN > WIN:
        marks.append('this_spill')
    prev, new, flags = a[6], a[8], a[7]
    out['events'] = [[BODY[fn], t0(off), prev]]
    old_b, new_b = dword_bytes(prev), dword_bytes(new)
    out['changes'] = [[0, off + k, new_b[k]] for k in range(4) if new_b[k] != old_b[k]]
    if flags & 1:
        out['events'].append([DELETE[fn], t0(off), new])
    out['ret'] = t0(off)
    marks.append('delete' if flags & 1 else 'keep')
    marks.append('flags_zero' if flags == 0 else 'flags_one' if flags == 1 else 'flags_bit0_more' if flags & 1 else 'flags_upper_only' if flags & ~0xFF and not flags & 0xFF else 'flags_even')
    marks.append('upper_set_bit0' if (flags & 1) and (flags & ~0xFF) else 'upper_clear_bit0' if flags & 1 else 'bit0_clear')
    marks.append('prev_own' if prev == VTABLE[fn] else 'prev_zero' if prev == 0 else 'prev_ones' if prev == M32 else 'prev_other')
    marks.append('changes0' if not out['changes'] else 'changes4' if len(out['changes']) == 4 else 'changes_some')
    return out


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    # sin x87: la pila de coma flotante debe quedar vacia (todas las etiquetas vacias)
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
