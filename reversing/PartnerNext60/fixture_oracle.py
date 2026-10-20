"""Oraculo independiente del banco PartnerNext60: CTaskComplexPartner::CreateNextSubTask (0x683AD0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x683AD0 (thiscall, ECX=this -> ESI, un argumento de pila ped -> EBP, `ret 4`, devuelve CTask* en EAX; marco SEH propio; guarda EBX/EBP/ESI/EDI):
  683AED  eax = [esi+38h] (m_partner) ; test ; je 6840AA (-> 0)
  683AF9  ecx = [esi+34h] (m_taskId) ; eax = [eax+47Ch] ; push ecx ; ecx = eax+4 ; call 681740 (FindActiveTaskByType) ; test ; je 6840AA ; cmp [eax+38h],ebp ; jne 6840AA
  683B20  al = byte [esi+59h] (m_partnerState): 6 -> 683B27 ; 5 -> 683B56 ; 4 -> 683E41 ; 3 -> 683EFE ; 2 -> 683F7D ; 1 -> 684018 ; otro -> 6840AA
  estado 6 (683B27): byte [esi+6Eh] == 0 -> 0 ; byte [esi+5Bh] == 0 -> 0 ; --byte [esi+5Bh] ; call [vtable+34h] (GetPartnerSequence) ; devuelve su EAX
  estado 5 (683B56): byte [esi+5Dh] == 0:
      call 6822B0 (estado con signo -- movsx -- de la tarea del partner, 0 si no la hay; llama a 681740) ; < 5 -> StandStill (683C38)
      eax = [ebp+14h]... [ebp+47Ch]+14h = tarea por defecto del ped (m_aPrimaryTasks[4]); nula -> estado = 6 ; GetTaskType (vtable +10h) != 390h -> estado = 6 ; GetWanderType (+2Ch) != 0 -> estado = 6
      41CCB0 (forward: matriz [ebp+14h] -> (+0x10,+0x14,+0x18) crudo; sin matriz (-sin h, cos h, 0) con fsin/fcos de [ebp+10h]) ; GetNodeHeadingFromVector(-fx, -fy) (53CDC0, cdecl) ; AX
      new(0x38) (61A5A0) ; Wander = 48E4F0(4, dir = AL, 1) ; 66B150: [wander+34h] = [0B7CB84h] + [86FCDCh] (100000) ; SetTask(wander, 4, 0) (681AF0) ; estado = 6 ; StandStill
      byte [esi+5Dh] != 0: byte [esi+5Ah] == 1: 6822B0 == 6 -> estado = 6 ; StandStill
      si no: geometria (683CB0..683E1E, ver el modelo) ; StandStill
  estado 4 (683E41): 6822B0 : 4 o 5 -> (byte [+5Ah]: -1 -> 0) ; t = 681740 ; (byte [t+5Ah]: -1 -> 1) ; estado = 5 ; StandStill
      si no: (byte [+5Ah]: -1 -> 1) ; t = 681740 ; (byte [t+5Ah]: -1 -> 0) ; [t+40h..48h] = [esi+4Ch..54h] ; StandStill
  estado 3 (683EFE): GetTaskType(sub) == 387h -> new(0x28) ; TurnToFace(partner, 0.5f, 0.2f) 66B890 ; si no GetTaskType(sub) == 398h -> estado = 4 ; StandStill ; si no 0
  estado 2 (683F7D): GetTaskType(sub) != 0CBh -> 0 ; point.x == 0 y point.y == 0 (fcomp con 858B50h = 0.0, banderas 44h: solo igual) -> StandStill ;
      si no estado = 3 ; word [esi+70h] = 0 ; new(0x28) ; GoToPointAndStandStill 668120(4, &point, 0.1f, 0, 0, 1)
  estado 1 (684018): GetTaskType(sub) != 0CBh -> 0 ; byte [esi+58h] != 0 -> 681FE0(ped, &point, &target) y [t+40h..48h] = target con t = 681740 ; estado = 2 ; StandStill
  StandStill = new(0x20) ; 62F310(50, 0, 0, 8.0f) ; el original devuelve el objeto o 0 si la reserva es nula.
Constructores (modelados desde su desensamblado): StandStill 62F310 (+0x61A390): [+4]=0 [+8]=50 [+0C]=0 [+10]=0 byte[+14]=0 byte[+15]=0 byte[+18]=0 byte[+19]=0 [+1C]=41000000h ;
  Wander 48E4F0 (+66F450, +61A3B0): [+4]=0 [+8]=0 [+0C]=4 byte[+10]=dir [+14]=3F000000h word[+18]=FFFFh word[+1C]=FFFFh [+20]=0 byte[+24]=(fill & F0h)|1 [+28]=0 [+2C]=0 byte[+30]=0 byte[+31]=0 [+34]=0 ;
  GoTo 668120 (+509760 distinto -> copia): [+4]=0 [+8]=0 [+0C]=4 [+10..18]=point [+1C]=0.1f -> (minimo 0.5f) 3F000000h [+20]=0 byte[+24]=(fill & E4h)|6 ;
  Turn 66B890 (+61A3B0): [+4]=0 [+8]=0 [+0C]=partner byte[+10]=1 [+20]=3F000000h [+24]=3E4CCCCDh (el RegisterReference del partner no hace nada: edificio sin IPL).
Ningun llamador del EXE lee ECX/EDX de salida (ver README.md).
"""
import math
import struct

CASES = 4096
NAME = 'CTaskComplexPartner::CreateNextSubTask'
CASES_BY_NAME = {NAME: CASES}
NAMES = [NAME]

OFF_STATE, OFF_FIRST, OFF_UPD, OFF_COUNTER, OFF_TARGET = 0x59, 0x5A, 0x5B, 0x70, 0x4C
OFF_OTHER_FIRST, OFF_OTHER_POINT = 0x5A, 0x40
OTHER_POINT_FILL = 0xE1
C1, C2, C3 = 0x3F7D70A4, 0x3F8147AE, 0x3CA3D70A    # 0.99f, 1.01f, 0.02f
NAN_INDEFINITE = 0xFFC00000


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def bits(f):
    return struct.unpack('<I', struct.pack('<f', f))[0]


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x007FFFFF) != 0


def s8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def fsqrt(x):
    if math.isnan(x):
        return x
    return f32(math.sqrt(x)) if x >= 0 else math.nan


def norm2(x, y):
    """CVector2D::Normalise del EXE (0x44E480): con x*x + y*y <= 0 (ordenado) solo x = 1; si no (tambien NaN) multiplica por 1 / sqrt(suma)."""
    s = f32(f32(x * x) + f32(y * y))
    if s <= 0.0:
        return 1.0, y
    q = fsqrt(s)
    inv = math.nan if math.isnan(q) else (math.inf if q == 0.0 else f32(1.0 / q))
    return f32(x * inv), f32(y * inv)


def x87_sincos(h):
    """fsin / fcos del x87 sobre el float h (aproximado con math: el banco compara con 2 ulp). |h| >= 2^63 deja el operando; +-inf da el NaN indefinido."""
    if math.isnan(h):
        return h, h
    if math.isinf(h):
        return math.nan, math.nan
    if abs(h) >= 2.0 ** 63:
        return h, h
    return f32(math.sin(h)), f32(math.cos(h))


def words_fill(fill):
    return [(fill & 0xFF) * 0x01010101] * 16


def setb(words, off, n, value):
    for i in range(n):
        pos = off + i
        w, sh = pos // 4, 8 * (pos % 4)
        words[w] = (words[w] & ~(0xFF << sh) & 0xFFFFFFFF) | (((value >> (8 * i)) & 0xFF) << sh)


def expected_fn(inp, case):
    if inp['this_off'] != 0x40:
        raise RuntimeError('Fixture invalida en %d: this_off' % case)
    ev = []
    stats = {}
    this_now = {OFF_STATE: inp['state'] & 0xFF, OFF_FIRST: inp['first'] & 0xFF, OFF_UPD: inp['upd'] & 0xFF}
    this_orig = dict(this_now)
    counter_new = None
    target_new = None
    other_first_now = inp['other_first'] & 0xFF
    other_point_new = None
    shift = None
    allocs = [None, None]
    newn = [0]
    ret = 0
    fills = [inp['fill0'] & 0xFF, inp['fill1'] & 0xFF]

    def find():
        ev.append(['find', 'I+4', inp['task_id'] & 0xFFFFFFFF])

    def new(size):
        ev.append(['new', size])
        k = newn[0]
        newn[0] += 1
        if k >= 2 or inp['alloc%d_null' % k]:
            stats['alloc%d_null_used' % k] = True
            return None
        return k

    def build(k, writer, label_words=None):
        w = words_fill(fills[k])
        writer(w, fills[k])
        allocs[k] = w
        return 'A%d+0' % k

    def stand():
        k = new(0x20)
        if k is None:
            return 0

        def wr(w, fill):
            setb(w, 4, 4, 0); setb(w, 8, 4, 50); setb(w, 0xC, 4, 0); setb(w, 0x10, 4, 0)
            setb(w, 0x14, 1, 0); setb(w, 0x15, 1, 0); setb(w, 0x18, 1, 0); setb(w, 0x19, 1, 0); setb(w, 0x1C, 4, 0x41000000)
        return build(k, wr)

    def done():
        changes = []
        for off in sorted(this_now):
            if this_now[off] != this_orig[off]:
                changes.append([off, this_now[off]])
        if counter_new is not None:
            old = inp['counter'] & 0xFFFF
            for i in range(2):
                nb, ob = (counter_new >> (8 * i)) & 0xFF, (old >> (8 * i)) & 0xFF
                if nb != ob:
                    changes.append([OFF_COUNTER + i, nb])
        if target_new is not None:
            old = [(inp['target'][j // 4] >> (8 * (j % 4))) & 0xFF for j in range(12)]
            for j in range(12):
                nb = (target_new[j // 4] >> (8 * (j % 4))) & 0xFF
                if nb != old[j]:
                    changes.append([OFF_TARGET + j, nb])
        changes.sort()
        other_changes = []
        if other_first_now != inp['other_first'] & 0xFF:
            other_changes.append([OFF_OTHER_FIRST, other_first_now])
        if other_point_new is not None:
            for j in range(12):
                nb = (other_point_new[j // 4] >> (8 * (j % 4))) & 0xFF
                if nb != OTHER_POINT_FILL:
                    other_changes.append([OFF_OTHER_POINT + j, nb])
        other_changes.sort()
        obs_allocs = []
        for k in range(2):
            if allocs[k] is None:
                obs_allocs.append({'vtable_written': False, 'words': [None] + words_fill(fills[k])[1:], 'outside_ok': True})
            else:
                obs_allocs.append({'vtable_written': True, 'words': [None] + allocs[k][1:], 'outside_ok': True})
        want = {'ret': ret, 'events': ev, 'nevents': len(ev), 'this_changes': changes, 'other_changes': other_changes, 'shift': shift, 'ped_other_ok': True,
                'others_intact': True, 'alloc0': obs_allocs[0], 'alloc1': obs_allocs[1]}
        return want

    def finish(path):
        nonlocal ret
        want = done()
        stats.update({
            'family%d' % inp['family']: True, 'path_' + path: True,
            'ret_null': want['ret'] == 0, 'ret_obj': isinstance(want['ret'], str) and want['ret'][0] == 'A', 'ret_seq': isinstance(want['ret'], str) and want['ret'][0] == 'Q',
            'alloc0_built': allocs[0] is not None, 'alloc1_built': allocs[1] is not None,
            'time_wrap': (inp['time_ms'] + 100000) > 0xFFFFFFFF and path == 'wander',
            'state_noncanon': (inp['state'] & 0xFF) > 7, 'first_noncanon': (inp['first'] & 0xFF) not in (0, 1, 0xFF),
            'stack_seed': True,
        })
        return want, stats

    state = inp['state'] & 0xFF
    if inp['partner_null']:
        return finish('partner_null')
    find()
    if not inp['found']:
        return finish('find_null')
    if not inp['other_is_ped']:
        return finish('other_ped')

    def other_state():
        find()
        return s8(inp['other_state'])

    if state == 6:
        if not (inp['streamed'] & 0xFF) or not (inp['upd'] & 0xFF):
            stats['seq_blocked_streamed'] = not (inp['streamed'] & 0xFF)
            stats['seq_blocked_upd'] = bool(inp['streamed'] & 0xFF) and not (inp['upd'] & 0xFF)
            return finish('seq_blocked')
        this_now[OFF_UPD] = (this_now[OFF_UPD] - 1) & 0xFF
        ev.append(['seq', 'T+0'])
        sr = inp['seq_ret'] & 0xFFFFFFFF
        ret = 0 if sr == 0 else 'Q+0' if sr == 1 else sr
        return finish('seq')
    if state == 5:
        if not (inp['face'] & 0xFF):
            os_ = other_state()
            if os_ >= 5:
                dk = inp['def_kind']
                wander = False
                if dk != 0:
                    ev.append(['type', 'W+0'])
                    if inp['def_type'] & 0xFFFFFFFF == 0x390:
                        ev.append(['wtype', 'W+0'])
                        wander = (inp['wander_type'] & 0xFFFFFFFF) == 0
                if wander:
                    if inp['ped_mat']:
                        fx, fy = inp['fwd'][0], inp['fwd'][1]
                        ev.append(['node', fx ^ 0x80000000, fy ^ 0x80000000])
                        stats['heading_matrix'] = True
                    else:
                        h = fl(inp['heading'])
                        s, c = x87_sincos(h)
                        xo, yo = -s, c   # out.x = fchs(sin), out.y = cos; argumentos: -out.x, -out.y
                        ev.append(['node~', bits(-xo), bits(-yo)])
                        stats['heading_sincos'] = True
                    k = new(0x38)
                    if k is None:
                        raise RuntimeError('Fixture invalida en %d: reserva nula del Wander (el EXE escribiria en nulo+0x34)' % case)
                    dirb = inp['heading_ret'] & 0xFF
                    tm = (inp['time_ms'] + 100000) & 0xFFFFFFFF

                    def wr(w, fill):
                        setb(w, 4, 4, 0); setb(w, 8, 4, 0); setb(w, 0xC, 4, 4); setb(w, 0x10, 1, dirb); setb(w, 0x14, 4, 0x3F000000)
                        setb(w, 0x18, 2, 0xFFFF); setb(w, 0x1C, 2, 0xFFFF); setb(w, 0x20, 4, 0); setb(w, 0x24, 1, (fill & 0xF0) | 1)
                        setb(w, 0x28, 4, 0); setb(w, 0x2C, 4, 0); setb(w, 0x30, 1, 0); setb(w, 0x31, 1, 0); setb(w, 0x34, 4, tm)
                    build(k, wr)
                    ev.append(['settask', 'J+4', 'A%d+0' % k, 4, 0])
                    this_now[OFF_STATE] = 6
                    ret = stand()
                    return finish('wander')
                this_now[OFF_STATE] = 6
                stats['nonface_def_skip'] = True
                ret = stand()
                return finish('nonface_high')
            ret = stand()
            return finish('nonface_low')
        if (inp['first'] & 0xFF) == 1:
            os_ = other_state()
            if os_ == 6:
                this_now[OFF_STATE] = 6
            ret = stand()
            return finish('face_first1')
        # geometria
        px, py = fl(inp['ped'][0]), fl(inp['ped'][1])
        qx, qy = fl(inp['part'][0]), fl(inp['part'][1])
        vx, vy = f32(px - qx), f32(py - qy)
        dist = fsqrt(f32(f32(vx * vx) + f32(vy * vy)))
        if fl(C1) < dist < fl(C2):
            this_now[OFF_STATE] = 6
            ret = stand()
            return finish('arrived')
        vx, vy = norm2(vx, vy)
        dx, dy = f32(f32(vx + qx) - px), f32(f32(vy + qy) - py)
        ln = fsqrt(f32(f32(dx * dx) + f32(dy * dy)))
        if ln > fl(C3):
            dx, dy = norm2(dx, dy)
            dx, dy = f32(dx * fl(C3)), f32(dy * fl(C3))
            stats['shift_normalised'] = True
        rx, ry = fl(inp['right'][0]), fl(inp['right'][1])
        fx, fy = fl(inp['fwd'][0]), fl(inp['fwd'][1])
        s0, s1 = f32(f32(rx * dx) + f32(ry * dy)), f32(f32(fx * dx) + f32(fy * dy))
        shift = [bits(s0), bits(s1)]
        stats['shift_nan'] = math.isnan(s0) or math.isnan(s1)
        ret = stand()
        return finish('shift')
    if state == 4:
        os_ = other_state()
        if os_ in (4, 5):
            if (this_now[OFF_FIRST]) == 0xFF:
                this_now[OFF_FIRST] = 0
            find()
            if other_first_now == 0xFF:
                other_first_now = 1
            this_now[OFF_STATE] = 5
            ret = stand()
            return finish('s4_both')
        if this_now[OFF_FIRST] == 0xFF:
            this_now[OFF_FIRST] = 1
        find()
        if other_first_now == 0xFF:
            other_first_now = 0
        other_point_new = list(inp['target'])
        target_new = None
        ret = stand()
        return finish('s4_not')
    if state == 3:
        ev.append(['type', 'S+0'])
        st = inp['sub_type'] & 0xFFFFFFFF
        if st == 0x387:
            k = new(0x28)
            if k is None:
                ret = 0
                return finish('s3_turn_null')

            def wr(w, fill):
                setb(w, 4, 4, 0); setb(w, 8, 4, 0); setb(w, 0xC, 4, 0); w[3] = 'P+0'; setb(w, 0x10, 1, 1); setb(w, 0x20, 4, 0x3F000000); setb(w, 0x24, 4, 0x3E4CCCCD)
            ret = build(k, wr)
            return finish('s3_turn')
        ev.append(['type', 'S+0'])
        if st == 0x398:
            this_now[OFF_STATE] = 4
            ret = stand()
            return finish('s3_to4')
        return finish('s3_null')
    if state == 2:
        ev.append(['type', 'S+0'])
        if (inp['sub_type'] & 0xFFFFFFFF) != 0xCB:
            return finish('s2_null')
        if fl(inp['point'][0]) == 0.0 and fl(inp['point'][1]) == 0.0:
            ret = stand()
            return finish('s2_stand')
        this_now[OFF_STATE] = 3
        counter_new = 0
        k = new(0x28)
        if k is None:
            return finish('s2_goto_null')
        pt = list(inp['point'])

        def wr(w, fill):
            setb(w, 4, 4, 0); setb(w, 8, 4, 0); setb(w, 0xC, 4, 4)
            for j in range(3):
                setb(w, 0x10 + 4 * j, 4, pt[j])
            setb(w, 0x1C, 4, 0x3F000000); setb(w, 0x20, 4, 0); setb(w, 0x24, 1, (fill & 0xE4) | 6)
        ret = build(k, wr)
        return finish('s2_goto')
    if state == 1:
        ev.append(['type', 'S+0'])
        if (inp['sub_type'] & 0xFFFFFFFF) != 0xCB:
            return finish('s1_null')
        if inp['lead'] & 0xFF:
            ev.append(['calc', 'T+0', 'D+0', 'T+64', 'T+76'])
            target_new = list(inp['calc'])
            find()
            other_point_new = list(inp['calc'])
            stats['lead'] = True
        this_now[OFF_STATE] = 2
        ret = stand()
        return finish('s1_lead' if inp['lead'] & 0xFF else 's1_nolead')
    return finish('default')


def expected(function, inp, case):
    if function == NAME:
        return expected_fn(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    inp = dict(family=15, state=6, task_id=0x4B4, partner_null=0, found=1, other_is_ped=1, other_state=5, other_first=0xFF, upd=3, streamed=1, face=0, first=0xFF, lead=1,
               sub_type=0xCB, counter=0, point=[0x3F800000, 0x40000000, 0], target=[0x41000000, 0x41100000, 0x41200000], calc=[0x42000000, 0x42100000, 0x42200000],
               def_kind=3, def_type=0x390, wander_type=0, ped_mat=1, ped=[0, 0], part=[0, 0], heading=0, fwd=[0x3F800000, 0], right=[0, 0x3F800000], heading_ret=0x1234ABCD,
               alloc0_null=0, alloc1_null=0, time_ms=1000, seq_ret=1, fill0=0x77, fill1=0x33, eax=0, edx=0, stack_seed=0, this_off=0x40)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # nulos iniciales
    w, st = expected_fn(_base(partner_null=1), 0)
    assert w['ret'] == 0 and w['events'] == [] and st['path_partner_null']
    w, st = expected_fn(_base(found=0), 0)
    assert w['ret'] == 0 and w['events'] == [['find', 'I+4', 0x4B4]] and st['path_find_null']
    w, st = expected_fn(_base(other_is_ped=0), 0)
    assert w['ret'] == 0 and len(w['events']) == 1 and st['path_other_ped']
    # estado 6: secuencia
    w, st = expected_fn(_base(), 1)
    assert w['ret'] == 'Q+0' and w['events'] == [['find', 'I+4', 0x4B4], ['seq', 'T+0']] and w['this_changes'] == [[0x5B, 2]] and st['path_seq']
    w, st = expected_fn(_base(streamed=0), 1)
    assert w['ret'] == 0 and w['this_changes'] == [] and st['path_seq_blocked']
    w, st = expected_fn(_base(upd=0), 1)
    assert w['ret'] == 0 and w['this_changes'] == [] and st['seq_blocked_upd']
    w, st = expected_fn(_base(seq_ret=0), 1)
    assert w['ret'] == 0 and w['events'][-1] == ['seq', 'T+0']
    # estado 5 sin mirar: estado del partner < 5 (con signo) o >= 5
    w, st = expected_fn(_base(state=5, other_state=0x80), 2)
    assert w['this_changes'] == [] and w['events'][2] == ['new', 0x20] and st['path_nonface_low']
    w, st = expected_fn(_base(state=5, other_state=5), 2)
    assert st['path_wander'] and w['this_changes'] == [[0x59, 6]] and [e[0] for e in w['events']] == ['find', 'find', 'type', 'wtype', 'node', 'new', 'settask', 'new'], w['events']
    assert w['alloc0']['words'][13] == 1000 + 100000 and w['alloc0']['words'][3] == 4 and (w['alloc0']['words'][4] & 0xFF) == 0xCD, w['alloc0']['words']
    assert w['alloc1']['words'][2] == 50 and w['ret'] == 'A1+0'
    w, st = expected_fn(_base(state=5, other_state=5, def_kind=0, def_type=0), 2)
    assert st['path_nonface_high'] and w['this_changes'] == [[0x59, 6]] and [e[0] for e in w['events']] == ['find', 'find', 'new']
    # mirar: first == 1 ; geometria
    w, st = expected_fn(_base(state=5, face=1, first=1, other_state=6), 3)
    assert w['this_changes'] == [[0x59, 6]] and st['path_face_first1']
    w, st = expected_fn(_base(state=5, face=1, first=0, ped=[0x3F7D70A4, 0], part=[0, 0]), 3)   # 0.99f exacto: no llega (no es > 0.99f)
    assert st['path_shift'] and w['shift'] is not None
    w, st = expected_fn(_base(state=5, face=1, first=0, ped=[0x3F7D70A5, 0], part=[0, 0]), 3)   # justo por encima de 0.99f: llega
    assert st['path_arrived'] and w['this_changes'] == [[0x59, 6]] and w['shift'] is None
    w, st = expected_fn(_base(state=5, face=1, first=0, ped=[0x3F8147AE, 0], part=[0, 0]), 3)   # 1.01f exacto: no llega
    assert st['path_shift']
    w, st = expected_fn(_base(state=5, face=1, first=0, ped=[0x3F8147AD, 0], part=[0, 0]), 3)
    assert st['path_arrived']
    # estado 4
    w, st = expected_fn(_base(state=4, other_state=4), 4)
    assert w['this_changes'] == [[0x59, 5], [0x5A, 0]] and w['other_changes'] == [[0x5A, 1]] and st['path_s4_both']
    w, st = expected_fn(_base(state=4, other_state=0), 4)
    assert w['this_changes'] == [[0x5A, 1]] and [0x5A, 0] in w['other_changes'] and st['path_s4_not']
    # estado 3
    w, st = expected_fn(_base(state=3, sub_type=0x387), 5)
    assert w['ret'] == 'A0+0' and w['alloc0']['words'][3] == 'P+0' and st['path_s3_turn']
    w, st = expected_fn(_base(state=3, sub_type=0x398), 5)
    assert w['this_changes'] == [[0x59, 4]] and st['path_s3_to4'] and [e[0] for e in w['events']] == ['find', 'type', 'type', 'new']
    w, st = expected_fn(_base(state=3, sub_type=0x1), 5)
    assert w['ret'] == 0 and st['path_s3_null']
    # estado 2
    w, st = expected_fn(_base(state=2, point=[0, 0x80000000, 5]), 6)
    assert st['path_s2_stand'] and w['this_changes'] == []
    w, st = expected_fn(_base(state=2), 6)
    assert st['path_s2_goto'] and w['this_changes'] == [[0x59, 3]] and w['alloc0']['words'][4] == 0x3F800000
    w, st = expected_fn(_base(state=2, counter=0xFFFF), 6)
    assert w['this_changes'] == [[0x59, 3], [0x70, 0], [0x71, 0]]
    # estado 1
    w, st = expected_fn(_base(state=1), 7)
    assert w['this_changes'][-1] == [0x59, 2] and ['calc', 'T+0', 'D+0', 'T+64', 'T+76'] in w['events'] and st['path_s1_lead'] and w['other_changes']
    w, st = expected_fn(_base(state=1, lead=0), 7)
    assert w['this_changes'] == [[0x59, 2]] and w['other_changes'] == [] and st['path_s1_nolead']
    # sin rama
    for sv in (0, 7, 8, 0x80, 0xFF):
        w, st = expected_fn(_base(state=sv), 8)
        assert w['ret'] == 0 and w['events'] == [['find', 'I+4', 0x4B4]] and st['path_default']
    # fixtures invalidas
    try:
        expected_fn(_base(state=5, other_state=5, alloc0_null=1), 9)
        raise SystemExit('el oraculo debia rechazar la fixture invalida')
    except RuntimeError:
        pass
    print('ok')
