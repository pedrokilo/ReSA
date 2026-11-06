"""Oraculo independiente (Python) de Fn108A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (dumpbin /disasm), lo que debe observarse de las diecisiete funciones del lote 108A
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. Los floats de 32 bits se decodifican con `struct` (exactos en un
double de Python, incluidos desnormales); NaN no es ordenable con nada (como FCOMP + FNSTSW + TEST AH,41h/05h del EXE y COMISS de la DLL).

Funciones (direccion, convencion; los bytes leidos del EXE entre parentesis):
 0 CMBlur::MotionBlurRender 0x71D700 (`ret`): no hace nada.                                       1 CMBlur::MotionBlurOpen 0x71D710 (`mov eax,1; ret`): true.   2 CMBlur::MotionBlurClose 0x71D720: true.
 3 CMBlur::SetDrunkBlur 0x71D730: `fld arg; fcomp 1.0; test ah,5; jnp` -> si NO (arg < 1) (NaN incluido) Drunkness = 1.0; si no `fcomp 0.0; test ah,41h; je` -> si arg > 0 se guarda arg tal cual; si no (<= 0, -0.0 incluido) 0.
 4 CMBlur::ClearDrunkBlur 0x71D780: CTimer::ms_fTimeScale (0xB7CB64) = 1.0 y Drunkness (0xC73C58) = 0.
 5 CRubbish::SetVisibility 0x7204C0, 6 CHeli::SpecialHeliPreRender 0x6C4750, 7 CPlaneBanners::Init 0x717820 (`ret`): no hacen nada.
 8 CTaskSimpleInAir::ApplyRollAndPitch 0x678E80 (`ret 4`): no hace nada.
 9 CTaskSimpleGoToPointFine::SetMoveRatio 0x65F3A0 (`mov eax,[esp+4]; mov [ecx+20h],eax; ret 4`): this+0x20 = el valor (bits crudos).
10 CVehicleModelInfo::GetWheelSize 0x6A06F0 (`mov al,[esp+4]; test al,al; je; fld [ecx+4Ch]...`: byte bajo del argumento distinto de cero -> `fld dword [ecx+40h]`, si no `fld dword [ecx+44h]`; ret 4): ST0 = el float cargado (80 bits).
11 CTaskManager::GetLastTaskOf(CTask*) 0x681970: nulo -> 0; si no sigue [vtabla+8] (GetSubTask) hasta que devuelve nulo y devuelve la ultima tarea no nula.
12 CTaskManager::GetLastTaskOf(ePrimaryTasks) 0x681A00 (`mov ecx,[ecx+eax*4]` y el mismo recorrido; ret 4).
13 CTaskSimpleDuck::StopFireGun 0x61C420: `fld [ecx+1Ch]; fcomp 0.0; test ah,44h; jp` (movimiento x != 0 o NaN -> true); animacion [ecx+10h] nula -> true; `fld [anim+18h]; fcomp 1.0; test ah,5; jnp`
   (mezcla < 1 ordenada -> true; NaN no); byte [ecx+19h] distinto de cero -> true; palabra con signo [ecx+0Eh] > 0 -> true; si no false.
14 CTaskSimpleDuck::AbortBecauseOfOtherDuck 0x692340 (`ret 4`): animacion de agachado (+0x10): si no es nula, SetFinishCallback(0x4D1390, 0) (0x4CEBE0) y +0x10 = 0; animacion de movimiento (+0x14): si no es nula,
   [anim+1Ch] = 0xC1000000 (-8.0), SetFinishCallback(0x4D1390, 0) y +0x14 = 0; bytes +0x1A y +0x18 = 1.
15 SayJacked 0x6497A0: GetVehicleTypeForAudio (0x4F4F00, ecx = vehiculo+0x138), `test ax,ax`: 0 -> Say(0x76), si no Say(0x77) sobre el primer ped con (retraso, 1.0f, 0, 0, 0); devuelve (ax de Say != 0xFFFF).
16 SayJacking 0x6497F0: tipo (0x4F4F00); ax == 1 -> Say(0x79) sobre el jacker; si no segunda consulta: 0 -> `call [vtabla del audio del jacked +1Ch]` (ecx = jacked+0x294; al != 0 -> Say(0x7A) si no Say(0x7B)),
   si no Say(0x7C); devuelve (ax de Say != 0xFFFF).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
A_TASK, A_PED, A_PED2, A_VEH, A_ANIM, A_ANIM2, A_CHAIN = 0, 1, 2, 3, 4, 5, 6
CB = 0xDEFA0001
ONE = 0x3F800000


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def fval(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def ext80(bits):
    """codificacion de 80 bits (10 bytes, little endian) del resultado de `fld dword` de un patron de float"""
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        if m == 0:
            mant, exp = 0x8000000000000000, 0x7FFF
        else:   # NaN: se fuerza el bit de silencio y la carga se desplaza 40 bits
            mant, exp = 0xC000000000000000 | (m << 40), 0x7FFF
    elif e == 0:
        if m == 0:
            mant, exp = 0, 0
        else:
            length = m.bit_length()
            mant, exp = m << (64 - length), 16383 + length - 150
    else:
        mant, exp = (m | 0x800000) << 40, e - 127 + 16383
    return mant.to_bytes(8, 'little') + ((s << 15) | exp).to_bytes(2, 'little')


def fp_stack(hexstr):
    """lista de (st, 10 bytes) de las posiciones validas de una imagen fnsave normalizada"""
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


def base(c):
    """resultado por defecto: nada cambia salvo lo que diga cada funcion"""
    return {'events': [], 'marks': ['run'], 'changes': [], 'fields': [], 'drunk': c['a'][40], 'scale': c['a'][41], 'al': None, 'eax_tag': None, 'st0': None}


def expected0(c):
    e = base(c)
    e['marks'] = ['args_zero' if c['family'] == 0 else ('args_ones' if c['family'] == 1 else ('args_float' if c['family'] == 2 else 'args_random'))]
    return e


def expected1(c):
    e = base(c)
    e['al'] = 1
    f = c['family']
    e['marks'] = ['cam_zero' if f == 0 else ('cam_ones' if f == 1 else ('cam_one' if f == 2 else 'cam_random'))]
    return e


def expected2(c):
    e = base(c)
    e['al'] = 1
    return e


def expected3(c):
    e = base(c)
    bits = c['a'][0]
    v = fval(bits)
    if not (v < 1.0):
        e['drunk'] = ONE
        e['marks'] = ['nan'] if is_nan(bits) else (['above_one'] if v > 1.0 else ['one'])
        if v == float('inf'):
            e['marks'].append('inf')
    elif not (v > 0.0):
        e['drunk'] = 0
        e['marks'] = ['zero'] if bits == 0 else (['neg_zero'] if bits == 0x80000000 else ['neg'])
    else:
        e['drunk'] = bits
        e['marks'] = ['between']
    if 0 < (bits & 0x7FFFFFFF) < 0x00800000:
        e['marks'].append('denormal')
    return e


def expected4(c):
    e = base(c)
    e['drunk'] = 0
    e['scale'] = ONE
    return e


def expected5(c):
    e = base(c)
    v = c['a'][0]
    e['marks'] = ['zero' if v == 0 else ('one' if v == 1 else 'other')] + (['garbage_high'] if v >> 8 else [])
    return e


def expected6(c):
    return base(c)


def expected8(c):
    e = base(c)
    e['marks'] = [['ped_arena', 'ped_null', 'ped_raw', 'ped_raw'][c['a'][0]]]
    return e


def expected9(c):
    e = base(c)
    a = c['a']
    e['fields'] = [[A_TASK, 0x20, a[0]]]
    e['marks'] = ['unchanged' if a[0] == a[1] else 'changed'] + (['nan'] if is_nan(a[0]) else []) + (['neg_zero'] if a[0] == 0x80000000 else [])
    return e


def expected10(c):
    e = base(c)
    a = c['a']
    front = (a[0] & 0xFF) != 0
    v = a[1] if front else a[2]
    e['st0'] = ext80(v)
    e['marks'] = ['front' if front else 'rear'] + (['nan'] if is_nan(v) else []) + (['denormal'] if 0 < (v & 0x7FFFFFFF) < 0x00800000 else []) + (['garbage_high'] if a[0] >> 8 else [])
    return e


def expected11(c):
    e = base(c)
    n = c['a'][0]
    e['events'] = [[2, tag(A_CHAIN + i, 0)] for i in range(n)]
    e['eax_tag'] = tag(A_CHAIN + n - 1, 0) if n else 0
    e['marks'] = ['len%d' % n]
    return e


def expected12(c):
    e = base(c)
    a = c['a']
    idx = a[0]
    n = a[1 + idx]
    e['events'] = [[2, tag(A_CHAIN + idx * 3 + j, 0)] for j in range(n)]
    e['eax_tag'] = tag(A_CHAIN + idx * 3 + n - 1, 0) if n else 0
    e['marks'] = ['idx%d' % idx, 'slot_empty' if n == 0 else 'slot_len%d' % n]
    return e


def expected13(c):
    e = base(c)
    a = c['a']
    mx, blend = a[2], a[4]
    abort_byte = (a[1] >> 8) & 0xFF
    counter = s16(a[0] >> 16)
    marks = []
    if (mx & 0x7FFFFFFF) != 0 or is_nan(mx):
        r = 1
        marks.append('move_nan' if is_nan(mx) else 'move_nonzero')
    elif a[3] != 0:
        r = 1
        marks.append('no_anim')
    elif fval(blend) < 1.0:
        r = 1
        marks.append('blend_lt1')
    else:
        if is_nan(blend):
            marks.append('blend_nan')
        if abort_byte != 0:
            r = 1
            marks.append('aborting')
        elif counter > 0:
            r = 1
            marks.append('counter_pos')
        else:
            r = 0
            marks.append('false')
    e['al'] = r
    e['marks'] = marks
    return e


def expected14(c):
    e = base(c)
    a = c['a']
    duck = a[0] == 0
    move = a[1] == 0
    ev = []
    if duck:
        ev.append([5, tag(A_ANIM, 0), CB, 0])
    if move:
        ev.append([5, tag(A_ANIM2, 0), CB, 0])
    e['events'] = ev
    e['fields'] = [[A_TASK, 0x10, 0], [A_TASK, 0x14, 0], [A_TASK, 0x18, (a[2] & 0xFF00FF00) | 0x00010001], [A_ANIM, 0x1C, a[5]], [A_ANIM2, 0x1C, 0xC1000000 if move else a[3]]]
    e['marks'] = ['duck_anim' if duck else 'no_duck_anim', 'move_anim' if move else 'no_move_anim'] + (['both'] if duck and move else []) + (['none'] if not duck and not move else [])
    e['marks'].append(['ped_arena', 'ped_null', 'ped_raw', 'ped_raw'][a[4]])
    return e


def say_ev(ctx, delay):
    return [31, tag(A_PED, 0), ctx, delay, ONE, 0, 0, 0]


def expected15(c):
    e = base(c)
    a = c['a']
    t = a[0] & 0xFFFF
    ctx = 0x76 if t == 0 else 0x77
    e['events'] = [[10, tag(A_VEH, 0x138)], say_ev(ctx, a[1])]
    e['al'] = 1 if (a[2] & 0xFFFF) != 0xFFFF else 0
    e['marks'] = ['car' if t == 0 else ('bike' if t == 1 else ('generic' if t == 2 else 'other_type')), 'say_fail' if e['al'] == 0 else 'say_ok'] + (['junk_high'] if a[0] >> 16 else [])
    return e


def expected16(c):
    e = base(c)
    a = c['a']
    t = a[0] & 0xFFFF
    ev = [[10, tag(A_VEH, 0x138)]]
    if t == 1:
        ctx, m = 0x79, 'bike'
    else:
        ev.append([10, tag(A_VEH, 0x138)])
        if t == 0:
            ev.append([11, tag(A_PED2, 0x294)])
            if (a[3] & 0xFF) != 0:
                ctx, m = 0x7A, 'car_female'
            else:
                ctx, m = 0x7B, 'car_male'
        else:
            ctx, m = 0x7C, 'generic' if t == 2 else 'other_type'
    ev.append(say_ev(ctx, a[1]))
    e['events'] = ev
    e['al'] = 1 if (a[2] & 0xFFFF) != 0xFFFF else 0
    e['marks'] = [m, 'say_fail' if e['al'] == 0 else 'say_ok']
    return e


MODELS = {0: expected0, 1: expected1, 2: expected2, 3: expected3, 4: expected4, 5: expected5, 6: expected6, 7: expected6, 8: expected8, 9: expected9, 10: expected10,
          11: expected11, 12: expected12, 13: expected13, 14: expected14, 15: expected15, 16: expected16}


def expected(c):
    return MODELS[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('fields') != e['fields']:
        return 'campos: %s != %s' % (o.get('fields'), e['fields'])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('drunk_final') != e['drunk']:
        return 'Drunkness final %s != %s' % (o.get('drunk_final'), e['drunk'])
    if o.get('scale_final') != e['scale']:
        return 'escala de tiempo final %s != %s' % (o.get('scale_final'), e['scale'])
    if e['al'] is not None and o.get('al') != e['al']:
        return 'AL %s != %s' % (o.get('al'), e['al'])
    if e['al'] is None and 'al' in o:
        return 'AL registrado sin modelo'
    if e['eax_tag'] is not None and o.get('eax_tag') != e['eax_tag']:
        return 'EAX %s != %s' % (o.get('eax_tag'), e['eax_tag'])
    valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != e['st0']:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], e['st0'].hex())
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
