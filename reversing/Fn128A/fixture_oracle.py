"""Oraculo independiente (Python) de Fn128A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los veintiun destructores con borrado del lote 128A (clases de tarea, todas con el mismo operator delete CTask 0x61A5B0)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico). Las veintiun funciones (30 bytes) tienen la misma forma:
    push esi; mov esi,ecx; call BODY; test byte [esp+8],1; je +; push esi; call DELETE; add esp,4; mov eax,esi; pop esi; ret 4
  (BODY = cuerpo del destructor de la clase, thiscall sin argumentos; DELETE = operator delete de la clase, cdecl con `this`; el bit 0 del BYTE de banderas manda; devuelve this; `ret 4`.)
  idx  direccion  clase                                   BODY                      DELETE
   0   0x64B2B0   CTaskSimpleCarGetIn                        0x646710                 0x61A5B0
   1   0x642F80   CTaskComplexCarDrive                       0x63CA40                 0x61A5B0
   2   0x46F670   CTaskSimpleCarDriveTimed                   0x46F690                 0x61A5B0
   3   0x64B3C0   CTaskSimpleCarShuffle                      0x646890                 0x61A5B0
   4   0x64ADE0   CTaskSimpleCarOpenLockedDoorFromOutside    0x6460F0                 0x61A5B0
   5   0x64AE70   CTaskSimplePickUpBike                      0x646230                 0x61A5B0
   6   0x64BDC0   CTaskSimpleCarGetOut                       0x647260                 0x61A5B0
   7   0x64B930   CTaskSimpleCarSetPedInAsDriver             0x647170                 0x61A5B0
   8   0x64C060   CTaskSimpleCarSlowBeDraggedOut             0x6482C0                 0x61A5B0
   9   0x655FE0   CTaskComplexEvasiveDiveAndGetUp            0x653740                 0x61A5B0
  10   0x6259C0   CTaskSimpleThrowProjectile                 0x61F700                 0x61A5B0
  11   0x67FA60   CTaskComplexStuckInAir                     0x67BA70                 0x61A5B0
  12   0x656AE0   CTaskSimpleHurtPedWithCar                  0x653F30                 0x61A5B0
  13   0x4635F0   CTaskSimpleNone                            0x463560                 0x61A5B0
  14   0x64AF30   CTaskSimpleCarCloseDoorFromInside          0x646380                 0x61A5B0
  15   0x636F00   CTaskSimpleIKChain                         0x633A90                 0x61A5B0
  16   0x48E4C0   CTaskSimpleSay                             0x48E4E0                 0x61A5B0
  17   0x692E30   CTaskSimpleFacial                          0x690CB0                 0x61A5B0
  18   0x637A40   CTaskSimpleSitIdle                         0x6311B0                 0x61A5B0
  19   0x637D40   CTaskSimpleStandUp                         0x631320                 0x61A5B0
  20   0x637D90   CTaskComplexSitDownThenIdleThenStandUp     0x6314A0                 0x61A5B0

Banco: BODY y DELETE se sustituyen por falsos identicos en el EXE y en el cuerpo de la DLL. El falso de BODY registra [direccion de BODY, ECX (etiqueta de arena), dword que tenia [ECX] al entrar], deja
en [ECX] el dword del caso (a[8], la vtable de la base) y devuelve en EAX un valor del caso (a[9], que NO es this). El falso de DELETE registra [direccion de DELETE, argumento (etiqueta), dword de [argumento]
al entrar] (es decir, lo que dejo el cuerpo: el borrado va DESPUES de destruir) y no libera nada. Valor devuelto = this (la etiqueta de arena) siempre; sin tocar la x87.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskSimpleCarGetIn::ScalarDeletingDestructor', 'CTaskComplexCarDrive::ScalarDeletingDestructor', 'CTaskSimpleCarDriveTimed::ScalarDeletingDestructor', 'CTaskSimpleCarShuffle::ScalarDeletingDestructor', 'CTaskSimpleCarOpenLockedDoorFromOutside::ScalarDeletingDestructor', 'CTaskSimplePickUpBike::ScalarDeletingDestructor', 'CTaskSimpleCarGetOut::ScalarDeletingDestructor', 'CTaskSimpleCarSetPedInAsDriver::ScalarDeletingDestructor', 'CTaskSimpleCarSlowBeDraggedOut::ScalarDeletingDestructor', 'CTaskComplexEvasiveDiveAndGetUp::ScalarDeletingDestructor', 'CTaskSimpleThrowProjectile::ScalarDeletingDestructor', 'CTaskComplexStuckInAir::ScalarDeletingDestructor', 'CTaskSimpleHurtPedWithCar::ScalarDeletingDestructor', 'CTaskSimpleNone::ScalarDeletingDestructor', 'CTaskSimpleCarCloseDoorFromInside::ScalarDeletingDestructor', 'CTaskSimpleIKChain::ScalarDeletingDestructor', 'CTaskSimpleSay::ScalarDeletingDestructor', 'CTaskSimpleFacial::ScalarDeletingDestructor', 'CTaskSimpleSitIdle::ScalarDeletingDestructor', 'CTaskSimpleStandUp::ScalarDeletingDestructor', 'CTaskComplexSitDownThenIdleThenStandUp::ScalarDeletingDestructor']
ENTRIES = (0x64B2B0, 0x642F80, 0x46F670, 0x64B3C0, 0x64ADE0, 0x64AE70, 0x64BDC0, 0x64B930, 0x64C060, 0x655FE0, 0x6259C0, 0x67FA60, 0x656AE0, 0x4635F0, 0x64AF30, 0x636F00, 0x48E4C0, 0x692E30, 0x637A40, 0x637D40, 0x637D90)
BODY = (0x646710, 0x63CA40, 0x46F690, 0x646890, 0x6460F0, 0x646230, 0x647260, 0x647170, 0x6482C0, 0x653740, 0x61F700, 0x67BA70, 0x653F30, 0x463560, 0x646380, 0x633A90, 0x48E4E0, 0x690CB0, 0x6311B0, 0x631320, 0x6314A0)
DELETE = (0x61A5B0,) * 21
VTABLE = (0x86ED50, 0x86E934, 0x859E50, 0x86ED74, 0x86ECC0, 0x86ECE4, 0x86EE4C, 0x86EE28, 0x86EF0C, 0x86F268, 0x86D790, 0x870608, 0x86F2E4, 0x859CFC, 0x86ED08, 0x86DB48, 0x85A1DC, 0x870AB8, 0x86DF18, 0x86DF3C, 0x86DF60)
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
