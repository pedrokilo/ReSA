"""Oraculo independiente (Python) de Fn125A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los trece destructores con borrado del lote 125A
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico). Las trece funciones (30 bytes) tienen la misma forma:
    push esi; mov esi,ecx; call BODY; test byte [esp+8],1; je +; push esi; call DELETE; add esp,4; mov eax,esi; pop esi; ret 4
  (BODY = cuerpo del destructor de la clase, thiscall sin argumentos; DELETE = operator delete de la clase, cdecl con `this`; el bit 0 del BYTE de banderas manda; devuelve this; `ret 4`.)
  idx  direccion  clase                                   BODY                      DELETE
   0   0x6F1120   CBoat                                   0x6F00F0                  0x6E2D90 (CVehicle::operator delete: marca el hueco libre del pool de vehiculos 0xB74494)
   1   0x5B1DE0   CCutsceneObject                         0x5B1A80 (vtable 0x868A60, jmp ~CObject 0x59F660)   0x5A1F20 (CObject::operator delete, pool 0xB7449C)
   2   0x59EB50   CDummyObject                            0x59EAB0 (vtable 0x866E78, jmp ~CDummy 0x532560)    0x5326F0 (CDummy::operator delete, pool 0xB744A0)
   3   0x6C7F90   CMonsterTruck                           0x6C7D10 (vtable 0x8717D8, jmp ~CAutomobile 0x6A61E0) 0x6E2D90
   4   0x4994C0   CTaskComplexBeCop                       0x4994E0                  0x61A5B0 (CTask::operator delete, pool 0xB744A8)
   5   0x638AC0   CTaskComplexUsePairedAttractor          0x6331E0                  0x61A5B0   (el listado decia CTaskComplexBeInGroup)
   6   0x5F66E0   CTaskComplexBeStill                     0x5F6700                  0x61A5B0
   7   0x4B89D0   CTaskComplexCarDriveMissionFleeScene    0x4B89F0                  0x61A5B0
   8   0x4B8AB0   CTaskComplexCarDriveMissionKillPed      0x4B8AD0                  0x61A5B0
   9   0x46A6C0   CTaskComplexClimb                       0x46A6E0                  0x61A5B0
  10   0x6289D0   CTaskComplexDestroyCarMelee             0x621DA0                  0x61A5B0   (el listado decia CTaskComplexDestroyCar)
  11   0x628C60   CTaskComplexDestroyCarArmed             0x622010                  0x61A5B0   (el listado decia CTaskComplexDestroyCarMelee)
  12   0x629060   CTaskComplexRoadRage                    0x622110                  0x61A5B0   (el listado decia CTaskComplexDestroyCarArmed)

Banco: BODY y DELETE se sustituyen por falsos identicos en el EXE y en el cuerpo de la DLL. El falso de BODY registra [direccion de BODY, ECX (etiqueta de arena), dword que tenia [ECX] al entrar], deja
en [ECX] el dword del caso (a[8], la vtable de la base) y devuelve en EAX un valor del caso (a[9], que NO es this). El falso de DELETE registra [direccion de DELETE, argumento (etiqueta), dword de [argumento]
al entrar] (es decir, lo que dejo el cuerpo: el borrado va DESPUES de destruir) y no libera nada. Valor devuelto = this (la etiqueta de arena) siempre; sin tocar la x87.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CBoat::ScalarDeletingDestructor', 'CCutsceneObject::ScalarDeletingDestructor', 'CDummyObject::ScalarDeletingDestructor', 'CMonsterTruck::ScalarDeletingDestructor',
         'CTaskComplexBeCop::ScalarDeletingDestructor', 'CTaskComplexUsePairedAttractor::ScalarDeletingDestructor', 'CTaskComplexBeStill::ScalarDeletingDestructor',
         'CTaskComplexCarDriveMissionFleeScene::ScalarDeletingDestructor', 'CTaskComplexCarDriveMissionKillPed::ScalarDeletingDestructor', 'CTaskComplexClimb::ScalarDeletingDestructor',
         'CTaskComplexDestroyCarMelee::ScalarDeletingDestructor', 'CTaskComplexDestroyCarArmed::ScalarDeletingDestructor', 'CTaskComplexRoadRage::ScalarDeletingDestructor']
ENTRIES = (0x6F1120, 0x5B1DE0, 0x59EB50, 0x6C7F90, 0x4994C0, 0x638AC0, 0x5F66E0, 0x4B89D0, 0x4B8AB0, 0x46A6C0, 0x6289D0, 0x628C60, 0x629060)
BODY = (0x6F00F0, 0x5B1A80, 0x59EAB0, 0x6C7D10, 0x4994E0, 0x6331E0, 0x5F6700, 0x4B89F0, 0x4B8AD0, 0x46A6E0, 0x621DA0, 0x622010, 0x622110)
DELETE = (0x6E2D90, 0x5A1F20, 0x5326F0, 0x6E2D90) + (0x61A5B0,) * 9
VTABLE = (0x8721A0, 0x868A60, 0x866E78, 0x8717D8, 0x85A428, 0x86E258, 0x86C7BC, 0x85B95C, 0x85B994, 0x859DCC, 0x86D994, 0x86D9C4, 0x86D9F0)
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
