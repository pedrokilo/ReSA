"""Oraculo independiente (Python) de Fn107A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 107A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion ni de Fixtures.hpp: la tabla de funciones (vtables, desplazamientos y colaborador) se
reescribe aqui desde el desensamblado (dumpbin /disasm del EXE canonico).

Destructores de eventos (0x4AF890, 0x4B1990, 0x4B2210, 0x4AC720, 0x4ACA70, 0x4B17F0, 0x4B1170, 0x4AF400, 0x4AF700, 0x4B0DA0, 0x4B2130, 0x4B2470, 0x4B1D50, 0x4AC920, 0x4ACDD0, 0x4ACFF0, 0x4AE3C0,
  0x4AE670, 0x4B1520, 0x4B26A0, 0x4AE7A0, 0x4AEA50; thiscall, ECX = evento, `ret`): [+0] = vtable propia; para cada puntero de la lista (en orden), si no es nulo, 0x571A00(puntero, &campo) (CEntity::CleanUpOldReference;
  0x571B70, RegisterReference, en 0x4B1D50); [+0] = 0x85AB38 (vtable de CEvent). El EXE envuelve la funcion en un marco SEH que solo desenrolla.
Destructores triviales (0x4AC010 CEvent, 0x4AC480 CEventEditableResponse): `mov dword ptr [ecx], 0x85AB38; ret`; EAX, ECX y EDX salen como entraron.
RoundVectorToDecimals (0x50A120; cdecl, (float*, int); `ret`): para x, y, z en ese orden: ST0 = 0x50A0A0(bits del float, digitos) (cdecl, dos dwords) y `fstp dword` en el campo; el siguiente se lee DESPUES de guardar el anterior.
  El falso del banco devuelve el double 0x3FF0000000000000 | (bits & 0xFFFFF) << 32 | digitos (sin aritmetica x87); el float guardado es ese double redondeado al mas cercano (par).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
A_EV, A_ENTA, A_ENTB, A_VEC = range(4)
RAW1, RAW2 = 0x00BADC00, 0x00BADC40
CEVENT = 0x85AB38

NAMES = [
    'CEventAcquaintancePed::Destructor', 'CEventVehicleDamage::Destructor', 'CEventAreaCodes::Destructor', 'CEventGunShot::Destructor', 'CEventPedCollisionWithPed::Destructor',
    'CEventCopCarBeingStolen::Destructor', 'CEventVehicleOnFire::Destructor', 'CEventAttractor::Destructor', 'CEventVehicleToSteal::Destructor', 'CEventVehicleDied::Destructor',
    'CEventSeenPanickedPed::Destructor', 'CEventPlayerCommandToGroup::Destructor', 'CEventCarUpsideDown::Destructor', 'CEventVehicleCollision::Destructor', 'CEventObjectCollision::Destructor',
    'CEventBuildingCollision::Destructor', 'CEventPotentialWalkIntoVehicle::Destructor', 'CEventPotentialWalkIntoObject::Destructor', 'CEventStuckInAir::Destructor', 'CEventDanger::Destructor',
    'CEventPotentialWalkIntoPed::Destructor', 'CEventVehicleHitAndRun::Destructor', 'CEvent::Destructor', 'CEventEditableResponse::Destructor', 'Common::RoundVectorToDecimals']
ENTRIES = [0x4AF890, 0x4B1990, 0x4B2210, 0x4AC720, 0x4ACA70, 0x4B17F0, 0x4B1170, 0x4AF400, 0x4AF700, 0x4B0DA0, 0x4B2130, 0x4B2470, 0x4B1D50, 0x4AC920, 0x4ACDD0, 0x4ACFF0, 0x4AE3C0, 0x4AE670,
           0x4B1520, 0x4B26A0, 0x4AE7A0, 0x4AEA50, 0x4AC010, 0x4AC480, 0x50A120]
# (vtable propia, [(desplazamiento del puntero, colaborador 0x571A00 / 0x571B70)]) leidos de `mov [esi], vtable` y `lea eax, [esi+off]` del desensamblado de cada funcion
CLEAN, REG = 0x571A00, 0x571B70
DTORS = {
    0x4AF890: (0x85B250, [(0x14, CLEAN)]),
    0x4B1990: (0x85B588, [(0x14, CLEAN), (0x18, CLEAN)]),
    0x4B2210: (0x85B7C0, [(0x0C, CLEAN)]),
    0x4AC720: (0x85ABE0, [(0x14, CLEAN)]),
    0x4ACA70: (0x85AC68, [(0x14, CLEAN)]),
    0x4B17F0: (0x85B548, [(0x0C, CLEAN), (0x10, CLEAN)]),
    0x4B1170: (0x85B440, [(0x14, CLEAN)]),
    0x4AF400: (0x85B1B8, [(0x18, CLEAN)]),
    0x4AF700: (0x85B210, [(0x0C, CLEAN)]),
    0x4B0DA0: (0x85B400, [(0x0C, CLEAN)]),
    0x4B2130: (0x85B778, [(0x14, CLEAN)]),
    0x4B2470: (0x85B800, [(0x18, CLEAN)]),
    0x4B1D50: (0x85B660, [(0x0C, REG)]),
    0x4AC920: (0x85AC28, [(0x14, CLEAN)]),
    0x4ACDD0: (0x85ACA8, [(0x14, CLEAN)]),
    0x4ACFF0: (0x85ACE8, [(0x14, CLEAN)]),
    0x4AE3C0: (0x85AED0, [(0x14, CLEAN)]),
    0x4AE670: (0x85AF18, [(0x14, CLEAN)]),
    0x4B1520: (0x85B508, [(0x0C, CLEAN)]),
    0x4B26A0: (0x85B848, [(0x14, CLEAN)]),
    0x4AE7A0: (0x85AF60, [(0x20, CLEAN)]),
    0x4AEA50: (0x85AFA8, [(0x10, CLEAN), (0x0C, CLEAN)]),
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


def expected_dtor(c, pre):
    a = c['a']
    vt, refs = DTORS[ENTRIES[c['fn']]]
    modes = [a[0], a[1]]
    marks = []
    ev = []
    for i, (off, coll) in enumerate(refs):
        p = [tag(A_ENTA, 0) if i == 0 else tag(A_ENTB, 0), 0, RAW1 if i == 0 else RAW2][modes[i]]
        marks.append('ref%d_%s' % (i + 1, ['arena', 'null', 'raw'][modes[i]]))
        if p:
            ev.append([20 if coll == CLEAN else 11, p, tag(A_EV, off), vt])
    if len(refs) == 2 and (modes[0] != 1) and (modes[1] != 1):
        marks.append('both_refs')
    if len(refs) == 2 and modes[0] == 1 and modes[1] == 1:
        marks.append('no_refs')
    marks.append('vt_own' if a[2] == vt else ('vt_base' if a[2] == CEVENT else 'vt_other'))
    if refs[0][1] == REG:
        marks.append('register_quirk')
    new = {j: b for j, b in enumerate(le32(CEVENT))}
    return {'events': ev, 'marks': marks, 'changes': changes_from(pre, A_EV, new), 'regs_kept': None}


def expected_trivial(c, pre):
    a = c['a']
    new = {j: b for j, b in enumerate(le32(CEVENT))}
    marks = ['vt_base' if a[0] == CEVENT else ('vt_zero' if a[0] == 0 else 'vt_other')]
    return {'events': [], 'marks': marks, 'changes': changes_from(pre, A_EV, new), 'regs_kept': True}


def f32_of_double_bits(b64):
    d = struct.unpack('<d', struct.pack('<Q', b64))[0]
    return struct.unpack('<I', struct.pack('<f', d))[0]


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def expected_round(c, pre):
    a = c['a']
    n = a[3] & M32
    ev = []
    new = {}
    marks = []
    for i in range(3):
        bits = a[i] & M32
        ev.append([30, bits, n])
        r = f32_of_double_bits(0x3FF0000000000000 | ((bits & 0xFFFFF) << 32) | n)
        for j, b in enumerate(le32(r)):
            new[4 * i + j] = b
        if is_nan_bits(bits):
            marks.append('nan_in')
        if (bits & 0x7FFFFFFF) == 0x7F800000:
            marks.append('inf_in')
        if (bits & 0x7FFFFFFF) == 0:
            marks.append('zero_in')
        if (bits & 0x7F800000) == 0 and (bits & 0x7FFFFF) != 0:
            marks.append('denormal_in')
    sn = n - (1 << 32) if n & 0x80000000 else n
    marks.append('digits_neg' if sn < 0 else ('digits_zero' if sn == 0 else 'digits_pos'))
    if abs(sn) > 250:
        marks.append('digits_extreme')
    if a[0] == a[1] == a[2]:
        marks.append('same_xyz')
    if any(a[i] & M32 != 0 and not is_nan_bits(a[i] & M32) and (a[i] & 0x7F800000) != 0x7F800000 and (a[i] & 0x7F800000) != 0 for i in range(3)):
        marks.append('finite_normal')
    return {'events': ev, 'marks': sorted(set(marks)), 'changes': changes_from(pre, A_VEC, new), 'regs_kept': None}


def expected(c, obs=None):
    kind = ENTRIES[c['fn']]
    pre = obs['pre'] if obs else '00' * 0x40
    if kind == 0x50A120:
        return expected_round(c, pre if obs else '00' * 0x10)
    if kind in (0x4AC010, 0x4AC480):
        return expected_trivial(c, pre)
    return expected_dtor(c, pre)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if e['regs_kept'] is not None and o.get('regs_kept') != e['regs_kept']:
        return 'EAX/ECX/EDX no conservados: %s' % (o.get('regs_kept'),)
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
