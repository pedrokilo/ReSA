"""Oraculo independiente del banco PartnerFirstSub31: CTaskComplexPartner::CreateFirstSubTask (0x681F20).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x681F20 (thiscall, ECX=this -> ESI, 1 argumento de pila `ped` -> EDI, `ret 4`, marco SEH propio con `fs:[0]`, guarda ESI/EDI):
  681F38  al=[esi+58h] (m_leadSpeaker, BYTE) ; test al,al ; je 681F76
  681F44  cmp dword [esi+34h],4B4h (m_taskId == TASK_COMPLEX_PARTNER_CHAT) ; jne 681F76
  681F4D  al=[esi+74h] (m_conversationEnabled, BYTE) ; test al,al ; je 681F76
  681F54  eax=[esi+38h] (m_partner) ; push eax ; push edi (ped) ; call 4E50E0 (CAEPedSpeechAudioEntity::RequestPedConversation(ped, partner), cdecl) ; add esp,8
  681F61  test al,al ; jne 681F72
  681F65  al=[esi+75h] ; test al,al ; je 681FBC (devuelve NULO: xor eax,eax)   [field_75 == 0]
  681F6C  byte [esi+74h]=0 ; jmp 681F76                                         [field_75 != 0]
  681F72  byte [esi+76h]=1                                                        [conversacion cargada]
  681F76  ecx=edi ; call 5E0480 (CPed::StopPlayingHandSignal, thiscall sin argumentos)
  681F7D  push 20h ; call 61A5A0 (CTask::operator new(0x20), cdecl) ; add esp,4 ; eax nulo -> 681FBC (devuelve nulo)
  681F97  push 41000000h (8.0f) ; push 0 ; push 0 ; push 32h ; ecx=eax ; call 62F310 (CTaskSimpleStandStill(50, false, false, 8.0f), `ret 10h`) ; devuelve el objeto (EAX = this del constructor)
Cualquier valor distinto de 0 de los bytes +0x58/+0x74/+0x75 y de AL de la peticion cuenta como verdadero. El constructor del EXE (0x62F310 -> 0x61A390 + campos) deja el objeto:
  +0 vtable, +4 = 0, +8 = 50, +0C = 0, +10 = 0, +14 (byte) = 0, +15 (byte) = 0, +18 (byte) = 0, +19 (byte) = 0, +1C = 0x41000000; el resto (+16/17, +1A/1B) intacto.
Llamadores del EXE: solo por tabla virtual (CTaskComplex::... ranura +0x24: vtables 0x870664 / 0x8707C4 / 0x870824 y los saltos 0x6823B0 (Deal) y 0x6825A0 (Greet)).

Eventos que registran los falsos (se anotan los bytes de `this` en el instante de la llamada, para exigir el orden frente a las escrituras):
  (siempre 6 palabras: kind,a,b,c,d,e; las no usadas a 0)
  [1, ped, partner, byte+0x74, byte+0x76, byte+0x75]   RequestPedConversation        [2, ped, byte+0x74, byte+0x76, 0, 0]   StopPlayingHandSignal        [3, 0x20, byte+0x74, byte+0x76, 0, 0]   operator new
"""
M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexPartner::CreateFirstSubTask'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CONV_AT, F75_AT, LOADED_AT = 0x74, 0x75, 0x76
OBJ_BYTES = 0x20
CHAT = 0x4B4


def _obj_changes(fill):
    """Bytes del objeto (offsets 4..0x1F) que el constructor deja distintos del relleno."""
    written = {4: 0, 5: 0, 6: 0, 7: 0, 8: 50, 9: 0, 10: 0, 11: 0, 12: 0, 13: 0, 14: 0, 15: 0, 16: 0, 17: 0, 18: 0, 19: 0, 20: 0, 21: 0,
               24: 0, 25: 0, 28: 0, 29: 0, 30: 0, 31: 0x41}
    return [[o, v] for o, v in sorted(written.items()) if v != fill]


def expected_first(inp, case):
    if inp['this_off'] != 0x40 or inp['obj_off'] != 0x20:
        raise RuntimeError('Disposicion de arena inesperada')
    lead = inp['lead'] & 0xFF
    conv = inp['conv'] & 0xFF
    f75 = inp['f75'] & 0xFF
    loaded = inp['loaded'] & 0xFF
    req = inp['req'] & 0xFF
    ped = inp['ped'] & M32
    partner = inp['partner'] & M32
    chat = lead != 0 and (inp['task_id'] & M32) == CHAT
    events = []
    changes = []
    null_by_request = False
    request_asked = chat and conv != 0
    if request_asked:
        events.append([1, ped, partner, conv, loaded, f75])
        if req != 0:
            if loaded != 1:
                changes.append([LOADED_AT, 1])
            loaded = 1
        elif f75 != 0:
            changes.append([CONV_AT, 0])
            conv = 0
        else:
            null_by_request = True
    alloc_null = bool(inp['alloc_null'])
    if null_by_request:
        ret = 0
        obj = []
    else:
        events.append([2, ped, conv, loaded, 0, 0])   # el C++ registra siempre 6 palabras por evento (kind,a,b,c,d,e)
        events.append([3, OBJ_BYTES, conv, loaded, 0, 0])
        if alloc_null:
            ret, obj = 0, []
        else:
            ret, obj = 1, _obj_changes(inp['fill'] & 0xFF)
    changes.sort()
    want = {'ret': ret, 'events': events, 'this_changes': changes,
            'obj_vtable_written': ret == 1, 'obj_changes': obj, 'obj_outside_ok': True}
    stats = {
        'family%d' % inp['family']: True,
        'lead_zero': lead == 0, 'lead_one': lead == 1, 'lead_noncanon': lead > 1,
        'type_chat': (inp['task_id'] & M32) == CHAT, 'type_other': (inp['task_id'] & M32) != CHAT,
        'chat': chat, 'chat_conv': request_asked, 'chat_no_conv': chat and not request_asked,
        'request_true': request_asked and req != 0, 'request_false_f75': request_asked and req == 0 and f75 != 0,
        'request_false_null': null_by_request,
        'req_noncanon': request_asked and req > 1, 'conv_noncanon': request_asked and (inp['conv'] & 0xFF) > 1,
        'f75_noncanon': request_asked and req == 0 and f75 > 1,
        'loaded_already1': request_asked and req != 0 and (inp['loaded'] & 0xFF) == 1,
        'loaded_set': request_asked and req != 0 and (inp['loaded'] & 0xFF) != 1,
        'alloc_null': alloc_null and not null_by_request, 'object_built': ret == 1,
        'req_high_nonzero': inp['req_high'] != 0,
        'ped_zero': ped == 0, 'ped_ffff': ped == M32, 'partner_zero': partner == 0, 'partner_ffff': partner == M32,
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_first(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    base = dict(family=3, lead=1, task_id=CHAT, conv=1, f75=0, loaded=0, req=1, req_high=0xABCDEF, alloc_null=False, fill=0x77, ped=0x1234, partner=0x5678,
                eax=0, edx=0, stack_seed=0, this_off=0x40, obj_off=0x20)
    # conversacion aceptada: +0x76 = 1, luego mano + new + objeto
    w, st = expected_first(base, 0)
    assert w['ret'] == 1 and w['this_changes'] == [[LOADED_AT, 1]] and st['request_true'] and st['loaded_set'] and st['object_built']
    assert w['events'] == [[1, 0x1234, 0x5678, 1, 0, 0], [2, 0x1234, 1, 1, 0, 0], [3, 0x20, 1, 1, 0, 0]], w['events']
    assert [4, 0] in w['obj_changes'] and [8, 50] in w['obj_changes'] and [31, 0x41] in w['obj_changes']
    assert not any(o in (0, 1, 2, 3, 22, 23, 26, 27) for o, _ in w['obj_changes'])
    # +0x76 ya a 1: sin cambios en this
    w, st = expected_first(dict(base, loaded=1), 1)
    assert w['this_changes'] == [] and st['loaded_already1']
    # rechazada con field_75 != 0: +0x74 = 0 ANTES de la mano y del new
    w, st = expected_first(dict(base, req=0, f75=0x80), 2)
    assert w['this_changes'] == [[CONV_AT, 0]] and w['events'][1] == [2, 0x1234, 0, 0, 0, 0] and w['ret'] == 1 and st['f75_noncanon']
    # rechazada con field_75 == 0: nulo sin mas llamadas
    w, st = expected_first(dict(base, req=0, f75=0), 3)
    assert w['ret'] == 0 and len(w['events']) == 1 and w['this_changes'] == [] and w['obj_changes'] == [] and not w['obj_vtable_written'] and st['request_false_null']
    # sin conversacion / no jefe / otro tipo: no se pregunta
    for kw in (dict(conv=0), dict(lead=0), dict(task_id=0x4B3), dict(task_id=CHAT ^ 0x10000)):
        w, st = expected_first(dict(base, **kw), 4)
        assert [e[0] for e in w['events']] == [2, 3] and w['ret'] == 1 and w['this_changes'] == []
    # lead no canonico cuenta como verdadero; los 24 bits altos de AL no cuentan
    w, st = expected_first(dict(base, lead=0x80, req=0x80, req_high=0xFFFFFF), 5)
    assert [e[0] for e in w['events']] == [1, 2, 3] and st['lead_noncanon'] and st['req_noncanon']
    # operator new nulo: devuelve nulo tras la mano; el objeto no se toca
    w, st = expected_first(dict(base, alloc_null=True), 6)
    assert w['ret'] == 0 and [e[0] for e in w['events']] == [1, 2, 3] and w['obj_changes'] == [] and st['alloc_null']
    # relleno igual a un valor escrito: ese byte no cambia
    w, st = expected_first(dict(base, fill=0x41), 7)
    assert [31, 0x41] not in w['obj_changes'] and [8, 50] in w['obj_changes']
    print('ok')
