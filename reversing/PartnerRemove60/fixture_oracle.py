"""Oraculo independiente del banco PartnerRemove60: CTaskComplexPartner::RemoveStreamedAnims (0x682370).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x682370 (thiscall, ECX=this -> EBP, sin argumentos, `ret`, void; guarda EBP/ESI/EDI):
  682373  al = byte [ebp+6Eh] (m_requiredAnimsStreamedIn, BYTE) ; test al,al ; je 6823A6 (fin)
  68237C  eax = &this[5Eh] ; esi = eax ; edi = 858B54h ; ecx = 1 ; repe cmpsb  (compara el PRIMER byte del nombre con el de la cadena vacia 858B54h)
  68238F  je 6823A2 (nombre vacio)
  682393  (nombre no vacio) eax = 4D3990(&this[5Eh]) (GetAnimationBlockIndex(m_animBlockName)) ; 4D3FD0(eax) (RemoveAnimBlockRef(idx))
  6823A2  byte [ebp+6Eh] = 0 ; fin
Los sitios de llamada de 0x4D3990 / 0x4D3FD0 son cdecl (`add esp,8` tras la pareja). Llamadores: la ranura 12 (+0x30) de las vtables 0x870664 (CTaskComplexPartner), 0x8707C4 (CTaskComplexPartnerChat) y 0x870754 (CTaskComplexPartnerDeal);
el escaneo de llamadores del lote 60 no halla lectores de ECX/EDX/EAX (ver README.md).
Colaboradores sustituidos en el banco por falsos identicos en ambos lados (parche de entrada en el EXE y en la DLL): registran sus argumentos en orden.
`this` solo cambia el byte +0x6E (a 0). La entrada de CAnimBlock del indice no se modifica.
"""
CASES = 4096
NAME = 'CTaskComplexPartner::RemoveStreamedAnims'
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
    name = 'T+%d' % 0x5E
    events = []
    changes = []
    if flag == 0:
        path = 'skip'
    elif first == 0:
        changes.append([OFF_FLAG, 0])
        path = 'empty'
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
        'name_empty': first == 0, 'name_nonempty': first != 0, 'name_high': first >= 0x80,
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
    inp = dict(family=2, flag=1, name_first=0x41, idx_kind=3, idx=7, num_blocks=120, loaded=1, eax=0, edx=0, stack_seed=0, this_off=0x40)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) con referencia (cualquier byte != 0) y nombre no vacio: indice, RemoveAnimBlockRef y flag = 0
    for fv in (1, 2, 0x80, 0xFF):
        want, st = expected_fn(_base(flag=fv), 0)
        assert want['events'] == [['index', 'T+94'], ['remove_ref', 7]] and want['this_changes'] == [[0x6E, 0]] and st['path_remove']
    # 2) nombre vacio: flag = 0 sin llamadas
    for fv in (1, 2, 0xFF):
        want, st = expected_fn(_base(flag=fv, name_first=0), 1)
        assert want['events'] == [] and want['this_changes'] == [[0x6E, 0]] and st['path_empty']
    # 3) sin referencia: nada (nombre vacio o no)
    for first in (0, 0x41, 0xFF):
        want, st = expected_fn(_base(flag=0, name_first=first), 2)
        assert want['events'] == [] and want['this_changes'] == [] and st['path_skip']
    # 4) indices de borde
    want, st = expected_fn(_base(idx=119, idx_kind=1), 3)
    assert want['events'][1] == ['remove_ref', 119] and st['idx_last']
    want, st = expected_fn(_base(idx=0, idx_kind=0), 3)
    assert want['events'][1] == ['remove_ref', 0] and st['idx_first']
    # 5) fixtures invalidas
    for bad in (_base(idx=120), _base(this_off=0x20)):
        try:
            expected_fn(bad, 5)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except RuntimeError:
            pass
    print('ok')
