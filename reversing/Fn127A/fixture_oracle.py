"""Oraculo independiente (Python) de Fn127A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los veintidos destructores con borrado del lote 127A (clases de tarea, todas con el mismo operator delete CTask 0x61A5B0)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico). Las veintidos funciones (30 bytes) tienen la misma forma:
    push esi; mov esi,ecx; call BODY; test byte [esp+8],1; je +; push esi; call DELETE; add esp,4; mov eax,esi; pop esi; ret 4
  (BODY = cuerpo del destructor de la clase, thiscall sin argumentos; DELETE = operator delete de la clase, cdecl con `this`; el bit 0 del BYTE de banderas manda; devuelve this; `ret 4`.)
  idx  direccion  clase                                   BODY                      DELETE
   0   0x421250   CTaskComplexLeaveAnyCar                0x421270                  0x61A5B0
   1   0x6424F0   CTaskComplexScreamInCarThenLeave       0x63C2C0                  0x61A5B0
   2   0x642370   CTaskComplexLeaveCarAndFlee            0x63C030                  0x61A5B0
   3   0x640E40   CTaskComplexEnterBoatAsDriver          0x63B650                  0x61A5B0
   4   0x6577D0   CTaskComplexEvasiveCower               0x6554E0                  0x61A5B0
   5   0x68DAB0   CTaskComplexKillCriminal               0x68BF30                  0x61A5B0
   6   0x643760   CTaskSimpleCarSetTempAction            0x63D730                  0x61A5B0
   7   0x6637A0   CTaskGangHassleVehicle                 0x65FB60                  0x61A5B0
   8   0x6390A0   CTaskComplexUseScriptedAttractor       0x633360                  0x61A5B0
   9   0x4B8820   CTaskComplexUseClosestFreeScriptedAttractorRun 0x4B8840                  0x61A5B0
  10   0x4B88E0   CTaskComplexUseClosestFreeScriptedAttractorSprint 0x4B8900                  0x61A5B0
  11   0x639190   CTaskComplexOnFire                     0x6333C0                  0x61A5B0
  12   0x6980F0   CTaskComplexExtinguishFires            0x6975F0                  0x61A5B0
  13   0x656E50   CTaskComplexWalkRoundBuildingAttempt   0x654950                  0x61A5B0
  14   0x676330   CTaskInteriorBeInOffice                0x675260                  0x61A5B0
  15   0x676860   CTaskInteriorUseInfo                   0x675A90                  0x61A5B0
  16   0x677760   CTaskInteriorSitAtDesk                 0x676080                  0x61A5B0
  17   0x676620   CTaskInteriorGoToInfo                  0x675810                  0x61A5B0
  18   0x6761E0   CTaskInteriorBeInHouse                 0x675000                  0x61A5B0
  19   0x64CC40   CTaskSimpleCarFallOut                  0x648DE0                  0x61A5B0
  20   0x64ACE0   CTaskSimpleCarOpenDoorFromOutside      0x645EE0                  0x61A5B0
  21   0x64B000   CTaskSimpleCarCloseDoorFromOutside     0x646570                  0x61A5B0

Banco: BODY y DELETE se sustituyen por falsos identicos en el EXE y en el cuerpo de la DLL. El falso de BODY registra [direccion de BODY, ECX (etiqueta de arena), dword que tenia [ECX] al entrar], deja
en [ECX] el dword del caso (a[8], la vtable de la base) y devuelve en EAX un valor del caso (a[9], que NO es this). El falso de DELETE registra [direccion de DELETE, argumento (etiqueta), dword de [argumento]
al entrar] (es decir, lo que dejo el cuerpo: el borrado va DESPUES de destruir) y no libera nada. Valor devuelto = this (la etiqueta de arena) siempre; sin tocar la x87.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskComplexLeaveAnyCar::ScalarDeletingDestructor', 'CTaskComplexScreamInCarThenLeave::ScalarDeletingDestructor', 'CTaskComplexLeaveCarAndFlee::ScalarDeletingDestructor', 'CTaskComplexEnterBoatAsDriver::ScalarDeletingDestructor', 'CTaskComplexEvasiveCower::ScalarDeletingDestructor', 'CTaskComplexKillCriminal::ScalarDeletingDestructor', 'CTaskSimpleCarSetTempAction::ScalarDeletingDestructor', 'CTaskGangHassleVehicle::ScalarDeletingDestructor', 'CTaskComplexUseScriptedAttractor::ScalarDeletingDestructor', 'CTaskComplexUseClosestFreeScriptedAttractorRun::ScalarDeletingDestructor', 'CTaskComplexUseClosestFreeScriptedAttractorSprint::ScalarDeletingDestructor', 'CTaskComplexOnFire::ScalarDeletingDestructor', 'CTaskComplexExtinguishFires::ScalarDeletingDestructor', 'CTaskComplexWalkRoundBuildingAttempt::ScalarDeletingDestructor', 'CTaskInteriorBeInOffice::ScalarDeletingDestructor', 'CTaskInteriorUseInfo::ScalarDeletingDestructor', 'CTaskInteriorSitAtDesk::ScalarDeletingDestructor', 'CTaskInteriorGoToInfo::ScalarDeletingDestructor', 'CTaskInteriorBeInHouse::ScalarDeletingDestructor', 'CTaskSimpleCarFallOut::ScalarDeletingDestructor', 'CTaskSimpleCarOpenDoorFromOutside::ScalarDeletingDestructor', 'CTaskSimpleCarCloseDoorFromOutside::ScalarDeletingDestructor']
ENTRIES = (0x421250, 0x6424F0, 0x642370, 0x640E40, 0x6577D0, 0x68DAB0, 0x643760, 0x6637A0, 0x6390A0, 0x4B8820, 0x4B88E0, 0x639190, 0x6980F0, 0x656E50, 0x676330, 0x676860, 0x677760, 0x676620, 0x6761E0, 0x64CC40, 0x64ACE0, 0x64B000)
BODY = (0x421270, 0x63C2C0, 0x63C030, 0x63B650, 0x6554E0, 0x68BF30, 0x63D730, 0x65FB60, 0x633360, 0x4B8840, 0x4B8900, 0x6333C0, 0x6975F0, 0x654950, 0x675260, 0x675A90, 0x676080, 0x675810, 0x675000, 0x648DE0, 0x645EE0, 0x646570)
DELETE = (0x61A5B0,) * 22
VTABLE = (0x858EB0, 0x86E8D8, 0x86E8AC, 0x86E7D0, 0x86F3BC, 0x870A00, 0x86EA6C, 0x86F9D4, 0x86E2DC, 0x85B904, 0x85B930, 0x86E308, 0x870DD0, 0x86F334, 0x870218, 0x8702E8, 0x87035C, 0x8702BC, 0x8701E8, 0x86EFD0, 0x86EC9C, 0x86ED2C)
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
