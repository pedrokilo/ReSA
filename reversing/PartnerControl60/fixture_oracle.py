"""Oraculo independiente del banco PartnerControl60: CTaskComplexPartner::ControlSubTask (0x6840D0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x6840D0 (thiscall, ECX=this -> ESI, un argumento de pila ped -> EDI, `ret 4`, devuelve CTask* en EAX; guarda ESI/EDI):
  6840D3  cmp byte [esi+59h],1 ; jg 6840EC            (m_partnerState con signo > 1 -> 6840EC)
  6840DE  eax = [esi+38h] (m_partner) ; test ; je 6840EC
  6840E5  al = byte [esi+5Ch] (m_taskCompleted) ; test al,al ; je 684126        (estado <= 1, hay partner y completed == 0: no se aborta)
  6840EC  eax = [esi+38h] ; test ; je 684115 (sin partner: se intenta abortar)
  6840F3  ecx = [esi+34h] (m_taskId) ; eax = [eax+47Ch] ; push ecx ; ecx = eax+4 ; call 681740 (CTaskManager::FindActiveTaskByType)
  684105  test eax,eax ; je 684115 ; cmp [eax+38h],edi ; jne 684115 ; al = byte [esi+5Ch] ; test al,al ; je 684126
  684115  ecx = [esi+8] (m_pSubTask) ; push 0 ; push 1 ; push edi ; call [vtable+18h] (MakeAbortable(ped, 1, 0)) ; test al,al ; jne 684152 (devuelve 0)
  684126  ecx = [esi+8] ; call [vtable+10h] (GetTaskType) ; cmp eax,387h ; jne 684159
  684135  inc word [esi+70h] ; cmp word [esi+70h],96h ; jle 684159 (con signo)
  684141  MakeAbortable(ped, 1, 0) de nuevo ; test al,al ; je 684159 ; 684152: eax = 0 ; ret 4
  684159  push 1 ; ecx = edi ; call 5E0360 (DropEntityThatThisPedIsHolding(true)) ; edx = [esi] ; ecx = esi ; call [edx+2Ch] (StreamRequiredAnims)
  684169  push edi ; ecx = 0C15448h (g_ikChainMan) ; call 6181A0 (IsLooking(ped)) ; test al,al ; je 684188
  684178  push 1F4h ; push edi ; ecx = 0C15448h ; call 618280 (AbortLookAt(ped, 500))
  684188  eax = [esi+8] ; ret 4
Unicos escritores de `this`: el word +0x70 (solo si el tipo de la subtarea es 0x387). Ningun llamador del EXE lee ECX/EDX/EAX de salida mas alla del retorno (ver README.md).
"""
CASES = 4096
NAME = 'CTaskComplexPartner::ControlSubTask'
CASES_BY_NAME = {NAME: CASES}
NAMES = [NAME]
IK = 0xC15448
OFF_COUNTER = 0x70


def s8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def s16(v):
    v &= 0xFFFF
    return v - 65536 if v >= 32768 else v


def expected_fn(inp, case):
    if inp['this_off'] != 0x40:
        raise RuntimeError('Fixture invalida en %d: this_off' % case)
    state = s8(inp['state'])
    partner_null = bool(inp['partner_null'])
    completed = inp['completed'] & 0xFF
    events = []
    changes = []
    path = ''
    abort_now = True
    if state <= 1 and not partner_null and completed == 0:
        abort_now = False
        path = 'skip_state'
    elif not partner_null:
        events.append(['find', 'I+4', inp['task_id'] & 0xFFFFFFFF])
        if inp['found'] and inp['other_is_ped'] and completed == 0:
            abort_now = False
            path = 'skip_sync'
    ret = 'S+0'
    done = False
    if abort_now:
        events.append(['abort', 'S+0', 'D+0', 1, 0])
        if inp['abort1'] & 0xFF:
            ret = 0
            done = True
            path = 'abort1_true'
        else:
            path = path or 'abort1_false'
    counter_after = inp['counter'] & 0xFFFF
    counter_over = False
    counter_abort = False
    if not done:
        events.append(['type', 'S+0'])
        if (inp['sub_type'] & 0xFFFFFFFF) == 0x387:
            counter_after = (counter_after + 1) & 0xFFFF
            if s16(counter_after) > 150:
                counter_over = True
                events.append(['abort', 'S+0', 'D+0', 1, 0])
                # El falso de MakeAbortable alterna abort1/abort2 por orden de llamada: sin la primera llamada (abort_now falso) la segunda usa abort1
                if inp['abort1' if not abort_now else 'abort2'] & 0xFF:
                    ret = 0
                    done = True
                    counter_abort = True
    if not done:
        events.append(['drop', 'D+0', 1])
        events.append(['stream', 'T+0'])
        events.append(['looking', IK, 'D+0'])
        if inp['looking'] & 0xFF:
            events.append(['abort_look', IK, 'D+0', 500])
    old = inp['counter'] & 0xFFFF
    if counter_after != old:
        for k in range(2):
            nb = (counter_after >> (8 * k)) & 0xFF
            ob = (old >> (8 * k)) & 0xFF
            if nb != ob:
                changes.append([OFF_COUNTER + k, nb])
    want = {'ret': ret, 'events': events, 'nevents': len(events), 'this_changes': changes, 'others_intact': True}
    stats = {
        'family%d' % inp['family']: True,
        'path_' + (path or 'check'): True,
        'ret_null': ret == 0, 'ret_sub': ret != 0,
        'partner_null': partner_null, 'found': bool(inp['found']) and not partner_null,
        'state_neg': state < 0, 'state_le1': state <= 1, 'completed_nonzero': completed != 0, 'completed_noncanon': completed > 1,
        'sub_387': (inp['sub_type'] & 0xFFFFFFFF) == 0x387, 'sub_near': (inp['sub_type'] & 0xFFFFFFFF) != 0x387 and (inp['sub_type'] & 0xFFF) == 0x387,
        'counter_over': counter_over, 'counter_abort': counter_abort, 'counter_neg': (inp['sub_type'] & 0xFFFFFFFF) == 0x387 and s16(counter_after) < 0,
        'counter_edge': (inp['sub_type'] & 0xFFFFFFFF) == 0x387 and 149 <= old <= 151,
        'counter_changed': bool(changes),
        'looking_true': not done and bool(inp['looking'] & 0xFF), 'looking_false': not done and not (inp['looking'] & 0xFF),
        'looking_noncanon': not done and (inp['looking'] & 0xFF) > 1,
        'abort_noncanon': abort_now and (inp['abort1'] & 0xFF) > 1,
        'abort_hi_bits': (inp['abort1'] & 0xFFFFFF00) != 0,
        'full_path': not done,
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_fn(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    inp = dict(family=0, state=1, partner_null=0, completed=0, found=1, other_is_ped=1, abort1=0, abort2=0, sub_type=0x300, counter=0, looking=0, task_id=0x4B4,
               eax=0, edx=0, stack_seed=0, this_off=0x40)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) estado <= 1, partner, completed 0: no Find, no abortar; ruta completa con IsLooking falso
    want, st = expected_fn(_base(), 0)
    assert want['events'] == [['type', 'S+0'], ['drop', 'D+0', 1], ['stream', 'T+0'], ['looking', IK, 'D+0']] and want['ret'] == 'S+0' and st['path_skip_state']
    # 2) estado 0x80 (-128 con signo) cuenta como <= 1; 0x7F no
    want, st = expected_fn(_base(state=0x80), 0)
    assert st['path_skip_state'] and st['state_neg']
    want, st = expected_fn(_base(state=0x7F, found=0), 0)
    assert want['events'][:2] == [['find', 'I+4', 0x4B4], ['abort', 'S+0', 'D+0', 1, 0]], want['events']
    # 3) completed != 0 con partner sincronizado: abortar; AL != 0 -> devuelve 0 sin mas
    want, st = expected_fn(_base(state=5, completed=1, abort1=0x80), 1)
    assert want['events'] == [['find', 'I+4', 0x4B4], ['abort', 'S+0', 'D+0', 1, 0]] and want['ret'] == 0 and st['path_abort1_true'] and st['abort_noncanon']
    # 4) sincronizado y completed == 0: no se aborta
    want, st = expected_fn(_base(state=5), 2)
    assert want['events'][0] == ['find', 'I+4', 0x4B4] and want['events'][1] == ['type', 'S+0'] and st['path_skip_sync']
    # 5) Find nulo / tarea con otro partner: abortar; resultado falso sigue
    for kw in (dict(found=0), dict(other_is_ped=0)):
        want, st = expected_fn(_base(state=5, **kw), 3)
        assert want['events'][:3] == [['find', 'I+4', 0x4B4], ['abort', 'S+0', 'D+0', 1, 0], ['type', 'S+0']] and st['path_abort1_false']
    # 6) sin partner: sin Find, abortar
    want, st = expected_fn(_base(partner_null=1, state=0), 3)
    assert want['events'][0] == ['abort', 'S+0', 'D+0', 1, 0] and st['partner_null']
    # 7) subtarea 0x387: contador; > 150 con signo -> segunda llamada a MakeAbortable
    for counter, over in ((148, False), (149, False), (150, True), (151, True), (0x7FFF, False), (0xFFFF, False), (0x8000, False), (0xFFFE, False)):
        want, st = expected_fn(_base(sub_type=0x387, counter=counter, abort1=1, abort2=1), 4)
        assert st['counter_over'] == over, (counter, over)
        assert st['counter_abort'] == over
        assert want['ret'] == (0 if over else 'S+0')
    want, st = expected_fn(_base(sub_type=0x387, counter=0xFF), 4)
    assert want['this_changes'] == [[0x70, 0], [0x71, 1]], want['this_changes']
    want, st = expected_fn(_base(sub_type=0x387, counter=0xFFFF), 4)
    assert want['this_changes'] == [[0x70, 0], [0x71, 0]]
    # 8) IsLooking con AL != 0 (aunque sea no canonico): AbortLookAt(ped, 500)
    want, st = expected_fn(_base(looking=0x1200), 5)
    assert want['events'][-1] == ['looking', IK, 'D+0'] and not st['looking_true']
    want, st = expected_fn(_base(looking=0x12FF), 5)
    assert want['events'][-1] == ['abort_look', IK, 'D+0', 500] and st['looking_noncanon']
    # 9) fixtures invalidas
    try:
        expected_fn(_base(this_off=0x20), 5)
        raise SystemExit('el oraculo debia rechazar la fixture invalida')
    except RuntimeError:
        pass
    print('ok')
