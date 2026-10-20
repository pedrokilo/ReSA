"""Oraculo independiente del banco PartnerStream60: CTaskComplexPartner::StreamRequiredAnims (0x682310).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x682310 (thiscall, ECX=this -> EBP, sin argumentos, `ret`, void; guarda EBP/ESI/EDI):
  682313  al = byte [ebp+6Eh] (m_requiredAnimsStreamedIn, BYTE) ; test al,al ; jne 68236B (fin)
  68231C  eax = &this[5Eh] ; esi = eax ; edi = 858B54h ; ecx = 1 ; repe cmpsb  (compara el PRIMER byte del nombre con el de la cadena vacia 858B54h)
  68232F  je 682367 (nombre vacio)
  682333  (nombre no vacio) eax = 4D3990(&this[5Eh]) (GetAnimationBlockIndex(m_animBlockName)) ; dl = byte [eax*20h + 0B5D4B0h] (CAnimBlock[idx].IsLoaded, ms_aAnimBlocks = 0B5D4A0h) ; cmp dl,1 ; je 68235E
  68234C  (IsLoaded != 1, BYTE entero) 4087E0(eax + 63E7h, 8) (CStreaming::RequestModel(IFPToModelId(idx), KEEP_IN_MEMORY)) ; fin (sin tocar +0x6E)
  68235E  (IsLoaded == 1) 4D3FB0(eax) (AddAnimBlockRef) ; cae a 682367
  682367  byte [ebp+6Eh] = 1 ; fin
Los sitios de llamada de 0x4D3990 / 0x4D3FB0 / 0x4087E0 son cdecl (`add esp,N` tras la llamada). Llamadores: la ranura 11 (+0x2C) de las vtables 0x870664 (CTaskComplexPartner) y 0x8707C4 (CTaskComplexPartnerChat),
solo `CTaskComplexPartner::ControlSubTask` 0x6840D0 (`call [edx+2Ch]` en 0x684166), que sobrescribe ECX (`mov ecx,0C15448h`) y no lee EDX ni EAX.
Colaboradores sustituidos en el banco por falsos identicos en ambos lados (parche de entrada en el EXE y en la DLL): registran sus argumentos en orden.
`this` solo cambia el byte +0x6E (a 1). La entrada de CAnimBlock del indice no se modifica.
"""
CASES = 4096
NAME = 'CTaskComplexPartner::StreamRequiredAnims'
CASES_BY_NAME = {NAME: CASES}
NAMES = [NAME]
OFF_FLAG = 0x6E
MODEL_BASE, KEEP_IN_MEMORY = 0x63E7, 8


def expected_fn(inp, case):
    nb, idx = inp['num_blocks'], inp['idx']
    if not 0 <= idx < nb or inp['this_off'] != 0x40:
        raise RuntimeError('Fixture invalida en %d: idx %d fuera de [0, %d)' % (case, idx, nb))
    flag = inp['flag'] & 0xFF
    first = inp['name_first'] & 0xFF
    loaded = inp['loaded'] & 0xFF
    name = 'T+%d' % 0x5E
    events = []
    changes = []
    if flag != 0:
        path = 'skip'
    elif first == 0:
        changes.append([OFF_FLAG, 1])
        path = 'empty'
    else:
        events.append(['index', name])
        if loaded != 1:
            events.append(['request', idx + MODEL_BASE, KEEP_IN_MEMORY])
            path = 'request'
        else:
            events.append(['add_ref', idx])
            changes.append([OFF_FLAG, 1])
            path = 'add'
    want = {'events': events, 'nevents': len(events), 'block_intact': True, 'this_changes': changes}
    stats = {
        'family%d' % inp['family']: True,
        'path_' + path: True,
        'flag_zero': flag == 0, 'flag_one': flag == 1, 'flag_noncanon': flag > 1,
        'name_empty': first == 0, 'name_nonempty': first != 0, 'name_high': first >= 0x80,
        'loaded_zero': loaded == 0, 'loaded_one': loaded == 1, 'loaded_noncanon': loaded > 1,
        'loaded_odd_noncanon': loaded > 1 and loaded & 1 == 1,
        'idx_first': idx == 0, 'idx_last': idx == nb - 1, 'idx_mid': idx == nb // 2, 'idx_other': 0 < idx < nb - 1 and idx != nb // 2,
        'flag_changed': bool(changes),
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_fn(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    inp = dict(family=1, flag=0, name_first=0x41, idx_kind=3, idx=7, num_blocks=120, loaded=1, eax=0, edx=0, stack_seed=0, this_off=0x40)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) flag 0, nombre no vacio, bloque cargado: indice, AddAnimBlockRef y flag = 1
    want, st = expected_fn(_base(), 0)
    assert want['events'] == [['index', 'T+94'], ['add_ref', 7]] and want['this_changes'] == [[0x6E, 1]] and st['path_add']
    # 2) bloque no cargado (0, o no canonico != 1): RequestModel(idx + 0x63E7, 8), sin cambios
    for lv in (0, 2, 3, 0x7F, 0x80, 0xFF):
        want, st = expected_fn(_base(loaded=lv), 1)
        assert want['events'] == [['index', 'T+94'], ['request', 7 + 0x63E7, 8]] and want['this_changes'] == [] and st['path_request']
    # 3) nombre vacio: flag = 1 sin llamadas (aunque el bloque no este cargado)
    for lv in (0, 1, 2):
        want, st = expected_fn(_base(name_first=0, loaded=lv), 2)
        assert want['events'] == [] and want['this_changes'] == [[0x6E, 1]] and st['path_empty']
    # 4) con referencia (cualquier byte != 0): nada, con o sin nombre
    for fv in (1, 2, 0x80, 0xFF):
        for first in (0, 0x41, 0xFF):
            want, st = expected_fn(_base(flag=fv, name_first=first), 3)
            assert want['events'] == [] and want['this_changes'] == [] and st['path_skip']
    # 5) indices de borde
    want, st = expected_fn(_base(idx=119, idx_kind=1), 4)
    assert want['events'][1] == ['add_ref', 119] and st['idx_last']
    want, st = expected_fn(_base(idx=0, idx_kind=0, loaded=0), 4)
    assert want['events'][1] == ['request', 0x63E7, 8] and st['idx_first']
    # 6) fixtures invalidas
    for bad in (_base(idx=120), _base(this_off=0x20)):
        try:
            expected_fn(bad, 5)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except RuntimeError:
            pass
    print('ok')
