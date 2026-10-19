"""Oraculo independiente del banco PartnerShoveSeq28: CTaskComplexPartnerShove::GetPartnerSequence (0x683120).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x683120 (thiscall, ECX=this -> EDI, sin argumentos, `ret`; marco SEH propio con fs:[0]; guarda EBX/ESI/EDI):
  683120..683135 push -1 / push 84673Ch / marco SEH; push ecx(vacio) ; push ebx esi edi ; edi = this
  68313D  eax = 61A5A0(0x40)                    ; CTask::operator new  (pool de tareas; 61A5A0: ecx=[B744A8] ; jmp 61A500)
  683149  esi = eax ? 632BD0(eax) : 0           ; CTaskComplexSequence::CTaskComplexSequence (ctor 0x632BD0)
  683162  ebx = -1 ; eax = 61A5A0(0x28)         ; segunda reserva
  683177  si eax: edx = [edi+38h] (m_partner) ; push 3A83126Fh ; push [86FC7C] (=0.5f en .rdata) ; push edx ; ecx=eax ; call 66B890   ; ctor Turn(partner, 0.5f, 0.001f)
          si eax == 0: eax = 0
  68319E  push eax ; ecx = esi ; call 632D10 (AddTask)                           ; AddTask(esi, turn)   (esi puede ser 0 solo si la 1.ª reserva fallo: no se prueba)
  6831AA  al = [edi+58h] (m_leadSpeaker) ; test al,al ; push 20h
          al != 0: eax = 61A5A0(0x20) ; si eax: ecx=eax ; push 0 ; push 40800000h ; push 0A4h ; push 0 ; call 61A8B0     ; Run(grupo 0, anim 0xA4, 4.0f, false)
          al == 0: lo mismo con anim 8Eh
          si eax == 0: eax = 0
  683213  ecx = esi ; push eax ; call 632D10 ; eax = esi (la secuencia) ; restaura SEH ; ret
Cualquier llamador: CTaskComplexPartner::CreateNextSubTask 0x683B46 (`call [eax+34h]`) -> 0x6840AC : `mov ecx,[esp+30h]` (ECX sobrescrito), EDX no se lee, EAX = retorno.

Colaboradores (desensamblados en 0x61A500, 0x632BD0, 0x632D10, 0x66B890, 0x61A8B0, 0x61A390, 0x61A3B0):
  Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0), si no al = 1 ; prueba el bit 7 del estado ; si libre:
      estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
  Secuencia (0x632BD0, tras 0x61A3B0 {[+4]=0, [+8]=0, vtable}): [+0C]=0, [+30]=0, [+34]=0, byte[+38]=0, [+3C]=0, 8 dwords a 0 desde +10 ; bytes +39..+3B no se escriben.
  AddTask (0x632D10): primera de las 8 posiciones [+10..+2C] con 0 -> la tarea (aunque sea 0).
  Turn (0x66B890, tras 0x61A3B0): [+0C]=partner ; byte[+10]=1 ; si partner != 0 -> RegisterReference(0x571B70)(&[+0C]) ; [+20]=0.5f ; [+24]=0.001f ; el resto queda como estaba (0xCD).
  Run (0x61A8B0, tras 0x61A390 {[+4]=0}): [+08]=0 ; byte[+0C] = ((hold&1)<<2) | (byte[+0C] & F8) ; [+10]=grupo ; [+14]=anim ; [+18]=mezcla ; word[+1C]=190h (los 2 bytes altos no se escriben).
  RegisterReference (cuerpo en CEntity::RegisterReferenceWithResidue): no hace nada si el tipo es edificio, no temporal, no procedural y sin IPL, ni si ya hay una referencia con el mismo
      m_ppEntity ; si no: toma pEmptyList, la inserta en cabeza de m_pReferences con m_ppEntity.
Las vtables (palabra 0 de cada tarea) se sustituyen por GetTaskType: Secuencia 0xF4 (632C60), Turn 0x398 (66B900), Run 0x190 (m_nTaskType).
"""
import sys

M32 = 0xFFFFFFFF
CASES = 6144
NAME = 'CTaskComplexPartnerShove::GetPartnerSequence'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_SEQ, TYPE_TURN, TYPE_RUN = 0xF4, 0x398, 0x190
F_HALF, F_MAXHEADING, F_BLEND = 0x3F000000, 0x3A83126F, 0x40800000
ANIM_LEAD, ANIM_FOLLOW = 0xA4, 0x8E
REFS_OFF = 0x24


def pool_alloc(win, pos, last):
    """Siguiente reserva: devuelve (rango en la ventana o None, nuevo cursor, nuevo pos)."""
    if pos < len(win):
        return pos, win[pos][0], pos + 1
    return None, 0, pos          # sin ranura libre: el cursor queda en 0


def seq_words(turn_label, run_label):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0
    for k in range(4, 12):
        w[k] = 0
    # AddTask: primera posicion libre (con 0); AddTask(0) deja el 0 en su sitio
    slots = [0] * 8
    for label in (turn_label, run_label):
        for i in range(8):
            if slots[i] == 0:
                slots[i] = label if label is not None else 0
                break
    for i in range(8):
        w[4 + i] = slots[i]
    w[12] = 0
    w[13] = 0
    w[14] = 0xCDCDCD00           # byte [+38] = 0 ; bytes +39..+3B sin escribir
    w[15] = 0
    return w


def turn_words(partner_label):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = partner_label if partner_label is not None else 0
    w[4] = 0xCDCDCD01            # byte [+10] = 1
    w[8] = F_HALF
    w[9] = F_MAXHEADING
    return w


def run_words(anim):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0xCDCDCDC8            # byte [+0C] = ((0&1)<<2) | (0xCD & 0xF8) = 0xC8
    w[4] = 0                     # grupo
    w[5] = anim
    w[6] = F_BLEND
    w[7] = 0xCDCD0190            # word [+1C] = 0x190
    return w


def expected_shove(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(4, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    if inp['refs_off'] != REFS_OFF:
        raise RuntimeError('Desplazamiento de m_pReferences inesperado: %#x' % inp['refs_off'])
    # --- reservas (orden: secuencia, turn, run)
    pos = 0
    cur = last
    ranks = []
    for _ in range(3):
        r, cur, pos = pool_alloc(win, pos, cur)
        ranks.append(r)
    seq_rank, turn_rank, run_rank = ranks
    if seq_rank is None:
        raise RuntimeError('Fixture sin ninguna ranura libre en %d (el EXE se detendria)' % case)
    has_partner = bool(inp['has_partner'])
    label = lambda k: 'W%d+0' % k
    # --- cambios de estado del pool
    changes = []
    for r in ranks:
        if r is not None:
            idx, st = win[r]
            changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
    changes.sort()
    # --- Turn y referencias
    refs_head, refs_next = inp['refs']['head'], inp['refs']['next']
    registered = False
    dup = False
    skipped_building = False
    partner_refs = []
    old_chain = []
    if has_partner:
        for j in range(inp['refs_len']):
            old_chain.append(['N%d' % j, inp['node_pp'][j]])
        partner_refs = [list(e) for e in old_chain]
    ref_changes = []
    empty_head = 'R%d' % refs_head if refs_head >= 0 else 0
    ppentity_label = 'W%d+12' % turn_rank if turn_rank is not None else None
    if turn_rank is not None and has_partner:
        building_skip = inp['etype'] == 1 and not inp['temp'] and not inp['proc'] and inp['ipl'] == 0
        if building_skip:
            skipped_building = True
        elif any(e[1] == ppentity_label for e in old_chain):
            dup = True
        else:
            registered = True
            ref_changes.append([refs_head, 'N0' if old_chain else 0, ppentity_label])
            empty_head = 'R%d' % refs_next if refs_next >= 0 else 0
            partner_refs = [['R%d' % refs_head, ppentity_label]] + partner_refs
    # --- tareas
    anim = ANIM_LEAD if inp['lead'] != 0 else ANIM_FOLLOW
    tasks = []
    tasks.append({'type': TYPE_SEQ, 'words': seq_words(label(turn_rank) if turn_rank is not None else None, label(run_rank) if run_rank is not None else None), 'slot': seq_rank})
    if turn_rank is not None:
        tasks.append({'type': TYPE_TURN, 'words': turn_words('E+0' if has_partner else None), 'slot': turn_rank})
    if run_rank is not None:
        tasks.append({'type': TYPE_RUN, 'words': run_words(anim), 'slot': run_rank})
    want = {'ret': label(seq_rank), 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_ok': True, 'partner_other_ok': True, 'nodes_ok': True, 'partner_refs': partner_refs, 'empty_head': empty_head, 'ref_changes': ref_changes}
    nalloc = sum(r is not None for r in ranks)
    stats = {
        'family%d' % inp['family']: True,
        'alloc3': nalloc == 3, 'alloc2': nalloc == 2, 'alloc1': nalloc == 1,
        'turn_null': turn_rank is None, 'run_null': run_rank is None,
        'partner_null': not has_partner,
        'registered': registered, 'dup_ref': dup, 'skip_building': skipped_building,
        'lead_zero': inp['lead'] == 0, 'lead_nonzero': inp['lead'] != 0, 'lead_noncanon': inp['lead'] > 1, 'lead_high': inp['lead'] >= 0x80,
        'anim_lead': anim == ANIM_LEAD, 'anim_follow': anim == ANIM_FOLLOW,
        'last_at_cap1': last == cap - 1, 'search_wraps': bool(win) and win[0][0] <= last,
        'win_consecutive': nalloc >= 2 and win[1][0] == (win[0][0] + 1) % cap,
        'id_wrap': any((win[r][1] & 0x7F) == 0x7F for r in ranks if r is not None),
        'id_zero': any((win[r][1] & 0x7F) == 0 for r in ranks if r is not None),
        'cursor_zero_after_fail': nalloc < 3 and cur == 0,
        'pool_mode%d' % inp['pool_mode']: True,
        'refs0': has_partner and inp['refs_len'] == 0 and turn_rank is not None, 'refs1': has_partner and inp['refs_len'] == 1 and turn_rank is not None,
        'refs2': has_partner and inp['refs_len'] == 2 and turn_rank is not None,
    }
    if has_partner:
        stats['etype%d' % inp['etype']] = True
        stats['building_ipl'] = inp['etype'] == 1 and inp['ipl'] != 0 and turn_rank is not None
        stats['building_temp'] = inp['etype'] == 1 and bool(inp['temp']) and turn_rank is not None
        stats['building_proc'] = inp['etype'] == 1 and bool(inp['proc']) and turn_rank is not None
    stats['stack_seed'] = True
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_shove(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    # 1) pool abundante, partner de tipo peaton sin referencias, m_leadSpeaker != 0
    win = [[10, 0x80 | 0x7F], [11, 0x85], [12, 0x80], [13, 0x80]]
    inp = dict(family=2, lead=0x80, has_partner=1, etype=3, temp=0, proc=0, ipl=0, refs_len=0, dup=0, pool_mode=0, pool_k=3, eax=0, edx=0, stack_seed=0, refs_off=REFS_OFF,
               pool=dict(cap=5000, last=9, free_total=1000, win=win), refs=dict(head=5, next=6), node_pp=[])
    want, st = expected_shove(inp, 0)
    assert want['ret'] == 'W0+0' and want['last_after'] == 12 and want['state_changes'] == [[10, 0], [11, 6], [12, 1]]
    t = want['tasks']
    assert [x['slot'] for x in t] == [0, 1, 2] and [x['type'] for x in t] == [TYPE_SEQ, TYPE_TURN, TYPE_RUN]
    assert t[0]['words'][4] == 'W1+0' and t[0]['words'][5] == 'W2+0' and t[0]['words'][6] == 0
    assert t[1]['words'][3] == 'E+0' and t[2]['words'][5] == ANIM_LEAD
    assert want['ref_changes'] == [[5, 0, 'W1+12']] and want['empty_head'] == 'R6' and want['partner_refs'] == [['R5', 'W1+12']]
    assert st['registered'] and st['alloc3'] and st['id_wrap'] and st['anim_lead']
    # 2) lead == 0 usa la animacion 0x8E; partner nulo no registra y deja 0 en +0C
    want, st = expected_shove(dict(inp, lead=0, has_partner=0), 1)
    assert want['tasks'][2]['words'][5] == ANIM_FOLLOW and want['tasks'][1]['words'][3] == 0 and want['ref_changes'] == [] and want['empty_head'] == 'R5'
    # 3) edificio sin IPL, no temporal ni procedural: no registra
    want, st = expected_shove(dict(inp, etype=1), 2)
    assert st['skip_building'] and want['ref_changes'] == [] and want['empty_head'] == 'R5'
    want, st = expected_shove(dict(inp, etype=1, ipl=7), 2)
    assert st['registered'] and want['ref_changes'] == [[5, 0, 'W1+12']]
    # 4) referencia ya registrada: no se anade otra
    want, st = expected_shove(dict(inp, refs_len=2, dup=1, node_pp=[0xA5A50000, 'W1+12']), 3)
    assert st['dup_ref'] and want['ref_changes'] == [] and want['partner_refs'] == [['N0', 0xA5A50000], ['N1', 'W1+12']]
    want, st = expected_shove(dict(inp, refs_len=2, node_pp=[0xA5A50000, 0xA5A50011]), 3)
    assert want['partner_refs'] == [['R5', 'W1+12'], ['N0', 0xA5A50000], ['N1', 0xA5A50011]] and want['ref_changes'] == [[5, 'N0', 'W1+12']]
    # 5) una sola ranura libre: la secuencia sin tareas y cursor a 0 por el fallo; dos libres: la tarea de animacion falla y el turn queda en la posicion 0
    p1 = dict(inp, pool=dict(cap=5000, last=4990, free_total=1, win=[[3, 0x83]]))
    want, st = expected_shove(p1, 4)
    assert [x['slot'] for x in want['tasks']] == [0] and want['last_after'] == 0 and want['tasks'][0]['words'][4:12] == [0] * 8 and want['ref_changes'] == [] and st['alloc1']
    p2 = dict(inp, pool=dict(cap=5000, last=4999, free_total=2, win=[[0, 0x80], [1, 0xFF]]))
    want, st = expected_shove(p2, 5)
    assert [x['slot'] for x in want['tasks']] == [0, 1] and want['last_after'] == 0 and want['tasks'][0]['words'][4] == 'W1+0' and want['tasks'][0]['words'][5] == 0
    assert want['state_changes'] == [[0, 1], [1, 0]] and st['alloc2'] and st['run_null']
    # 6) con Turn nulo (no puede ocurrir con dos libres, pero el modelo de AddTask lo cubre): la tarea de animacion ocupa la posicion 0
    w = seq_words(None, 'W2+0')
    assert w[4] == 'W2+0' and w[5] == 0
    # 7) una ventana inconsistente se detecta
    try:
        expected_shove(dict(inp, pool=dict(cap=5000, last=9, free_total=1, win=win)), 6)
        raise SystemExit('el oraculo debia rechazar la ventana inconsistente')
    except RuntimeError:
        pass
    print('ok')
