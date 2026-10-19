"""Oraculo independiente del banco FightAbort31: CTaskSimpleFight::MakeAbortable (0x6239F0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x6239F0 (thiscall, ECX=this -> ESI, 3 argumentos de pila ped/prioridad/evento, `ret 0Ch`; guarda EBP, ESI, EDI; sin marco SEH):
  6239F0  push ebp ; ebp=[esp+0Ch] (prioridad) ; cmp ebp,1 ; push esi ; esi=ecx ; je 623A0D ; cmp ebp,2 ; je 623A0D
  623A02  byte [esi+26h]=1 (m_nNextCommand) ; pop esi ; xor al,al ; pop ebp ; ret 0Ch           (prioridad distinta de 1 y 2: devuelve 0)
  623A0D  push edi ; edi=[esp+18h] (evento) ; test edi,edi ; je 623A36
  623A16  eax=[edi] ; ecx=edi ; call [eax+8] (GetEventPriority) ; cmp eax,1Fh ; jle 623A2E (DEVUELVE 0, comparacion CON SIGNO)
  623A22  edx=[edi] ; ecx=edi ; call [edx+8] (SE VUELVE A PEDIR) ; cmp eax,3Ch ; jne 623A36 ; 623A2E: xor al,al ; ret 0Ch
  623A36  eax=[esi+1Ch] (m_pAnim) ; test eax,eax ; je 623A5F
  623A3D  cmp ebp,2 ; jne 623A49 ; dword [eax+1Ch] (m_BlendDelta) = 0xC47A0000 (-1000.0)
  623A49  ecx=[esi+1Ch] ; SetDeleteCallback(0x4D1390, 0) (0x4CEBC0, thiscall ret 8; push 0, push 0x4D1390) ; dword [esi+1Ch]=0
  623A5F  ecx=[esi+20h] (m_pIdleAnim) ; test ecx,ecx ; edi=[esp+10h] (ped) ; je 623AD3
  623A6A  SetDeleteCallback(0x4D1390, 0) sobre ecx ; ecx=[esi+20h] (se recarga)
  623A79  fld [ecx+18h] ; fcomp 0.0 ; test ah,41h ; jne 623ACC  -> sigue solo si PESO > 0 (con NaN no)
  623A89  fld [ecx+1Ch] ; fcomp 0.0 ; test ah,1 ; jne 623ACC     -> sigue solo si DELTA >= 0 (con NaN no)
  623A99  ebp==2: BlendAnimation(clump=[edi+18h], grupo=[edi+4D4h], 3, 1000.0f=0x447A0000) ; si no: ... 16.0f=0x41800000 ; (0x4D4610 cdecl, 4 argumentos, add esp,10h) -- SIN comprobar ped nulo
  623ACC  dword [esi+20h]=0
  623AD3  test edi,edi ; je 623B00 ; ecx=edi ; call 5DF8F0 (CPed::IsPlayer) ; test al,al ; je 623B00
  623AE2  eax=[edi+480h] (m_pPlayerData, SIN comprobar nulo) ; eax+=0Ch ; dword [eax]=0 ; dword [eax+4]=0 (m_vecFightMovement) ; push edi ; ecx=esi ; call 61C9B0 (FindTargetOnGround, ret 4)
  623B00  byte [esi+8]=1 (m_bIsFinished) ; mov al,1 ; ret 0Ch
Solo AL cuenta como retorno (0/1). Llamadores: solo tabla virtual (ranura 6 de CTask::MakeAbortable); el sitio de llamada no lee ECX/EDX tras volver.

Eventos que registran los falsos (6 enteros): [1, 2, 5, 0, 0, 0] GetEventPriority ; [2, etiqueta(asoc.), callback, dato, 0, 0] SetDeleteCallback ; [3, clump, grupo, anim, bits_delta, 0] BlendAnimation ;
[4, etiqueta(ped), 0, 0, 0, 0] IsPlayer ; [5, etiqueta(tarea), etiqueta(ped), 0, 0, 0] FindTargetOnGround. Etiquetas: 0 nulo, 1 tarea, 2 anim, 3 reposo, 4 ped, 5 evento, 6 datos del jugador.
Cambios: [arena, desplazamiento respecto al objeto, byte nuevo] con arena 0 tarea, 1 anim, 2 reposo, 3 ped, 4 datos del jugador, 5 evento.
"""
import struct
import sys

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskSimpleFight::MakeAbortable'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
SLOT = 2
CB = 0x4D1390
FINISHED_AT, ANIM_AT, IDLE_AT, NEXT_AT = 8, 0x1C, 0x20, 0x26
DELTA_AT = 0x1C
PD_FIGHT_AT = 0xC
NEG_1000 = 0xC47A0000
BLEND_IMMEDIATE = 0x447A0000
BLEND_URGENT = 0x41800000
L_TASK, L_ANIM, L_IDLE, L_PED, L_EV, L_PD = 1, 2, 3, 4, 5, 6
# tamanos de arena (suma a mano guarda + ventana + guarda; comprobados por fixture_mirror.py contra Fixtures.hpp)
SIZES = {'this_bytes': 0x28, 'this_off': 0x40, 'this_arena': 0x100, 'anim_bytes': 0x3C, 'anim_off': 0x20, 'anim_arena': 0xA0, 'ped_bytes': 0x4D8, 'ped_off': 0x40, 'ped_arena': 0x558,
         'pd_bytes': 0x20, 'pd_off': 0x20, 'pd_arena': 0x80, 'ev_bytes': 0x10, 'ev_off': 0x20, 'ev_arena': 0x80, 'slots': 16}


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def le(v):
    return [(v >> (8 * i)) & 0xFF for i in range(4)]


def change_word(changes, arena, off, old, new):
    """Anade los bytes que cambian al escribir la palabra `new` donde habia `old`."""
    for i, (a, b) in enumerate(zip(le(old), le(new))):
        if a != b:
            changes.append([arena, off + i, b])


def expected_abort(inp, case):
    if inp['slot'] != SLOT:
        raise RuntimeError('Ranura de la vtable inesperada %r' % inp['slot'])
    prio = inp['prio'] & M32
    ev, changes = [], []
    ped = inp['ped_kind']
    if prio not in (1, 2):
        if (inp['next0'] & 0xFF) != 1:
            changes.append([0, NEXT_AT, 1])
        want = {'ret': 0, 'events': ev, 'changes': changes}
        return want, _stats(inp, 'idle_priority', False, False, False)
    ret = 1
    rejected = None
    if inp['has_event']:
        ev.append([1, SLOT, L_EV, 0, 0, 0])
        t = s32(inp['ev_type'])
        if t <= 0x1F:
            rejected = 'low'
        else:
            ev.append([1, SLOT, L_EV, 0, 0, 0])      # el EXE vuelve a pedir el tipo
            if t == 0x3C:
                rejected = '3c'
    if rejected:
        want = {'ret': 0, 'events': ev, 'changes': changes}
        return want, _stats(inp, 'rejected_' + rejected, False, False, False)
    if inp['has_anim']:
        if prio == 2:
            change_word(changes, 1, DELTA_AT, inp['anim_delta'], NEG_1000)
        ev.append([2, L_ANIM, CB, 0, 0, 0])
        change_word(changes, 0, ANIM_AT, inp['anim_ptr'], 0)
    blended = False
    if inp['has_idle']:
        ev.append([2, L_IDLE, CB, 0, 0, 0])
        w, d = fl(inp['idle_weight']), fl(inp['idle_delta'])
        if w > 0.0 and d >= 0.0:                       # NaN: falso
            if ped == 0:
                raise RuntimeError('fixture invalida: el EXE desreferencia un ped nulo en la mezcla del reposo (caso %d)' % case)
            ev.append([3, inp['ped_clump'] & M32, inp['ped_group'] & M32, 3, BLEND_IMMEDIATE if prio == 2 else BLEND_URGENT, 0])
            blended = True
        change_word(changes, 0, IDLE_AT, inp['idle_ptr'], 0)
    player = False
    if ped != 0:
        ev.append([4, L_PED, 0, 0, 0, 0])
        if ped == 2:
            player = True
            change_word(changes, 4, PD_FIGHT_AT, inp['pd_x'], 0)
            change_word(changes, 4, PD_FIGHT_AT + 4, inp['pd_y'], 0)
            ev.append([5, L_TASK, L_PED, 0, 0, 0])
    if (inp['finished0'] & 0xFF) != 1:
        changes.append([0, FINISHED_AT, 1])
    changes.sort()
    want = {'ret': ret, 'events': ev, 'changes': changes}
    return want, _stats(inp, 'accepted', inp['has_anim'] != 0, inp['has_idle'] != 0, blended, player)


def _stats(inp, kind, anim=False, idle=False, blended=False, player=False):
    prio = inp['prio'] & M32
    s = {
        'family%d' % inp['family']: True, 'kind_' + kind: True,
        'prio_0': prio == 0, 'prio_1': prio == 1, 'prio_2': prio == 2, 'prio_other': prio not in (0, 1, 2),
        'prio_high_bit': bool(prio & 0x80000000),
        'event_present': inp['has_event'] != 0, 'event_absent': inp['has_event'] == 0,
        'type_1f': inp['has_event'] != 0 and inp['ev_type'] == 0x1F, 'type_20': inp['has_event'] != 0 and inp['ev_type'] == 0x20,
        'type_3c': inp['has_event'] != 0 and inp['ev_type'] == 0x3C, 'type_neg': inp['has_event'] != 0 and bool(inp['ev_type'] & 0x80000000),
        'anim_released': anim, 'idle_released': idle, 'idle_blended': blended, 'idle_blend_immediate': blended and prio == 2, 'idle_blend_urgent': blended and prio == 1,
        'idle_not_blended': idle and not blended, 'player_zeroed': player, 'ped_null': inp['ped_kind'] == 0, 'ped_not_player': inp['ped_kind'] == 1,
        'anim_delta_written': anim and prio == 2, 'anim_only': anim and not idle, 'idle_only': idle and not anim,
        'finished_was_not1': (inp['finished0'] & 0xFF) != 1, 'finished_was_1': (inp['finished0'] & 0xFF) == 1,
        'next_was_not1': (inp['next0'] & 0xFF) != 1, 'stack_seed': True,
    }
    return s


def expected(function, inp, case):
    if function == NAME:
        return expected_abort(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    base = dict(family=3, prio=1, has_event=0, ev_type=0, has_anim=1, has_idle=1, ped_kind=2, anim_delta=0x3F800000, idle_weight=0x3F800000, idle_delta=0, pd_x=0x11223344, pd_y=0,
                finished0=0, next0=5, ret=0, eax=0, edx=0, stack_seed=0, anim_ptr=0x00401000, idle_ptr=0x00402000, ped_clump=0xAABBCCDD, ped_group=7, this_off=0x40, slot=2)
    # urgente, todo presente, jugador: callback del golpe, reposo mezclado a 16.0, datos a cero, tarea terminada
    want, st = expected_abort(base, 0)
    assert want['ret'] == 1
    assert want['events'] == [[2, 2, CB, 0, 0, 0], [2, 3, CB, 0, 0, 0], [3, 0xAABBCCDD, 7, 3, BLEND_URGENT, 0], [4, 4, 0, 0, 0, 0], [5, 1, 4, 0, 0, 0]], want['events']
    assert sorted(c for c in want['changes'] if c[0] == 0) == [[0, 8, 1], [0, 0x1D, 0], [0, 0x1E, 0], [0, 0x21, 0], [0, 0x22, 0]], want['changes']   # 0x00401000 y 0x00402000 a cero
    assert [c for c in want['changes'] if c[0] == 4] == [[4, 0xC, 0], [4, 0xD, 0], [4, 0xE, 0], [4, 0xF, 0]]    # pd_x = 0x11223344 a cero; pd_y ya era 0
    assert not [c for c in want['changes'] if c[0] == 1]                                     # urgente: la anim conserva su delta
    assert st['kind_accepted'] and st['idle_blended'] and st['idle_blend_urgent'] and st['player_zeroed']
    # inmediata: delta -1000 en la anim (0x3F800000 -> 0xC47A0000: cambian los dos bytes altos), mezcla a 1000.0
    want, st = expected_abort(dict(base, prio=2), 1)
    assert [3, 0xAABBCCDD, 7, 3, BLEND_IMMEDIATE, 0] in want['events']
    assert sorted(c for c in want['changes'] if c[0] == 1) == [[1, 0x1E, 0x7A], [1, 0x1F, 0xC4]]
    want, st = expected_abort(dict(base, prio=2, anim_delta=0xC47A0000), 2)
    assert not [c for c in want['changes'] if c[0] == 1]
    # prioridad ocioso: solo next = 1
    want, st = expected_abort(dict(base, prio=0, next0=1), 3)
    assert want == {'ret': 0, 'events': [], 'changes': []} and st['kind_idle_priority']
    want, _ = expected_abort(dict(base, prio=3, next0=9), 4)
    assert want['changes'] == [[0, 0x26, 1]] and want['ret'] == 0
    # evento: tipo <= 0x1F con signo -> una sola llamada y nada mas; 0x3C -> dos llamadas
    want, st = expected_abort(dict(base, has_event=1, ev_type=0x1F), 5)
    assert want == {'ret': 0, 'events': [[1, 2, 5, 0, 0, 0]], 'changes': []} and st['kind_rejected_low']
    want, st = expected_abort(dict(base, has_event=1, ev_type=0x80000000), 6)
    assert want['ret'] == 0 and len(want['events']) == 1
    want, st = expected_abort(dict(base, has_event=1, ev_type=0x3C), 7)
    assert want['ret'] == 0 and want['events'] == [[1, 2, 5, 0, 0, 0]] * 2 and want['changes'] == [] and st['kind_rejected_3c']
    want, st = expected_abort(dict(base, has_event=1, ev_type=0x20), 8)
    assert want['ret'] == 1 and want['events'][:2] == [[1, 2, 5, 0, 0, 0]] * 2
    # reposo que no se mezcla (NaN, peso <= 0, delta < 0) y ped nulo
    for w, d in ((0x7FC00000, 0), (0, 0), (0x80000000, 0), (0x3F800000, 0xBF800000), (0x3F800000, 0x7FC00000), (0xBF800000, 0)):
        want, st = expected_abort(dict(base, idle_weight=w, idle_delta=d), 9)
        assert not [e for e in want['events'] if e[0] == 3] and st['idle_not_blended'], (hex(w), hex(d))
    want, st = expected_abort(dict(base, idle_weight=0, ped_kind=0), 10)
    assert [e for e in want['events'] if e[0] == 4] == [] and st['ped_null']
    try:
        expected_abort(dict(base, ped_kind=0), 11)
        raise SystemExit('el oraculo debia rechazar un ped nulo con mezcla')
    except RuntimeError:
        pass
    # delta == -0.0 cuenta como >= 0 ; peso denormal positivo cuenta como > 0
    want, _ = expected_abort(dict(base, idle_weight=1, idle_delta=0x80000000), 12)
    assert [e for e in want['events'] if e[0] == 3]
    # ya terminada con 1 y siguiente igual: no hay cambios en +8
    want, _ = expected_abort(dict(base, finished0=1, has_anim=0, has_idle=0, ped_kind=1), 13)
    assert want['changes'] == [] and want['events'] == [[4, 4, 0, 0, 0, 0]]
    try:
        expected_abort(dict(base, slot=5), 14)
        raise SystemExit('el oraculo debia rechazar una ranura incorrecta')
    except RuntimeError:
        pass
    print('ok')
