"""Oraculo independiente (Python) de Fn115A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 20 funciones del lote 115A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion ni de Fixtures.hpp: la tabla de funciones (vtables, desplazamientos, argumentos, colaboradores
y su orden) se reescribe aqui desde el desensamblado (dumpbin /disasm del EXE canonico).

Cuerpos de destructor (thiscall, ECX = objeto, `ret`; los del EXE llevan un marco SEH que solo desenrolla):  [+0] = vtable propia; si el puntero de la referencia no es nulo, 0x571A00(puntero, &campo)
  (CEntity::CleanUpOldReference); y la cola: 0x4AC010 (~CEvent: vtable 0x85AB38), 0x61A3A0 (~CTask: vtable 0x86D48C) o 0x61A3D0 (~CTaskComplex: borra la subtarea y deja 0x86D48C); la de CTaskComplexSequence
  llama antes a 0x632C10 (Flush). Los colaboradores son falsos: registran [objeto, vtable en ese momento] y escriben la vtable base que escribe el original.
    0x609140 CEventDontJoinPlayerGroup: vtable 0x86D0E0, referencia +0xC, cola 0x4AC010;   0x65DCF0 CEventPassObject: 0x86F8B8, +0xC, 0x4AC010;
    0x620EF0 CTaskComplexKillPedOnFoot: 0x86D894, +0x10, 0x61A3D0;   0x6389F0 CTaskComplexSequence: 0x86E200, sin referencia, 0x632C10 y 0x61A3D0;
    0x647170 CTaskSimpleCarSetPedInAsDriver: 0x86EE28, +0x10, 0x61A3A0;   0x647950 CTaskSimpleCarSetPedOut: 0x86EEB8, +8, 0x61A3A0;
    0x667D50 CTaskSimpleGoTo: 0x86FD1C, sin referencia, 0x61A3A0;   0x669D20 CTaskComplexWander: 0x86FE84, 0x61A3D0;   0x67A060 CTaskComplexJump: 0x870570, 0x61A3D0.
Constructores (thiscall, ECX = objeto, argumentos en la pila, `ret N`, EAX = this): ver CTOR_OPS (una lista de operaciones por funcion, leida de cada `mov` del desensamblado).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
A_OBJ, A_ENTA, A_ENTB = range(3)
RAW1, RAW2 = 0x00BADC00, 0x00BADC40
PRE_VT = 0xDEADBEEF
OBJ_BYTES = 0x80

NAMES = [
    'CEventDontJoinPlayerGroup::Destructor', 'CEventPassObject::Destructor', 'CTaskComplexKillPedOnFoot::Destructor', 'CTaskComplexSequence::Destructor',
    'CTaskSimpleCarSetPedInAsDriver::Destructor', 'CTaskSimpleCarSetPedOut::Destructor', 'CTaskSimpleGoTo::Destructor', 'CTaskComplexWander::Destructor', 'CTaskComplexJump::Destructor',
    'CTaskSimpleAnim::Constructor', 'CTaskSimplePlayHandSignalAnim::Constructor', 'CTaskSimpleDrown::Constructor', 'CTaskComplexAttractorPartnerWait::Constructor',
    'CTaskComplexUseGoggles::Constructor', 'CTaskComplexUseEffectRunning::Constructor', 'CTaskComplexUseEffectSprinting::Constructor', 'CTaskComplexStuckInAir::Constructor',
    'CTaskSimpleArrestPed::Constructor', 'CTaskAllocatorPlayerCommandAttack::Constructor', 'CTaskSimpleFacial::Constructor']
ENTRIES = [0x609140, 0x65DCF0, 0x620EF0, 0x6389F0, 0x647170, 0x647950, 0x667D50, 0x669D20, 0x67A060,
           0x61A6C0, 0x61AE50, 0x62FEC0, 0x633250, 0x634EF0, 0x636780, 0x636860, 0x67BA40, 0x68B620, 0x69C480, 0x690C70]
CLEAN = 0x571A00
EV_BASE_DTOR, TASK_DTOR, COMPLEX_DTOR = 'event', 'task', 'complex'
# destructores: (vtable propia, desplazamiento de la referencia o None, colas en orden)
DTORS = {
    0x609140: (0x86D0E0, 0xC, ['event']),
    0x65DCF0: (0x86F8B8, 0xC, ['event']),
    0x620EF0: (0x86D894, 0x10, ['complex']),
    0x6389F0: (0x86E200, None, ['flush', 'complex']),
    0x647170: (0x86EE28, 0x10, ['task']),
    0x647950: (0x86EEB8, 0x8, ['task']),
    0x667D50: (0x86FD1C, None, ['task']),
    0x669D20: (0x86FE84, None, ['complex']),
    0x67A060: (0x870570, None, ['complex']),
}
# colas: (clase de evento del falso, vtable que escribe el falso o None)
TAIL = {'event': (32, 0x85AB38), 'task': (30, 0x86D48C), 'complex': (31, 0x86D48C), 'flush': (33, None)}

# constructores: lista de operaciones en el orden del EXE.
#   ('base', evento)            llamada al constructor base falso: evento 10 (0x61A390) o 12 (0x61A3B0): [objeto, vtable en ese momento]
#   ('b', off, expr)            byte [off] = expr          ('d', off, expr) dword [off] = expr
#   ('bits', arg)               byte [+0xC] = ((arg & 1) << 2) | (byte previo & 0xF8)
#   ('vt', valor)               [+0] = valor
#   ('reg', arg_ptr, off)       si el puntero no es nulo: RegisterReference(puntero, &[off]) (evento 11)
#   ('call', evento, args)      llamada al colaborador falso con los argumentos dados
# expr: ('arg', i) | ('arglow', i) | ('const', v) | ('argne0', i) | ('and', off, mask) (byte previo & mask)
CTORS = {
    0x61A6C0: dict(args='D', ops=[('base', 10), ('bits', 0), ('vt', 0x86D504), ('d', 8, ('const', 0))], marks='hold'),
    0x61AE50: dict(args='DDBB', ops=[('base', 10), ('bits', 3), ('d', 0x10, ('arg', 0)), ('d', 8, ('const', 0)), ('d', 0x1C, ('const', 0)), ('d', 0x20, ('const', 0)),
                                     ('vt', 0x86D5B8), ('d', 0x14, ('arg', 1)), ('b', 0x18, ('argne0', 2))], marks='hand'),
    0x62FEC0: dict(args='', ops=[('base', 10), ('d', 8, ('const', 0)), ('d', 0x10, ('const', 0)), ('d', 0x14, ('const', 0)), ('d', 0x1C, ('const', 0)), ('d', 0x24, ('const', 0)),
                                 ('d', 0xC, ('const', 0x8C)), ('d', 0x18, ('const', 0x40800000)), ('b', 0x20, ('and', 0x20, 0xFC)), ('vt', 0x86DE30), ('b', 0x28, ('const', 1))], marks='run'),
    0x633250: dict(args='BD', ops=[('base', 12), ('b', 0xC, ('arglow', 0)), ('vt', 0x86E284), ('d', 0x10, ('arg', 1))], marks='byte'),
    0x634EF0: dict(args='', ops=[('base', 12), ('vt', 0x86E4A8)], marks='run'),
    0x636780: dict(args='PP', ops=[('call', 40, ['p0', 'p1']), ('vt', 0x86E594), ('d', 0x1C, ('const', 6))], marks='fx'),
    0x636860: dict(args='PP', ops=[('call', 40, ['p0', 'p1']), ('vt', 0x86E5C0), ('d', 0x1C, ('const', 7))], marks='fx'),
    0x67BA40: dict(args='', ops=[('base', 12), ('vt', 0x870608)], marks='run'),
    0x68B620: dict(args='P', ops=[('base', 10), ('vt', 0x870984), ('d', 8, ('ptr', 0)), ('b', 0xC, ('const', 0)), ('d', 0x10, ('const', 0)), ('reg', 0, 8)], marks='ped'),
    0x69C480: dict(args='PDD', ops=[('call', 50, []), ('call', 41, ['p0', 'a1', 'player']), ('vt', 0x870E78), ('d', 0x1C, ('arg', 2))], marks='attack'),
    0x690C70: dict(args='DD', ops=[('base', 10), ('vt', 0x870AB8), ('d', 8, ('const', 0)), ('d', 0xC, ('const', 0)), ('b', 0x10, ('const', 0)), ('b', 0x11, ('const', 0)),
                                   ('d', 0x18, ('arg', 1)), ('d', 0x14, ('arg', 0))], marks='run'),
}
# el unico puntero que un constructor guarda en el objeto (sus bytes no se comparan; se informan como etiqueta): CTaskSimpleArrestPed +8
PTR_FIELD = {0x68B620: 8}


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def le32(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def ptr_value(i, mode):
    return [tag(A_ENTA if i == 0 else A_ENTB, 0), 0, RAW1 if i == 0 else RAW2][mode]


def changes_from(pre_hex, new):
    """[[arena, desplazamiento, byte nuevo]] de los bytes del objeto que difieren de la imagen previa; `new` = {desplazamiento: byte}"""
    pre = bytes.fromhex(pre_hex)
    return [[A_OBJ, off, b] for off, b in sorted(new.items()) if pre[off] != b]


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
    vt, ref_off, tails = DTORS[ENTRIES[c['fn']]]
    marks = []
    ev = []
    cur_vt = vt
    img = {}
    if ref_off is not None:
        p = ptr_value(0, a[0])
        marks.append('ref_' + ['arena', 'null', 'raw'][a[0]])
        if p:
            ev.append([20, p, tag(A_OBJ, ref_off), cur_vt])
    marks.append('vt_own' if a[2] == vt else ('vt_base' if a[2] == 0x85AB38 else 'vt_other'))
    final = vt
    for t in tails:
        kind, writes = TAIL[t]
        ev.append([kind, tag(A_OBJ, 0), final])
        if writes is not None:
            final = writes
    new = {j: b for j, b in enumerate(le32(final))}
    return {'events': ev, 'marks': marks, 'changes': changes_from(pre, new)}


def expected_ctor(c, pre):
    a = c['a']
    spec = CTORS[ENTRIES[c['fn']]]
    prev = bytes.fromhex(pre)
    new = {}
    cur_vt = PRE_VT
    ev = []
    marks = []
    kinds = spec['args']

    def arg(i):
        if kinds[i] == 'P':
            return ptr_value(i, a[i])
        return a[i]

    def setd(off, v):
        for j, b in enumerate(le32(v & M32)):
            new[off + j] = b

    def cur_byte(off):
        return new.get(off, prev[off])

    def val(expr):
        k = expr[0]
        if k == 'arg':
            return arg(expr[1])
        if k == 'arglow':
            return arg(expr[1]) & 0xFF
        if k == 'ptr':
            return arg(expr[1])
        if k == 'const':
            return expr[1]
        if k == 'argne0':
            return 1 if (arg(expr[1]) & 0xFF) != 0 else 0
        if k == 'and':
            return cur_byte(expr[1]) & expr[2]
        raise ValueError(expr)

    for op in spec['ops']:
        k = op[0]
        if k == 'base':
            ev.append([op[1], tag(A_OBJ, 0), cur_vt])
        elif k == 'bits':
            new[0xC] = ((arg(op[1]) & 1) << 2) | (cur_byte(0xC) & 0xF8)
        elif k == 'vt':
            setd(0, op[1])
            cur_vt = op[1]
        elif k == 'b':
            new[op[1]] = val(op[2]) & 0xFF
        elif k == 'd':
            setd(op[1], val(op[2]))
        elif k == 'reg':
            p = arg(op[1])
            if p:
                ev.append([11, p, tag(A_OBJ, op[2]), cur_vt])
        elif k == 'call':
            kind, names = op[1], op[2]
            if kind == 40:
                ev.append([40, tag(A_OBJ, 0), arg(0), arg(1), cur_vt])
            elif kind == 41:
                player = [tag(A_ENTB, 0), 0, RAW2][a[8]]
                ev.append([41, tag(A_OBJ, 0), arg(0), a[1], player, cur_vt])
            elif kind == 50:
                ev.append([50, M32])
            else:
                raise ValueError(kind)
        else:
            raise ValueError(op)
    m = spec['marks']
    if m == 'hold':
        marks.append('hold_bit%d' % (a[0] & 1))
    elif m == 'hand':
        marks += ['hold_bit%d' % (a[3] & 1), 'fat_nonzero' if a[2] & 0xFF else 'fat_zero']
    elif m == 'byte':
        marks.append('byte_nonzero' if a[0] & 0xFF else 'byte_zero')
    elif m == 'fx':
        marks += ['fx_' + ['arena', 'null', 'raw'][a[0]], 'ent_' + ['arena', 'null', 'raw'][a[1]]]
    elif m == 'ped':
        marks.append('ped_' + ['arena', 'null', 'raw'][a[0]])
    elif m == 'attack':
        marks += ['target_' + ['arena', 'null', 'raw'][a[0]], 'player_' + ['arena', 'null', 'raw'][a[8]]]
    else:
        marks.append('run')
    ptr_off = PTR_FIELD.get(ENTRIES[c['fn']])
    ch = changes_from(pre, new)
    if ptr_off is not None:
        ch = [x for x in ch if not (ptr_off <= x[1] < ptr_off + 4)]
    out = {'events': ev, 'marks': marks, 'changes': ch, 'ret': tag(A_OBJ, 0)}
    if ptr_off is not None:
        out['ptrs'] = [arg(0)]
    else:
        out['ptrs'] = []
    return out


def expected(c, obs=None):
    entry = ENTRIES[c['fn']]
    pre = obs['pre'] if obs else '00' * OBJ_BYTES
    if entry in CTORS:
        return expected_ctor(c, pre)
    return expected_dtor(c, pre)


def required_marks(name):
    """marcas de cobertura que cada funcion debe alcanzar (run_local.py: minimo por marca)"""
    entry = ENTRIES[NAMES.index(name)]
    if entry in DTORS:
        vt, ref_off, tails = DTORS[entry]
        return (['ref_arena', 'ref_null', 'ref_raw'] if ref_off is not None else []) + ['vt_own', 'vt_base', 'vt_other']
    m = CTORS[entry]['marks']
    return {'hold': ['hold_bit0', 'hold_bit1'], 'hand': ['hold_bit0', 'hold_bit1', 'fat_zero', 'fat_nonzero'], 'byte': ['byte_zero', 'byte_nonzero'],
            'fx': ['fx_arena', 'fx_null', 'fx_raw', 'ent_arena', 'ent_null', 'ent_raw'], 'ped': ['ped_arena', 'ped_null', 'ped_raw'],
            'attack': ['target_arena', 'target_null', 'target_raw', 'player_arena', 'player_null', 'player_raw'], 'run': ['run']}[m]


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    for k in ('ret', 'ptrs'):
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
