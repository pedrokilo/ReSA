"""Oraculo independiente del banco PartnerChatAbort30: CTaskComplexPartnerChat::MakeAbortable (0x682C60).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x682C60 (thiscall, ECX=this -> ESI, 3 argumentos de pila ped/prioridad/evento, `ret 0Ch`; sin marco SEH; guarda ESI):
  682C60  edx=[esp+0Ch] (evento) ; push esi ; push edx ; edx=[esp+10h] (prioridad) ; esi=ecx ; ecx=[esi+8] (m_pSubTask, SIN comprobar nulo)
  682C6F  eax=[ecx] (vtable) ; push edx (prioridad) ; edx=[esp+10h] (ped) ; push edx ; call [eax+18h]   -> subtarea->MakeAbortable(ped, prioridad, evento) (thiscall, ECX=subtarea)
  682C7A  test al,al ; je 682C9B (devuelve 0: xor al,al ; pop esi ; ret 0Ch)
  682C7E  al=[esi+74h] (m_conversationEnabled) ; test al,al ; je 682C95
  682C85  al=[esi+76h] (m_pedConversationLoaded) ; test al,al ; je 682C95
  682C8C  call 4E52A0 (CAEPedSpeechAudioEntity::ReleasePedConversation, cdecl sin argumentos) ; byte [esi+76h]=0
  682C95  al=1 ; pop esi ; ret 0Ch
Solo cuenta AL de la subtarea (test al,al) y de los dos bytes de `this` (test al,al: cualquier valor distinto de 0 cuenta). El retorno es AL (0 o 1); los 24 bits altos de EAX son residuo.
Unico efecto en memoria: +0x76 = 0 tras liberar. Llamadores: solo por tabla virtual (CTask::MakeAbortable, que lee AL).

Eventos que registran los falsos: [1, slot, etiqueta de la subtarea (1 = la falsa), ped, prioridad, evento, 0] y [2, byte +0x74, byte +0x76, 0, 0, 0] (ReleasePedConversation).
"""
import sys

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexPartnerChat::MakeAbortable'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
SLOT = 6
LOADED_AT = 0x76


def expected_abort(inp, case):
    if inp['slot'] != SLOT:
        raise RuntimeError('Ranura de la vtable inesperada %r' % inp['slot'])
    res = inp['res'] & 0xFF
    conv = inp['conv'] & 0xFF
    loaded = inp['loaded'] & 0xFF
    events = [[1, SLOT, 1, inp['ped'] & M32, inp['prio'] & M32, inp['event'] & M32]]
    changes = []
    released = False
    if res != 0:
        if conv != 0 and loaded != 0:
            events.append([2, conv, loaded, 0, 0, 0])
            released = True
            changes.append([LOADED_AT, 0])
        ret = 1
    else:
        ret = 0
    want = {'ret': ret, 'events': _pad(events),
            'this_changes': changes, 'sub_ok': True}
    stats = {
        'family%d' % inp['family']: True,
        'res_zero': res == 0, 'res_one': res == 1, 'res_noncanon': res > 1,
        'conv_zero': conv == 0, 'conv_one': conv == 1, 'conv_noncanon': conv > 1,
        'loaded_zero': loaded == 0, 'loaded_one': loaded == 1, 'loaded_noncanon': loaded > 1,
        'released': released,
        'abortable_no_release': res != 0 and not released,
        'abortable_conv_only': res != 0 and conv != 0 and loaded == 0,
        'abortable_loaded_only': res != 0 and conv == 0 and loaded != 0,
        'not_abortable_would_release': res == 0 and conv != 0 and loaded != 0,
        'res_high_nonzero': inp['res_high'] != 0,
        'ped_zero': inp['ped'] == 0, 'prio_zero': inp['prio'] == 0, 'event_zero': inp['event'] == 0,
        'ped_ffff': inp['ped'] == M32, 'event_ffff': inp['event'] == M32, 'prio_ffff': inp['prio'] == M32,
        'stack_seed': True,
    }
    return want, stats


def _pad(events):
    return [list(e) for e in events]


def expected(function, inp, case):
    if function == NAME:
        return expected_abort(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    base = dict(family=1, res=1, res_high=0xABCDEF, conv=1, loaded=1, ped=0x1234, prio=2, event=0xFFFFFFFF, eax=0, edx=0, stack_seed=0, this_off=0x40, slot=6)
    want, st = expected_abort(base, 0)
    assert want['ret'] == 1 and want['this_changes'] == [[0x76, 0]]
    assert want['events'] == [[1, 6, 1, 0x1234, 2, 0xFFFFFFFF], [2, 1, 1, 0, 0, 0]], want['events']
    assert st['released'] and st['res_high_nonzero'] and st['event_ffff']
    # AL no canonico y bytes no canonicos: cuentan como verdadero
    want, st = expected_abort(dict(base, res=0x80, conv=0xFF, loaded=2), 1)
    assert want['ret'] == 1 and want['this_changes'] == [[0x76, 0]] and want['events'][1] == [2, 0xFF, 2, 0, 0, 0] and st['res_noncanon']
    # subtarea no abortable: no se toca nada y se devuelve 0 aunque conv y cargada sean verdaderos
    want, st = expected_abort(dict(base, res=0), 2)
    assert want['ret'] == 0 and want['this_changes'] == [] and len(want['events']) == 1 and st['not_abortable_would_release']
    # abortable pero sin liberar
    for conv, loaded in ((0, 0), (0, 1), (1, 0)):
        want, st = expected_abort(dict(base, conv=conv, loaded=loaded), 3)
        assert want['ret'] == 1 and want['this_changes'] == [] and len(want['events']) == 1 and st['abortable_no_release']
    # solo cuenta AL: los 24 bits altos no cambian nada
    a, _ = expected_abort(dict(base, res_high=0), 4)
    b, _ = expected_abort(dict(base, res_high=0xFFFFFF), 4)
    assert a == b
    try:
        expected_abort(dict(base, slot=5), 5)
        raise SystemExit('el oraculo debia rechazar una ranura incorrecta')
    except RuntimeError:
        pass
    print('ok')
