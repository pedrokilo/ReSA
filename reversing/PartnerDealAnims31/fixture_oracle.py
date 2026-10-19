"""Oraculo independiente del banco PartnerDealAnims31: CTaskComplexPartnerDeal::StreamRequiredAnims (0x6823C0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x6823C0 (thiscall, ECX=this -> ESI, sin argumentos, `ret`, void; guarda ESI):
  6823C3  al = byte [esi+6Eh] (m_requiredAnimsStreamedIn, BYTE) ; test al,al ; jne 682410
  sin referencia (byte == 0):
    6823CA  call 65E7F0 (ShouldLoadGangAnims) ; test al,al ; je 68242F (fin)
    6823D3  eax = 4D3990(esi+5Eh) (GetAnimationBlockIndex(m_animBlockName)) ; dl = byte [eax*20h + 0B5D4B0h] (CAnimBlock[idx].IsLoaded, ms_aAnimBlocks = 0B5D4A0h) ; cmp dl,1 ; je 682401
    6823EF  (IsLoaded != 1, BYTE entero) 4087E0(eax + 63E7h, 8) (CStreaming::RequestModel(IFPToModelId(idx), KEEP_IN_MEMORY)) ; fin
    682401  (IsLoaded == 1) 4D3FB0(eax) (AddAnimBlockRef) ; byte [esi+6Eh] = 1 ; fin
  con referencia (byte != 0, cualquier valor):
    682410  call 65E7F0 ; test al,al ; jne 68242F (fin)
    682419  eax = 4D3990(esi+5Eh) ; 4D3FD0(eax) (RemoveAnimBlockRef) ; byte [esi+6Eh] = 0 ; fin
Los sitios de llamada de 0x4D3990 / 0x4D3FB0 / 0x4D3FD0 / 0x4087E0 / 0x65E7F0 son cdecl (`add esp,N` tras la llamada). Unico llamador de la funcion (ranura 11 de la vtable 0x870754): CTaskComplexPartner::ControlSubTask 0x6840D0,
`call [edx+2Ch]` en 0x684166, que sobrescribe ECX (`mov ecx,0C15448h`) y no lee EDX ni EAX.
Colaboradores sustituidos en el banco por falsos identicos en ambos lados (parche de entrada en el EXE y en la DLL): registran sus argumentos en orden.
`this` solo cambia el byte +0x6E (a 1 al referenciar, a 0 al soltar). La entrada de CAnimBlock del indice no se modifica.
"""
CASES = 4096
NAME = 'CTaskComplexPartnerDeal::StreamRequiredAnims'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
OFF_FLAG = 0x6E
MODEL_BASE, KEEP_IN_MEMORY = 0x63E7, 8


def expected_stream(inp, case):
    nb, idx = inp['num_blocks'], inp['idx']
    if not 0 <= idx < nb or inp['this_off'] != 0x40:
        raise RuntimeError('Fixture invalida en %d: idx %d fuera de [0, %d)' % (case, idx, nb))
    flag = inp['flag'] & 0xFF
    should = (inp['should'] & 0xFF) != 0
    loaded = inp['loaded'] & 0xFF
    name = 'T+%d' % 0x5E
    events = []
    changes = []
    path = ''
    if flag == 0:
        events.append(['should'])
        if not should:
            path = 'no_load'
        else:
            events.append(['index', name])
            if loaded != 1:
                events.append(['request', idx + MODEL_BASE, KEEP_IN_MEMORY])
                path = 'request'
            else:
                events.append(['add_ref', idx])
                changes.append([OFF_FLAG, 1])
                path = 'add'
    else:
        events.append(['should'])
        if should:
            path = 'keep'
        else:
            events.append(['index', name])
            events.append(['remove_ref', idx])
            changes.append([OFF_FLAG, 0])
            path = 'remove'
    want = {'events': events, 'nevents': len(events), 'block_intact': True, 'this_changes': changes}
    stats = {
        'family%d' % inp['family']: True,
        'path_' + path: True,
        'flag_zero': flag == 0, 'flag_one': flag == 1, 'flag_noncanon': flag > 1,
        'should_hi_bits': (inp['should_hi'] & 0xFFFFFF00) != 0,
        'loaded_zero': loaded == 0, 'loaded_one': loaded == 1, 'loaded_noncanon': loaded > 1,
        'loaded_odd_noncanon': loaded > 1 and loaded & 1 == 1,
        'idx_first': idx == 0, 'idx_last': idx == nb - 1, 'idx_mid': idx == nb // 2, 'idx_other': 0 < idx < nb - 1 and idx != nb // 2,
        'flag_changed': bool(changes),
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_stream(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    inp = dict(family=1, flag=0, should=1, should_hi=0, idx_kind=3, idx=7, num_blocks=120, loaded=1, eax=0, edx=0, stack_seed=0, this_off=0x40)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) sin referencia, hay que cargar, bloque cargado: AddAnimBlockRef y byte = 1
    want, st = expected_stream(_base(), 0)
    assert want['events'] == [['should'], ['index', 'T+94'], ['add_ref', 7]] and want['this_changes'] == [[0x6E, 1]] and st['path_add']
    # 2) bloque no cargado (0, o no canonico != 1): RequestModel(idx + 0x63E7, 8), sin cambios
    for lv in (0, 2, 3, 0x7F, 0x80, 0xFF):
        want, st = expected_stream(_base(loaded=lv), 1)
        assert want['events'] == [['should'], ['index', 'T+94'], ['request', 7 + 0x63E7, 8]] and want['this_changes'] == [] and st['path_request']
    # 3) no hay que cargar: solo la consulta
    want, st = expected_stream(_base(should=0, should_hi=0xABCDEF00), 2)
    assert want['events'] == [['should']] and want['this_changes'] == [] and st['path_no_load'] and st['should_hi_bits']
    # 4) con referencia (cualquier byte != 0): si hay que cargar no se hace nada ; si no, RemoveAnimBlockRef y byte = 0
    for fv in (1, 2, 0x80, 0xFF):
        want, st = expected_stream(_base(flag=fv), 3)
        assert want['events'] == [['should']] and want['this_changes'] == [] and st['path_keep']
        want, st = expected_stream(_base(flag=fv, should=0), 3)
        assert want['events'] == [['should'], ['index', 'T+94'], ['remove_ref', 7]] and want['this_changes'] == [[0x6E, 0]] and st['path_remove']
    # 5) indices de borde
    want, st = expected_stream(_base(idx=119, idx_kind=1), 4)
    assert want['events'][2] == ['add_ref', 119] and st['idx_last']
    want, st = expected_stream(_base(idx=0, idx_kind=0, loaded=0), 4)
    assert want['events'][2] == ['request', 0x63E7, 8] and st['idx_first']
    # 6) fixtures invalidas
    for bad in (_base(idx=120), _base(this_off=0x20)):
        try:
            expected_stream(bad, 5)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except RuntimeError:
            pass
    print('ok')
