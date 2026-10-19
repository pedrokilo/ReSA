"""Oraculo independiente del banco PartnerDealSeq31: CTaskComplexPartnerDeal::GetPartnerSequence (0x682440).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x682440 (thiscall, ECX=this -> EDI, sin argumentos, `ret`; marco SEH propio con fs:[0]; guarda EBX/ESI/EDI):
  682459..  edi=this ; eax = 61A5A0(0x40) ; esi = eax ? 632BD0(eax) : 0                                  ; CTaskComplexSequence (esi)
  682482    al=[edi+58h] (m_leadSpeaker, BYTE) ; ebx=-1 ; test al,al ; push 28h ; je 682506
  lead != 0 (68248E):
            eax = 61A5A0(0x28) ; si eax: 66B890(eax, [edi+38h]=partner, [86FC7C]=0.5f, 3CA3D70Ah=0.02f) ; 632D10(esi, eax-o-0)   ; AddTask(seq, turn)
            eax = 61A5A0(0x20) ; si eax: 61A8B0(eax, 34h, 11Fh, 40800000h=4.0f, 0)                                                  ; CTaskSimpleRunAnim(ANIM_GROUP_GANGS, DEALER_DEAL, 4.0f, false)
            632D10(esi, eax-o-0)
  lead == 0 (682506): igual con la animacion 120h (DRUGS_BUY)
  fin       eax = esi (la secuencia) ; restaura SEH ; ret
`this` no se escribe. El partner solo se pasa (no se desreferencia). Llamadores del EXE: virtual +34h desde CTaskComplexPartner::CreateNextSubTask 0x683B46 (solo EAX).

Colaboradores (desensamblados en 0x61A500, 0x632BD0, 0x632D10, 0x66B890, 0x61A8B0, 0x61A390, 0x61A3B0):
  Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0), si no al = 1 ; prueba el bit 7 del estado ; si libre:
      estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
  Secuencia (0x632BD0, tras 0x61A3B0 {[+4]=0, [+8]=0, vtable}): [+0C]=0, [+30]=0, [+34]=0, byte[+38]=0, [+3C]=0, 8 dwords a 0 desde +10 ; bytes +39..+3B no se escriben.
  AddTask (0x632D10): primera de las 8 posiciones [+10..+2C] con 0 -> la tarea (aunque sea 0).
  Turn (0x66B890, tras 0x61A3B0): [+0C]=partner ; byte[+10]=1 ; si partner != 0 -> RegisterReference(0x571B70)(&[+0C]) ; [+20]=0.5f ; [+24]=0.02f ; el resto queda como estaba (0xCD).
  RunAnim (0x61A8B0, tras 0x61A390 {[+4]=0}): byte[+0C]=((hold&1)<<2)|(byte&F8) = 0xC8 ; [+08]=0 ; vtable 86D528 ; [+10]=grupo ; [+14]=anim ; [+18]=blend ; word[+1C]=190h ; resto 0xCD.
  RegisterReference (cuerpo en CEntity::RegisterReferenceWithResidue): no hace nada si el tipo es edificio, no temporal, no procedural y sin IPL, ni si ya hay una referencia con el mismo
      m_ppEntity ; si no: toma pEmptyList, la inserta en cabeza de m_pReferences con m_ppEntity.
Las vtables (palabra 0 de cada tarea) se sustituyen por GetTaskType: Secuencia 0xF4 (632C60), Turn 0x398 (66B900), RunAnim 0x190 (word +0x1C).
"""
import sys

M32 = 0xFFFFFFFF
CASES = 4096
NAME = 'CTaskComplexPartnerDeal::GetPartnerSequence'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_SEQ, TYPE_TURN, TYPE_RUN = 0xF4, 0x398, 0x190
F_HALF, F_MAXHEADING, F_BLEND4 = 0x3F000000, 0x3CA3D70A, 0x40800000
GROUP_GANGS, ANIM_DEAL, ANIM_BUY = 0x34, 0x11F, 0x120
REFS_OFF = 0x24
NALLOC = 3


def pool_alloc(win, pos, last):
    """Siguiente reserva: devuelve (rango en la ventana o None, nuevo cursor, nuevo pos)."""
    if pos < len(win):
        return pos, win[pos][0], pos + 1
    return None, 0, pos          # sin ranura libre: el cursor queda en 0


def seq_words(labels):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0
    for k in range(4, 12):
        w[k] = 0
    slots = [0] * 8
    for label in labels:          # AddTask: primera posicion libre (con 0); AddTask(0) deja el 0 en su sitio
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


def run_words(group, anim):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0                     # [+4] (CTaskSimple)
    w[2] = 0                     # [+8]
    w[3] = 0xCDCDCDC8            # byte [+0C] = ((0&1)<<2) | (0xCD & 0xF8)
    w[4] = group
    w[5] = anim
    w[6] = F_BLEND4
    w[7] = 0xCDCD0190            # word [+1C] = 0x190 (TASK_SIMPLE_NAMED_ANIM) ; bytes +1E/+1F sin escribir
    return w


def expected_deal(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(NALLOC, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    if inp['refs_off'] != REFS_OFF:
        raise RuntimeError('Desplazamiento de m_pReferences inesperado: %#x' % inp['refs_off'])
    lead = (inp['lead'] & 0xFF) != 0
    has_partner = bool(inp['has_partner'])
    # --- reservas (orden: secuencia, turn, animacion)
    pos = 0
    cur = last
    ranks = []
    for _ in range(NALLOC):
        r, cur, pos = pool_alloc(win, pos, cur)
        ranks.append(r)
    seq_rank, turn_rank, run_rank = ranks
    if seq_rank is None:
        raise RuntimeError('Fixture sin ninguna ranura libre en %d (el EXE se detendria)' % case)
    label = lambda k, off=0: ('W%d+%d' % (k, off)) if k is not None else None
    partner_label = 'E+0' if has_partner else None
    # --- cambios de estado del pool
    changes = []
    for r in ranks:
        if r is not None:
            idx, st = win[r]
            changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
    changes.sort()
    # --- referencias: solo el Turn (+0xC), en orden de construccion
    chain_idx = inp['refs']['chain']
    old_chain = []
    if has_partner:
        for j in range(inp['refs_len']):
            old_chain.append(['N%d' % j, inp['node_pp'][j]])
    partner_refs = [list(e) for e in old_chain]
    regs = []
    if turn_rank is not None:
        regs.append((turn_rank, 12))
    building_skip = inp['etype'] == 1 and not inp['temp'] and not inp['proc'] and inp['ipl'] == 0
    registered = dup = skipped = 0
    ref_changes = []
    used = 0
    if has_partner:
        for rank, off in regs:
            pp = label(rank, off)
            if building_skip:
                skipped += 1
                continue
            if any(e[1] == pp for e in partner_refs):
                dup += 1
                continue
            node = chain_idx[used]
            ref_changes.append([node, partner_refs[0][0] if partner_refs else 0, pp])
            partner_refs.insert(0, ['R%d' % node, pp])
            used += 1
            registered += 1
    empty_head = 'R%d' % chain_idx[used] if chain_idx[used] >= 0 else 0
    ref_changes.sort()
    # --- tareas
    tasks = []
    seq_labels = [label(turn_rank), label(run_rank)]
    tasks.append({'type': TYPE_SEQ, 'words': seq_words(seq_labels), 'slot': seq_rank})
    if turn_rank is not None:
        tasks.append({'type': TYPE_TURN, 'words': turn_words(partner_label), 'slot': turn_rank})
    anim = ANIM_DEAL if lead else ANIM_BUY
    if run_rank is not None:
        tasks.append({'type': TYPE_RUN, 'words': run_words(GROUP_GANGS, anim), 'slot': run_rank})
    want = {'ret': label(seq_rank, 0), 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_changes': [], 'partner_other_ok': True, 'nodes_ok': True,
            'partner_refs': partner_refs, 'empty_head': empty_head, 'ref_changes': ref_changes}
    nalloc = sum(r is not None for r in ranks)
    stats = {
        'family%d' % inp['family']: True,
        'alloc3': nalloc == 3, 'alloc2': nalloc == 2, 'alloc1': nalloc == 1,
        'turn_null': turn_rank is None, 'run_null': run_rank is None,
        'partner_null': not has_partner,
        'lead_zero': not lead, 'lead_nonzero': lead, 'lead_noncanon': (inp['lead'] & 0xFF) > 1, 'lead_high': (inp['lead'] & 0xFF) >= 0x80,
        'registered1': registered == 1,
        'dup_ref': dup > 0, 'skip_building': skipped > 0,
        'refs0': has_partner and inp['refs_len'] == 0 and turn_rank is not None, 'refs1': has_partner and inp['refs_len'] == 1 and turn_rank is not None,
        'refs2': has_partner and inp['refs_len'] == 2 and turn_rank is not None,
        'last_at_cap1': last == cap - 1, 'search_wraps': bool(win) and win[0][0] <= last,
        'id_wrap': any((win[r][1] & 0x7F) == 0x7F for r in ranks if r is not None),
        'cursor_zero_after_fail': nalloc < NALLOC and cur == 0,
        'pool_mode%d' % inp['pool_mode']: True,
        'stack_seed': True,
    }
    if has_partner:
        stats['etype%d' % inp['etype']] = True
        stats['building_ipl'] = inp['etype'] == 1 and inp['ipl'] != 0 and turn_rank is not None
        stats['building_temp'] = inp['etype'] == 1 and bool(inp['temp']) and turn_rank is not None
        stats['building_proc'] = inp['etype'] == 1 and bool(inp['proc']) and turn_rank is not None
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_deal(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    win = [[10, 0x80 | 0x7F], [11, 0x85], [12, 0x80]]
    inp = dict(family=2, lead=1, has_partner=1, etype=3, temp=0, proc=0, ipl=0, refs_len=0, dup_kind=0, pool_mode=0, pool_k=3,
               eax=0, edx=0, stack_seed=0, refs_off=REFS_OFF, this_off=0x40,
               pool=dict(cap=5000, last=9, free_total=1000, win=win), refs=dict(chain=[5, 6, 7, 8]), node_pp=[])
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) lead, peaton, sin referencias previas: Turn registra una; RunAnim 0x34/0x11F
    want, st = expected_deal(_base(), 0)
    t = want['tasks']
    assert [x['slot'] for x in t] == [0, 1, 2] and [x['type'] for x in t] == [TYPE_SEQ, TYPE_TURN, TYPE_RUN]
    assert t[0]['words'][4:8] == ['W1+0', 'W2+0', 0, 0] and t[2]['words'][4:8] == [0x34, 0x11F, F_BLEND4, 0xCDCD0190] and t[2]['words'][3] == 0xCDCDCDC8
    assert t[1]['words'][9] == 0x3CA3D70A and t[1]['words'][8] == 0x3F000000 and t[1]['words'][3] == 'E+0' and t[1]['words'][4] == 0xCDCDCD01
    assert want['ret'] == 'W0+0' and want['last_after'] == 12 and want['state_changes'] == [[10, 0], [11, 6], [12, 1]]
    assert want['ref_changes'] == [[5, 0, 'W1+12']] and want['empty_head'] == 'R6' and want['partner_refs'] == [['R5', 'W1+12']] and want['this_changes'] == []
    assert st['lead_nonzero'] and st['registered1'] and st['alloc3'] and st['id_wrap']
    # 2) NO lead (byte 0): animacion 0x120 ; byte no canonico 0x80 sigue siendo lead
    want, st = expected_deal(_base(lead=0), 1)
    assert want['tasks'][2]['words'][5] == 0x120 and st['lead_zero']
    want, st = expected_deal(_base(lead=0x80), 1)
    assert want['tasks'][2]['words'][5] == 0x11F and st['lead_noncanon'] and st['lead_high']
    want, st = expected_deal(_base(lead=0x100), 1)   # solo cuenta el byte bajo
    assert want['tasks'][2]['words'][5] == 0x120
    # 3) partner nulo: el Turn no registra y guarda 0 ; el RunAnim no cambia
    want, st = expected_deal(_base(has_partner=0), 2)
    assert want['ref_changes'] == [] and want['tasks'][1]['words'][3] == 0 and want['empty_head'] == 'R5' and st['partner_null']
    # 4) edificio sin IPL no registra ; con IPL si
    want, st = expected_deal(_base(etype=1), 3)
    assert st['skip_building'] and want['ref_changes'] == []
    want, st = expected_deal(_base(etype=1, ipl=7), 3)
    assert not st['skip_building'] and len(want['ref_changes']) == 1 and st['building_ipl']
    # 5) referencia ya registrada del Turn
    want, st = expected_deal(_base(refs_len=2, dup_kind=1, node_pp=[0xA5A50000, 'W1+12']), 4)
    assert st['dup_ref'] and want['ref_changes'] == [] and want['partner_refs'] == [['N0', 0xA5A50000], ['N1', 'W1+12']]
    want, st = expected_deal(_base(refs_len=1, node_pp=[0xA5A50000]), 4)
    assert want['ref_changes'] == [[5, 'N0', 'W1+12']] and want['partner_refs'] == [['R5', 'W1+12'], ['N0', 0xA5A50000]]
    # 6) pool casi agotado: una libre -> solo la secuencia (AddTask con nulos: posicion 0 sigue en 0) ; dos -> secuencia y Turn ; la animacion falla
    p1 = _base(pool=dict(cap=5000, last=4990, free_total=1, win=[[3, 0x83]]))
    want, st = expected_deal(p1, 5)
    assert [x['slot'] for x in want['tasks']] == [0] and want['last_after'] == 0 and want['tasks'][0]['words'][4:12] == [0] * 8 and want['ref_changes'] == [] and st['alloc1']
    p2 = _base(pool=dict(cap=5000, last=4999, free_total=2, win=[[0, 0x80], [1, 0xFF]]))
    want, st = expected_deal(p2, 6)
    assert [x['slot'] for x in want['tasks']] == [0, 1] and want['last_after'] == 0 and want['tasks'][0]['words'][4:7] == ['W1+0', 0, 0] and st['alloc2'] and st['run_null']
    # el Turn falla pero la animacion no es posible (reservas consecutivas): con 1 libre el Turn y la animacion son nulos; AddTask(0) dos veces
    # 7) fixtures invalidas
    for bad in (_base(pool=dict(cap=5000, last=9, free_total=1, win=[])), _base(refs_off=0x10)):
        try:
            expected_deal(bad, 7)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except (RuntimeError, KeyError):
            pass
    print('ok')
